#include "net_connect.h"

#include "heap.h"

#define MAX_BUFFER              1460
#define MAX_LINES               10
#define REPORT_TIMEPERIOD_MS    10000


#define HTTPS_PORT              443
#define HTTP_PORT               80
#define MIN(A,B)                (A)<(B)?(A):(B)


static const char DOWNLOAD_HEADER[] =
"GET  %s HTTP/1.1\r\n"
"Host: %s\r\n"\
  "User-Agent: STSTVS\r\n"\
    "\r\n";

static const char IOTA_DOWNLOAD_HEADER[] =
"GET %s HTTP/1.1\r\n"
"Host: %s\r\n"\
"Content-Type: application/json\r\n"\
"\r\n";


typedef struct t_http_context
{
  int        hSocket ;
  uint32_t   status;
  uint64_t   contentLen;
  uint64_t   posFile;
}http_context_t;



static void      http_split_host_request(const char *pURL,char **pHttpHost,char **pHttpReq,bool *secure)
{
  static char host[50];
  char *pStr = 0;
  NET_ASSERT(pHttpHost , "Http host is undefined");
  NET_ASSERT(pHttpReq  , "Http req file name is uncorect");

  if(strncmp(pURL,"http://",strlen("http://")) == 0)
  {
    pStr = ((char *)pURL) + strlen("http://");
    *secure = false;
  }
  if(strncmp(pURL,"https://",strlen("https://")) == 0)
  {
    pStr = ((char *)pURL) + strlen("https://");
    *secure = true;
  }

  char *pHost = host;
  while(*pStr && *pStr != '/')
  {
    *pHost++ = *pStr++;
  }
  *pHost = 0;
  *pHttpHost  = host;
  *pHttpReq   = pStr;
}



static int http_read_response(int32_t sock, unsigned char *pHeader, uint32_t maxSize, uint32_t *pRetSize)
{
  unsigned char *pString = pHeader;
  int32_t ret=0;
  int32_t headerSize    = maxSize;
  uint32_t count = 0;
  
  while(headerSize>0)
  {
    ret = net_recv(sock, (void *)pString, 1,0);
    if(ret < 0) return ret;
  
    pString++;
    headerSize--;
    count++;
    if(count > 4 && strncmp((char *)pString-4,"\r\n\r\n",4)==0)
    {
      break;
    }
  }
  if(pRetSize) *pRetSize = count;
  if(headerSize==0) return -1;
  return NET_OK;
}


// fill a tbl with each line entries
static uint32_t service_lines( unsigned char *pBuffer,unsigned char **tLines,uint32_t maxLines)
{
  uint32_t index = 0;
  while(index  < maxLines && *pBuffer)
  {
    *tLines++ = pBuffer;
    // seek to the new line
    while(*pBuffer && *pBuffer != '\n') pBuffer++;
    index++;
    if(*pBuffer) pBuffer++;

  }
  return index;
}

// move to char ...
static char *http_seek_to(char *pString,char key)
{
  while(*pString && *pString != key) pString++;
  if(*pString) pString ++;
  return pString;
}

// move to char ...
static char *http_seek_while(char *pString,char key)
{
  while(*pString && *pString == key) pString++;
  return pString;
}
// move to char ...
static char *http_seek_while_not(char *pString,char key)
{
  while(*pString && *pString != key) pString++;
  return pString;
}

// check the response
static int http_check_response(http_context_t *pCtx,unsigned char *pBuffer)
{
  char   *tLines[MAX_LINES];
  uint32_t nbLines = service_lines(pBuffer,(unsigned char **)tLines,MAX_LINES);
  pCtx->contentLen = (uint64_t)-1;
  pCtx->status     = 400;
  pCtx->posFile    = 0;
  if(nbLines < 1) return NET_ERROR_GENERIC;

  // Extract the status from the first line
  char *pString = tLines[0];
  pString = http_seek_while_not(pString,' ');
  pString = http_seek_while(pString,' ');
  sscanf(pString,"%lu",&pCtx->status);
  // parses each header lines and only manage mandatory info
  for(int a = 0 ; a < nbLines ; a++)
  {
    if(strncmp(tLines[a],"Content-Length",strlen("Content-Length"))==0)
    {
      // move to the size
      char *pParam = http_seek_to(tLines[a],':');
      pParam = http_seek_while(pParam,' ');
      uint64_t len;
      sscanf(pParam,"%llu",&len);
      // fill len
      pCtx->contentLen = len;
    }
  }
  // check error status
  if(pCtx->status < 200 || pCtx->status >= 300) return NET_ERROR_GENERIC;
  return NET_OK;
}





