/**
  ******************************************************************************
  * @file    stsafea_crypto.c
  * @author  SMD/AME application teams
  * @version V3.3.1
  * @brief   STSAFE-A Middleware Crypto module.
  *          Provide Crypto services such as communication MAC, SHA computation
  *          Key access and other crypto functions.
  *          Provide __weak implementation of crypto interface APIs that must be
  *          implemented at application level.
  *
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
#include "stsafea_crypto.h"
#include <string.h>

/** MISRA C:2012 deviation rule has been granted for following rules:
  * - Rule-14.3_b - Medium: Conditional expression is always false.
  */


/** @addtogroup STSAFE_A1XX_CORE_MODULES
  * @{
  */


/** @addtogroup CRYPTO
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/** @addtogroup CRYPTO_Private_Types
  * @{
  */
/* Initial value type */
/**
  * @brief Initial value types enumeration.
  */
typedef enum
{
  CMAC_COMPUTATION = 0,
  RMAC_COMPUTATION = 1,
  C_ENCRYPTION     = 2,
  R_ENCRYPTION     = 3
} InitialValue;
/**
  * @}
  */


/* Private defines -----------------------------------------------------------*/
/** @addtogroup CRYPTO_Private_Constants
  * @{
  */
#define STSAFEA_CMD_TAG                                  ((uint8_t)0x01) /*!< CMD Tag for SHA processing */
#define STSAFEA_RSP_TAG                                  ((uint8_t)0x02) /*!< RSP Tag for SHA processing */
#define STSAFEA_INITIAL_VALUE_SIZE                       STSAFEA_HOST_KEY_LENGTH
#define STSAFEA_HOST_SECURE_CHANNEL_MODULUS              STSAFEA_HOST_KEY_LENGTH
#define STSAFEA_MAC_PACKET_SIZE                          16U            /*!<For compatibility with ST X-CUBE-CRYPTOLIB */
#define STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE     (sizeof(pStSafeA->MacCounter) + \
                                                          sizeof(pStSafeA->InOutBuffer.Header) + \
                                                          sizeof(pStSafeA->InOutBuffer.LV.Length))
#define STSAFEA_KNOWN_INPUT_TO_RMAC_COMPUTATION_SIZE     (sizeof(pStSafeA->InOutBuffer.Header) + \
                                                          sizeof(pStSafeA->InOutBuffer.LV.Length))
/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/** @addtogroup CRYPTO_Private_Variables
  * @{
  */
static void *pAesRMacCtx = NULL;
static uint8_t aRMacBuffer[16];
static uint8_t RMacBufferSize;
/**
  * @}
  */


/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void  ComputeInitialValue(StSafeA_Handle_t *pStSafeA, InitialValue InSubject, uint8_t *pOutInitialValue);
static void  StSafeA_Copy_TLVBuffer(uint8_t *pDest, StSafeA_TLVBuffer_t *pSrcTLV, uint16_t Size);
static const uint8_t *StSafeA_Memrchr(void *pSource, uint8_t CharToFind, size_t Size);
#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
  static StSafeA_ResponseCode_t StSafeA_MAC_SHA_PreProcess(StSafeA_Handle_t *pStSafeA);
  static StSafeA_ResponseCode_t StSafeA_MAC_SHA_PostProcess(StSafeA_Handle_t *pStSafeA);
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

/* Functions Definition ------------------------------------------------------*/
/** @addtogroup CRYPTO_Exported_Functions
  * @{
  */


/**
  * @brief   StSafeA_InitHASH
  *          Executes the SHA Initialization. Used for STSAFE-A1xx signature session.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  None
  */
void StSafeA_InitHASH(StSafeA_Handle_t *pStSafeA)
{
  if (pStSafeA != NULL)
  {
    StSafeA_SHA_Init(pStSafeA->HashObj.HashType, &pStSafeA->HashObj.HashCtx);
  }
}

/**
  * @brief   StSafeA_ComputeHASH
  *          Compute the HASH value. Used for STSAFE-A1xx signature session.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  None
  */
void StSafeA_ComputeHASH(StSafeA_Handle_t *pStSafeA)
{
  if ((pStSafeA != NULL) && (pStSafeA->HashObj.HashCtx != NULL))
  {
    StSafeA_SHA_Final(pStSafeA->HashObj.HashType, &pStSafeA->HashObj.HashCtx, pStSafeA->HashObj.HashRes);
  }
}

