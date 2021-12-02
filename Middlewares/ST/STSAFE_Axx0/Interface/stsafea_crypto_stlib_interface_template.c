/**
  ******************************************************************************
  * @file    stsafea_crypto_stdlib_interface_template.c
  * @author  SMD/AME application teams
  * @version V3.2.0
  * @brief   Crypto Interface file to support the crypto services required by the
  *          STSAFE-A Middleware and offered by the STM32 crypto library:
  *           + Key Management
  *           + SHA
  *           + AES
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * STSAFE DRIVER SOFTWARE LICENSE AGREEMENT (SLA0088)
  *
  * BY INSTALLING, COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE
  * OR ANY PART THEREOF (AND THE RELATED DOCUMENTATION) FROM STMICROELECTRONICS
  * INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (STMICROELECTRONICS),
  * THE RECIPIENT, ON BEHALF OF HIMSELF OR HERSELF, OR ON BEHALF OF ANY ENTITY BY WHICH
  * SUCH RECIPIENT IS EMPLOYED AND/OR ENGAGED AGREES TO BE BOUND BY THIS SOFTWARE LICENSE
  * AGREEMENT.
  *
  * Under STMicroelectronics’ intellectual property rights, the redistribution,
  * reproduction and use in source and binary forms of the software or any part thereof,
  * with or without modification, are permitted provided that the following conditions
  * are met:
  * 1.  Redistribution of source code (modified or not) must retain any copyright notice,
  *     this list of conditions and the disclaimer set forth below as items 10 and 11.
  * 2.  Redistributions in binary form, except as embedded into a microcontroller or
  *     microprocessor device or a software update for such device, must reproduce any
  *     copyright notice provided with the binary code, this list of conditions, and the
  *     disclaimer set forth below as items 10 and 11, in documentation and/or other
  *     materials provided with the distribution.
  * 3.  Neither the name of STMicroelectronics nor the names of other contributors to this
  *     software may be used to endorse or promote products derived from this software or
  *     part thereof without specific written permission.
  * 4.  This software or any part thereof, including modifications and/or derivative works
  *     of this software, must be used and execute solely and exclusively in combination
  *     with a secure microcontroller device from STSAFE family manufactured by or for
  *     STMicroelectronics.
  * 5.  No use, reproduction or redistribution of this software partially or totally may be
  *     done in any manner that would subject this software to any Open Source Terms.
  *     “Open Source Terms” shall mean any open source license which requires as part of
  *     distribution of software that the source code of such software is distributed
  *     therewith or otherwise made available, or open source license that substantially
  *     complies with the Open Source definition specified at www.opensource.org and any
  *     other comparable open source license such as for example GNU General Public
  *     License(GPL), Eclipse Public License (EPL), Apache Software License, BSD license
  *     or MIT license.
  * 6.  STMicroelectronics has no obligation to provide any maintenance, support or
  *     updates for the software.
  * 7.  The software is and will remain the exclusive property of STMicroelectronics and
  *     its licensors. The recipient will not take any action that jeopardizes
  *     STMicroelectronics and its licensors' proprietary rights or acquire any rights
  *     in the software, except the limited rights specified hereunder.
  * 8.  The recipient shall comply with all applicable laws and regulations affecting the
  *     use of the software or any part thereof including any applicable export control
  *     law or regulation.
  * 9.  Redistribution and use of this software or any part thereof other than as  permitted
  *     under this license is void and will automatically terminate your rights under this
  *     license.
  * 10. THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY
  *     EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
  *     OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS, WHICH ARE DISCLAIMED TO THE FULLEST
  *     EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE
  *     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  *     THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  *     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  *     ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 11. EXCEPT AS EXPRESSLY PERMITTED HEREUNDER, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS
  *     OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF
  *     STMICROELECTRONICS OR ANY THIRD PARTY.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "stsafea_crypto.h"
#include "stsafea_interface_conf.h"
#include MCU_PLATFORM_INCLUDE

#include "crypto.h"
#include "AES/CMAC/aes_cmac.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#if (USE_PRE_LOADED_HOST_KEYS)
static uint8_t  aHostCipherKey[STSAFEA_HOST_KEY_LENGTH]; /*!< STSAFE-A's Host cipher key */
static uint8_t  aHostMacKey   [STSAFEA_HOST_KEY_LENGTH]; /*!< STSAFE-A's Host Mac key */
#else
static uint8_t  aHostCipherKey[] = {0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88}; /*!< STSAFE-A's Host cipher key */
static uint8_t  aHostMacKey   [] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; /*!< STSAFE-A's Host Mac key */
#endif /* USE_PRE_LOADED_HOST_KEYS */

