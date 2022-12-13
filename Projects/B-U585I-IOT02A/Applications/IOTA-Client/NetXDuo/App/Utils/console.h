/**
  ******************************************************************************
  * @file   console.h
  * @author SRA/Central LAB
  * @brief  Header file for console.c
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/** @addtogroup B-U585I-IOT02A
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
/** 
 * @brief  Receives a string from serial port
 * @param  str: destination of the read input
 * @param  length: characters to read (if no EOL is found)
 * @retval Length of the string
*/
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