/**
  * @brief   StSafeA_ComputeCMAC
  *          Compute the CMAC value. Used on prepared command.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  None
  */
void StSafeA_ComputeCMAC(StSafeA_Handle_t *pStSafeA)
{
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    uint8_t host_mac_computation = pStSafeA->InOutBuffer.Header & (uint8_t)STSAFEA_CMD_HEADER_SCHN_HOSTEN;
    void *p_aes_cmac_ctx = NULL;

    uint16_t length;

    /* C-MAC computation */
    StSafeA_AES_MAC_Start(&p_aes_cmac_ctx);

    /* Compute IV for Host C-MAC */
    if (host_mac_computation != 0U)
    {
      ComputeInitialValue(pStSafeA, CMAC_COMPUTATION, aRMacBuffer);
      StSafeA_AES_MAC_Update(aRMacBuffer, STSAFEA_MAC_PACKET_SIZE, p_aes_cmac_ctx);
    }

    /* Payload  */
    length = pStSafeA->InOutBuffer.LV.Length;

    aRMacBuffer[0] = (host_mac_computation == 0U) ? ((uint8_t)pStSafeA->MacCounter & 0x7FU) : 0x00U;
    StSafeA_Copy_TLVBuffer(&aRMacBuffer[1], &pStSafeA->InOutBuffer, STSAFEA_MAC_PACKET_SIZE - 1U);

    if (length > (STSAFEA_MAC_PACKET_SIZE - (uint16_t)STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE))
    {
      StSafeA_AES_MAC_Update(aRMacBuffer, STSAFEA_MAC_PACKET_SIZE, p_aes_cmac_ctx);

      StSafeA_AES_MAC_LastUpdate(&pStSafeA->InOutBuffer.LV.Data[STSAFEA_MAC_PACKET_SIZE -
                                                                STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE],
                                 length - STSAFEA_MAC_PACKET_SIZE +
                                          (uint16_t)STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE,
                                 p_aes_cmac_ctx);
    }
    else
    {
      StSafeA_AES_MAC_LastUpdate(aRMacBuffer, length + (uint16_t)STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE,
                                 p_aes_cmac_ctx);
    }

    StSafeA_AES_MAC_Final(&pStSafeA->InOutBuffer.LV.Data[length], &p_aes_cmac_ctx);

    /* R-MAC computation */
    if ((pStSafeA->InOutBuffer.Header & (uint8_t)STSAFEA_CMD_HEADER_RMACEN) == (uint8_t)STSAFEA_CMD_HEADER_RMACEN)
    {
      pAesRMacCtx = NULL;
      RMacBufferSize = 0;

      StSafeA_AES_MAC_Start(&pAesRMacCtx);

      /* Compute IV for Host R-MAC */
      if (host_mac_computation != 0U)
      {
        ComputeInitialValue(pStSafeA, RMAC_COMPUTATION, aRMacBuffer);
        StSafeA_AES_MAC_Update(aRMacBuffer, STSAFEA_MAC_PACKET_SIZE, pAesRMacCtx);
      }

      /* C-MAC computation */
      aRMacBuffer[0] = (host_mac_computation == 0U) ? ((pStSafeA->MacCounter + 1U) | 0x80U) : 0x80U;
      StSafeA_Copy_TLVBuffer(&aRMacBuffer[1], &pStSafeA->InOutBuffer, STSAFEA_MAC_PACKET_SIZE - 1U);

      if (length > (STSAFEA_MAC_PACKET_SIZE - STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE))
      {
        StSafeA_AES_MAC_Update(aRMacBuffer, STSAFEA_MAC_PACKET_SIZE, pAesRMacCtx);

        StSafeA_AES_MAC_Update(&pStSafeA->InOutBuffer.LV.Data[STSAFEA_MAC_PACKET_SIZE -
                                                              STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE],
                               ((uint16_t)(((uint32_t)length - STSAFEA_MAC_PACKET_SIZE +
                                            STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE) / STSAFEA_MAC_PACKET_SIZE)) *
                               STSAFEA_MAC_PACKET_SIZE,
                               pAesRMacCtx);

        RMacBufferSize = (uint8_t)((uint32_t)length - STSAFEA_MAC_PACKET_SIZE + STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE) %
                         STSAFEA_MAC_PACKET_SIZE;
        (void)memcpy(aRMacBuffer, &pStSafeA->InOutBuffer.LV.Data[length - RMacBufferSize], RMacBufferSize);
      }
      else
      {
        RMacBufferSize = (uint8_t)(length + STSAFEA_KNOWN_INPUT_TO_CMAC_COMPUTATION_SIZE);
      }
    }

    pStSafeA->InOutBuffer.LV.Length += STSAFEA_MAC_LENGTH;
  }
}

