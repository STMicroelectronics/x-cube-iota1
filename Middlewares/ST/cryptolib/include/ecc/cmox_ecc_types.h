/**
  ******************************************************************************
  * @file    cmox_ecc_types.h
  * @brief   This file provides the types used within the ECC module
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

#ifndef CMOX_ECC_TYPES_H
#define CMOX_ECC_TYPES_H

#include "cmox_common.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @brief Structure for the ECC context
  */
typedef struct
{
  cmox_membuf_handle_st membuf_str;       /*!< Memory buffer structure */
  cmox_math_funcs_t     math_ptr;         /*!< Math customization structure */
  uint32_t              magic_num_check;  /*!< Magic number for diagnostic checks */
} cmox_ecc_handle_t;

/**
  * @brief Pointer type for the ECC curve function parameter
  */
typedef const struct cmox_ecc_implStruct_st *cmox_ecc_impl_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* CMOX_ECC_TYPES_H */
