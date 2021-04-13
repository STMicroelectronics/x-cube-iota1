/**
  ******************************************************************************
  * @file  menu.c
  * @author 1.0.0
  * @brief This file provides functions for managing the application
  *        through a serial terminal
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
#include "menu.h"

#include "main.h"
#include "console.h"

#include "iota_conf.h"
#include "serial_comm.h"
#include "terminal_manager.h"

#include "test.h"

/* Function definition -------------------------------------------------------*/


/**
 * @brief  Handle the infinite loop sending a new transaction every tx_interval min
 * @param  None
 * @retval None
 */
void main_menu(void)
{
  bool exit_program = false;
  
  while(!exit_program) {    
    int32_t choice = -1;
    
    terminal_cls();
    
    // Menu
    terminal_print_frame("IOTA CLIENT", '+', '|', '-', WW, RED);
    iota_printf("|%*s|\r\n", -WW, "");
    iota_printf("|%*s|\r\n", -WW, " 1. Send index;");
    iota_printf("|%*s|\r\n", -WW, " 2. Wallet send tx;");
    iota_printf("|%*s|\r\n", -WW, " 3. Other functions;");
    iota_printf("|%*s|\r\n", -WW, "");
    iota_printf("|%*s|\r\n", -WW, " 0. Exit.");
    iota_printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    
    // Get the choice
    while ((choice < 0) || (choice > 5))  {
//      choice = serial_get_int_single("Choose one of the options:");
      choice = serial_get_int("Choose one of the options:");
      //iota_printf("You chose %d\r\n", choice);
    }
    
    switch(choice) 
    {
    case 0:
//      exit_program = true;
      break;
    case 1:
      send_index();
      serial_press_any();
      break;
    case 2:
      wallet_send_tx();
      serial_press_any();
      break;
    case 3:
      test_menu();
      //serial_press_any();
      break;      
    default:
      iota_printf("Wrong choice. Try again.\r\n");
    }
    
  }
}

void test_menu(void)
{
  bool end_of_test = false;
  
  while(!end_of_test) {    
    terminal_cls();    
    terminal_print_frame("FUNCTION SELECTION", '+', '|', '-', WW, MAGENTA);
    iota_printf("|%*s|\r\n", -WW, "");
    iota_printf("|%*s|\r\n", -WW, " 1.  Client get info;");
    iota_printf("|%*s|\r\n", -WW, " 2.  Client get output;");
    iota_printf("|%*s|\r\n", -WW, " 3.  Client get balance;");
    iota_printf("|%*s|\r\n", -WW, " 4.  Client get message;");
    iota_printf("|%*s|\r\n", -WW, " 5.  Client find message;");
    iota_printf("|%*s|\r\n", -WW, " 6.  Client message builder;");
    iota_printf("|%*s|\r\n", -WW, " 7.  Client get tips;");
    iota_printf("|%*s|\r\n", -WW, " 8.  Client output from addr;");
    iota_printf("|%*s|\r\n", -WW, " 9.  Client response error;");
    iota_printf("|%*s|\r\n", -WW, " 10. Client send message;");
    iota_printf("|%*s|\r\n", -WW, " 11. Test wallet;");
    iota_printf("|%*s|\r\n", -WW, " 12. Test crypto;");
    iota_printf("|%*s|\r\n", -WW, "");
    iota_printf("|%*s|\r\n", -WW, " 0.  Back to the main menu.");
    iota_printf("|%*s|\r\n", -WW, "");
    terminal_print_line('+', '-', WW);
    iota_printf("\r\n");
    
    // Read the choice
//    int32_t choice = serial_get_int_single("Test choice:");
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
      terminal_print_frame("Test wallet", '*', '*', '*', WW, BLUE);
      //test_core();
      test_wallet();
      terminal_print_frame("End [Test wallet]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    case 12:
      terminal_print_frame("Test crypto", '*', '*', '*', WW, BLUE);
      test_crypto();
      terminal_print_frame("End [Test crypto]", '*', '*', '*', WW, BLUE);
      serial_press_any();
      break;
    default:
      iota_printf("\r\nWrong choice [%ld]. Try again.\r\n\r\n", choice);
      break;
    }
    iota_printf("\r\n");
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
