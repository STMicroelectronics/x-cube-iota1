/**
  ******************************************************************************
  * @file    cmox_mac_retvals.h
  * @brief   return values for MAC functionality
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CMOX_MAC_RETVALS_H
#define CMOX_MAC_RETVALS_H

/* Include files -------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** @addtogroup CMOX_MAC
  * @{
  */

/** @defgroup CMOX_MAC_RETVALS MAC return values
  * @{
  */

/* Macros --------------------------------------------------------------------*/

/**
  * @brief MAC operation successfully performed
  */
#define CMOX_MAC_SUCCESS                        ((cmox_mac_retval_t)0x00030000U)


/**
  * @brief Some error happens internally in the MAC module
  */
#define CMOX_MAC_ERR_INTERNAL                   ((cmox_mac_retval_t)0x00030001U)

/**
  * @brief One or more parameter has been wrongly passed to the function
  *        (e.g. pointer to NULL)
  */
#define CMOX_MAC_ERR_BAD_PARAMETER              ((cmox_mac_retval_t)0x00030002U)

/**
  * @brief Error on performing the operation
  *        (e.g. an operation has been called before initializing the handle)
  */
#define CMOX_MAC_ERR_BAD_OPERATION              ((cmox_mac_retval_t)0x00030003U)

/**
  * @brief Authentication of the tag has been successfull
  */
#define CMOX_MAC_AUTH_SUCCESS                   ((cmox_mac_retval_t)0x0003C726U)

/**
  * @brief Authentication of the tag failed
  */
#define CMOX_MAC_AUTH_FAIL                      ((cmox_mac_retval_t)0x00036E93U)

/* Public types --------------------------------------------------------------*/

/**
  * @brief MAC module return value type
  */
typedef uint32_t cmox_mac_retval_t;

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_MAC_RETVALS_H */
