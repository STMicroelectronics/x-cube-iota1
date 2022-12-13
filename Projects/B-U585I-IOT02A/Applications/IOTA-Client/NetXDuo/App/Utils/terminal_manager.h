/**
  ******************************************************************************
  * @file   terminal_manager.h
  * @author SRA/Central LAB
  * @brief  Header file for terminal_manager.c
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
 
/** @addtogroup B-U585I-IOT02A
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
/**
 * @brief  Clear the screen.
 * @param  None
 * @retval None
 */
void terminal_cls             (void);
/**
 * @brief  Clear the screen (n lines).
 * @param  n Number of lines to clear
 * @retval None
 */
void terminal_clear_lines     (int32_t n);
/**
 * @brief  Move the cursor.
 * @param  x X-axis
 * @param  y Y-axis
 * @retval None
 */
void terminal_go_to           (int32_t x, int32_t y);
/**
 * @brief  Move the cursor and clear the screen.
 * @param  x X-axis
 * @param  y Y-axis
 * @retval None
 */
void terminal_go_and_clear    (int32_t x, int32_t y);
/**
 * @brief  Save the.
 * @param  None
 * @retval None
 */
void terminal_save_cursor     (void);
/**
 * @brief  Go back to the saved cursor position.
 * @param  None
 * @retval None
 */
void terminal_delete_back     (void);
/**
 * @brief  Set screen color.
 * @param  c color code
 * @retval None
 */
void terminal_set_color       (color_t c);
/**
 * @brief  Reset screen color.
 * @param  None
 * @retval None
 */
void terminal_reset_color     (void);
/**
 * @brief  Print framed text [line; text; line].
 * @param  text Text
 * @param  a Delimiter
 * @param  b Border
 * @param  tb Line
 * @param  width Frame width
 * @param  col Colore code
 * @retval None
 */
void terminal_print_frame     (const char* text, char a, char b, char tb, size_t width, 
                               color_t col);

/**
 * @brief  Print a centered text.
 * @param  text Text to be printed
 * @param  border Border
 * @param  width Text width
 * @retval None
 */
void terminal_print_centered  (const char* text, char b, size_t width);
/**
 * @brief  Print line.
 * @param  del Delimiter
 * @param  l Line
 * @param  width Width
 * @retval None
 */
void terminal_print_line      (char del, char line, size_t width);
/**
 * @brief  Move the cursor to the given position and print the delimiter.
 * @param  del Delimiter
 * @param  pos Position
 * @retval None
 */
void terminal_print_delimiter (char del, size_t pos);
/**
 * @brief  Print a delimited text.
 * @param  text Text to be printed
 * @param  del Delimiter
 * @param  width Width
 * @retval None
 */
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
