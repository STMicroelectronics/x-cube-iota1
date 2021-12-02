/**
  ******************************************************************************
  * @file    cmox_low_level.h
  * @brief   This file exports symbols needed to use low level module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef CMOX_LOW_LEVEL_H
#define CMOX_LOW_LEVEL_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** @addtogroup CMOX_CRYPTO
  * @{
  */

/** @addtogroup CMOX_INIT Initialization module
  * @{
  */

/**
  * @brief          CMOX library low level initialization
  * @note           Implements low level initialization required by cryptographic
  *                 library to operate properly
  * @param          pArg User defined parameter that is transmitted from Initialize service
  * @retval         Initialization status: @ref CMOX_INIT_SUCCESS / @ref CMOX_INIT_FAIL
  */
cmox_init_retval_t cmox_ll_init(void *pArg);

/**
  * @brief          CMOX library low level de-initialization
  * @param          pArg User defined parameter that is transmitted from Finalize service
  * @retval         De-initialization status: @ref CMOX_INIT_SUCCESS / @ref CMOX_INIT_FAIL
  */
cmox_init_retval_t cmox_ll_deInit(void *pArg);

/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_LOW_LEVEL_H */
