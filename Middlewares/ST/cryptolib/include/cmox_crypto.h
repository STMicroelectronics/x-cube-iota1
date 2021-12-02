/**
  ******************************************************************************
  * @file    cmox_crypto.h
  * @brief   Header file including all the supported cryptographic algorithms
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

#ifndef CMOX_CRYPTO_H
#define CMOX_CRYPTO_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** @defgroup CMOX_CRYPTO Cortex-M Optimized Crypto Stack
  * @{
  */

/** @defgroup CMOX_HASH Hash module
  * @{
  */

#include "hash/cmox_sha256.h"
#include "hash/cmox_sha512.h"

/**
  * @}
  */

/** @defgroup CMOX_MAC MAC module
  * @{
  */

#include "mac/cmox_hmac.h"

/**
  * @}
  */

/** @defgroup CMOX_ECC ECC module
  * @{
  */
#include "ecc/cmox_eddsa.h"
/**
  * @}
  */

/** @defgroup CMOX_UTILS Utils module
  *  @{
  */
#include "utils/cmox_utils_compare.h"
/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_CRYPTO_H */