#if (USE_SIGNATURE_SESSION)
#ifdef INCLUDE_SHA256
static SHA256ctx_stt                  SHA256_ctx_stt;
#endif /* INCLUDE_SHA256 */

#ifdef INCLUDE_SHA384
static SHA384ctx_stt                  SHA384_ctx_stt;
#endif /* INCLUDE_SHA384 */
#endif /* USE_SIGNATURE_SESSION */

#ifdef INCLUDE_CMAC
static AESCMACctx_stt                 cipher_ctx;
#endif /* INCLUDE_CMAC */

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/** @addtogroup CRYPTO_IF_Exported_Functions_Group1 Host MAC and Cipher keys Initialization
  *  @brief    Crypto Interface APIs to be implemented at application level. Templates are provided.
  *
@verbatim
 ===============================================================================
           ##### Host MAC and Cipher keys Initialization functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_HostKeys_Init
  *          Initialize STSAFE-Axxx Host MAC and Cipher Keys that will be used by the crypto interface layer
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   None
  * @retval  0 if success. An error code otherwise
  */
int32_t StSafeA_HostKeys_Init()
{
#if (USE_PRE_LOADED_HOST_KEYS)
  /* This is just a very easy example to retrieve keys pre-loaded at the end of the MCU Flash
     and load them into the SRAM. Host MAC and Chiper Keys are previously pre-stored at the end 
     of the MCU flash (e.g. by the SDK Pairing Application example) .
     It's up to the user to protect the MAC and Chiper keys and to find the proper
     and most secure way to retrieve them when needed or to securely keep them into
     a protected volatime memory during the application life */

  /* Host MAC Key */
  uint32_t host_mac_key_addr = FLASH_BASE + FLASH_SIZE - 2U * (STSAFEA_HOST_KEY_LENGTH);

  /* Host Cipher Key */
  uint32_t host_cipher_key_addr = FLASH_BASE + FLASH_SIZE - (STSAFEA_HOST_KEY_LENGTH);

  /* Set and keep the keys that will be used during the Crypto / MAC operations */
  (void)memcpy(aHostMacKey, (uint8_t *)host_mac_key_addr,    STSAFEA_HOST_KEY_LENGTH);
  (void)memcpy(aHostCipherKey, (uint8_t *)host_cipher_key_addr, STSAFEA_HOST_KEY_LENGTH);
#endif /* USE_PRE_LOADED_HOST_KEYS */

  return 0;
}

/**
  * @}
  */


#if (USE_SIGNATURE_SESSION)

/** @addtogroup CRYPTO_IF_Exported_Functions_Group2 HASH Functions
  *  @brief    Crypto Interface APIs to be implemented at application level. Templates are provided.
  *
@verbatim
 ===============================================================================
                          ##### HASH functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_SHA_Init
  *          SHA initialization function to initialize the SHA context
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   InHashType : type of SHA
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   ppShaCtx : SHA context to be initialized
  * @retval  None
  */
