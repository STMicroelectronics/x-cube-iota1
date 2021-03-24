/**
  ******************************************************************************
  * @file   terminal_manager.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for the serial terminal 
  *         window management
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

/* Includes ------------------------------------------------------------------*/
#include "terminal_manager.h"

#include <stdio.h>
#include <stdarg.h>
#include "iota_conf.h"

/* Function definition -------------------------------------------------------*/
// Working with the terminal

/**
 * @brief  Clear the screen.
 * @param  None
 * @retval None
 */
void terminal_cls(void)
{
  iota_printf("%c[2J", 27);
  terminal_go_to(0,0);
}

/**
 * @brief  Clear the screen (n lines).
 * @param  n Number of lines to clear
 * @retval None
 */
void terminal_clear_lines(int32_t n)
{
  // Does not delete the current line
  for(int32_t i = 0; i < n; i++) {
    iota_printf("%c[F\033[K", 27);
  }
}

/**
 * @brief  Move the cursor.
 * @param  x X-axis
 * @param  y Y-axis
 * @retval None
 */
void terminal_go_to(int32_t x, int32_t y)
{
  iota_printf("%c[%ld;%ldH", 27, x, y);
}

/**
 * @brief  Move the cursor and clear the screen.
 * @param  x X-axis
 * @param  y Y-axis
 * @retval None
 */
void terminal_go_and_clear(int32_t x, int32_t y)
{
  iota_printf("%c[%ld;%ldH%c[0J", 27, x, y, 27);
}

/**
 * @brief  Save the.
 * @param  None
 * @retval None
 */
void terminal_save_cursor(void)
{
  iota_printf("%c[s", 27);
}

/**
 * @brief  Go back to the saved cursor position.
 * @param  None
 * @retval None
 */
void terminal_delete_back(void)
{
  // Go back to the saved cursor position
  iota_printf("%c[u", 27);
  // Delete from here to the end
  iota_printf("%c[0J", 27);
}

/**
 * @brief  Set screen color.
 * @param  c color code
 * @retval None
 */
void terminal_set_color(color_t c)
{
  iota_printf("%c[%dm", 27, c);
}

/**
 * @brief  Reset screen color.
 * @param  None
 * @retval None
 */
void terminal_reset_color(void)
{
  iota_printf("%c[0m", 27);
}

// Various print functions
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
void terminal_print_frame(const char* text, char a, char b, char tb, size_t width, 
                          color_t col)
{
  // Set the chosen color
  terminal_set_color(col);
  
  // Print framed text [line; text; line]
  terminal_print_line(a, tb, width);
  terminal_print_centered(text, b, width);
  terminal_print_line(a, tb, width);
  
  // Reset color
  terminal_reset_color();
}

/**
 * @brief  Print a centered text.
 * @param  text Text to be printed
 * @param  border Border
 * @param  width Text width
 * @retval None
 */
void terminal_print_centered(const char* text, char border, size_t width)
{
  // Text size
  size_t b = strlen(text);
  // Left empty space
  size_t a = (width - b) / 2;
  // Right empty space
  size_t c = width - a - b;
  
  // [b]<empty>[text]<empty>[b]
  iota_printf("%c%*.s%s%*.s%c\r\n", border, a, "", text, c, "", border);
}

/**
 * @brief  Move the cursor to the given position and print the delimiter.
 * @param  del Delimiter
 * @param  pos Position
 * @retval None
 */
void terminal_print_delimiter(char del, size_t pos)
{
  // Move the cursor to the given position and print the delimiter
  iota_printf("%c[%dG%c\r\n", 27, pos, del);
}

/**
 * @brief  Print a delimited text.
 * @param  text Text to be printed
 * @param  del Delimiter
 * @param  width Width
 * @retval None
 */
void terminal_print_delimited(const char* text, char del, size_t width)
{
  iota_printf("%c %s %c[%dG%c\r\n", del, text, 27, width, del);
}

/**
 * @brief  Print line.
 * @param  del Delimiter
 * @param  l Line
 * @param  width Width
 * @retval None
 */
void terminal_print_line(char del, char l, size_t width)
{
  char line[width];
  (void)memset(line, l, width);
  
  iota_printf("%c%.*s%c\r\n", del, width, line, del);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
