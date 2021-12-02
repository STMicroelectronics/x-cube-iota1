/**
  ******************************************************************************
  * @file    net_conf.h
  * @author  MCD Application Team
  * @brief   This file provides the configuration for net
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef NET_CONF_H
#define NET_CONF_H

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

/** @defgroup NET_Config Net Config
 * @{
 */

/** @defgroup NET_Config_EXPORTED_DEFINES Exported Defines
 * @{
 */
#define NET_MBEDTLS_HOST_SUPPORT

/**
 * @}
 */

#include "net_conf_template.h"

/** @defgroup NET_Config_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/**
  * @brief  probe function to register wifi to connectivity framwotk
  * @param  ll_drv_obj
  * @retval 0 on success; -1 otherwise
  */
int32_t wifi_probe(void **ll_drv_obj);

/**
  * @brief  Interrupt handler for  Data RDY signal
  * @param  None
  * @retval None
  */
void    SPI_WIFI_ISR(void);

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

#endif /* NET_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
