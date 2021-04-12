/**
  ******************************************************************************
  * @file   iota_api_test.h
  * @author SRA/Central LAB
  * @brief  IOTA configuration file
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "DD-MM-2021 12:00:00 PM"

#define FW_VERSION_NAME "X-CUBE-IOTA1"

#include <stdio.h>
#define iota_printf (void)printf
#define LINE_LENGTH   27

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