void StSafeA_SHA_Init(StSafeA_HashTypes_t InHashType, void **ppShaCtx)
{
  switch (InHashType)
  {
#ifdef INCLUDE_SHA256
    case STSAFEA_SHA_256:
      *ppShaCtx = &SHA256_ctx_stt;
      ((SHA256ctx_stt *)*ppShaCtx)->mTagSize = CRL_SHA256_SIZE;
      ((SHA256ctx_stt *)*ppShaCtx)->mFlags = E_HASH_DEFAULT;
      (void)SHA256_Init(*ppShaCtx);
      break;
#endif /* INCLUDE_SHA256 */

#ifdef INCLUDE_SHA384
    case STSAFEA_SHA_384:
      *ppShaCtx = &SHA384_ctx_stt;
      ((SHA384ctx_stt *)*ppShaCtx)->mTagSize = CRL_SHA384_SIZE;
      ((SHA384ctx_stt *)*ppShaCtx)->mFlags = E_HASH_DEFAULT;
      (void)SHA384_Init(*ppShaCtx);
      break;
#endif /* INCLUDE_SHA384 */

    default:
      break;
  }
}

/**
  * @brief   StSafeA_SHA_Update
  *          SHA update function to process SHA over a message data buffer.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   InHashType : type of SHA
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   pShaCtx : SHA context
  * @param   pInMessage : message data buffer
  * @param   InMessageLength : message data buffer length
  * @retval  None
  */
void StSafeA_SHA_Update(StSafeA_HashTypes_t InHashType, void *pShaCtx, uint8_t *pInMessage, uint32_t InMessageLength)
{
  if (pShaCtx != NULL)
  {
    switch (InHashType)
    {
#ifdef INCLUDE_SHA256
      case STSAFEA_SHA_256:
        (void)SHA256_Append(pShaCtx, pInMessage, (int32_t)InMessageLength);
        break;
#endif /* INCLUDE_SHA256 */

#ifdef INCLUDE_SHA384
      case STSAFEA_SHA_384:
        (void)SHA384_Append(pShaCtx, pInMessage, (int32_t)InMessageLength);
        break;
#endif /* INCLUDE_SHA384 */

      default:
        break;
	  }
  }
}

/**
  * @brief   StSafeA_SHA_Final
  *          SHA final function to finalize the SHA Digest
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   InHashType : type of SHA
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   ppShaCtx : SHA context to be finalized
  * @param   pMessageDigest : message digest data buffer
  * @retval  None
  */
void StSafeA_SHA_Final(StSafeA_HashTypes_t InHashType, void **ppShaCtx, uint8_t *pMessageDigest)
{
  int32_t DigestLength;

  if (*ppShaCtx != NULL)
  {
    switch (InHashType)
    {
#ifdef INCLUDE_SHA256
      case STSAFEA_SHA_256:
        (void)SHA256_Finish(*ppShaCtx, pMessageDigest, &DigestLength);
        *ppShaCtx = NULL;
        break;
#endif /* INCLUDE_SHA256 */

#ifdef INCLUDE_SHA384
      case STSAFEA_SHA_384:
        (void)SHA384_Finish(*ppShaCtx, pMessageDigest, &DigestLength);
        *ppShaCtx = NULL;
        break;
#endif /* INCLUDE_SHA384 */

      default:
        break;
    }
  }
}

#endif /* USE_SIGNATURE_SESSION */

/**
  * @}
  */


/** @addtogroup CRYPTO_IF_Exported_Functions_Group3 AES Functions
  *  @brief    Crypto Interface APIs to be implemented at application level. Templates are provided.
  *
@verbatim
 ===============================================================================
                          ##### AES functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_AES_MAC_Start
  *          Start AES MAC computation
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   ppAesMacCtx : AES MAC context
  * @retval  None
  */
