/**
  ******************************************************************************
  * @file    cmox_md.h
  * @brief   Header file containing the definitions for MD hash functions
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
#ifndef CMOX_MD_H
#define CMOX_MD_H

#include "cmox_hash.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @addtogroup CMOX_HASH
  * @{
  */

/**
  * @defgroup CMOX_MD MD hash module
  * @{
  */

/* Public types --------------------------------------------------------------*/

/**
  * @defgroup CMOX_MD_PUBLIC_TYPES MD public types
  * @{
  */

/**
  * @brief MD Virtual Table
  *
  * This type specifies a pointer to the virtual table containing the methods
  * for the particular MD algorithm
  */
typedef const struct cmox_md_vtableStruct_st *cmox_md_vtable_t;

/**
  * @brief MD engine common structure definition
  */
typedef struct
{
  cmox_md_vtable_t table; /*!< MD virtual table */
  uint32_t flags;         /*!< 32 bit flags, used to perform keyschedule */
  size_t tagLen;          /*!< Size of the required Digest */
  size_t blockLen;        /*!< Internal: Size of processed blocks */
  size_t labelLen;        /*!< Internal: Size of the final label of the MD padding scheme */
  uint32_t bitCount[2];   /*!< Internal: Keeps the count of processed bits */
} cmox_md_engineCommon_t;

/**
  * @brief MD small engine structure definition
  *        (Used by SHA1, SHA224, SHA256, SM3)
  */
typedef struct
{
  cmox_md_engineCommon_t engine;  /*!< Engine of the MD function */
  uint8_t internalBuffer[64];     /*!< Internal: It's a buffer with the data to be hashed */
  uint32_t internalState[8];      /*!< Internal: Keeps the internal state */
} cmox_mdSmall_engine_t;

/**
  * @brief MD large engine structure definition (Used by SHA384, SHA512)
  */
typedef struct
{
  cmox_md_engineCommon_t engine;  /*!< Engine of the MD function */
  uint8_t internalBuffer[128];    /*!< Internal: It's a buffer with the data to be hashed */
  uint64_t internalState[8];      /*!< Internal: Keeps the internal state */
} cmox_mdLarge_engine_t;

/**
  * @brief Generic MD small handle structure
  */
typedef struct
{
  cmox_hash_handle_t super; /*!< General hash module */
  cmox_mdSmall_engine_t md; /*!< MD small engine */
} cmox_mdSmall_handle_t;

/**
  * @brief Generic MD large handle structure
  */
typedef struct
{
  cmox_hash_handle_t super; /*!< General hash module */
  cmox_mdLarge_engine_t md; /*!< MD large engine */
} cmox_mdLarge_handle_t;

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

#endif /* CMOX_MD_H */
