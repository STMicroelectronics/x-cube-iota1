/**
  ******************************************************************************
  * @file   http_lib.c
  * @author SRA/Central LAB
  * @brief  Helper functions for building HTTP GET and POST requests, and
  *         stream reading.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under SLA0055, the "License";
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at www.st.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "http_lib.h"
#include "main.h"
#include "msg.h"
#include "net_connect.h"

/* Private defines -----------------------------------------------------------*/
#define REPORT_TIMEPERIOD_MS    10000

#define HTTP_READ_BUFFER_SIZE   1024    /**< Size of the HTTP receive buffer. */
#define HTTP_HEADER             "HTTP/1.1"

/* Private typedef -----------------------------------------------------------*/

/* Private variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int http_connect(http_context_t *http_context, sockaddr_in_t *addr);

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief   Create an HTTP request string from the parameter list.
 * @note    The request buffer is allocated by the callee and must be released 
 *          by calling http_req_destroy() after usage.
 * @param   Out:  req_buf         Output buffer, allocated by the callee.
 * @param   In:   path            HTTP path string.
 * @param   In:   host        Remote host.
 * @param   In:   extra_headers   HTTP headers to be appended to the request header. Single buffer. Each line must be ended by \r\n.
 * @param   In:   post_buffer     The document body to be sent, in case of a POST request. Must be NULL for a GET request.
 * @retval  Error code
 *            >0              Success, length of the request string written to req_buf.
 *            HTTP_ERR (<0)   Failure
 */
int http_req_create(char *req_buf,
                    const char * const path,
                    const char * const host,
                    const char * const extra_headers,
                    byte_buf_t* const post_buffer)
{
  int rc = HTTP_OK;
    
  size_t req_buf_len = strlen(path) + strlen(host);
  msg_http_debug("After path+host -- req_buf_len = %d\n", req_buf_len);
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

  if (post_buffer == NULL)
  {
    /* HTTP GET request */
    rc = snprintf(req_buf, req_buf_len,
                  "GET %s %s\r\n"
                  "Host: %s\r\n"
                  "%s\r\n\r\n",
                  path, HTTP_HEADER, host, (extra_headers == NULL) ? "" : extra_headers);
  }
  else
  {
    /* HTTP POST request */
    rc = snprintf(req_buf, req_buf_len,
                  "POST %s %s\r\n"
                  "Host: %s\r\n"
                  "Content-Length: %d\r\n"
                  "%s\r\n\r\n",
                  path, HTTP_HEADER, host, post_buffer->len, (extra_headers == NULL) ? "" : extra_headers);
  }
  if (rc >= req_buf_len)
  {
    msg_http_error("Memory overflow preparing the HTTP request string. %d >= %d\n", rc, req_buf_len);
    rc = HTTP_ERR; /* The allocated destination buffer is too small. */
  }

  return rc; /* Return the string length, or a <0 error code. */
}

static int http_connect(http_context_t *pCtx, sockaddr_in_t *addr)
{
  int ret = 0;

  ret = net_connect( pCtx->sock, (sockaddr_t *)addr, sizeof( sockaddr_in_t ) );

  if (NET_OK != ret)
  {
    msg_http_error("Could not connect socket #%ld.\n", pCtx->sock);
  }
  else
  {
    pCtx->connection_is_open = true;
    return HTTP_OK;
  }

  return HTTP_ERR_CONNECT;
}

/**
 * @brief   Open an HTTP progressive download session.
 * @note    The internal session context is allocated by the callee.
 * @param   In: pCtx    Pointer to the session context.
 * @retval  Error code
 *            HTTP_OK        (0)  Success
 *            HTTP_ERR_OPEN (<0)  Failure
 */
int http_open(http_context_t * const pCtx)
{
  int ret = 0;

  if (pCtx != NULL)
  {
    pCtx->connection_is_open = false;

    sockaddr_in_t addr;
    addr.sin_len = sizeof(sockaddr_in_t);
    if (net_if_gethostbyname(NULL,(sockaddr_t *)&addr, (char_t*) pCtx->host) < 0)
    {
      msg_http_error("Could not find host ipaddr (%s)\n", pCtx->host);
      return HTTP_ERR_OPEN;
    }
    addr.sin_port = NET_HTONS(pCtx->port);

    pCtx->sock = net_socket(NET_AF_INET, NET_SOCK_STREAM, NET_IPPROTO_TCP);
    if (pCtx->sock < 0)
    {
      msg_http_error("Could not create a socket.\n");
      return HTTP_ERR_OPEN;
    }
    else
    {
      //msg_http_debug("Socket #%d allocated.\n", pCtx->sock);
      msg_http_debug("http_open pCtx->host=%s\n", pCtx->host);
      if (pCtx->use_tls == true)
      {
#define NET_READ_TIMEOUT 30000
        uint32_t timeout = NET_READ_TIMEOUT;
        bool false_val = false;
        //bool true_val = true;
        ret  = net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_SECURE, NULL, 0);
        ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_RCVTIMEO, (void *) &timeout, sizeof(uint32_t));
        ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_CA_CERT, (void *)lUserConfigPtr->tls_root_ca_cert, strlen(lUserConfigPtr->tls_root_ca_cert) + 1);
        ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_NAME, (void *)pCtx->host, strlen(pCtx->host) + 1);
        //ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_VERIFICATION, (void *) &true_val, sizeof(bool));
        ret |= net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_TLS_SERVER_VERIFICATION, (void *) &false_val, sizeof(bool));
      }
    }

    if (NET_OK != ret)
    {
      msg_http_error("Could not set the socket options (ret=%d).\n", ret);
    }
    else
    {
      ret = http_connect(pCtx, &addr);
    }

    if (NET_OK != ret)
    {
      msg_http_error("Could not open HTTP session due to previous error...\n");
    }
    else
    {
      //msg_http_debug("Socket #%d opened.\n", pCtx->sock);
      return HTTP_OK;
    }

  }
  return HTTP_ERR_OPEN;
}

