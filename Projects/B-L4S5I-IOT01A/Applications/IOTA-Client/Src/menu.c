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

#include "stsafe.h"
#include "test.h"

#ifdef CRYPTO_USE_STLIB
#include "cmox_init.h"
#endif /* CRYPTO_USE_STLIB */

/* Constants -----------------------------------------------------------------*/
#ifndef L2SEC
#define MAX_CHOICE_NUM 7
#else
#define MAX_CHOICE_NUM 8
#endif /* L2SEC */

static void test_menu(void);
static void stsafe_menu(void);
#ifdef L2SEC
static void l2sec_menu(void);
#endif /* L2SEC */
  
/* Function definition -------------------------------------------------------*/

void main_menu(void)
{
  bool exit_program = false;

#ifdef CRYPTO_USE_STLIB
  /* Initialize cryptographic library */
  if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
  {
    printf("cmox_initialize error\r\n");
  }
#endif /* CRYPTO_USE_STLIB */

  while(!exit_program) {
    int32_t choice = -1;
    
    terminal_cls();

    // Menu
    terminal_print_frame("IOTA CLIENT", '+', '|', '-', WW, RED);
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 1. Node info;");
    printf("|%*s|\r\n", -WW, " 2. Get data message;");
    printf("|%*s|\r\n", -WW, " 3. Send data message;");
    printf("|%*s|\r\n", -WW, " 4. Send sensor message;");
    printf("|%*s|\r\n", -WW, " 5. Send encrypted data;");
#ifndef L2SEC
    printf("|%*s|\r\n", -WW, " 6. STSAFE examples;");
    printf("|%*s|\r\n", -WW, " 7. Test functions;");
#else
    printf("|%*s|\r\n", -WW, " 6. L2Sec examples;");
    printf("|%*s|\r\n", -WW, " 7. STSAFE examples;");
    printf("|%*s|\r\n", -WW, " 8. Test functions;");
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
      get_data_message();
      serial_press_any();
      break;
    case 3:
      send_data_message();
      serial_press_any();
      break;
    case 4:
      send_sensor_index();
      serial_press_any();
      break;
    case 5:
      send_enc_data();
      serial_press_any();
      break;
#ifndef L2SEC
    case 6:
      stsafe_menu();
      break;
    case 7:
      test_menu();
      break;
#else
    case 6:
      l2sec_menu();
      break;
    case 7:
      stsafe_menu();
      break;
    case 8:
      test_menu();
      break;
#endif /* L2SEC */
    default:
      printf("Wrong choice. Try again.\r\n");
    }    
  }

#ifdef CRYPTO_USE_STLIB
  /* No more need of cryptographic services, finalize cryptographic library */
  if (cmox_finalize(NULL) != CMOX_INIT_SUCCESS)
  {
    printf("cmox_finalize error\r\n");
  }
#endif /* CRYPTO_USE_STLIB */
}

