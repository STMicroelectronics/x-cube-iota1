/**
  ******************************************************************************
  * @file    net_conf_template.h
  * @author  MCD Application Team
  * @brief   Configures the network socket APIs.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NET_CONF_TEMPLATE_H
#define NET_CONF_TEMPLATE_H


#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

/* Please comment if MBEDTLS on host side is not used */
#define NET_MBEDTLS_HOST_SUPPORT

/* Please uncomment if device supports Secure TCP connection */
/* #define NET_MBEDTLS_DEVICE_SUPPORT */

#ifdef NET_USE_RTOS
#include "cmsis_os.h"
#define NET_CALLOC net_calloc
#define NET_MALLOC pvPortMalloc
#define NET_FREE   vPortFree
#else
#define NET_MALLOC malloc
#define NET_CALLOC calloc
#define NET_FREE  free
#endif /* NET_USE_RTOS */


/* MbedTLS configuration */
#ifdef NET_MBEDTLS_HOST_SUPPORT
#define NET_MBEDTLS_DEBUG_LEVEL 1

#if !defined(MBEDTLS_CONFIG_FILE)
#define MBEDTLS_CONFIG_FILE "mbedtls/config.h"
#endif /* MBEDTLS_CONFIG_FILE */
#endif /* NET_MBEDTLS_HOST_SUPPORT */


#define NET_MAX_SOCKETS_NBR            5

#define NET_IF_NAME_LEN                128
#define NET_DEVICE_NAME_LEN            64
#define NET_DEVICE_ID_LEN              64
#define NET_DEVICE_VER_LEN             64


#define NET_SOCK_DEFAULT_RECEIVE_TO    60000
#define NET_SOCK_DEFAULT_SEND_TO       60000
#define NET_UDP_MAX_SEND_BLOCK_TO      1024
#define NET_USE_DEFAULT_INTERFACE      1

#ifdef  ENABLE_NET_DBG_INFO
#define NET_DBG_INFO(...)  do { \
                                (void) printf(__VA_ARGS__); \
                              } while (0)
#else
#define NET_DBG_INFO(...)
#endif /* ENABLE_NET_DBG_INFO */

#define NET_DBG_ERROR(...)  do { \
                                 (void) printf("\nERROR: %s:%d ",__FILE__,__LINE__) ;\
                                 (void)printf(__VA_ARGS__);\
                                 (void)printf("\n"); \
                               } while (false)

#define NET_DBG_PRINT(...)  do { \
                                 (void)printf("%s:%d ",__FILE__,__LINE__) ;\
                                 (void)printf(__VA_ARGS__);\
                                 (void)printf("\n"); \
                               } while (false)

#define NET_ASSERT(test,s)  do { if (!(test)) {\
                                 (void) printf("Assert Failed %s %d : %s\n",__FILE__,__LINE__,(s)); \
                                   while(true) {}; }\
                               } while (false)

#define NET_PRINT(...)  do { \
                                 (void) printf(__VA_ARGS__);\
                                 (void) printf("\n"); \
                               } while (false)

#define NET_PRINT_WO_CR(...)   do { \
                                    (void) printf(__VA_ARGS__);\
                                  } while (false)

#define NET_WARNING(...)  do { \
                                 (void) printf("Warning %s:%d ",__FILE__,__LINE__) ;\
                                 (void) printf(__VA_ARGS__);\
                                 (void) printf("\n"); \
                               } while (false)

#ifdef __cplusplus
}
#endif

#endif /* NET_CONF_TEMPLATE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
