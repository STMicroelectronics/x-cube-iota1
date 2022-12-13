/**
  ******************************************************************************
  * @file    stsafe.h
  * @author  LINKS CyberSecurity 
  * @brief   Header for stsafe.c module
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under SLA0055, the "License";
  * You may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at www.st.com
  *
  ******************************************************************************
  */
#ifndef STSAFE_H
#define STSAFE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */

/** @addtogroup B-U585I-IOT02A
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @defgroup STSAFE STsafe
 * @{
 */

/** @defgroup STSAFE_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Exported functions ------------------------------------------------------- */
/**
 * @brief  Init the STSAFE-A110.
 * @param  None
 * @retval 0 in success
 */
int stsafe_init(void);
/**
 * @brief  Wrap-unwrap envelope example.
 * @param  None
 * @retval 0 in success
 */
int stsafe_wrap_unwrap(void);

/* *********************** STSAFE-L2SEC Integration ************************ */
/**
 * @brief  Init STSAFE for L2SEC (pairing + PSK setting).
 * @param  None
 * @retval 0 in success
 */
int stsafe_l2sec_init(void);
/**
 * @brief  Unwrap the envelope containing the PSK.
 * @param  outPSK The output PSK
 * @retval 0 in success
 */
int stsafe_PSK_unwrap(uint8_t *outPSK);
/**
 * @brief  Verify an ECDSA Signature with STSAFE.
 * @param  inSignatureRS The ECDSA signature
 * @param  inData Pointer to byte array to be authenticated
 * @param  inDataLen Length of the byte array
 * @retval 0 in success
 */
int stsafe_AuthSign_verify(uint8_t *inSignatureRS,
                           uint8_t *inData,
                           size_t inDataLen);
/**
 * @brief  Create the ECDSA Signature with STSAFE
 *         using the PrivateKey of the Leaf Certificate.
 * @param  inSignatureRS The ECDSA signature
 * @param  inData Pointer to byte array to be signed
 * @param  inDataLen Length of the byte array
 * @retval 0 in success
 */
int stsafe_AuthSign_sign(uint8_t *inData,
                         size_t inDataLen,
                         uint8_t *outSign);
/**
 * @brief  Generate Unsigned Byte Array using TRNG of STSAFE.
 * @param  outRnd Pointer to the random byte array
 * @param  inLen Length of the byte array
 * @retval 0 in success
 */
int stsafe_GenRandom(uint8_t *outRnd, uint8_t inLen);
/* *********************** STSAFE-L2SEC Integration ************************ */

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

#endif /* STSAFE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
