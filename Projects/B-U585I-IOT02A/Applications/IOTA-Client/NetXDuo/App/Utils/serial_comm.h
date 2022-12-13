/**
  ******************************************************************************
  * @file   serial_comm.h
  * @author SRA/Central LAB
  * @brief  Header file for serial_comm.c
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

#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup B-U585I-IOT02A
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @defgroup IOTA_SERIAL IOTA Serial
 * @{
 */

/** @defgroup IOTA_SERIAL_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */
/**
 * @brief  Wait for any input from the user.
 * @param  None
 * @retval None
 */
void serial_press_any(void);
/**
 * @brief  Wait for a yes/no answer from the user.
 * @param  question The question string
 * @retval True if answer is yes, false otherwise
 */
bool serial_get_yes_no(char* question);
/**
 * @brief  Wait for a choice from the user.
 * @param  question The question string
 * @param  a The first choice
 * @param  b The second choice
 * @retval True if answer is the first choice, false otherwise
 */
bool serial_get_choice(char* question, char a, char b);
/**
 * @brief  Read an integer from the user input.
 * @param  question The question string
 * @retval The integer
 */
int32_t serial_get_int(char* question);
/**
 * @brief  Read a single digit integer from the user input.
 *         Equivalent to serial_get_int_fixed(question, 1);
 * @param  question The question string
 * @retval The single integer
 */
int32_t serial_get_int_single(char* question);
/**
 * @brief  Read a fixed digit integer from the user input.
 * @param  question The question string
 * @param  digits Number of digits
 * @retval The integer
 */
int32_t serial_get_int_fixed(char* question, size_t digits);
/**
 * @brief  Read an integer from the user input greater than min.
 * @param  question The question string
 * @param  min The minimum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_min(char* question, int32_t min);
/**
 * @brief  Read an integer from the user input lower than max.
 * @param  question The question string
 * @param  max The maximum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_max(char* question, int32_t max);
/**
 * @brief  Read an integer from the user input between min and max.
 * @param  question The question string
 * @param  min The minimum integer to be entered
 * @param  max The maximum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_min_max(char* question, int32_t min, int32_t max);
/**
 * @brief  Read an ascii string from the user input.
 * @param  question The question string
 * @param  destination The destination buffer
 * @param  max_length The maximum string length
 * @retval None
 */
void serial_get_ascii(char* question, char* destination, size_t max_size);
/**
 * @brief  Read an ascii message from the user input.
 * @param  msg_ascii The ascii buffer
 * @retval None
 */
void serial_get_message_ascii(char* msg_ascii);
/**
 * @brief  Get the pow and broadcast options.
 * @param  options The options
 * @retval None
 */
void serial_get_options(bool* options);
/**
 * @brief  Read the Full Node URL.
 * @param  node_url The url buffer
 * @retval None
 */
void serial_get_full_node_url(char* node_url);

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

#endif /* SERIAL_COMM_H */