/**
  * @brief   StSafeA_ComputeRMAC
  *          Compute the RMAC value. Used on received response.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  None
  */
void StSafeA_ComputeRMAC(StSafeA_Handle_t *pStSafeA)
{
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    uint16_t length = pStSafeA->InOutBuffer.LV.Length;

    StSafeA_Copy_TLVBuffer(&aRMacBuffer[RMacBufferSize], &pStSafeA->InOutBuffer,
                           STSAFEA_MAC_PACKET_SIZE - (uint16_t)RMacBufferSize);

    if ((length + RMacBufferSize) > (uint16_t)(STSAFEA_MAC_PACKET_SIZE - STSAFEA_KNOWN_INPUT_TO_RMAC_COMPUTATION_SIZE))
    {
      StSafeA_AES_MAC_Update(aRMacBuffer, STSAFEA_MAC_PACKET_SIZE, pAesRMacCtx);

      StSafeA_AES_MAC_LastUpdate(&pStSafeA->InOutBuffer.LV.Data[(uint32_t)STSAFEA_MAC_PACKET_SIZE - RMacBufferSize -
                                                                STSAFEA_KNOWN_INPUT_TO_RMAC_COMPUTATION_SIZE],
                                 length + RMacBufferSize - STSAFEA_MAC_PACKET_SIZE +
                                          (uint16_t)STSAFEA_KNOWN_INPUT_TO_RMAC_COMPUTATION_SIZE,
                                 pAesRMacCtx);
    }
    else
    {
      StSafeA_AES_MAC_LastUpdate(aRMacBuffer,
                                 length + RMacBufferSize + (uint16_t)STSAFEA_KNOWN_INPUT_TO_RMAC_COMPUTATION_SIZE,
                                 pAesRMacCtx);
    }

    StSafeA_AES_MAC_Final(&pStSafeA->InOutBuffer.LV.Data[length], &pAesRMacCtx);

    pStSafeA->InOutBuffer.LV.Length += STSAFEA_MAC_LENGTH;
  }
}

/**
  * @brief   StSafeA_DataEncryption
  *          Executes data encryption (AES CBC) on the STSAFE-A1xx data buffer.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success,  an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_DataEncryption(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  uint8_t padding_length;

  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    uint8_t initial_value[STSAFEA_INITIAL_VALUE_SIZE];
    /* Compute initial value */
    ComputeInitialValue(pStSafeA, C_ENCRYPTION, initial_value);
    status_code = ((StSafeA_AES_ECB_Encrypt(initial_value, initial_value,
                                            STSAFEA_KEY_TYPE_AES_128) == 0) ? STSAFEA_OK : STSAFEA_CRYPTO_LIB_ISSUE);

    /* Encrypt data */
    if (status_code == STSAFEA_OK)
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = 0x80;
      pStSafeA->InOutBuffer.LV.Length++;
      padding_length = STSAFEA_HOST_SECURE_CHANNEL_MODULUS -
                       (uint8_t)(pStSafeA->InOutBuffer.LV.Length % (STSAFEA_HOST_SECURE_CHANNEL_MODULUS));
      (void)memset(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length],
                   0x00,
                   padding_length * sizeof(padding_length));
      pStSafeA->InOutBuffer.LV.Length += padding_length;

      status_code = (StSafeA_AES_CBC_Encrypt(&pStSafeA->InOutBuffer.LV.Data[0],
                                             pStSafeA->InOutBuffer.LV.Length,
                                             &pStSafeA->InOutBuffer.LV.Data[0],
                                             initial_value,
                                             STSAFEA_KEY_TYPE_AES_128) == 0) ? STSAFEA_OK : STSAFEA_CRYPTO_LIB_ISSUE;
    }
  }
  return status_code;
}

