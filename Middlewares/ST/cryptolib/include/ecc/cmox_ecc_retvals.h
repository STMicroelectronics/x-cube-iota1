/**
  ******************************************************************************
  * @file    cmox_ecc_retvals.h
  * @brief   This file provides the error types and code for the ECC modules
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

#ifndef CMOX_ECC_RETVALS_H
#define CMOX_ECC_RETVALS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @addtogroup CMOX_ECC
  * @{
  */

/**
  * @defgroup CMOX_ECC_RETVALS ECC return values
  * @{
  */

/**
  * @brief Return value type for ECC module
  */
typedef uint32_t cmox_ecc_retval_t;

#define CMOX_ECC_SUCCESS                ((cmox_ecc_retval_t)0x00060000) /*!< Success */
#define CMOX_ECC_ERR_INTERNAL           ((cmox_ecc_retval_t)0x00060001) /*!< Internal computat. error (e.g. hash) */
#define CMOX_ECC_ERR_BAD_PARAMETERS     ((cmox_ecc_retval_t)0x00060003) /*!< Bad input parameters */
#define CMOX_ECC_ERR_INVALID_PUBKEY     ((cmox_ecc_retval_t)0x00060008) /*!< Invalid Public Key value */
#define CMOX_ECC_ERR_INVALID_SIGNATURE  ((cmox_ecc_retval_t)0x00060009) /*!< Invalid Signature value */
#define CMOX_ECC_ERR_WRONG_RANDOM       ((cmox_ecc_retval_t)0x0006000B) /*!< Random not compliant with the API (Recall with other random material) */
#define CMOX_ECC_ERR_MEMORY_FAIL        ((cmox_ecc_retval_t)0x0006000C) /*!< Not enough memory */
#define CMOX_ECC_ERR_MATHCURVE_MISMATCH ((cmox_ecc_retval_t)0x0006000E) /*!< Math customization not supported by current ECC curve */
#define CMOX_ECC_ERR_ALGOCURVE_MISMATCH ((cmox_ecc_retval_t)0x0006000F) /*!< ECC curve not supported by current functionality */
#define CMOX_ECC_AUTH_SUCCESS           ((cmox_ecc_retval_t)0x0006C726) /*!< ECC signature successful verification */
#define CMOX_ECC_AUTH_FAIL              ((cmox_ecc_retval_t)0x00066E93) /*!< ECC signature not verified */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_ECC_RETVALS_H */