/**
 * @brief   Close an HTTP progressive download session.
 * @note    The internal session context is freed by the callee.
 * @param   In: pCtx   Session handle.
 * @retval  Error code
 *            HTTP_OK   (0)  Success
 *            HTTP_ERR (<0)  Failure
 */
int http_close(http_context_t * const pCtx)
{
  int rc = HTTP_ERR;

  if (pCtx != NULL)
  {
    int ret = 0;
    ret = net_closesocket(pCtx->sock);
    pCtx->connection_is_open = false;

    if (ret == NET_OK)
    {
      //msg_http_debug("Socket #%d closed.\n", pCtx->sock);
      rc = HTTP_OK;
    }
    else
    {
      msg_http_error("Could not close and destroy the socket #%ld.\n", pCtx->sock);
    }
  }

  return rc;
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
  response_body,
  response_header,
  response_code
};


/**
 * @brief   Read from an HTTP progressive download session.
 * @param   In: pCtx            Session handle.
 * @param   Out: response       Output response.
 * @param   In: extra_headers   String containing additional HTTP headers to send. Each line must end with \r\n. "" for no header at all.
 * @param   In: postbuffer      The POST payload
 * @retval  >=0 Success:
 *                HTTP_OK
 *           <0 Error:
 *                HTTP_ERR            Bad input parameter. Or allocation error.
 *                HTTP_ERR_CLOSED     The connection was closed by the server during the previous http_read(). It is allowed by the protocol.
 *                HTTP_ERR_SEND       Error sending request.
 *                HTTP_ERR_RECV       Error receiving response.
 *                HTTP_ERR_PARSE      Error parsing response.
 */
int http_read(http_context_t * const pCtx,
              http_response_t* response,
              const char * const extra_headers,
              byte_buf_t* const post_buffer)
{
  int rc = HTTP_OK;
  int send_bytes = 0;
  char req_buf[512];
  memset(req_buf, 0, sizeof(req_buf));

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
    send_bytes = http_req_create(req_buf, pCtx->path, pCtx->host, extra_headers, post_buffer);
    if (send_bytes < 0)
    {
      rc = HTTP_ERR;
    }
  }
      
  if (rc == HTTP_OK) 
  {
    /* Send the HTTP headers. */
    rc = net_send(pCtx->sock, (uint8_t *) req_buf, send_bytes, 0);
    msg_http_debug("Request (len=%d): \n%s\n", strlen(req_buf), req_buf);
    msg_http_debug("Send the HTTP headers -- rc/send_bytes (%d/%d).\n", rc, send_bytes);
    if (rc != send_bytes)
    {
      msg_http_error("Header send failed (%d/%d).\n", rc, send_bytes);
      rc = HTTP_ERR_SEND;
    }
    else
    {
      rc = HTTP_OK;
      /* Send the POST body if applicable. */
      if (post_buffer != NULL)
      {
        int offset = 0;
        int size_left = post_buffer->len;
        while(offset < post_buffer->len)
        {
          rc = net_send(pCtx->sock, post_buffer->data+offset, size_left, 0);
          if (rc < 0)
          {
            msg_http_error("POST body send failed (%d/%d).\n", rc, post_buffer->len);
            rc = HTTP_ERR_SEND;
            break;
          }
          offset += rc;
          size_left -= rc;

          rc = HTTP_OK;
        }
      }

      /* Get and parse the server response. */
      msg_http_debug("After sending POST body rc (%d).\n", rc);
#ifdef NET_PERF
      uint32_t elapsed_time;
      uint32_t start_time;
      start_time = NET_TICK();
#endif /* NET_PERF */
      if (rc == HTTP_OK)
      {
        http_roundtripper_t rt;
        http_parser_init(&rt, responseFuncs, response);

        bool needmore = true;
        uint8_t buffer[HTTP_READ_BUFFER_SIZE];
        while (needmore) {
          const char* data = (char *)buffer;
          int ndata = net_recv(pCtx->sock, buffer, sizeof(buffer), 0);
          if (ndata < 0) {
            msg_http_error("Error receiving data (ret=%d)\n", ndata);
            http_parser_reset(&rt);
            return HTTP_ERR_RECV;
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
          http_parser_reset(&rt);
          return HTTP_ERR_PARSE;
        }

        http_parser_reset(&rt);
      }
#ifdef NET_PERF
      elapsed_time = NET_TICK() - start_time;
      msg_http_debug("Duration recv response %d ms \n", elapsed_time);
#endif /* NET_PERF */
    }
  }

  msg_http_debug("Response: %d\n", response->code);
  if (response->body->data) {
    msg_http_debug("response->body->len=%d\n%.*s\n", response->body->len, response->body->len, response->body->data);
  }
  return rc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