void StSafeA_AES_MAC_Start(void **ppAesMacCtx)
{
#ifdef INCLUDE_CMAC
  *ppAesMacCtx = &cipher_ctx;

  ((AESCMACctx_stt *)*ppAesMacCtx)->mFlags = E_SK_DEFAULT;
  ((AESCMACctx_stt *)*ppAesMacCtx)->pmKey = aHostMacKey;
  ((AESCMACctx_stt *)*ppAesMacCtx)->mKeySize = 16;
  ((AESCMACctx_stt *)*ppAesMacCtx)->mTagSize = 16;
  (void)AES_CMAC_Encrypt_Init((AESCMACctx_stt *)*ppAesMacCtx);
#endif /* INCLUDE_CMAC */
}

/**
  * @brief   StSafeA_AES_MAC_Update
  *          Update / Add data to MAC computation
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pInData : data buffer
  * @param   InDataLength : data buffer length
  * @param   pAesMacCtx : AES MAC context
  * @retval  None
  */
void StSafeA_AES_MAC_Update(uint8_t *pInData, uint16_t InDataLength, void *pAesMacCtx)
{
#ifdef INCLUDE_CMAC
  if (pAesMacCtx != NULL)
  {
    (void)AES_CMAC_Encrypt_Append(pAesMacCtx, pInData, (int32_t)InDataLength);
  }
#endif /* INCLUDE_CMAC */
}

/**
  * @brief   StSafeA_AES_MAC_LastUpdate
  *          Update / Add data to MAC computation
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pInData : data buffer
  * @param   InDataLength : data buffer length
  * @param   pAesMacCtx : AES MAC context
  * @retval  None
  */
void StSafeA_AES_MAC_LastUpdate(uint8_t *pInData, uint16_t InDataLength, void *pAesMacCtx)
{
#ifdef INCLUDE_CMAC
  if (pAesMacCtx != NULL)
  {
    ((AESCMACctx_stt *)pAesMacCtx)->mFlags = E_SK_FINAL_APPEND;
    (void)AES_CMAC_Encrypt_Append(pAesMacCtx, pInData, (int32_t)InDataLength);
  }
#endif /* INCLUDE_CMAC */
}

/**
  * @brief   StSafeA_AES_MAC_Final
  *          Finalize AES MAC computation
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pOutMac : calculated MAC
  * @param   ppAesMacCtx : AES MAC context
  * @retval  None
  */
void StSafeA_AES_MAC_Final(uint8_t *pOutMac, void **ppAesMacCtx)
{
#ifdef INCLUDE_CMAC
  int32_t outputLength = 0;
  if (*ppAesMacCtx != NULL)
  {
    (void)AES_CMAC_Encrypt_Finish(*ppAesMacCtx, pOutMac, &outputLength);
    *ppAesMacCtx = NULL;
  }
#endif /* INCLUDE_CMAC */
}

/**
  * @brief   StSafeA_AES_ECB_Encrypt
  *          AES ECB Encryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pInData : plain data buffer
  * @param   pOutData : encrypted output data buffer
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise
  */
int32_t StSafeA_AES_ECB_Encrypt(uint8_t *pInData, uint8_t *pOutData, uint8_t InAesType)
{
#if defined INCLUDE_ECB && defined INCLUDE_ENCRYPTION
  int32_t status_code;
  AESECBctx_stt AESctx;
  int32_t outputLength = 0;
  uint32_t keySize;

  switch (InAesType)
  {
    case STSAFEA_KEY_TYPE_AES_128:
    case STSAFEA_KEY_TYPE_AES_256:
      AESctx.mFlags = E_SK_DEFAULT;
      keySize = STSAFEA_AES_KEY_BITSIZE((uint32_t)InAesType) / 8U;
      AESctx.mKeySize = (int32_t)keySize;
      status_code = AES_ECB_Encrypt_Init(&AESctx, aHostCipherKey, NULL);
      if (status_code == AES_SUCCESS)
      {
        status_code = AES_ECB_Encrypt_Append(&AESctx, pInData, (int32_t)STSAFEA_HOST_KEY_LENGTH, pOutData, &outputLength);
        if (status_code == AES_SUCCESS)
        {
          status_code = AES_ECB_Encrypt_Finish(&AESctx, &pOutData[outputLength], &outputLength);
        }
      }
      break;

    default:
      status_code = 1;
      break;
  }

  return status_code;
#else
  return 1;
#endif /* INCLUDE_ECB - INCLUDE_ENCRYPTION */
}

