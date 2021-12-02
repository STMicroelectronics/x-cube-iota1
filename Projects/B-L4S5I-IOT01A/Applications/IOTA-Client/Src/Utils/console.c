/**
  ******************************************************************************
  * @file   console.c
  * @author SRA/Central LAB
  * @brief  This file provides implementation of standard input/output
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
