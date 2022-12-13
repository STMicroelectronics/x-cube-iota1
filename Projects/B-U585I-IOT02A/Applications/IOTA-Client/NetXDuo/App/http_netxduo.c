#include "nx_api.h"
#include "nxd_dns.h"
#include "app_azure_rtos.h"
#include "app_netxduo.h"
#include "iota_conf.h"
#include "iota_cert.h"
#include "cJSON.h"
#include "client/network/http.h"


#define HTTP_READ_BUFFER_SIZE PAYLOAD_SIZE
#define NX_WEB_HTTP_SESSION_MAX 2

static NX_WEB_HTTP_CLIENT http_client;

extern NX_DNS DNSClient;
extern NX_PACKET_POOL AppPool;
extern NX_IP IpInstance;


VOID NetXDuo_InitHooks(VOID)
{
  cJSON_Hooks hooks;

  hooks.malloc_fn = SysAlloc;
  hooks.free_fn = SysFree;
  cJSON_InitHooks(&hooks);
}

static VOID http_response_body(byte_buf_t* const response, UCHAR* data, ULONG size)
{
  if (byte_buf_append(response, (byte_t*)data, (size_t)size) == false)
  {
    // OOM or NULL data
    printf("Append response data failed\n");
  }
}

static UINT http_receive_response(byte_buf_t* const response)
{
  UINT get_status;
  UINT ret;
  UINT bytes_in_packet;
  ULONG bytes_offset;
  ULONG bytes_copied;
  NX_PACKET *receive_packet;
  UCHAR receive_buffer[HTTP_READ_BUFFER_SIZE];

  /* Receive response data from the server. Loop until all data is received. */
  get_status = NX_SUCCESS;
  while(get_status != NX_WEB_HTTP_GET_DONE)
  {
    get_status = nx_web_http_client_response_body_get(&http_client, &receive_packet, NX_WAIT_FOREVER);

    /* Check for error. */
    if (get_status != NX_SUCCESS && get_status != NX_WEB_HTTP_GET_DONE)
    {
      printf("HTTP get packet failed, error: 0x%x\n", get_status);
      break;
    }
    else
    {
      bytes_in_packet = receive_packet->nx_packet_length;
      bytes_offset = 0;
      while(bytes_in_packet > 0)
      {
        ret = nx_packet_data_extract_offset(receive_packet, bytes_offset, receive_buffer, HTTP_READ_BUFFER_SIZE, &bytes_copied);
        if(ret)
        {
          printf("Error in extracting response body data: 0x%x\n", ret);
        }
        receive_buffer[bytes_copied] = 0;

        // Append data to response buffer
        http_response_body(response, receive_buffer, bytes_copied);

        // Reset all variables and move markers accordingly
        bytes_in_packet -= bytes_copied;
        bytes_offset += bytes_copied;

        for(int i = 0; i < HTTP_READ_BUFFER_SIZE; i++)
        {
          receive_buffer[i] = 0;
        }
        bytes_copied = 0;
      }
      /* Regardless if an error occurs extracting the data, release the packet. We are done with it. */
      nx_packet_release(receive_packet);
    }
  }

  return get_status;
}

VOID http_client_init(VOID) {}

VOID http_client_clean(VOID) {}

int http_client_get(http_client_config_t const* const config, byte_buf_t* const response, long* status)
{
  UINT ret;
  NXD_ADDRESS node_ip_address;

  /* Create an HTTP client instance.  */
  ret = nx_web_http_client_create(&http_client, "HTTP Client", &IpInstance, &AppPool, 1024*10);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP Client creation: 0x%x\n", ret);
    return -1;
  }

  node_ip_address.nxd_ip_version = 4;
  /* Look up an IPv4 address over IPv4. */
  ret = nx_dns_host_by_name_get(&DNSClient, (UCHAR *)config->host,
                                &node_ip_address.nxd_ip_address.v4, DEFAULT_TIMEOUT);
  if (ret != NX_SUCCESS)
  {
    printf("Error in DNS Host by name: 0x%x\n", ret);
    goto done;
  }
  printf("Node IP address: %lu.%lu.%lu.%lu\n",
		  node_ip_address.nxd_ip_address.v4 >> 24,
		  node_ip_address.nxd_ip_address.v4 >> 16 & 0xFF,
		  node_ip_address.nxd_ip_address.v4 >> 8 & 0xFF,
		  node_ip_address.nxd_ip_address.v4 & 0xFF);

  /* Connect to the node. */
  ret = nx_web_http_client_connect(&http_client, &node_ip_address, config->port, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP Connect: 0x%x\n", ret);
    goto done;
  }


  /* Initialize HTTP request. */
  ret = nx_web_http_client_request_initialize(&http_client,
                                              NX_WEB_HTTP_METHOD_GET,
											  (CHAR *)config->path,
											  (CHAR *)config->host,
											  0, /* Used by PUT and POST */
											  NX_FALSE, /* If true, input_size is ignored. */
											  NX_NULL, //"name",
											  NX_NULL, // "password",
											  NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request initialization: 0x%x\n", ret);
    goto done;
  }

  /* Send the HTTP request we just built. */
  ret = nx_web_http_client_request_send(&http_client, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request send: 0x%x\n", ret);
    goto done;
  }

  /* Receive and parse the server response */
  *status = http_receive_response(response);