/**
  * @brief   StSafeA_AES_CBC_Encrypt
  *          AES CBC Encryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pInData : plain data buffer
  * @param   InDataLength : plain data buffer length
  * @param   pOutData : encrypted output data buffer
  * @param   InInitialValue : initial value
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise
  */
int32_t StSafeA_AES_CBC_Encrypt(uint8_t *pInData, uint16_t InDataLength, uint8_t *pOutData,
                                uint8_t *InInitialValue, uint8_t InAesType)
{
#if defined INCLUDE_CBC && defined INCLUDE_ENCRYPTION
  int32_t status_code;
  AESCBCctx_stt AESctx;
  int32_t outputLength = 0;
  uint32_t keySize;

  switch (InAesType)
  {
    case STSAFEA_KEY_TYPE_AES_128:
    case STSAFEA_KEY_TYPE_AES_256:
      AESctx.mFlags = E_SK_DEFAULT;
      keySize = STSAFEA_AES_KEY_BITSIZE((uint32_t)InAesType) / 8U;
      AESctx.mKeySize = (int32_t)keySize;
      AESctx.mIvSize = 16;
      status_code = AES_CBC_Encrypt_Init(&AESctx, aHostCipherKey, InInitialValue);
      if (status_code == AES_SUCCESS)
      {
        status_code = AES_CBC_Encrypt_Append(&AESctx, pInData, (int32_t)InDataLength, pOutData, &outputLength);
        if (status_code == AES_SUCCESS)
        {
          status_code = AES_CBC_Encrypt_Finish(&AESctx, &pOutData[outputLength], &outputLength);
        }
      }
      break;

    default:
      status_code = 1;
      break;
  }

  return status_code;
#else
  return 1;
#endif /* INCLUDE_CBC - INCLUDE_ENCRYPTION */
}

/**
  * @brief   StSafeA_AES_CBC_Decrypt
  *          AES CBC Decryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware
  *
  * @param   pInData : encrypted data buffer
  * @param   InDataLength : encrypted data buffer length
  * @param   pOutData : plain output data buffer
  * @param   InInitialValue : initial value
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise
  */
int32_t StSafeA_AES_CBC_Decrypt(uint8_t *pInData, uint16_t InDataLength, uint8_t *pOutData,
                                uint8_t *InInitialValue, uint8_t InAesType)
{
#if defined INCLUDE_CBC && defined INCLUDE_DECRYPTION
  int32_t status_code;
  AESCBCctx_stt AESctx;
  int32_t outputLength = 0;
  uint32_t keySize;

  switch (InAesType)
  {
    case STSAFEA_KEY_TYPE_AES_128:
    case STSAFEA_KEY_TYPE_AES_256:
      AESctx.mFlags = E_SK_DEFAULT;
      keySize = STSAFEA_AES_KEY_BITSIZE((uint32_t)InAesType) / 8U;
      AESctx.mKeySize = (int32_t)keySize;
      AESctx.mIvSize = 16;
      status_code = AES_CBC_Decrypt_Init(&AESctx, aHostCipherKey, InInitialValue);
      if (status_code == AES_SUCCESS)
      {
        status_code = AES_CBC_Decrypt_Append(&AESctx, pInData, (int32_t)InDataLength, pOutData, &outputLength);
        if (status_code == AES_SUCCESS)
        {
          status_code = AES_CBC_Decrypt_Finish(&AESctx, &pOutData[outputLength], &outputLength);
        }
      }
      break;

    default:
      status_code = 1;
      break;
  }

  return status_code;
#else
  return 1;
#endif /* INCLUDE_CBC - INCLUDE_DECRYPTION */
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