/**
  * @brief   StSafeA_DataDecryption
  *          Executes data decryption (AES CBC) on the STSAFE-A1xx data buffer.
  *
  * @param   pStSafeA : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success,  an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_DataDecryption(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  const uint8_t *p_padding;

  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    uint8_t initial_value[STSAFEA_INITIAL_VALUE_SIZE];

    /* Compute initial value */
    ComputeInitialValue(pStSafeA, R_ENCRYPTION, initial_value);
    status_code = ((StSafeA_AES_ECB_Encrypt(initial_value, initial_value,
                                            STSAFEA_KEY_TYPE_AES_128) == 0) ? STSAFEA_OK : STSAFEA_CRYPTO_LIB_ISSUE);

    /* Decrypt data */
    if (status_code == STSAFEA_OK)
    {
      status_code = (StSafeA_AES_CBC_Decrypt(&pStSafeA->InOutBuffer.LV.Data[0],
                                             pStSafeA->InOutBuffer.LV.Length,
                                             &pStSafeA->InOutBuffer.LV.Data[0],
                                             initial_value,
                                             STSAFEA_KEY_TYPE_AES_128) == 0) ? STSAFEA_OK : STSAFEA_CRYPTO_LIB_ISSUE;

      if (status_code == STSAFEA_OK)
      {
        p_padding = StSafeA_Memrchr(&pStSafeA->InOutBuffer.LV.Data[0], 0x80,
                                                     pStSafeA->InOutBuffer.LV.Length);

        uint32_t padding_pos = (uint32_t)&p_padding[0];
        uint32_t buffer_pos  = (uint32_t)&pStSafeA->InOutBuffer.LV.Data[0];
        uint16_t new_length  = (uint16_t)(padding_pos - buffer_pos);
        if ((p_padding == NULL) ||
            (padding_pos > (uint32_t)(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length - 1U])) ||
            (new_length < (pStSafeA->InOutBuffer.LV.Length - (uint16_t)(STSAFEA_HOST_SECURE_CHANNEL_MODULUS))))
        {
          status_code = STSAFEA_CRYPTO_LIB_ISSUE;
        }
        else
        {
          pStSafeA->InOutBuffer.LV.Length = new_length;
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_MAC_SHA_PrePostProcess
  *          Pre or Post Process the MAC and/or SHA before transmitting or after receiving from STSAFE_A1xx device.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer
  * @param   MacShaProcessType : Specify if the function is called for command pre-processing or response post-processing.
  *          This parameter can be one of the StSafeA_MacShaProcess_t enum values:
  *            @arg STSAFEA_MAC_SHA_PRE_PROCESS : for command pre-processing
  *            @arg STSAFEA_MAC_SHA_POST_PROCESS: for response post-processing
  * @retval  STSAFEA_OK if success,  an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_MAC_SHA_PrePostProcess(StSafeA_Handle_t *pStSafeA,
                                                      StSafeA_MacShaProcess_t MacShaProcessType)
{
#if (STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
  STSAFEA_UNUSED_PTR(pStSafeA); 
  STSAFEA_UNUSED_VAR(MacShaProcessType); 
  return STSAFEA_OK;
#else
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    switch (MacShaProcessType)
    {
      case STSAFEA_MAC_SHA_PRE_PROCESS:
        /* Compute & concatenate CMAC to buffer */
        status_code = StSafeA_MAC_SHA_PreProcess(pStSafeA);
        break;

      case STSAFEA_MAC_SHA_POST_PROCESS:
        /* Compute & compare RMAC */
        status_code = StSafeA_MAC_SHA_PostProcess(pStSafeA);
        break;

      default:
        break;
    }

#if (USE_SIGNATURE_SESSION)
    if ((status_code == STSAFEA_OK) && (pStSafeA->HashObj.HashCtx != NULL))
    {
      uint16_t length;
      uint8_t  hash_buffer[STSAFEA_HASH_LENGTH];
      /* Compute command hash */
      if (MacShaProcessType == STSAFEA_MAC_SHA_PRE_PROCESS)
      {
        hash_buffer[0] = STSAFEA_CMD_TAG;
      }
      else
      {
        hash_buffer[0] = STSAFEA_RSP_TAG;
      }

      length = (uint16_t)SWAP2BYTES((uint32_t)pStSafeA->InOutBuffer.LV.Length + 1U);
      (void)memcpy(&hash_buffer[1], &length, sizeof(length));
      hash_buffer[3] = pStSafeA->InOutBuffer.Header;

      StSafeA_SHA_Update(pStSafeA->HashObj.HashType,
                         pStSafeA->HashObj.HashCtx,
                         hash_buffer,
                         sizeof(hash_buffer));

      StSafeA_SHA_Update(pStSafeA->HashObj.HashType,
                         pStSafeA->HashObj.HashCtx,
                         &pStSafeA->InOutBuffer.LV.Data[0],
                         pStSafeA->InOutBuffer.LV.Length);
    }
#endif /* USE_SIGNATURE_SESSION */
  }
  return status_code;
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

}

/**
  * @}
  */


/* Interface weak functions definitions --------------------------------------*/

/** @addtogroup CRYPTO_Interface_Functions
  *  @brief    These functions MUST be implemented at application layer
  * @{
  */

/**
  * @brief   StSafeA_HostKeys_Init
  *          Initialize STSAFE-A1xx Host MAC and Cipher Keys that will be used by the crypto interface layer.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   None
  * @retval  0 if success. An error code otherwise.
  */
__weak int32_t StSafeA_HostKeys_Init(void)
{
#if (STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
  return 0;
#else
  return 1;
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
}

/**
  * @brief   StSafeA_SHA_Init
  *          SHA initialization function to initialize the SHA context
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   InHashType : type of SHA.
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   ppShaCtx : SHA context to be initialized.
  * @retval  None
  */
__weak void StSafeA_SHA_Init(StSafeA_HashTypes_t InHashType, void **ppShaCtx)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_VAR(InHashType);
  STSAFEA_UNUSED_PTR(ppShaCtx);
}

