/**
  ******************************************************************************
  * @file   serial_comm.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for the serial terminal 
  *         input/output management
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

/**
 * @brief  Wait for any input from the user.
 * @param  None
 * @retval None
 */
void serial_press_any(void)
{
  iota_printf("\r\nPress any key to continue.\r\n");
  (void)getchar();
  iota_printf("\r\n\n");
  terminal_clear_lines(1);
}

/**
 * @brief  Wait for a yes/no answer from the user.
 * @param  question The question string
 * @retval True if answer is yes, false otherwise
 */
bool serial_get_yes_no(char* question)
{
  char c;
  iota_printf("\r\n");
  do {
    terminal_clear_lines(1);
    iota_printf("%s ", question);
    (void)fflush(stdout);
    c = getchar();
    iota_printf("\r\n");
  } while (!((c == 'y') || (c == 'Y') || (c == 'n') || (c == 'N')));
  
  if ((c == 'y') || (c == 'Y')) {
    return true;
  } 
  else {
    return false;
  }
}

/**
 * @brief  Wait for a choice from the user.
 * @param  question The question string
 * @param  a The first choice
 * @param  b The second choice
 * @retval True if answer is the first choice, false otherwise
 */
bool serial_get_choice(char* question, char a, char b)
{
  // To make sure
  a = tolower(a);
  b = tolower(b);
  
  iota_printf("\r\n");
  char c;
  do {
    terminal_clear_lines(1);
    iota_printf("%s ", question);
    (void)fflush(stdout);
    c = getchar();//uartReceiveChar();
    iota_printf("\r\n");
    
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

/**
 * @brief  Read an integer from the user input.
 * @param  question The question string
 * @retval The integer
 */
int32_t serial_get_int(char* question)
{	
  iota_printf("%s ", question);
  (void)fflush(stdout);
  (void)uartReceiveString(ans_buffer, BUFFER_LENGTH);
  iota_printf("\r\n");
  
  return atoi(ans_buffer);
}

/**
 * @brief  Read a single digit integer from the user input.
 *         Equivalent to serial_get_int_fixed(question, 1);
 * @param  question The question string
 * @retval The single integer
 */
int32_t serial_get_int_single(char* question)
{
  char c;
  int32_t ret = 0;
	
  iota_printf("%s ", question);
  (void)fflush(stdout);
  c = getchar();//uartReceiveChar();
  iota_printf("\r\n");
		
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

/**
 * @brief  Read a fixed digit integer from the user input.
 * @param  question The question string
 * @param  digits Number of digits
 * @retval The integer
 */
int32_t serial_get_int_fixed(char* question, size_t digits)
{
  if (digits > BUFFER_LENGTH) {
    digits = (BUFFER_LENGTH - 1);
  }
  iota_printf("%s ", question);
  (void)fflush(stdout);
  (void)uartReceiveString(ans_buffer, digits);
  iota_printf("\r\n");
  
  return atoi(ans_buffer);
}

/**
 * @brief  Read an integer from the user input greater than min.
 * @param  question The question string
 * @param  min The minimum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_min(char* question, int32_t min)
{
  int32_t n = serial_get_int(question);
  while (n < min) {
    iota_printf("Too low.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

/**
 * @brief  Read an integer from the user input lower than max.
 * @param  question The question string
 * @param  max The maximum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_max(char* question, int32_t max)
{
  int32_t n = serial_get_int(question);
  while (n > max) {
    iota_printf("Too high.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

/**
 * @brief  Read an integer from the user input between min and max.
 * @param  question The question string
 * @param  min The minimum integer to be entered
 * @param  max The maximum integer to be entered
 * @retval The integer
 */
int32_t serial_get_int_min_max(char* question, int32_t min, int32_t max) 
{
  int32_t n = serial_get_int(question);
  while ((n < min) || (n > max)) {
    iota_printf("Out of range.\r\n\r\n");
    n = serial_get_int(question);
  }
  
  return n;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief  Read a IOTA address parameters.
 * @param  sec The security level
 * @param  idx The address index
 * @retval None
 */
void serial_get_address_parameters(int8_t* sec, uint32_t* idx)
{
  // Security level
  int32_t s = -1;
  while ((s < 0) || (s > 3)) {
    s = serial_get_int_single("-> Security level:");
  }
  
  // Index
  int32_t i = -1;
  while (i < 0) {
    i = serial_get_int("-> Index:");
  }
  
  *sec = s;
  *idx = i;
}

/**
 * @brief  Read the Full Node URL.
 * @param  node_url The url buffer
 * @retval None
 */
void serial_get_full_node_url(char* node_url)
{
  // Get the node_url
  do {
    iota_printf("Enter the full node URL:\r\n");
    (void)uartReceiveString(node_url, 50);
    iota_printf("\r\n");

    iota_printf("You entered: \"%s\"\r\n", node_url);
  } while (!serial_get_yes_no("Do you want to use this URL? (Y/N)"));
}

//--------------------------------------------------------------------------------------------------
/**
 * @brief  Read an ascii string from the user input.
 * @param  question The question string
 * @param  destination The destination buffer
 * @param  max_length The maximum string length
 * @retval None
 */
void serial_get_ascii(char* question, char* destination, size_t max_length)
{	
  if (max_length > BUFFER_LENGTH - 1) {
    max_length = BUFFER_LENGTH - 1;
  }
  iota_printf("%s ", question);
  (void)fflush(stdout);
  (void)uartReceiveString(ans_buffer, max_length);
  iota_printf("\r\n");
  
  if (max_length < strlen(ans_buffer)) {
    ans_buffer[max_length] = '\0';
  }
  (void)strcpy(destination, ans_buffer);
}

/**
 * @brief  Read an ascii message from the user input.
 * @param  msg_ascii The ascii buffer
 * @retval None
 */
void serial_get_message_ascii(char* msg_ascii)
{		
  iota_printf("\r\n\r\n\r\n\r\n\r\n");
  do {
    terminal_clear_lines(5);
    
    iota_printf("Enter the message:\r\n");
    (void)uartReceiveString(ans_buffer, 1337);
    iota_printf("\r\nYou entered: \"%s\"\r\n", ans_buffer);
  } while (!serial_get_yes_no("Use this message? (Y/N)"));
  
  (void)strcpy(msg_ascii, ans_buffer);
}

/**
 * @brief  Get the pow and broadcast options.
 * @param  options The options
 * @retval None
 */
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
    iota_printf("| %-10s %s\033[%dG|\r\n", "PoW:", (local_pow ? "Local (diver)" : "attachToTangle (full node)"), WW+2);
    iota_printf("| %-10s %s\033[%dG|\r\n", "Broadcast:", (broadcast ? "yes" : "no"), WW+2);
    iota_printf("| %-10s %s\033[%dG|\r\n", "Reattach:", (reattach ? "enabled" : "disabled"), WW+2);
    terminal_print_line('+', '-', WW);
    
    approved_opt = serial_get_yes_no("Proceed with these options? (Y/N)");
    iota_printf("\r\n");
  };
  
  // Save chosen options
  options[0] = local_pow;
  options[1] = broadcast;
  options[2] = reattach;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