static int32_t  http_file_download(int socket,const char *pHost,char *pReq)
{
  uint8_t               *pBuffer =0;
  uint32_t              elapsed_time;
  uint32_t              start_time;
  uint32_t              local_start_time;
  
  uint32_t              cptCount = 0;
  uint32_t              retSize;
  uint32_t              len;
  http_context_t        httpCtx;
  float                 refBw;


  // construct the header from the host and the request 
  pBuffer = NET_MALLOC(MAX_BUFFER);
  NET_ASSERT(pBuffer,"Malloc failed");
  int rc = snprintf((char *)pBuffer,MAX_BUFFER-1,IOTA_DOWNLOAD_HEADER,pReq,pHost);
  printf("Http request =\n%s\n", pBuffer);

  // send GET HTTP request
  net_send(socket, pBuffer, rc, 0);
  
  // wait for the response
  if( http_read_response(socket,pBuffer,MAX_BUFFER-1,&retSize) != NET_OK)  
  {
    printf("Http server : no answer from download request\n");
    goto exit;
  }
  
  if( http_check_response(&httpCtx,pBuffer) != NET_OK)
  {
    printf("Http server : error checking HTTP request response\n");
    goto exit;
  }  

  len = httpCtx.contentLen;
  len=MAX_BUFFER*130;
//  printf("Network Perf:Download %s %d bytes\n",pHost, len);
  local_start_time = start_time = NET_TICK();
//  net_perf_start();
  while(len)
  {
    int32_t count;
    int szInSByte  = MIN(MAX_BUFFER,len);

    count  = net_recv(socket, (void *)pBuffer, szInSByte,0);
    if(count  == -1)
    {
      break;
    }
    cptCount += count  ;
    
    elapsed_time = NET_TICK() - local_start_time;
    if(elapsed_time  > REPORT_TIMEPERIOD_MS)
    {
      float refBw;
      refBw  =   8.0F*cptCount / elapsed_time;
      printf("Network Perf: Read  %d bytes Remain %d bytes Bw: %.1f Kb/s\n",cptCount,len,refBw);
      cptCount = 0;
      local_start_time = NET_TICK();
    }
    
#ifdef CHECK_RECV_DATA
      for(int a = 0; a < count ; a++)
      {
        if(pBuffer[a] != (unsigned char )refData)
        {
          printf("Network Perf: Payload corrupted offset",cptCount);
          break;
          
        }
        refData++;
      }
#endif
    len  -= count;
   // extern stat_t       stat;
    //printf("Mbedtls cycle %d\n",stat.mbedtls_recv_cycle);
  }
  elapsed_time = NET_TICK() - start_time;
//  net_perf_report();
  refBw  =   8.0F*cptCount / elapsed_time;
//  printf("Network Perf: Read  %d bytes Remain %d bytes duration %d ms Bw: %.1f Kb/s\n",cptCount,len,elapsed_time,refBw);

  printf("Http server answer =\n%s\n", pBuffer);

exit:
  NET_FREE(pBuffer);
  return NET_OK;
}

extern  char tls_root_ca_cert[];


#define HTTP_FILE_URL  "http://public.st.free.fr/500MO.bin"
//#define HTTPS_FILE_URL  "https://wireless.murata.com/pub/RFM/data/usd-m2_revA.pdf"
//#define HTTPS_FILE_URL  "https://api.lb-0.testnet.chrysalis2.com"
#define HTTPS_FILE_URL  "https://api.lb-0.testnet.chrysalis2.com/api/v1/tips"


void test_http_download(void)
{
  char          *hostname;
  char          *pHttpReq;
  bool          secure;
  char          *url;
  int           sock;
  int           port;
  net_sockaddr_t addr;

  url=HTTPS_FILE_URL; 

  static int index = 0;
  while(1)
  {
#ifdef HEAP_DEBUG
  stack_measure_prologue();
#endif
    printf("\n####### Download #%d from %s #########\n", ++index, url);
    http_split_host_request(url,&hostname,&pHttpReq,&secure);
    
    addr.sa_len = sizeof(sockaddr_in_t);
    if (net_if_gethostbyname(NULL,&addr,hostname) < 0)
    {
      printf("Could not find hostname %s\n",hostname);
      //return;
      continue;
    }
    
    port = HTTP_PORT;
    if (secure)
    {
      port = HTTPS_PORT;
    }
    
#if  NET_USE_IPV6
    if (addr.sa_family == NET_AF_INET)
    {
      sockaddr_in_t*       addr_ip4=(sockaddr_in_t*) &addr;
      addr_ip4->sin_port = NET_HTONS(port);
    }
    else
    {
      sockaddr_in6_t*       addr_ip6=(sockaddr_in6_t*) &addr;
      addr_ip6->sin6_port = NET_HTONS(port);
    }
#else
    sockaddr_in_t*       addr_ip4=(sockaddr_in_t*) &addr;
    addr_ip4->sin_port = NET_HTONS(port);
#endif
    
    
    sock = net_socket (NET_AF_INET, NET_SOCK_STREAM, NET_IPPROTO_TCP);
    if (sock < 0 )
    {
      printf("Could not create the socket.\n");
      //return;
      continue;
    }
    
    if (true == secure)
    {
      bool          check_server_identity= false;
      int32_t       ret;
#if 0
      ret  = net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_SECURE, NULL, 0);
      // ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_CA_CERT, tls_root_ca_cert, strlen(tls_root_ca_cert)+1);
      ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_NAME, hostname, strlen(hostname)+1);
      ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_VERIFICATION,&check_server_identity, sizeof(bool));
#else
#include "iot_flash_config.h"
extern const user_config_t *lUserConfigPtr;
      ret  = net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_SECURE, NULL, 0);
      ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_CA_CERT, (void *)lUserConfigPtr->tls_root_ca_cert, strlen(lUserConfigPtr->tls_root_ca_cert) + 1);
      ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_NAME, hostname, strlen(hostname) + 1);
      ret |= net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_VERIFICATION,&check_server_identity, sizeof(bool));
#endif /* 0 */
    }
    
    if( net_connect( sock, &addr, sizeof( addr ) ) == 0 )
    {
      printf("Connected to %s\n",hostname);
      http_file_download(sock, hostname,pHttpReq);
    }
    else
    {
      printf("Cannot connect to %s\n",hostname);
    }

    if( net_closesocket( sock ) != 0 )
    {
      printf("Cannot close the socket\n");
    }
#ifdef   HEAP_DEBUG
  stack_measure_epilogue();
  uint32_t heap_max,heap_current,stack_size;

  heap_stat(&heap_max,&heap_current,&stack_size);
  printf("Heap Max allocation 0x%x (%d), current allocation 0x%x (%d), Stack max size 0x%x (%d)\n",heap_max,heap_max,heap_current,heap_current,stack_size,stack_size);
#endif
    //HAL_Delay(1000);
  }
}