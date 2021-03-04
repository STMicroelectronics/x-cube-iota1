/**
  ******************************************************************************
  * @file    http_util.c
  * @author  MCD Application Team
  * @brief   Helper functions for building HTTP GET and POST requests, and 
  *          stream reading.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "http.h"
#include "http_lib.h"
#include "main.h"
//#include "iota_conf.h"
#include "msg.h"
#include "net_connect.h"

/* Private defines -----------------------------------------------------------*/
#define HTTP_MAX_HOST_SIZE        80      /**< Max length of the http server hostname. */
#define HTTP_MAX_QUERY_SIZE       500      /**< Max length of the http query string. */
#define HTTP_READ_BUFFER_SIZE     1024    /**< Size of the HTTP receive buffer.
                                               Must be larger than the HTTP response header length. That is about 300-800 bytes. */
#define HTTP_HEADER               "HTTP/1.1"

#define MIN(a,b)        ( ((a)<(b)) ? (a) : (b) )

/* Private typedef -----------------------------------------------------------*/
/** 
 * @brief HTTP progressive download internal session context. 
 */
typedef struct {
  int32_t sock;                       /**< Network socket handle. */
  char query[HTTP_MAX_QUERY_SIZE];    /**< HTTP query parsed from the URL. Must be resent in each range request. */
  char hostname[HTTP_MAX_HOST_SIZE];  /**< HTTP full qualified server name. Must be resent in each range request. */
  int port;
  bool connection_is_open;            /**< HTTP keep-alive connection status. */
} http_context_t;


/* Private variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int http_connect(http_context_t *http_context);

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief   Create an HTTP request string from the parameter list.
 * @note    The request buffer is allocated by the callee and must be released 
 *          by calling http_req_destroy() after usage.
 * @param   Out:  req_buf         Output buffer, allocated by the callee.
 * @param   In:   query           HTTP query string.
 * @param   In:   hostname        Remote hostname.
 * @param   In:   extra_headers   HTTP headers to be appended to the request header. Single buffer. Each line must be ended by \r\n.
 * @param   In:   post_buffer     The document body to be sent, in case of a POST request. Must be NULL for a GET request.
 * @retval  Error code
 *            >0              Success, length of the request string written to req_buf.
 *            HTTP_ERR (<0)   Failure
 */
int http_req_create(char ** const req_buf,
                    const char * const query,
                    const char * const hostname,
                    const char * const extra_headers,
                    byte_buf_t* const post_buffer)
{
  int rc = HTTP_OK;
    
  size_t req_buf_len = strlen(query) + strlen(hostname);
  msg_http_debug("After query+hostname -- req_buf_len = %d\n", req_buf_len);
  req_buf_len += (extra_headers == NULL) ? 0 : strlen(extra_headers);
  msg_http_debug("After extra_headers -- req_buf_len = %d\n", req_buf_len);
  if (post_buffer == NULL)
  {
    /* HTTP GET request */
    req_buf_len  += 47; /* Overall fixed-size length of the request string .*/
  }
  else
  { 
    /* HTTP POST request */
    req_buf_len += 48 + post_buffer->len/10 +100; /* Overall fixed-size length of the request string .*/
    msg_http_debug("After Overall fixed-size length -- req_buf_len = %d (post_buffer->len=%d)\n", req_buf_len, post_buffer->len);
  }
  
  *req_buf = malloc(req_buf_len);
  if (req_buf == NULL)
  {
    rc = HTTP_ERR;
  }
  else
  {
    if (post_buffer == NULL)
    {
      /* HTTP GET request */
      rc = snprintf(*req_buf, req_buf_len,
        "GET /%s %s\r\n"
        "Host: %s\r\n"
        "%s\r\n\r\n",
        query, HTTP_HEADER, hostname, (extra_headers == NULL) ? "" : extra_headers);
    }
    else
    {
      /* HTTP POST request */
      rc = snprintf(*req_buf, req_buf_len,
        "POST /%s %s\r\n"
        "Host: %s\r\n"
        "Content-Length: %d\r\n"
        "%s\r\n\r\n",
        query, HTTP_HEADER, hostname, post_buffer->len, (extra_headers == NULL) ? "" : extra_headers);
    }
    if (rc >= req_buf_len)
    {
      msg_http_error("Memory overflow preparing the HTTP request string. %d >= %d\n", rc, req_buf_len);
      rc = HTTP_ERR; /* The allocated destination buffer is too small. */
      free(*req_buf);
    }
  }
  
  return rc; /* Return the string length, or a <0 error code. */
}


