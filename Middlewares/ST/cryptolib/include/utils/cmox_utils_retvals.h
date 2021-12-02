/**
  ******************************************************************************
  * @file    cmox_utils_retvals.h
  * @brief   This file provides the error types and code for UTILS modules
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

#ifndef CMOX_UTILS_RETVALS_H
#define CMOX_UTILS_RETVALS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @addtogroup CMOX_UTILS
  * @{
  */

/** @defgroup CMOX_UTILS_RETVALS Utils return values
  * @{
  */

/**
  * Return value type for the UTILS module
  */
typedef uint32_t cmox_utils_retval_t;

#define CMOX_UTILS_AUTH_SUCCESS       ((cmox_utils_retval_t)0x0007C726) /*!< UTILS operation successfully performed */
#define CMOX_UTILS_AUTH_FAIL          ((cmox_utils_retval_t)0x00076E93) /*!< Input buffers are different */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_UTILS_RETVALS_H */
