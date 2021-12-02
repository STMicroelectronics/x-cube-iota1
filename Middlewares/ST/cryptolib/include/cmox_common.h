/**
  ******************************************************************************
  * @file    cmox_common.h
  * @brief   This file provides the types used within the RSA module
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

#ifndef CMOX_COMMON_H
#define CMOX_COMMON_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  *  @brief Structure to store information on the static memory
  */
typedef struct
{
  uint8_t *MemBuf;    /*!< Pointer to the pre-allocated memory buffer */
  size_t  MemBufSize; /*!< Total size of the pre-allocated memory buffer */
  size_t  MemBufUsed; /*!< Currently used portion of the buffer */
  size_t  MaxMemUsed; /*!< Max memory used */
} cmox_membuf_handle_st;


/**
  * @brief Mathematical functions customizations for RSA and ECC
  */
typedef const struct cmox_math_funcsStruct_st *cmox_math_funcs_t;

extern const cmox_math_funcs_t CMOX_MATH_FUNCS_SMALL;              /*!< Smaller footprint and slower performance */
extern const cmox_math_funcs_t CMOX_MATH_FUNCS_FAST;               /*!< Bigger footprint and faster performance */
extern const cmox_math_funcs_t CMOX_MATH_FUNCS_SUPERFAST256;       /*!< Bigger footprint and faster performance,
                                                                        speed up those ECC curves whose length is in the
                                                                        range [225, 256] bits, e.g. Curve25519, Secp256,
                                                                        Bpp256, Ed25519, Frp256, SM2. */


/**
  * @brief Modular exponentiation functions customizations for RSA
  */
typedef const struct cmox_modexp_funcStruct_st *cmox_modexp_func_t;

extern const cmox_modexp_func_t CMOX_MODEXP_PUBLIC;           /*!< Suggested Modexp value for the target device public operations */

extern const cmox_modexp_func_t CMOX_MODEXP_PRIVATE_LOWMEM;   /*!< Constant-time (for Private operations) using Low Memory */
extern const cmox_modexp_func_t CMOX_MODEXP_PRIVATE_MIDMEM;   /*!< Constant-time (for Private operations) using Mid Memory */
extern const cmox_modexp_func_t CMOX_MODEXP_PRIVATE_HIGHMEM;  /*!< Constant-time (for Private operations) using High Memory */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_COMMON_H */
