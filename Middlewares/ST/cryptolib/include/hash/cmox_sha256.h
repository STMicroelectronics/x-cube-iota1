/**
  ******************************************************************************
  * @file    cmox_sha256.h
  * @brief   Header file for the SHA256 hash function
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
#ifndef CMOX_SHA256_H
#define CMOX_SHA256_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Include files -------------------------------------------------------------*/
#include "cmox_hash.h"
#include "cmox_md.h"

/**
  * @addtogroup CMOX_MD
  * @{
  */

/**
  * @defgroup CMOX_SHA256 SHA256 hash module
  * @{
  */

/* Macros --------------------------------------------------------------------*/

/** @defgroup CMOX_SHA256_MACROS SHA256 macros
  * @{
  */

#define CMOX_SHA256_SIZE 32U  /*!< Maximum size in bytes of a SHA256 digest */

/**
  * @}
  */

/* Public types --------------------------------------------------------------*/

/** @defgroup CMOX_SHA256_PUBLIC_TYPES SHA256 module public types
  * @{
  */

/**
  * @brief SHA256 handle type definition
  */
typedef cmox_mdSmall_handle_t cmox_sha256_handle_t;

/**
  * @}
  */

/* Public methods prototypes -------------------------------------------------*/

/** @defgroup CMOX_SHA256_PUBLIC_METHODS SHA256 module public method prototypes
  * @{
  */

/**
  * @brief SHA256 constructor
  *
  * @param P_pThis Pointer to the SHA256 handle to initialize
  * @return cmox_hash_handle_t* Pointer to a general hash handle. This will be
  *              used by the general purpose hash functions in order to perform
  *              the algorithm
  */
cmox_hash_handle_t *cmox_sha256_construct(cmox_sha256_handle_t *P_pThis);

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
#endif /* __cplusplus */

#endif /* CMOX_SHA256_H */
