/**
  ******************************************************************************
  * @file    cmox_utils_compare.h
  * @brief   Public comparison function and utilities
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

#ifndef CMOX_UTILS_COMPARE_H
#define CMOX_UTILS_COMPARE_H

#include "utils/cmox_utils_retvals.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** @addtogroup CMOX_UTILS
  * @{
  */

/**
  *  @brief      Compares two buffers in a fault-secure way
  *  @param[in]  P_pBuf1       First buffer
  *  @param[in]  P_Len1        Length of the first input buffer
  *  @param[in]  P_pBuf2       Second buffer
  *  @param[in]  P_Len2        Length of the second input buffer
  *  @param[out] P_pFaultCheck Optional value to check, together with the retval, to verify if some fault happened
  *  @retval     CMOX_UTILS_AUTH_SUCCESS  The two buffers are equal
  *  @retval     CMOX_UTILS_AUTH_FAIL     The two buffers are different
  *  @warning    If it's necessary to be protected against fault attacks, it's very important that P_Len1 and P_Len2
  *              are different memory locations (or registers), otherwise a single fault could be fatal.
  *  @note       P_pFaultCheck value, if the parameter is provided, MUST be checked to
  *              be equal to the retval, and both MUST be equal to the successful value.
  *              P_pFaultCheck MUST be checked only if the main result is successful,
  *              and has no relevance if the main result is not successful.
  *              Every comparison (both for the return value and for P_pFaultCheck) must be done against
  *              the success value, and not comparing the value with the failure value. Indeed, in presence of faults,
  *              especially P_pFaultCheck, could be a dirty value.
  */
cmox_utils_retval_t cmox_utils_compare(const uint8_t *P_pBuf1,
                                       uint32_t      P_Len1,
                                       const uint8_t *P_pBuf2,
                                       uint32_t      P_Len2,
                                       uint32_t      *P_pFaultCheck);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* CMOX_UTILS_COMPARE_H */
