/**
  ******************************************************************************
  * @file    cmox_init.h
  * @author  MCD Application Team
  * @brief   This file exports symbols needed to use init module
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

#ifndef CMOX_INIT_H
#define CMOX_INIT_H

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
  * @brief Initialization target type
  */
typedef uint32_t cmox_init_target_t;

#define CMOX_INIT_TARGET_AUTO       ((cmox_init_target_t)0x00000000)  /*!< Let the cryptographic library auto-detect running STM32 target Series */
#define CMOX_INIT_TARGET_F0         ((cmox_init_target_t)0x46300000)  /*!< Select a STM32F0 Series target */
#define CMOX_INIT_TARGET_F1         ((cmox_init_target_t)0x46310000)  /*!< Select a STM32F1 Series target */
#define CMOX_INIT_TARGET_F2         ((cmox_init_target_t)0x46320000)  /*!< Select a STM32F2 Series target */
#define CMOX_INIT_TARGET_F3         ((cmox_init_target_t)0x46330000)  /*!< Select a STM32F3 Series target */
#define CMOX_INIT_TARGET_F4         ((cmox_init_target_t)0x46340000)  /*!< Select a STM32F4 Series target */
#define CMOX_INIT_TARGET_F7         ((cmox_init_target_t)0x46370000)  /*!< Select a STM32F7 Series target */
#define CMOX_INIT_TARGET_H7         ((cmox_init_target_t)0x48370000)  /*!< Select a STM32H72x/STM32H73x/STM32H74x/STM32H75x Series target */
#define CMOX_INIT_TARGET_H7AB       ((cmox_init_target_t)0x48378000)  /*!< Select a STM32H7Ax/STM32H7Bx Series target */
#define CMOX_INIT_TARGET_G0         ((cmox_init_target_t)0x47300000)  /*!< Select a STM32G0 Series target */
#define CMOX_INIT_TARGET_G4         ((cmox_init_target_t)0x47340000)  /*!< Select a STM32G4 Series target */
#define CMOX_INIT_TARGET_L0         ((cmox_init_target_t)0x4C300000)  /*!< Select a STM32L0 Series target */
#define CMOX_INIT_TARGET_L1         ((cmox_init_target_t)0x4C310000)  /*!< Select a STM32L1 Series target */
#define CMOX_INIT_TARGET_L4         ((cmox_init_target_t)0x4C340000)  /*!< Select a STM32L4/4+ Series target */
#define CMOX_INIT_TARGET_L5         ((cmox_init_target_t)0x4C350000)  /*!< Select a STM32L5 Series target */
#define CMOX_INIT_TARGET_WB         ((cmox_init_target_t)0x57420000)  /*!< Select a STM32WB Series target */
#define CMOX_INIT_TARGET_WL         ((cmox_init_target_t)0x574C0000)  /*!< Select a STM32WL Series target */

/** @brief    Initialization structure
  */
typedef struct
{
  cmox_init_target_t  target;     /*!< User target specification */
  void *pArg;                     /*!< User defined parameter that is transmitted to Low Level services */
} cmox_init_arg_t;

/**
  * @brief Return value type for Initialization module
  */
typedef uint32_t cmox_init_retval_t;    /*!< Initialization return value type */

#define CMOX_INIT_SUCCESS       ((cmox_init_retval_t)0x00000000) /*!< UTILS operation successfully performed */
#define CMOX_INIT_FAIL          ((cmox_init_retval_t)0x00000001) /*!< Input buffers are different */

/**
  * @brief          Initialize CMOX library
  * @param          pInitArg Initialization parameter see @ref cmox_init_arg_t
  * @note           pInitArg can be set to NULL: is equivalent to
  *                   pInitArg->target = CMOX_INIT_TARGET_AUTO
  *                   pInitArg->pArg = NULL
  * @retval         Initialization status: @ref CMOX_INIT_SUCCESS / @ref CMOX_INIT_FAIL
  */
cmox_init_retval_t cmox_initialize(cmox_init_arg_t *pInitArg);

/**
  * @brief          Finalize CMOX library
  * @param          pArg User defined parameter that is transmitted to Low Level services
  * @retval         Finalization status: @ref CMOX_INIT_SUCCESS / @ref CMOX_INIT_FAIL
  */
cmox_init_retval_t cmox_finalize(void *pArg);

/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_INIT_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
