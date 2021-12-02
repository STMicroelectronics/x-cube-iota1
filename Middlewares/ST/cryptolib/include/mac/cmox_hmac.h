/**
  ******************************************************************************
  * @file    cmox_hmac.h
  * @brief   Header file for the HMAC algorithm
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
#ifndef CMOX_HMAC_H
#define CMOX_HMAC_H

/* Include files -------------------------------------------------------------*/
#include "cmox_mac.h"
#include "hash/cmox_md.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** @addtogroup CMOX_MAC
  * @{
  */

/** @defgroup CMOX_HMAC HMAC
  * @{
  */

/* Public macros -------------------------------------------------------------*/

/* Public types --------------------------------------------------------------*/

/** @defgroup CMOX_HMAC_PUBLIC_TYPES HMAC public types
  * @{
  */

/**
  * @brief HMAC implementation
  *
  * This type specifies the used hash function for the HMAC construct.
  * This type is defined as a pointer to a structure, that contains the
  * functions needed for the specific implementation, defined in the library
  * internally
  */
typedef const struct cmox_hmac_implStruct_st *cmox_hmac_impl_t;

/**
  * @brief HMAC internal support functions
  *
  * This type specifies some functions used internally by the HMAC in order to
  * correctly compute the MAC. The structure pointed by the type is defined
  * in the library internally
  */
typedef const struct cmox_hmac_supportStruct_st *cmox_hmac_support_t;

/**
  * @brief HMAC handle structure definition
  */
typedef struct
{
  cmox_mac_handle_t super; /*!< General MAC handle */

  /**
    * @brief Hash function handle
    *
    * Depending on the chosen hash function, one of the two fields of the union
    * will be used.
    */
  union
  {
    cmox_mdSmall_handle_t md_small;     /*!< Handle for SHA-224, SHA-256, SM3 */
    cmox_mdLarge_handle_t md_large;     /*!< Handle for SHA-384, SHA-512 */
  } hash_context;

  cmox_hash_handle_t *hash;     /*!< Pointer to general hash handle */
  uint8_t key[128];             /*!< Buffer of bytes containing the derivated key */
  cmox_hmac_support_t support;  /*!< Support functions for HMAC computation */
} cmox_hmac_handle_t;

/**
  * @}
  */

/* Public constants ----------------------------------------------------------*/

/** @defgroup CMOX_HMAC_PUBLIC_CONSTANTS HMAC public constants
  * @{
  */

/** @defgroup CMOX_HMAC_IMPL HMAC implementations
  * @{
  */

/**
  * @brief Implementation of HMAC using SHA-256 (Defined internally)
  */
extern const cmox_hmac_impl_t CMOX_HMAC_SHA256;

/**
  * @brief Implementation of HMAC using SHA-512 (Defined internally)
  */
extern const cmox_hmac_impl_t CMOX_HMAC_SHA512;

/**
  * @}
  */

/** @defgroup CMOX_HMAC_ALGO HMAC single-call algorithms
  * @{
  */

/**
  * @brief Identifier of the HMAC-SHA256 mac algorithm for single-call function
  *   (Defined internally)
  */
extern const cmox_mac_algo_t CMOX_HMAC_SHA256_ALGO;

/**
  * @brief Identifier of the HMAC-SHA512 mac algorithm for single-call function
  *   (Defined internally)
  */
extern const cmox_mac_algo_t CMOX_HMAC_SHA512_ALGO;

/**
  * @}
  */

/**
  * @}
  */

/* Public methods prototypes -------------------------------------------------*/

/** @defgroup CMOX_HMAC_PUBLIC_METHODS HMAC public method prototypes
  * @{
  */

/**
  * @brief HMAC constructor
  *
  * The function is used for specifying which hash function to use in
  * order to implement the HMAC algorithm.
  *
  * @param P_pThis Pointer to the HMAC handle to initialize
  * @param P_impl  Constant that specifies the implementation to use.
  *                This parameter can be one of the following values:
  *                @arg CMOX_HMAC_SHA256
  *                @arg CMOX_HMAC_SHA512
  * @return cmox_mac_handle_t* Pointer to a general MAC handle. This will
  *                be used by the general purpose MAC functions in order to
  *                perform the algorithm
  */
cmox_mac_handle_t *cmox_hmac_construct(cmox_hmac_handle_t *P_pThis,
                                       cmox_hmac_impl_t P_impl);

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

#endif /* CMOX_HMAC_H */
