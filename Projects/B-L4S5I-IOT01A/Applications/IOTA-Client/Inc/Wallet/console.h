/**
  ******************************************************************************
  * @file   console.h
  * @author SRA/Central LAB
  * @brief  Header file for console.c
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

#ifndef CONSOLE_H
#define CONSOLE_H

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

/** @defgroup IOTA_CONSOLE IOTA Console
 * @{
 */

/** @defgroup IOTA_CONSOLE_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */
int32_t uartReceiveString(char* str, size_t length);

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

#endif /* CONSOLE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
