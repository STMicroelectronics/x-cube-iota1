/**
  ******************************************************************************
  * @file    iota_client.h
  * @author  SRA/Central LAB
  * @brief   Header for iota_wallet.c module
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef IOTA_CLIENT_H
#define IOTA_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

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

/** @addtogroup IOTA_CLIENT_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  IOTA client entry function
  * @param  None
  * @retval None
  */
void iota_client_run(void);

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

#endif /* IOTA_CLIENT_H */
