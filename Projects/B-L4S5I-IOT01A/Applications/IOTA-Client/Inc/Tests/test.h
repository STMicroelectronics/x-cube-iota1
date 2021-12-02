/**
  ******************************************************************************
  * @file   test.h
  * @author SRA/Central LAB
  * @brief  Header file common to tests
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup B-L4S5I-IOT01A
 * @{
 */

/** @addtogroup APPLICATIONS
 * @}
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @addtogroup IOTA_TEST
 * @{
 */

/** @defgroup IOTA_TEST_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

#define THROW(x) (void)fprintf(stderr, "EXCEPTION: " #x "\n");           \
                 (void)printf("Error in %s:%d\r\n", __FILE__, __LINE__); \
                  while(true) {};

/* Exported functions ------------------------------------------------------- */

__weak void setUp(void) {};
__weak void tearDown(void) {};

/**
 * @brief   A simple test for crypto features
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_crypto(void);
/**
 * @brief   A simple test for finding a message by index
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_find_message(void);
/**
 * @brief   A simple test for getting balance
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_balance(void);
/**
 * @brief   A simple test for getting a message by id
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_message(void);
/**
 * @brief   A simple test for output
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_output(void);
/**
 * @brief   A simple test for getting tips
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_tips(void);
/**
 * @brief   A simple test for getting node info
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
void test_info(void);
/**
 * @brief   A simple test for building a an indexation/transaction message
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_message_builder(void);
/**
 * @brief   A simple test for getting message children
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_msg_children(void);
/**
 * @brief   A simple test for getting message metadata
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_get_msg_metadata(void);
/**
 * @brief   A simple test for getting output from address
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_outputs_from_address(void);
/**
 * @brief   A simple error test
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_response_error(void);
/**
 * @brief   A simple test for sending an indexation message
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int test_send_message(void);

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* TEST_H */

