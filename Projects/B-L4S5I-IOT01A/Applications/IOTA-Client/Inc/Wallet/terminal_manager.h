/**
  ******************************************************************************
  * @file   terminal_manager.h
  * @author SRA/Central LAB
  * @brief  Header file for terminal_manager.c
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
#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include <string.h>

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

/** @defgroup IOTA_TERMINAL IOTA Terminal
 * @{
 */

/** @defgroup IOTA_TERMINAL_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants ------------------------------------------------------- */
// Window Width
#define WW    90 //165

/**
 * @}
 */

/** @defgroup IOTA_TERMINAL_EXPORTED_TYPES Exported Types
 * @{
 */

/* Exported types ----------------------------------------------------------- */
typedef enum {
  RED     = 31,
  GREEN   = 32,
  YELLOW  = 33,
  BLUE    = 34,
  MAGENTA = 35,
  CYAN    = 36,
  WHITE   = 37	
} color_t;

/**
 * @}
 */

/** @defgroup IOTA_TERMINAL_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */
/* Exported functions ------------------------------------------------------- */
void terminal_cls             (void);
void terminal_clear_lines     (int32_t n);

void terminal_go_to           (int32_t x, int32_t y);
void terminal_go_and_clear    (int32_t x, int32_t y);
void terminal_save_cursor     (void);
void terminal_delete_back     (void);

void terminal_set_color       (color_t c);
void terminal_reset_color     (void);

void terminal_print_frame     (const char* text, char a, char b, char tb, size_t width, 
                               color_t col);
void terminal_print_centered  (const char* text, char b, size_t width);

void terminal_print_line      (char del, char line, size_t width);
void terminal_print_delimiter (char del, size_t pos);
void terminal_print_delimited (const char* text, char del, size_t width);

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

#endif /* TERMINAL_MANAGER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
