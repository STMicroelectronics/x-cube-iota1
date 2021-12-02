/**
  ******************************************************************************
  * @file    cmox_cta.h
  * @brief   CTA specific features
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
#ifndef CMOX_CTA_H
#define CMOX_CTA_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @brief Provides a label in order to move tables into RAM/CCM/TCM and being
  *        protected against Cache Timing Attacks (CTA)
  * @note  This macro uses extended GCC preprocessor features, that can be used
  *        with ARMCC compiler and EWARM compiler with GCC extension enabled.
  *        If not supported by the user toolchain configuration, it is possible
  *        to replace the macro with the following:
  *
  * \code
  * #define CMOX_CTA_RESISTANT _Pragma("location=\"CMOX_CTA_PROTECTED_DATA\"")
  * \endcode
  */
#define CMOX_CTA_RESISTANT __attribute__((section("CMOX_CTA_PROTECTED_DATA")))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CMOX_CTA_H */
