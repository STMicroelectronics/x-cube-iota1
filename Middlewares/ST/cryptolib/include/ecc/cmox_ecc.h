/**
  ******************************************************************************
  * @file    cmox_ecc.h
  * @brief   This file provides common function for ECC module
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

#ifndef CMOX_ECC_H
#define CMOX_ECC_H

#include <stdint.h>
#include <stdlib.h>

#include "ecc/cmox_ecc_retvals.h"
#include "ecc/cmox_ecc_types.h"
#if !defined(CMOX_DEFAULT_FILE)
#include "cmox_default_config.h"
#else
#include CMOX_DEFAULT_FILE
#endif /* CMOX_DEFAULT_FILE */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** @addtogroup CMOX_ECC
  * @{
  */

/**
  * @defgroup CMOX_ECC_CURVES Supported curves implementations
  * @{
  */
extern const cmox_ecc_impl_t CMOX_ECC_ED25519_LOWMEM;       /*!< EDWARDS Ed25519 with general Edwards functions, low RAM usage */
extern const cmox_ecc_impl_t CMOX_ECC_ED25519_HIGHMEM;      /*!< EDWARDS Ed25519 with general Edwards functions, high RAM usage */
extern const cmox_ecc_impl_t CMOX_ECC_ED25519_OPT_LOWMEM;   /*!< EDWARDS Ed25519 with Edwards functions optimized for a = -1, low RAM usage */
extern const cmox_ecc_impl_t CMOX_ECC_ED25519_OPT_HIGHMEM;  /*!< EDWARDS Ed25519 with Edwards functions optimized for a = -1, high RAM usage */
/**
  * @}
  */

/**
  * @defgroup CMOX_ECC_OUTPUT_LENGTHS Macros for output buffers definitions
  * @{
  */
#define CMOX_ECC_ED25519_SIG_LEN          64u /*!< Byte length for a byte buffer containing a ED25519 signature */
#define CMOX_ECC_ED25519_PRIVKEY_LEN      64u /*!< Byte length for a byte buffer containing a ED25519 private key */
#define CMOX_ECC_ED25519_PUBKEY_LEN       32u /*!< Byte length for a byte buffer containing a ED25519 public key */
/**
  * @}
  */

/**
  * @defgroup CMOX_ECC_PUBLIC_METHODS ECC public method prototypes
  * @{
  */

/**
  * @brief         Mandatory function to set Low Level Mathematical Functions, and working memory buffer, for ECC
  * @param[in,out] P_pEccCtx  Context for ECC operations
  * @param[in]     P_Math     Structure pointer with the Low-level Mathematical functions
  *                           This parameter can be one of the following:
  *                              @arg CMOX_MATH_FUNCS_SMALL
  *                              @arg CMOX_MATH_FUNCS_FAST
  *                              @arg CMOX_MATH_FUNCS_SUPERFAST256
  * @param[in]     P_pBuf     The preallocated static buffer that will be used
  * @param[in]     P_BufLen   The size in bytes of the P_pBuf buffer
  */
void cmox_ecc_construct(cmox_ecc_handle_t        *P_pEccCtx,
                        const cmox_math_funcs_t  P_Math,
                        uint8_t                  *P_pBuf,
                        size_t                   P_BufLen);

/**
  * @brief         Clean the ECC context and the internal temporary buffer
  * @param[in,out] P_pEccCtx  Context for ECC operations
  */
void cmox_ecc_cleanup(cmox_ecc_handle_t *P_pEccCtx);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_ECC_H */
