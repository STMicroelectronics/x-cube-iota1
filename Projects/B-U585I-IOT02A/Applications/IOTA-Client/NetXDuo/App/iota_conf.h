/**
  ******************************************************************************
  * @file   iota_api_test.h
  * @author SRA/Central LAB
  * @brief  IOTA configuration file
  * 
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
  
#ifndef IOTA_CONF_H
#define IOTA_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup B-U585I-IOT02A
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @defgroup IOTA_CONF IOTA Configuration
 * @{
 */

#include "app_netxduo.h"

/** @defgroup IOTA_CONF_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants---------------------------------------------------------*/
#define FW_VERSION_MAJOR 3
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "GG-MM-YYYY HH:MM:SS PM"

#define FW_VERSION_NAME "X-CUBE-IOTA1"

#define LINE_LENGTH   27

// enable or disable test cases use the Tangle network.
// We don't enable it by default but enable it for a local test is recommended.
#define TEST_TANGLE_ENABLE 1

#define USE_DEVNET

#ifdef USE_DEVNET
/* Devnet node */
#define NODE_HOST "api.lb-0.h.chrysalis-devnet.iota.cafe"
#define NODE_PORT NX_WEB_HTTP_SERVER_PORT
#define NODE_USE_TLS false

#else

/* Mainnet node */
#define NODE_HOST "chrysalis-nodes.iota.org"
#define NODE_PORT NX_WEB_HTTPS_SERVER_PORT
#define NODE_USE_TLS true

#endif /* USE_DEVNET */

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

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* IOTA_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