/**
 * @brief   Destroy an HTTP request string created by http_req_create();
 * @param   In:   req_buf         Buffer to be freed.
 */
void http_req_destroy(const char *req_buf)
{
  free((void*)req_buf);
}

static int http_connect(http_context_t *http_context)
{
  http_context_t * pCtx = http_context;
  int ret = 0;

  if (!pCtx)
  {
    msg_error("error invalid handle");
    return HTTP_ERR;
  }

  sockaddr_in_t addr;
  addr.sin_len = sizeof(sockaddr_in_t);
  if (net_if_gethostbyname(NULL,(sockaddr_t *)&addr, (char_t*) pCtx->hostname) < 0)
  {
    msg_error("Could not find hostname ipaddr %s\n", pCtx->hostname);
    return -1;
  }

  addr.sin_port = NET_HTONS(pCtx->port);
  ret = net_connect( pCtx->sock,  (sockaddr_t *)&addr, sizeof( addr ) );

  if (NET_OK != ret)
  {
    msg_error("Could not open a socket.\n");
  }
  else
  {
    pCtx->connection_is_open = true;
    return HTTP_OK;
  }

  return HTTP_ERR;
}

/**
 * @brief   Open an HTTP progressive download session.
 * @note    The internal session context is allocated by the callee.
 * @param   In: pHnd    Pointer on the session handle.
 * @param   In: url     Source of the progressive download. e.g. http://john.doe:80/foo.bar
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_open(http_handle_t * const pHnd, const char *url)
{
  http_context_t * pCtx = 0;
  int ret = 0;

  pCtx = (http_context_t *) malloc(sizeof(http_context_t));

  if (pCtx != NULL)
  {
    pCtx->connection_is_open = false;
    bool tls = false;
    if (HTTP_OK == http_url_parse(pCtx->hostname, HTTP_MAX_HOST_SIZE, &pCtx->port, &tls, pCtx->query, HTTP_MAX_QUERY_SIZE, url))
    {
      pCtx->sock = net_socket(NET_AF_INET, NET_SOCK_STREAM, NET_IPPROTO_TCP);
      if (pCtx->sock < 0)
      {
        msg_http_error("Could not create a socket.\n");
      }
      else
      {
        msg_http_debug("http_open   pCtx->hostname=%s\n", pCtx->hostname);
        if (tls == true)
        {
          ret  = net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_SECURE, NULL, 0);
          ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_CA_CERT, (void *)lUserConfigPtr->tls_root_ca_cert, strlen(lUserConfigPtr->tls_root_ca_cert) + 1);
          ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_NAME, (void *)pCtx->hostname, strlen(pCtx->hostname) + 1);
//          ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_VERIFICATION, (void *) &true_val, sizeof(bool));
        }
      }
      
      if (NET_OK != ret)
      {
        msg_http_error("Could not set the socket options (ret=%d).\n", ret);
      }
      else
      {
        ret = http_connect(pCtx);
      }
      
      if (NET_OK != ret)
      {
        msg_http_error("Could not open a socket.\n");
      }
      else
      {
        *pHnd = (http_handle_t) pCtx;
        pCtx->connection_is_open = true;
        return HTTP_OK;
      }
    }

    free(pCtx);
  }
  return HTTP_ERR;
}


/**
 * @brief   Close an HTTP progressive download session.
 * @note    The internal session context is freed by the callee.
 * @param   In: hnd   Session handle.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_close(const http_handle_t hnd)
{
  int rc = HTTP_ERR;

  http_context_t * pCtx = (http_context_t *) hnd;
  if (pCtx != NULL)
  {
    int ret = 0;
    ret = net_closesocket(pCtx->sock);
    pCtx->connection_is_open = false;

    if (ret == NET_OK)
    {
      rc = HTTP_OK;
      free(pCtx);
    }
    else
    {
      msg_http_error("Could not close and destroy a socket.\n");
    }
  }

  return rc;
}

/**
 * @brief   Tells whether an HTTP progressive download session is still open, or has been closed by the server.
 * @param   In: hnd   Session handle.
 * @retval  true:   The session is open.
 *          false:  The session is closed: the handle should be freed by calling http_close().
 */
