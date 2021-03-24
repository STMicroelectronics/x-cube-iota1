/**
  ******************************************************************************
  * @file   console.c
  * @author SRA/Central LAB
  * @brief  This file provides implementation of standard input/output
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "console.h"
#include "iota_conf.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global UART handle */

/* Exported Functions --------------------------------------------------------*/

/** 
 * @brief  Receives a string from serial port
 * @param  str: destination of the read input
 * @param  length: characters to read (if no EOL is found)
 * @retval Length of the string
*/
int32_t uartReceiveString(char* buffer, size_t length)
{
  char c = '\n';
  char* ptr;
  ptr = buffer;
  
  while(length > 0) {
    c = getchar();//uartReceiveChar();
    
    if(c == '\n') {
      length = 0;
      *ptr = '\0';
    } else if (c == '\b') {
      if(ptr != buffer) {
        length++;
        ptr--;
      }
    } else {
      length--;
      *ptr = c;
      ptr++;
    }
  }
  
  if(c != '\n') {
    *ptr = '\0';
  }

  return strlen(buffer);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
