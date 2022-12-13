/**
  ******************************************************************************
  * @file   serial_comm.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for the serial terminal 
  *         input/output management
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
#include "serial_comm.h"

#include "main.h"
#include "console.h"

#include "iota_conf.h"

#include <ctype.h>

#include "terminal_manager.h"

/* Defines -------------------------------------------------------------------*/
#define BUFFER_LENGTH 1500

/* Global variables ----------------------------------------------------------*/
char    ans_buffer[BUFFER_LENGTH];

/* Function definition -------------------------------------------------------*/

void serial_press_any(void)
{
  printf("\r\nPress any key to continue.\r\n");
  (void)getchar();
  printf("\r\n\n");
  terminal_clear_lines(1);
}

bool serial_get_yes_no(char* question)
{
  char c;
  printf("\r\n");
  do {
    terminal_clear_lines(1);
    printf("%s ", question);
    (void)fflush(stdout);
    c = getchar();
    printf("\r\n");
  } while (!((c == 'y') || (c == 'Y') || (c == 'n') || (c == 'N')));
  
  if ((c == 'y') || (c == 'Y')) {
    return true;
  } 
  else {
    return false;
  }
}

bool serial_get_choice(char* question, char a, char b)
{
  // To make sure
  a = tolower(a);
  b = tolower(b);
  
  printf("\r\n");
  char c;
  do {
    terminal_clear_lines(1);
    printf("%s ", question);
    (void)fflush(stdout);
    c = getchar();//uartReceiveChar();
    printf("\r\n");
    
    c = tolower(c);
  } while (!((c == a) || (c == b)));
  
  if (c == a) {
    return true;
  } 
  else {
    return false;
  }
}

//------------------------------------------------------------------------------
// INTEGERS

int32_t serial_get_int(char* question)
{
  int ret;
  char *pEnd;
  printf("%s ", question);
  (void)fflush(stdout);
  ret = uartReceiveString(ans_buffer, BUFFER_LENGTH);
  printf("\r\n");
  if (ret == 0) {
    return -1;
  }

  return strtol(ans_buffer, &pEnd, 10);
}

int32_t serial_get_int_single(char* question)
{
  char c;
  int32_t ret = 0;
	
  printf("%s ", question);
  (void)fflush(stdout);
  c = getchar();//uartReceiveChar();
  printf("\r\n");
		
  switch (c) {
  case '0':
    ret = 0;
    break;
  case '1':
    ret = 1;
    break;
  case '2':
    ret = 2;
    break;
  case '3':
    ret = 3;
    break;
  case '4':
    ret = 4;
    break;
  case '5':
    ret = 5;
    break;
  case '6':
    ret = 6;
    break;
  case '7':
    ret = 7;
    break;
  case '8':
    ret = 8;
    break;
  case '9':
    ret = 9;
    break;
  default:
    break;
  }
	
  return ret;
}

int32_t serial_get_int_fixed(char* question, size_t digits)
{
  char *pEnd;
  if (digits > BUFFER_LENGTH) {
    digits = (BUFFER_LENGTH - 1);
  }
  printf("%s ", question);
  (void)fflush(stdout);
  (void)uartReceiveString(ans_buffer, digits);
  printf("\r\n");
  
  return strtol(ans_buffer, &pEnd, 10);
}

int32_t serial_get_int_min(char* question, int32_t min)
{
  int32_t n = serial_get_int(question);
  while (n < min) {
    printf("Too low.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

int32_t serial_get_int_max(char* question, int32_t max)
{
  int32_t n = serial_get_int(question);
  while (n > max) {
    printf("Too high.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

int32_t serial_get_int_min_max(char* question, int32_t min, int32_t max) 
{
  int32_t n = serial_get_int(question);
  while ((n < min) || (n > max)) {
    printf("Out of range.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

//-------------------------------------------------------------------------------------------------

void serial_get_full_node_url(char* node_url)
{
  // Get the node_url
  do {
    printf("Enter the full node URL:\r\n");
    (void)uartReceiveString(node_url, 50);
    printf("\r\n");

    printf("You entered: \"%s\"\r\n", node_url);
  } while (!serial_get_yes_no("Do you want to use this URL? (Y/N)"));
}

//--------------------------------------------------------------------------------------------------

void serial_get_ascii(char* question, char* destination, size_t max_length)
{	
  if (max_length > BUFFER_LENGTH - 1) {
    max_length = BUFFER_LENGTH - 1;
  }
  printf("%s ", question);
  (void)fflush(stdout);
  (void)uartReceiveString(ans_buffer, max_length);
  printf("\r\n");
  
  if (max_length < strlen(ans_buffer)) {
    ans_buffer[max_length] = '\0';
  }
  (void)strcpy(destination, ans_buffer);
}

void serial_get_message_ascii(char* msg_ascii)
{		
  printf("\r\n\r\n\r\n\r\n\r\n");
  do {
    terminal_clear_lines(5);
    
    printf("Enter the message:\r\n");
    (void)uartReceiveString(ans_buffer, 1337);
    printf("\r\nYou entered: \"%s\"\r\n", ans_buffer);
  } while (!serial_get_yes_no("Use this message? (Y/N)"));
  
  (void)strcpy(msg_ascii, ans_buffer);
}

void serial_get_options(bool* options)
{
  // Choose options
  bool approved_opt = false; 
  bool local_pow    = false;
  bool broadcast    = false;
  bool reattach     = false;
  int8_t del        = -1;
  
  while (!approved_opt) {
    reattach = false;
    
    if (del > 0) {
      terminal_clear_lines(del);
    }
    del = 11;
    
    local_pow = serial_get_yes_no("Do you want to use local PoW? (Y/N)");
    broadcast = serial_get_yes_no("Do you want to broadcast your transaction? (Y/N)");
    if (broadcast) {
      reattach = serial_get_yes_no("Do you want to reattach your transaction after 10 minutes? (Y/N)");
      del++;
    }
    
    terminal_print_frame("PoW and broadcast", '+', '|', '-', WW, GREEN);
    printf("| %-10s %s\033[%dG|\r\n", "PoW:", (local_pow ? "Local (diver)" : "attachToTangle (full node)"), WW+2);
    printf("| %-10s %s\033[%dG|\r\n", "Broadcast:", (broadcast ? "yes" : "no"), WW+2);
    printf("| %-10s %s\033[%dG|\r\n", "Reattach:", (reattach ? "enabled" : "disabled"), WW+2);
    terminal_print_line('+', '-', WW);
    
    approved_opt = serial_get_yes_no("Proceed with these options? (Y/N)");
    printf("\r\n");
  };
  
  // Save chosen options
  options[0] = local_pow;
  options[1] = broadcast;
  options[2] = reattach;
}