bool http_is_open(const http_handle_t hnd)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  return pCtx->connection_is_open;
}


/**
 * @brief   Parse and split an http url into hostname/port number/query string.
 * @param   Out: host           Hostname. Allocated by the caller.
 * @param   In:  host_max_len   Maximum length of the hostname.
 * @param   Out: proto          Application protocol (HTTP or HTTPS).
 * @param   Out: port           Port number.
 * @param   Out: query          HTTP query. Allocated by the caller.
 * @param   Out: tls            This is an HTTPS URL.
 * @param   In:  query_max_len  Maximum length of the query.
 * @param   In:  url            URL to be parsed.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_url_parse(char * const host, const int host_max_len, int * const port, bool * tls, char * const query, const int query_max_len, const char * url)
{
  bool has_query = true;
  const char * pc = NULL;

#ifdef URL_TRACE
  msg_http_debug("url: %s\n", url);
#endif

  memset(host, 0, host_max_len);
  memset(query, 0, query_max_len);

#define HT_PX  "http://"
#define HTS_PX  "https://"
  
  if (0 == strncmp(url, HTS_PX, strlen(HTS_PX)))
  {
    *tls = true;
    pc = url + strlen(HTS_PX);
  }
  
  if (0 == strncmp(url, HT_PX, strlen(HT_PX)))
  {
    *tls = false;
    pc = url + strlen(HT_PX);
  }
  
  if ( pc != NULL )
  {
    char *pHostTailCol = strchr(pc, ':');
    char *pHostTailSla = strchr(pc, '/');
    if ((pHostTailCol <= pHostTailSla) && (pHostTailCol != NULL))
    {
      /*host = */strncpy(host, pc, MIN(pHostTailCol - pc, host_max_len));
      pc = pHostTailCol + 1;
      sscanf(pc, "%d", port);
      pc = strchr(pc, '/');
      if (pc != NULL)
      {
        pc++;
      } else
      {
        has_query = false;
      }
    } else
    {
      if (pHostTailSla != NULL)
      {
        strncpy(host, pc, MIN(pHostTailSla - pc, host_max_len));
        pc = pHostTailSla + 1;
      } else
      {
        /*host = */strncpy(host, pc, host_max_len);
        has_query = false;
      }
      *port = (*tls) ? 443 : 80;
    }
    if (has_query)
    {
      /*query = */strncpy(query, pc, query_max_len);
    }

#ifdef URL_TRACE
    msg_http_debug("http host: %s\n", host);
    msg_http_debug("http port: %d\n", *port);
    msg_http_debug("http query: %s\n", query);
#endif

    return HTTP_OK;
  } else
  {
    /* Not an HTTP url. */
    return HTTP_ERR;
  }
}

static void* response_realloc(void* opaque, void* ptr, int size)
{
  return realloc(ptr, size);
}

static void response_body(void* opaque, const char* data, int size)
{
  http_response_t* response = (http_response_t*)opaque;
  if (byte_buf_append(response->body, (byte_t*)data, (size_t)size) == false)
  {
    // OOM or NULL data
    msg_http_error("append data failed\n");
  }
}

static void response_header(void* opaque, const char* ckey, int nkey, const char* cvalue, int nvalue)
{ /* doesn't care about headers */ }

static void response_code(void* opaque, int code)
{
  http_response_t* response = (http_response_t*)opaque;
  response->code = code;
}

