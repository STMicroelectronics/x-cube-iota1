/**
  ******************************************************************************
  * @file   http_lib.h
  * @author SRA/Central LAB
  * @brief  Helper functions for building HTTP GET and POST requests, and
  *         stream reading.
  *         Header for http_lib.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HTTP_LIB_H
#define HTTP_LIB_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "core/utils/byte_buffer.h"

#include "http.h"

/** @defgroup IOTA_C IOTA_C
 * @{
 */

/** @defgroup CLIENT Client
 * @{
 */

/** @defgroup NETWORK Network
 * @{
 */

/** @defgroup NETWORK_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants --------------------------------------------------------*/
#define HTTP_OK                0
#define HTTP_ERR               -1
#define HTTP_ERR_OPEN          -2  /**< HTTP OPEN error */
#define HTTP_ERR_CONNECT       -3  /**< HTTP CONNECT error */
#define HTTP_ERR_SEND          -4  /**< HTTP SEND error */
#define HTTP_ERR_RECV          -5  /**< HTTP RECV error */
#define HTTP_ERR_PARSE         -6  /**< HTTP PARSE error */
#define HTTP_ERR_CLOSED        -7  /**< The HTTP connection was closed by the server. */

/**
 * @}
 */

/** @defgroup NETWORK_EXPORTED_TYPES Exported Types
 * @{
 */

/* Exported types ------------------------------------------------------------*/
/**
 * @brief HTTP progressive download internal session context.
 */
typedef struct {
  int32_t sock;                 /**< Network socket handle. */
  char const* host;             /**< Domain name or IP as string */
  char const* path;             /**< HTTP path */
  char const* query;            /**< HTTP query */
  uint16_t port;                /**< port to connect */
  bool use_tls;                 /**< Use TLS or not */
  bool connection_is_open;      /**< HTTP keep-alive connection status. */
} http_context_t;

typedef enum {
  HTTP_PROTO_NONE = 0,
  HTTP_PROTO_HTTP,
  HTTP_PROTO_HTTPS
} http_proto_t;

typedef struct
{
  byte_buf_t* body;
  int code;
} http_response_t;

/**
 * @}
 */

/** @defgroup NETWORK_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions --------------------------------------------------------*/
int http_open(http_context_t * const pCtx);
int http_close(http_context_t * const pCtx);

int http_read(http_context_t * const pCtx,
              http_response_t* response,
              const char * const extra_headers,
              byte_buf_t* const post_buffer);

int http_req_create(char *req_buf,
                    const char * const path,
                    const char * const hostname,
                    const char * const extra_headers,
                    byte_buf_t* const post_buffer);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* HTTP_LIB_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
