/**
  ******************************************************************************
  * @file   wallet.h
  * @author 1.0.0
  * @brief  Header file for wallet.c
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
#ifndef WALLET_H
#define WALLET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_LightNode
 * @{
 */

/** @addtogroup IOTA_WALLET
 * @{
 */

/** @addtogroup IOTA_WALLET_EXPORTED_FUNCTIONS
 * @{
 */

//#define TEST_CRYPTO
//#define TEST_FIND_MESSAGE
//#define TEST_GET_BALANCE
//#define TEST_GET_MESSAGE
//#define TEST_GET_OUTPUT
//#define TEST_INFO
//#define TEST_MESSAGE_BUILDER

#define THROW(x) (void)fprintf(stderr, "EXCEPTION: " #x "\n");           \
                 (void)printf("Error in %s:%d\r\n", __FILE__, __LINE__); \
                  while(true) {};

/* Exported functions ------------------------------------------------------- */
// Transactions creation

// Other functionalities
//__weak int test_find_message(void) {printf("Enable \"TEST_FIND_MESSAGE\" macro to run the test\n"); return 0;}
//__weak int test_get_message(void) {printf("Enable \"TEST_GET_MESSAGE\" macro to run the test\n"); return 0;}
//__weak int test_get_balance(void)  {printf("Enable \"TEST_GET_BALANCE\" macro to run the test\n"); return 0;}
//__weak void test_info(void) {printf("Enable \"TEST_INFO\" macro to run the test\n");}
//__weak int test_get_output(void) {printf("Enable \"TEST_GET_OUTPUT\" macro to run the test\n"); return 0;}
//
//__weak int test_message_builder(void) {printf("Enable \"TEST_MESSAGE_BUILDER\" macro to run the test\n"); return 0;}
//
//__weak int test_crypto(void) {printf("Enable \"TEST_CRYPTO\" macro to run the test\n"); return 0;}

int test_crypto(void);
int test_find_message(void);
int test_get_balance(void);
int test_get_message(void);
int test_get_output(void);
int test_get_tips(void);
void test_info(void);
int test_message_builder(void);
int test_outputs_from_address(void);
int test_response_error(void);
int test_send_message(void);

__weak int test_core(void) {return 0;}

int test_wallet(void);

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

#endif /* WALLET_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/