static http_funcs_t responseFuncs = {
  response_realloc,
  response_body,
  response_header,
  response_code
};


/**
 * @brief   Read from an HTTP progressive download session.
 * @param   In: hnd             Session handle.
 * @param   Out: response       Output response.
 * @param   In: extra_headers   String containing additional HTTP headers to send. Each line must end with \r\n. "" for no header at all.
 * @param   In: postbuffer      The POST payload
 * @retval  >=0 Success:
 *                HTTP_OK
 *           <0 Error:
 *                HTTP_ERR            Bad input parameter. Or allocation error.
 *                HTTP_ERR_HTTP       Connection error, or unexpected response from the HTTP server.
 *                HTTP_ERR_CLOSED     The connection was closed by the server during the previous http_read(). It is allowed by the protocol.
 */
int http_read(const http_handle_t hnd,
              http_response_t* response,
              const char * const extra_headers,
              byte_buf_t* const post_buffer)
{
  int rc = HTTP_OK;
  int send_bytes = 0;
  char *req_buf = NULL;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (pCtx == NULL)
  {
    rc = HTTP_ERR;
  }
  else
  {
    if (pCtx->connection_is_open == false)
    {
      msg_http_error("The connection is not open.\n");
      rc = HTTP_ERR_CLOSED;
    }
  }
  
  if (rc == HTTP_OK)
  {
    send_bytes = http_req_create(&req_buf, pCtx->query, pCtx->hostname, extra_headers, post_buffer);
    if (send_bytes < 0)
    {
      rc = HTTP_ERR;
    }
  }
      
  if (rc == HTTP_OK) 
  {
    /* Send the HTTP headers. */
    rc = net_send(pCtx->sock, (uint8_t *) req_buf, send_bytes, 0);
    printf("Request: \n%s\n", req_buf);
    msg_http_debug("Send the HTTP headers -- rc/send_bytes (%d/%d).\n", rc, send_bytes);
    if (rc != send_bytes)
    {
      msg_http_error("Header send failed (%d/%d).\n", rc, send_bytes);
      rc = HTTP_ERR_HTTP;
    }
    else
    {
      rc = HTTP_OK;
      /* Send the POST body if applicable. */
      if (post_buffer != NULL)
      {
        rc = net_send(pCtx->sock, post_buffer->data, post_buffer->len, 0);
        msg_http_debug("Send the POST body -- post_buffer (%s) rc/post_buffer->len (%d/%d).\n", post_buffer->data, rc, post_buffer->len);
        if (rc != post_buffer->len)
        {
          msg_http_error("POST body send failed (%d/%d).\n", rc, post_buffer->len);
          rc = HTTP_ERR_HTTP;
        }
        else
        {
          rc = HTTP_OK;
        }
      }
      
      /* Get and parse the server response. */
      msg_http_debug("After sending POST body rc (%d).\n", rc);
      if (rc == HTTP_OK)
      {  
        http_roundtripper_t rt;
        http_parser_init(&rt, responseFuncs, response);

        bool needmore = true;
        uint8_t buffer[HTTP_READ_BUFFER_SIZE];
        while (needmore) {
          const char* data = (char *)buffer;
          int ndata = net_recv(pCtx->sock, buffer, sizeof(buffer), 0);
          if (ndata <= 0) {
            msg_http_error("Error receiving data\n");
            http_parser_free(&rt);
            return HTTP_ERR_HTTP;
          }

          while (needmore && ndata) {
            int read;
            needmore = http_parser_data(&rt, data, ndata, &read);
            ndata -= read;
            data += read;
          }
        }

        if (http_parser_iserror(&rt)) {
          msg_http_error("Error parsing data\n");
          http_parser_free(&rt);
          return HTTP_ERR_HTTP;
        }

        http_parser_free(&rt);
      }
    }
  }

  msg_http_debug("Response: %d\n", response->code);
  if (response->body->data) {
    msg_http_debug("%.*s\n", response->body->len, response->body->data);
  }
  return rc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