/**
  * @brief   StSafeA_SHA_Update
  *          SHA update function to process SHA over a message data buffer.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   InHashType : type of SHA.
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   pShaCtx : SHA context.
  * @param   pInMessage : message data buffer.
  * @param   InMessageLength : message data buffer length.
  * @retval  None
  */
__weak void StSafeA_SHA_Update(StSafeA_HashTypes_t InHashType, void *pShaCtx, uint8_t *pInMessage,
                               uint32_t InMessageLength)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_VAR(InHashType);
  STSAFEA_UNUSED_PTR(pShaCtx);
  STSAFEA_UNUSED_PTR(pInMessage);
  STSAFEA_UNUSED_VAR(InMessageLength);
}

/**
  * @brief   StSafeA_SHA_Final
  *          SHA final function to finalize the SHA Digest.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   InHashType : type of SHA.
  *          This parameter can be one of the StSafeA_HashTypes_t enum values:
  *            @arg STSAFEA_SHA_256: 256-bits
  *            @arg STSAFEA_SHA_384: 384-bits
  * @param   ppShaCtx : SHA context to be finalized.
  * @param   pMessageDigest : message digest data buffer.
  * @retval  None
  */
__weak void StSafeA_SHA_Final(StSafeA_HashTypes_t InHashType, void **ppShaCtx, uint8_t *pMessageDigest)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_VAR(InHashType);
  STSAFEA_UNUSED_PTR(ppShaCtx);
  STSAFEA_UNUSED_PTR(pMessageDigest);
}

/**
  * @brief   StSafeA_AES_MAC_Start
  *          Start AES MAC computation
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   ppAesMacCtx : AES MAC context.
  * @retval  None
  */
__weak void StSafeA_AES_MAC_Start(void **ppAesMacCtx)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(ppAesMacCtx);
}

/**
  * @brief   StSafeA_AES_MAC_Update
  *          Update / Add data to MAC computation.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pInData : data buffer.
  * @param   InDataLength : data buffer length.
  * @param   pAesMacCtx : AES MAC context.
  * @retval  None
  */
__weak void StSafeA_AES_MAC_Update(uint8_t *pInData, uint16_t InDataLength, void *pAesMacCtx)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pInData);
  STSAFEA_UNUSED_VAR(InDataLength);
  STSAFEA_UNUSED_PTR(pAesMacCtx);
}

/**
  * @brief   StSafeA_AES_MAC_LastUpdate
  *          Update / Add data to MAC computation.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pInData : data buffer.
  * @param   InDataLength : data buffer length.
  * @param   pAesMacCtx : AES MAC context.
  * @retval  None
  */
