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

/** @addtogroup B-L4S5I-IOT01A
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

/** @defgroup IOTA_CONF_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants---------------------------------------------------------*/
#define FW_VERSION_MAJOR 2
#define FW_VERSION_MINOR 1
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "01-12-2021 12:00:00 PM"

#define FW_VERSION_NAME "X-CUBE-IOTA1"

#define LINE_LENGTH   27

/* Devnet node */
#define DEV_NODE_HOST "api.lb-0.h.chrysalis-devnet.iota.cafe"
#define DEV_NODE_HOST_PORT 443
#define DEV_NODE_USE_TLS true

/* Mainnet node */
#define NODE_HOST "chrysalis-nodes.iota.org"
#define NODE_HOST_PORT 443
#define NODE_USE_TLS true

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