/* Private functions ---------------------------------------------------------*/
static void test_menu(void)
{
  bool end_of_test = false;
  
  while(!end_of_test) {    
    terminal_cls();    
    terminal_print_frame("FUNCTION SELECTION", '+', '|', '-', WW, MAGENTA);
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 1.  Client get info;");
    printf("|%*s|\r\n", -WW, " 2.  Client get output;");
    printf("|%*s|\r\n", -WW, " 3.  Client get balance;");
    printf("|%*s|\r\n", -WW, " 4.  Client get message;");
    printf("|%*s|\r\n", -WW, " 5.  Client find message;");
    printf("|%*s|\r\n", -WW, " 6.  Client message builder;");
    printf("|%*s|\r\n", -WW, " 7.  Client get tips;");
    printf("|%*s|\r\n", -WW, " 8.  Client output from addr;");
    printf("|%*s|\r\n", -WW, " 9.  Client response error;");
    printf("|%*s|\r\n", -WW, " 10. Client send message;");
    printf("|%*s|\r\n", -WW, " 11. Client get message metadata;");
    printf("|%*s|\r\n", -WW, " 12. Client get message children;");
    printf("|%*s|\r\n", -WW, " 13. Test crypto;");
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 0.  Back to the main menu.");
    printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    printf("\r\n");
    
    // Read the choice
    int32_t choice = serial_get_int("Test choice:");
    terminal_cls();
    
    switch(choice) 
    {
    case 0:
      terminal_print_centered("Back to the main menu", '!', WW);
      end_of_test = true;
      break;
    case 1:
      terminal_print_frame("Get node info", '*', '*', '*', WW, BLUE);
      test_info();
      terminal_print_frame("End [Get node info]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 2:
      terminal_print_frame("Get output", '*', '*', '*', WW, BLUE);
      test_get_output();
      terminal_print_frame("End [Get output]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 3:
      terminal_print_frame("Get balance", '*', '*', '*', WW, BLUE);
      test_get_balance();
      terminal_print_frame("End [Get balance]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 4:
      terminal_print_frame("Get message", '*', '*', '*', WW, BLUE);
      test_get_message();
      terminal_print_frame("End [Get message]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 5:
      terminal_print_frame("Find message", '*', '*', '*', WW, BLUE);
      test_find_message();
      terminal_print_frame("End [Find message]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 6:
      terminal_print_frame("Message builder", '*', '*', '*', WW, BLUE);
      test_message_builder();
      terminal_print_frame("End [Message builder]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 7:
      terminal_print_frame("Get tips", '*', '*', '*', WW, BLUE);
      test_get_tips();
      terminal_print_frame("End [Get tips]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 8:
      terminal_print_frame("Outputs from address", '*', '*', '*', WW, BLUE);
      test_outputs_from_address();
      terminal_print_frame("End [Outputs from address]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 9:
      terminal_print_frame("Response error", '*', '*', '*', WW, BLUE);
      test_response_error();
      terminal_print_frame("End [Response error]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 10:
      terminal_print_frame("Send message", '*', '*', '*', WW, BLUE);
      test_send_message();
      terminal_print_frame("End [Send message]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 11:
      terminal_print_frame("Get message metadata", '*', '*', '*', WW, BLUE);
      test_get_msg_metadata();
      terminal_print_frame("End [Get message metadata]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 12:
      terminal_print_frame("Get message children", '*', '*', '*', WW, BLUE);
      test_get_msg_children();
      terminal_print_frame("End [Get message children]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 13:
      terminal_print_frame("Test crypto", '*', '*', '*', WW, BLUE);
      test_crypto();
      terminal_print_frame("End [Test crypto]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    default:
      printf("\r\nWrong choice [%ld]. Try again.\r\n\r\n", choice);
      break;
    }
    printf("\r\n");
  }
}

static void stsafe_menu(void)
{
  bool end_of_stsafe = false;

  while(!end_of_stsafe) {
    terminal_cls();
    terminal_print_frame("FUNCTION SELECTION", '+', '|', '-', WW, MAGENTA);
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 1. STSAFE Echo;");
    printf("|%*s|\r\n", -WW, " 2. STSAFE Wrap/Unwrap;");
    printf("|%*s|\r\n", -WW, "");
    printf("|%*s|\r\n", -WW, " 0. Back to the main menu.");
    printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    printf("\r\n");

    int32_t choice = serial_get_int("Example choice:");
    terminal_cls();

    switch(choice)
    {
    case 0:
      terminal_print_centered("Back to the main menu", '!', WW);
      end_of_stsafe = true;
      break;
    case 1:
      terminal_print_frame("STSAFE Echo", '*', '*', '*', WW, BLUE);
      stsafe_echo();
      terminal_print_frame("End [STSAFE Echo]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 2:
      terminal_print_frame("STSAFE Wrap/Unwrap", '*', '*', '*', WW, BLUE);
      stsafe_wrap_unwrap();
      terminal_print_frame("End [STSAFE Wrap/Unwrap]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    default:
      printf("\r\nWrong choice [%ld]. Try again.\r\n\r\n", choice);
      break;
    }
    printf("\r\n");
  }
}

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
      printf("\r\nWrong choice [%ld]. Try again.\r\n\r\n", choice);
      break;
    }
    printf("\r\n");
  }
}
#endif /* L2SEC */