__weak void StSafeA_AES_MAC_LastUpdate(uint8_t *pInData, uint16_t InDataLength, void *pAesMacCtx)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pInData);
  STSAFEA_UNUSED_VAR(InDataLength);
  STSAFEA_UNUSED_PTR(pAesMacCtx);
}

/**
  * @brief   StSafeA_AES_MAC_Final
  *          Finalize AES MAC computation.
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pOutMac : calculated MAC.
  * @param   ppAesMacCtx : AES MAC context.
  * @retval  None
  */
__weak void StSafeA_AES_MAC_Final(uint8_t *pOutMac, void **ppAesMacCtx)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pOutMac);
  STSAFEA_UNUSED_PTR(ppAesMacCtx);
}

/**
  * @brief   StSafeA_AES_ECB_Encrypt
  *          AES ECB Encryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pInData : plain data buffer.
  * @param   pOutData : encrypted output data buffer.
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise.
  */
__weak int32_t StSafeA_AES_ECB_Encrypt(uint8_t *pInData, uint8_t *pOutData, uint8_t InAesType)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pInData);
  STSAFEA_UNUSED_PTR(pOutData);
  STSAFEA_UNUSED_VAR(InAesType);
  return 1;
}

/**
  * @brief   StSafeA_AES_CBC_Encrypt
  *          AES CBC Encryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pInData : plain data buffer.
  * @param   InDataLength : plain data buffer length.
  * @param   pOutData : encrypted output data buffer.
  * @param   InInitialValue : initial value.
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise.
  */
__weak int32_t StSafeA_AES_CBC_Encrypt(uint8_t *pInData, uint16_t InDataLength, uint8_t *pOutData,
                                       uint8_t *InInitialValue, uint8_t InAesType)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pInData);
  STSAFEA_UNUSED_VAR(InDataLength);
  STSAFEA_UNUSED_PTR(pOutData);
  STSAFEA_UNUSED_PTR(InInitialValue);
  STSAFEA_UNUSED_VAR(InAesType);
  return 1;
}

/**
  * @brief   StSafeA_AES_CBC_Decrypt
  *          AES CBC Decryption
  * @note    This is a weak function that MUST be implemented at application interface level.
  *          A specific example template stsafea_crypto_xxx_interface_template.c is provided with this Middleware.
  *
  * @param   pInData : encrypted data buffer.
  * @param   InDataLength : encrypted data buffer length.
  * @param   pOutData : plain output data buffer.
  * @param   InInitialValue : initial value.
  * @param   InAesType : type of AES. Can be one of the following values:
  *            @arg STSAFEA_KEY_TYPE_AES_128: AES 128-bits
  *            @arg STSAFEA_KEY_TYPE_AES_256: AES 256-bits
  * @retval  0 if success, an error code otherwise.
  */
__weak int32_t StSafeA_AES_CBC_Decrypt(uint8_t *pInData, uint16_t InDataLength, uint8_t *pOutData,
                                       uint8_t *InInitialValue, uint8_t InAesType)
{
  /* Prevent unused argument(s) compilation warning */
  STSAFEA_UNUSED_PTR(pInData);
  STSAFEA_UNUSED_VAR(InDataLength);
  STSAFEA_UNUSED_PTR(pOutData);
  STSAFEA_UNUSED_PTR(InInitialValue);
  STSAFEA_UNUSED_VAR(InAesType);
  return 1;
}

/**
  * @}
  */


/* Private functions definitions ---------------------------------------------*/
/** @addtogroup CRYPTO_Private_Functions
  * @{
  */

/**
  * @brief   ComputeInitialValue
  *          Compute the initial value used for CMAC/RMAC computation. C-Encryption or R-Encryption.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer.
  * @param   InSubject         : One of the  InitialValue enum values.
  * @param   pOutInitialValue  : The data pointer to be filled with the computed value.
  * @retval  None
  */
static void ComputeInitialValue(StSafeA_Handle_t *pStSafeA, InitialValue InSubject, uint8_t *pOutInitialValue)
{
  uint32_t host_cmac_sequence_counter = pStSafeA->HostMacSequenceCounter;

  if (InSubject != CMAC_COMPUTATION)
  {
    host_cmac_sequence_counter++;
  }

  pOutInitialValue[0] = (uint8_t)(host_cmac_sequence_counter >> 16) & 0xFFU;
  pOutInitialValue[1] = (uint8_t)(host_cmac_sequence_counter >> 8) & 0xFFU;
  pOutInitialValue[2] = (uint8_t)(host_cmac_sequence_counter & 0xFFU);
  pOutInitialValue[3] = (uint8_t)(((uint8_t)InSubject << 6) & 0xC0U);
  pOutInitialValue[4] = 0x80U;
  (void)memset(&pOutInitialValue[5], 0x00, (STSAFEA_INITIAL_VALUE_SIZE) - 5U);
}


