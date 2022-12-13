/**
  ******************************************************************************
  * @file   menu.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for managing the application
  *         through a serial terminal
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
#include "menu.h"

#include "main.h"
#include "console.h"

#include "iota_conf.h"
#include "serial_comm.h"
#include "terminal_manager.h"


/* Constants -----------------------------------------------------------------*/
#ifndef L2SEC
#define MAX_CHOICE_NUM 5
#else
#define MAX_CHOICE_NUM 6
#endif /* L2SEC */


#ifdef L2SEC
static void l2sec_menu(void);
#endif /* L2SEC */

  
/* Function definition -------------------------------------------------------*/

void main_menu(void)
{
  bool exit_program = false;

  while(!exit_program) {
    int32_t choice = -1;
    
    terminal_cls();

    // Menu
    terminal_print_frame("IOTA CLIENT", '+', '|', '-', WW, RED);
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 1. Node info;");
    printf("|%*s|\r\n", -WW, " 2. Send sensor message;");
#ifdef L2SEC
    printf("|%*s|\r\n", -WW, " 3. L2Sec examples;");
#endif /* L2SEC */
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 0. Exit.");
    printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    
    // Get the choice
    while ((choice < 0) || (choice > MAX_CHOICE_NUM))  {
      choice = serial_get_int("Choose one of the options:");
    }

    switch(choice) 
    {
    case 0:
      exit_program = true;
      break;
    case 1:
      node_info();
      serial_press_any();
      break;
    case 2:
      send_data_message();
      serial_press_any();
      break;
#ifdef L2SEC
    case 3:
      l2sec_menu();
      break;
#endif /* L2SEC */
    default:
      printf("Wrong choice. Try again.\r\n");
    }    
  }

}

/* Private functions ---------------------------------------------------------*/


#ifdef L2SEC
static void l2sec_menu(void)
{
  bool end_of_l2sec = false;

  while(!end_of_l2sec) {
    terminal_cls();
    terminal_print_frame("FUNCTION SELECTION", '+', '|', '-', WW, MAGENTA);
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 1. Send and receive IOTA L2Sec stream;");
    printf("|%*s|\r\n", -WW, " 2. Send only IOTA L2Sec stream;");
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 0. Back to the main menu.");
    printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    printf("\r\n");

    int32_t choice = serial_get_int("Example choice:");
    terminal_cls();

    int32_t n_messages;
    switch(choice)
    {
    case 0:
      terminal_print_centered("Back to the main menu", '!', WW);
      end_of_l2sec = true;
      break;
    case 1:
      terminal_print_frame("Example - Send and receive IOTA L2Sec stream", '*', '*', '*', WW, BLUE);
      n_messages = serial_get_int("Messages to be sent (0 means endless stream):");
      example_send_receive_l2sec_protected_stream(n_messages);
      terminal_print_frame("End [Example - Send and receive IOTA L2Sec stream]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 2:
      terminal_print_frame("Example - Send only IOTA L2Sec stream", '*', '*', '*', WW, BLUE);
      n_messages = serial_get_int("Messages to be sent (0 means endless stream):");
      example_only_send_l2sec_protected_stream(n_messages);
      terminal_print_frame("End [Example - Send only IOTA L2Sec stream]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    default:
#ifdef __ARMCC_VERSION
      printf("\r\nWrong choice [%d]. Try again.\r\n\r\n", choice);
#else
      printf("\r\nWrong choice [%ld]. Try again.\r\n\r\n", choice); 
#endif /* __ARMCC_VERSION */
      break;
    }
    printf("\r\n");
  }
}
#endif /* L2SEC */

