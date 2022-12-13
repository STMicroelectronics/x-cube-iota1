/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.h
  * @author  MCD Application Team
  * @brief   NetXDuo applicative header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "main.h"
#include "nxd_dns.h"
#include "nxd_dhcp_client.h"
#include "nxd_sntp_client.h"
#include "nx_web_http_client.h"
#include "nx_driver_emw3080.h" 
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define SNTP_SERVER_NAME             "time1.google.com"
#define SNTP_UPDATE_EVENT            1
/* Define how often the demo checks for SNTP updates. */
#define PERIODIC_CHECK_INTERVAL      (60 * NX_IP_PERIODIC_RATE)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define PRINT_IP_ADDRESS(addr)             do { \
                                                printf("STM32 %s: %lu.%lu.%lu.%lu \n", #addr, \
                                                (addr >> 24) & 0xff, \
                                                (addr >> 16) & 0xff, \
                                                (addr >> 8) & 0xff, \
                                                addr& 0xff);\
                                           }while(0)

#define PRINT_DATA(addr, port, data)       do { \
                                                printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \n", \
                                                (addr >> 24) & 0xff, \
                                                (addr >> 16) & 0xff, \
                                                (addr >> 8) & 0xff,  \
                                                (addr & 0xff), port, data); \
                                           } while(0)

#define PRINT_CNX_SUCC()                   do { \
                                                printf("SNTP client connected to NTP server : < %s > \n", SNTP_SERVER_NAME);\
                                           } while(0)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_NetXDuo_Init(VOID *memory_ptr);

/* USER CODE BEGIN EFP */
VOID NetXDuo_InitHooks(VOID);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PAYLOAD_SIZE             2360
#define NX_PACKET_POOL_SIZE      (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 32)
#define WINDOW_SIZE              512

#define DEFAULT_MEMORY_SIZE      1024
#define DEFAULT_MAIN_PRIORITY    10
#define DEFAULT_PRIORITY         5
#define THREAD_MEMORY_SIZE       15 * DEFAULT_MEMORY_SIZE

#define NULL_ADDRESS             0

#define DEFAULT_PORT             6000
#define TCP_SERVER_PORT          6001
#define TCP_SERVER_ADDRESS       IP_ADDRESS(192, 168, 1, 1)

#define MAX_PACKET_COUNT         100
#define DEFAULT_MESSAGE          "TCP Client on STM32U5-IOT"
#define DEFAULT_TIMEOUT          10 * NX_IP_PERIODIC_RATE
/* USER CODE END PD */

/* USER CODE BEGIN 1 */
#define USER_DNS_ADDRESS         IP_ADDRESS(1, 1, 1, 1)   /* User should configure it with his DNS address */
/* EPOCH_TIME_DIFF is equivalent to 70 years in sec
   calculated with www.epochconverter.com/date-difference
   This constant is used to delete difference between :
   Epoch converter (referenced to 1970) and SNTP (referenced to 1900) */
#define EPOCH_TIME_DIFF             2208988800
/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_NETXDUO_H__ */