#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)

/**
  * @brief   StSafeA_MAC_SHA_PreProcess
  *          Pre Process the command to send to STSAFE_A device computing and concatenating the C-MAC.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success,  an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_MAC_SHA_PreProcess(StSafeA_Handle_t *pStSafeA)
{
  if ((pStSafeA->InOutBuffer.Header & (uint8_t)STSAFEA_MAC_HOST_CMAC) == (uint8_t)STSAFEA_MAC_HOST_CMAC)
  {
    StSafeA_ComputeCMAC(pStSafeA);
  }
  return STSAFEA_OK;
}

/**
  * @brief   StSafeA_MAC_SHA_PostProcess
  *          Post Process the response received from STSAFE_A device checking the R-MAC.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success,  an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_MAC_SHA_PostProcess(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_OK;
  if (((uint8_t)pStSafeA->InOutBuffer.Header & (uint8_t)STSAFEA_CMD_HEADER_RMACEN) ==
                                                                     (uint8_t)STSAFEA_CMD_HEADER_RMACEN)
  {
    uint8_t a_rmac[STSAFEA_MAC_LENGTH];
    pStSafeA->InOutBuffer.LV.Length -= STSAFEA_MAC_LENGTH;
    (void)memcpy(a_rmac,
                 &pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length],
                 sizeof(a_rmac));
    StSafeA_ComputeRMAC(pStSafeA);

    if (memcmp(a_rmac,
               &pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length - STSAFEA_MAC_LENGTH],
               sizeof(a_rmac)) != 0)
    {
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length - STSAFEA_MAC_LENGTH],
                   a_rmac,
                   sizeof(a_rmac));
      status_code = STSAFEA_INVALID_RMAC;
    }
    else
    {
      pStSafeA->InOutBuffer.LV.Length -= STSAFEA_MAC_LENGTH;
    }
  }
  return status_code;
}

#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

/**
  * @brief   StSafeA_Copy_TLVBuffer
  *          Copy data from a TLV structure to a uint8_t array. Needed for CMAC, RMAC computation.
  *
  * @param   pDest : char array to copy data to.
  * @param   pSrcTLV : Source TLV structure pointer to copy data from.
  * @param   Size : size of data to copy in bytes.
  * @retval  None
  */
static void StSafeA_Copy_TLVBuffer(uint8_t *pDest, StSafeA_TLVBuffer_t *pSrcTLV, uint16_t Size)
{
  if ((pDest != NULL) && (pSrcTLV != NULL)  && (Size > 3U))
  {
    pDest[0] = pSrcTLV->Header;
    pDest[1] = (uint8_t)((pSrcTLV->LV.Length >> 8) & 0xFFU);
    pDest[2] = (uint8_t)pSrcTLV->LV.Length & 0xFFU;
    (void)memcpy(&pDest[3], pSrcTLV->LV.Data, (uint32_t)(Size) - 3U);
  }
}


/**
  * @brief   StSafeA_Memrchr
  *          Reverse memchr to find the last occurrence of 'c' in the buffer 's' of size 'n'.
  *
  * @param   pSource    : buffer pointer to search into.
  * @param   CharToFind : char to find.
  * @param   Size       : size.
  * @retval  pointer to the found char. NULL otherwise.
  */
static const uint8_t *StSafeA_Memrchr(void *pSource, uint8_t CharToFind, size_t Size)
{
  const uint8_t * pRet = NULL;
  uint8_t CharFound = 0U;
  /* Check input parameter */
  if (pSource != NULL)
  {
    uint8_t *p = (uint8_t *)pSource;
    uint32_t i = Size - 1U;

    while ((&p[i] != pSource) && (CharFound == 0U))
    {
      if (p[i] == CharToFind)
      {
        pRet = &p[i];
        CharFound = 1;
        
      }

      i--;
    }
  }
  return pRet;
}


/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