done:
  /* Clear out the HTTP client when we are done. */
  ret = nx_web_http_client_delete(&http_client);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP client deletion: 0x%x\n", ret);
  }

  return ret;
}

int http_client_post(http_client_config_t const* const config,
                     byte_buf_t const* const request,
					 byte_buf_t* const response,
					 long* status)
{
  UINT ret;
  NXD_ADDRESS node_ip_address;
  NX_PACKET *http_packet;

  /* Create an HTTP client instance.  */
  ret = nx_web_http_client_create(&http_client, "HTTP Client", &IpInstance, &AppPool, 1024*10);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP Client creation: 0x%x\n", ret);
    return -1;
  }

  node_ip_address.nxd_ip_version = 4;
  /* Look up an IPv4 address over IPv4. */
  ret = nx_dns_host_by_name_get(&DNSClient, (UCHAR *)config->host,
                                &node_ip_address.nxd_ip_address.v4, DEFAULT_TIMEOUT);
  if (ret != NX_SUCCESS)
  {
    printf("Error in DNS Host by name: 0x%x\n", ret);
    goto done;
  }
  printf("Node IP address: %lu.%lu.%lu.%lu\n",
		  node_ip_address.nxd_ip_address.v4 >> 24,
		  node_ip_address.nxd_ip_address.v4 >> 16 & 0xFF,
		  node_ip_address.nxd_ip_address.v4 >> 8 & 0xFF,
		  node_ip_address.nxd_ip_address.v4 & 0xFF);

  /* Connect to the node. */
  ret = nx_web_http_client_connect(&http_client, &node_ip_address, config->port, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP Connect: 0x%x\n", ret);
    goto done;
  }


  /* Initialize HTTP request. */
  ret = nx_web_http_client_request_initialize(&http_client,
                                              NX_WEB_HTTP_METHOD_POST,
											  (CHAR *)config->path,
											  (CHAR *)config->host,
											  request->len, /* Used by PUT and POST */
											  NX_FALSE, /* If true, input_size is ignored. */
											  NX_NULL, //"name",
											  NX_NULL, // "password",
											  NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request initialization: 0x%x\n", ret);
    goto done;
  }

  ret = nx_web_http_client_request_header_add(&http_client,
		  	  	  	  	  	  	  	  	  	  "Content-Type", strlen("Content-Type"),
											  "application/json",strlen("application/json"),
											  NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP header add: 0x%x\n", ret);
    goto done;
  }

  /* Send the HTTP request we just built. */
  ret = nx_web_http_client_request_send(&http_client, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request send: 0x%x\n", ret);
    goto done;
  }

  /* Create a new data packet request on the HTTP client instance. */
  ret = nx_web_http_client_request_packet_allocate(&http_client, &http_packet, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request packet allocation: 0x%x\n", ret);
    goto done;
  }

  /* At this point, we can fill the data into http_packet. */
  ret = nx_packet_data_append(http_packet, request->data, request->len, &AppPool, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP packet data append: 0x%x\n", ret);
    goto done;
  }

  /* Send data request packet to server. */
  ret = nx_web_http_client_request_packet_send(&http_client, http_packet, 0, NX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP request packet send: 0x%x\n", ret);
    goto done;
  }
  nx_packet_release(http_packet);


  /* Receive and parse the server response */
  *status = http_receive_response(response);

done:
  /* Clear out the HTTP client when we are done. */
  ret = nx_web_http_client_delete(&http_client);
  if (ret != NX_SUCCESS)
  {
    printf("Error in HTTP client deletion: 0x%x\n", ret);
  }

  return ret;
}

