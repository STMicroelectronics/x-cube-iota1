/**
  ******************************************************************************
  * @file    mbedtls_entropy.h
  * @author  MCD Application Team
  * @brief   Entropy collector interface for mbedTLS.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019-2021 STMicroelectronics International N.V.
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
#ifndef mbedtls_entropy_H
#define mbedtls_entropy_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup B-L4S5I-IOT01A
 * @{
 */

/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @defgroup MBEDTLS_ENTROPY MbedTLS Entropy
 * @{
 */

/** @defgroup MBEDTLS_ENTROPY_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/**
  * @brief The random number generator callback.
  * @param data Entropy context
  * @param output Buffer to fill
  * @param len Number of bytes desired
  * @retval  0:  Success.
  *        !=0:  Failure.
  */
int mbedtls_rng_raw( void *data, unsigned char *output, size_t len );

/**
 * @}
 */

/**
 * @}
 */
  
/**
 * @}
 */
  
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
#endif

#endif /* mbedtls_entropy_H */
