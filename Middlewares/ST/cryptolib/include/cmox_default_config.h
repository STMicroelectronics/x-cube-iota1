/**
  ******************************************************************************
  * @file    cmox_default_config.h
  * @brief   Header file for default configuration of some algorithms
  * @note    It is possible to replace this header file with a custom one with
  *          desired configuration. In this case it must be set the macro
  *          CMOX_DEFAULT_FILE with the name of the custom file.
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
#ifndef CMOX_DEFAULT_CONFIG_H
#define CMOX_DEFAULT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
  * @addtogroup CMOX_CRYPTO
  * @{
  */

/**
  * @defgroup CMOX_DEFAULT CMOX default configurations
  * @{
  */

/**
  * @defgroup CMOX_MATH_DEFAULT Default Math customizations
  * @{
  */

/**
  * @brief Flag indicating the default implementation for low level mathematical functions
  */
#define CMOX_MATH_FUNCS              CMOX_MATH_FUNCS_SMALL

/**
  * @}
  */

/**
  * @defgroup CMOX_ECC_DEFAULT Default ECC customizations
  * @{
  */
#define CMOX_ECC_CURVE_ED25519       CMOX_ECC_ED25519_OPT_LOWMEM  /*!< EDWARDS Ed25519 with Edwards functions optimized for a = -1, low RAM usage */
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

#endif /* CMOX_DEFAULT_CONFIG_H */
