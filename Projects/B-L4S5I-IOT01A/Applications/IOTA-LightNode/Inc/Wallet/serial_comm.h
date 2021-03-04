/**
  ******************************************************************************
  * @file   serial_comm.h
  * @author SRA/Central LAB
  * @brief  Header file for serial_comm.c
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

/** @addtogroup IOTA_LightNode
 * @{
 */

/** @defgroup IOTA_SERIAL IOTA Serial utility
 * @{
 */

/** @defgroup IOTA_SERIAL_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */
void serial_press_any(void);

bool serial_get_yes_no(char* question);
bool serial_get_choice(char* question, char a, char b);

int32_t serial_get_int(char* question);
int32_t serial_get_int_single(char* question);
int32_t serial_get_int_fixed(char* question, size_t digits);

int32_t serial_get_int_min(char* question, int32_t min);
int32_t serial_get_int_max(char* question, int32_t max);
int32_t serial_get_int_min_max(char* question, int32_t min, int32_t max);

void serial_get_ascii(char* question, char* destination, size_t max_size);

void serial_get_message_ascii(char* msg_ascii);
void serial_get_address_parameters(int8_t* sec, uint32_t* idx);

void serial_get_options(bool* options);

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

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_COMM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
