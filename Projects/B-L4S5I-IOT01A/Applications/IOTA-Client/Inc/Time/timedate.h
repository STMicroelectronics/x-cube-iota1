/**
  ******************************************************************************
  * @file    timedate.h
  * @author  MCD Application Team
  * @brief   Configuration of the RTC.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef TIMEDATE_H
#define TIMEDATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

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

/** @defgroup TIME Time
 * @{
 */

/** @defgroup TIME_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants --------------------------------------------------------*/
#define TD_OK             0
#define TD_ERR_CONNECT   -1   /**< Could not connect to the network and join the web server. */
#define TD_ERR_HTTP      -2   /**< Could not parse the time and date from the web server response. */
#define TD_ERR_RTC       -3   /**< Could not set the RTC. */
#define TD_ERR_TLS_CERT  -4   /**< The server certificate verification failed. Applicable only when force_apply is false. */

/**
 * @}
 */

/** @defgroup TIME_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions --------------------------------------------------------*/
int setRTCTimeDateFromNetwork(bool force_apply);

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

#endif /* TIMEDATE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
