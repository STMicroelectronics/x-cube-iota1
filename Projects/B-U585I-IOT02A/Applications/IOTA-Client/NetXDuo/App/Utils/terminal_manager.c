/**
  ******************************************************************************
  * @file   terminal_manager.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for the serial terminal 
  *         window management
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

/* Includes ------------------------------------------------------------------*/
#include "terminal_manager.h"

#include <stdio.h>
#include <stdarg.h>
#include "iota_conf.h"

/* Function definition -------------------------------------------------------*/
// Working with the terminal

void terminal_cls(void)
{
  printf("%c[2J", 27);
  terminal_go_to(0,0);
}

void terminal_clear_lines(int32_t n)
{
  // Does not delete the current line
  for(int32_t i = 0; i < n; i++) {
    printf("%c[F\033[K", 27);
  }
}

void terminal_go_to(int32_t x, int32_t y)
{
#ifdef __ARMCC_VERSION
  printf("%c[%d;%dH", 27, x, y);
#else
  printf("%c[%ld;%ldH", 27, x, y);
#endif /* __ARMCC_VERSION */
}

void terminal_go_and_clear(int32_t x, int32_t y)
{
#ifdef __ARMCC_VERSION
  printf("%c[%d;%dH%c[0J", 27, x, y, 27);
#else
  printf("%c[%ld;%ldH%c[0J", 27, x, y, 27);
#endif /* __ARMCC_VERSION */
}

void terminal_save_cursor(void)
{
  printf("%c[s", 27);
}

void terminal_delete_back(void)
{
  // Go back to the saved cursor position
  printf("%c[u", 27);
  // Delete from here to the end
  printf("%c[0J", 27);
}

void terminal_set_color(color_t c)
{
  printf("%c[%dm", 27, c);
}

void terminal_reset_color(void)
{
  printf("%c[0m", 27);
}

// Various print functions

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

void terminal_print_centered(const char* text, char border, size_t width)
{
  // Text size
  size_t b = strlen(text);
  // Left empty space
  size_t a = (width - b) / 2;
  // Right empty space
  size_t c = width - a - b;
  
  // [b]<empty>[text]<empty>[b]
  printf("%c%*.s%s%*.s%c\r\n", border, a, "", text, c, "", border);
}

void terminal_print_delimiter(char del, size_t pos)
{
  // Move the cursor to the given position and print the delimiter
  printf("%c[%dG%c\r\n", 27, pos, del);
}

void terminal_print_delimited(const char* text, char del, size_t width)
{
  printf("%c %s %c[%dG%c\r\n", del, text, 27, width, del);
}

void terminal_print_line(char del, char l, size_t width)
{
  char line[width];
  (void)memset(line, l, width);
  
  printf("%c%.*s%c\r\n", del, width, line, del);
}
