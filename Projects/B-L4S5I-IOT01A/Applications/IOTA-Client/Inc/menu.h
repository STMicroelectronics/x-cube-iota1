/**
  ******************************************************************************
  * @file   menu.h
  * @author SRA/Central LAB
  * @brief  Header file for menu.c
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
#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup B-L4S5I-IOT01A
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @defgroup IOTA_MENU IOTA Menu
 * @{
 */

/** @defgroup IOTA_MENU_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */
/**
 * @brief  Show the example/test options in the terminal.
 * @param  None
 * @retval None
 */
void main_menu(void);
/**
 * @brief   A simple example of sending an indexation message.
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int send_data_message(void);
/**
 * @brief   A simple example of sending encrypted string to the Tangle.
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int send_enc_data(void);
/**
 * @brief   A simple example of finding a message by id
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int get_data_message(void);
/**
 * @brief   A simple example of getting node info. 
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int node_info(void);
/**
 * @brief   A simple example of sending an indexation message containing the
 * sensor data to the Tangle directly.
 * @param   None
 * @retval  0:  Success.
 *        !=0:  Failure.
 */
int send_sensor_index(void);

#ifdef L2SEC
void example_send_receive_l2sec_protected_stream(int n_messages);
void example_only_send_l2sec_protected_stream(int n_messages);
#endif /* L2SEC */

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

#endif /* MENU_H */
