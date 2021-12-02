/**
  ******************************************************************************
  * @file    stsafea_core.c
  * @author  SMD/AME application teams
  * @version V3.3.2
  * @brief   STSAFE-A Middleware Core module.
  *          Provide all the core services related to the features offered by
  *          the STSAFE-A device such as:
  *           + Initialization and Configuration functions
  *           + General Purpose commands
  *           + Data Partition commands
  *           + Companion commands
  *           + Private and Public Key commands
  *           + Administrative commands
  *           + Additional commands
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
#include "stsafea_core.h"
#include "stsafea_crypto.h"
#include "stsafea_service.h"
#include <string.h>

/** MISRA C:2012 deviation rule has been granted for following rules:
  * - Rule-14.3_a - Medium: Conditional expression is always true.
  *
  * And add for the following rule if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1
  * - Rule-14.3_a - Medium: Conditional expression is always true.
  * - Rule-11.3   - A pointer to object type is cast to a pointer to different object type
  */


/** @addtogroup STSAFE_A1XX_CORE_MODULES
  * @{
  */


/** @addtogroup CORE
  * @{
  */


/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/** @addtogroup CORE_Private_Constants
  * @{
  */
/**
  * @brief STSAFEA Middleware version number
  */
#define STSAFEA_VERSION        ((STSAFEA_VERSION_MAIN << 24U) | \
                                (STSAFEA_VERSION_SUB1 << 16U) | \
                                (STSAFEA_VERSION_SUB2 <<  8U) | \
                                (STSAFEA_VERSION_RC))

#define STSAFEA_CRC_SUPPORT                                             ((uint8_t)1)            /*!< Crc support flag */
#define STSAFEA_PRIVATE_KEY_MODE_OF_OPERATION_CHANGE_RIGHT_MASK         ((uint16_t)0x8000)      /*!< Mask */
#define STSAFEA_PRIVATE_KEY_MODE_OF_OPERATION_AUTHORIZATION_FLAGS_MASK  ((uint16_t)0x000D)      /*!< Mask */
#define STSAFEA_PASSWORD_SIZE                                           ((uint16_t)16)          /*!< Password size */
#define STSAFEA_ONE_WAY_COUNTER_PRESENCE                                ((uint8_t)1)            /*!< One way counter presence */
#define STSAFEA_HOST_CMAC_INVALID_COUNTER                               ((uint32_t)0x00FFFFFF)  /*!< Invalid counter */

#define STSAFEA_RANDOM_ENTITY_AUTH_RESPONSE_LENGTH                      ((uint16_t)16)      /*!< Response length to random command for entity authentication */
#define STSAFEA_DECREMENT_RESPONSE_LENGTH                               ((uint16_t)4)       /*!< Response length to decrement command */
#define STSAFEA_VERIFY_MSG_SIGNATURE_RESPONSE_LENGTH                    ((uint16_t)1)       /*!< Response length to verify message authentication command */
#define STSAFEA_VERIFY_ENTITY_SIGNATURE_RESPONSE_LENGTH                 ((uint16_t)1)       /*!< Response length to verify entity authentication command */
#define STSAFEA_PRODUCT_DATA_QUERY_RESPONSE_LENGTH                      ((uint16_t)39)      /*!< Response length to product data query */
#define STSAFEA_DATA_PARTITION_QUERY_MIN_RESPONSE_LENGTH                ((uint16_t)1)       /*!< Minimum response length to data partition query */
#define STSAFEA_I2C_PARAMETERS_QUERY_RESPONSE_LENGTH                    ((uint16_t)2)       /*!< Response length to I2C parameters query */
#define STSAFEA_LIFE_CYCLE_STATE_QUERY_RESPONSE_LENGTH                  ((uint16_t)1)       /*!< Response length to life cycle state query */
#define STSAFEA_PUBLIC_KEY_SLOT_QUERY_MIN_RESPONSE_LENGTH               ((uint16_t)1)       /*!< Minimum response length to public key slot query */
#define STSAFEA_HOST_KEY_SLOT_QUERY_RESPONSE_LENGTH                     ((uint16_t)4)       /*!< Response length to host key slot query */
#define STSAFEA_LOCAL_ENVELOPE_QUERY_MIN_RESPONSE_LENGTH                ((uint16_t)7)       /*!< Minimum response length to local envelope key slot query */
#define STSAFEA_CMD_AUTH_CONFIG_QUERY_MIN_RESPONSE_LENGTH               ((uint16_t)2)       /*!< Minimum response length to command authorization configuration query */
#define STSAFEA_WRAP_UNWRAP_ENVELOPE_ADDITIONAL_RESPONSE_LENGTH         ((uint16_t)8)       /*!< Additional 8-bytes to consider for Wrap/Unwrap commands */
#define STSAFEA_VERIFY_PASSWORD_RESPONSE_LENGTH                         ((uint16_t)2)       /*!< Response length to I2C parameters query */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#define WORKAROUND_GENERATE_SIGNATURE /* STSAFE-A1x0 hangs when hash containing only NULL bytes */

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static StSafeA_ResponseCode_t StSafeA_TransmitCommand(StSafeA_Handle_t *pStSafeA);
static StSafeA_ResponseCode_t StSafeA_ReceiveResponse(StSafeA_Handle_t *pStSafeA);
static StSafeA_ResponseCode_t StSafeA_AssignLVResponse(StSafeA_LVBuffer_t *pDestLVBuffer,
                                                       StSafeA_LVBuffer_t *pSrcLVBuffer, uint16_t ExpectedLen);
static StSafeA_ResponseCode_t StSafeA_AssignLVBuffer(StSafeA_LVBuffer_t *pDestLVBuffer, uint8_t *pDataBuffer,
                                                     uint16_t ExpectedLen);
static void StSafeA_SetLVData(StSafeA_LVBuffer_t *pDestLVBuffer, uint8_t *pSrcLVData, uint16_t Length);
static void StSafeA_BuildCommandHeaderCMAC(StSafeA_Handle_t *pStSafeA, uint8_t CommandCode, uint8_t *pMAC);
#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
static void StSafeA_GetHostMacSequenceCounter(StSafeA_Handle_t *pStSafeA);
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
StSafeA_ResponseCode_t StSafeA_Process_Update(StSafeA_Handle_t *pStSafeA, uint8_t InAtomicity,
                                              uint8_t InChangeACIndicator, uint8_t InNewUpdateACRight,
                                              uint8_t InNewUpdateAC, uint8_t InZoneIndex,
                                              uint16_t InOffset, StSafeA_LVBuffer_t *pInLVData, uint8_t InMAC);

/* Functions Definition ------------------------------------------------------*/
/** @addtogroup CORE_Exported_Functions
  * @{
  */

/** @addgroup CORE_Exported_Functions_Group1 Initialization / Configuration Commands APIs
  *  @brief    STSAFE-A Core APIs related to initialization and configuration commands
  *
@verbatim
 ===============================================================================
              ##### Initialization / Configuration Commands #####
 ===============================================================================
[..]
    (+) Init
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_Init
  *          Create, initialize and assign the STSAFE-A1xx device handle.
  * @note    No other APIs can be called/used if this one has never been called
  *
  * @param   pStSafeA : Handle pointer for STSAFE-A1xx interface to be properly initialized
  * @param   pAllocatedRxTxBufferData : Rx/Tx Buffer used to transmit/receive data to/from STSAFEA.
  *          Memory for Rx/Tx buffer must be allocated before being passed to this function
  * @retval  STSAFEA_OK if success, an error code otherwise
  */
StSafeA_ResponseCode_t StSafeA_Init(StSafeA_Handle_t *pStSafeA, uint8_t *pAllocatedRxTxBufferData)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* STSAFE MW requires a data buffer to send/receive bytes ove the bus.
     For memory optimization reasons it's up to the application to allocate it,
     so that the application can deallocate it when STSAFE services are not required anymore */
  if ((pStSafeA != NULL) && (pAllocatedRxTxBufferData != NULL))
  {
    pStSafeA->CrcSupport    = STSAFEA_CRC_SUPPORT;
    pStSafeA->MacCounter    = 0;
    pStSafeA->InOutBuffer.LV.Length = 0;
    pStSafeA->InOutBuffer.LV.Data   = pAllocatedRxTxBufferData;
    pStSafeA->HostMacSequenceCounter = STSAFEA_HOST_CMAC_INVALID_COUNTER;

    pStSafeA->HashObj.HashType = STSAFEA_SHA_256;
    pStSafeA->HashObj.HashCtx = NULL;
    (void)memset(pStSafeA->HashObj.HashRes, 0, sizeof(pStSafeA->HashObj.HashRes));

    status_code = STSAFEA_UNEXPECTED_ERROR;
    /* Initialize the Board Support Package */
    if (StSafeA_HW_Init() == 0)
    {
      /* Initialize/Retrieve the Host MAC and Cipher Keys  */
      if (StSafeA_HostKeys_Init() == 0)
      {
        status_code = STSAFEA_OK;
      }
    }
  }

  return status_code;
}

/**
  * @}
  */


/** @addgroup CORE_Exported_Functions_Group2 General Purpose Commands APIs
  *  @brief    STSAFE-A APIs related to general purpose commands
  *
@verbatim
 ===============================================================================
              ##### General Purpose Commands #####
 ===============================================================================
[..]
    (+) Get Version
    (+) Echo
    (+) Reset
    (+) Generate Random
    (+) Hibernate
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_GetVersion
  *          Returns the STSAFE-A1xx Middleware revision
  *
  * @param   None
  * @retval  int32_t version in the format 0xXYZR (8bits for each XYZ decimal, R for release candidate)\n
  *                          [31:24] main version \n
  *                          [23:16] sub1 version \n
  *                          [15:8]  sub2 version \n
  *                          [7:0]  release candidate
  */
int32_t StSafeA_GetVersion(void)
{
  return (int32_t)STSAFEA_VERSION;
}

/**
  * @brief   StSafeA_Echo
  *          Executes the echo command expecting to receive back from STSAFEA the
  *          same data passed in the command.
  *
  * @param   pStSafeA       : STSAFE-A1xx object pointer.
  * @param   pInEchoData    : Pointer to bytes array.
  * @param   InRespDataLen  : Expected data length in bytes of 'echoed' data (pOutLVResponse->Data).\n
  *                           Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutLVResponse : LV structure pointer to be filled with the response data.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC          : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Echo(
  StSafeA_Handle_t *pStSafeA,
  uint8_t *pInEchoData,
  uint16_t InRespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pInEchoData != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InRespDataLen));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_ECHO, &InMAC);

    /* Build command Data */
    STSAFEA_CHECK_SIZE(InMAC, 0U, InRespDataLen);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[0], pInEchoData, InRespDataLen);
    pStSafeA->InOutBuffer.LV.Length = InRespDataLen;

    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = InRespDataLen + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_ECHO);

      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        status_code = StSafeA_AssignLVResponse(pOutLVResponse, &pStSafeA->InOutBuffer.LV, InRespDataLen);
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_Reset
  *          Reset command executed to reset the volatile attributes to their initial value.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Reset(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_RESET, &InMAC);

    pStSafeA->InOutBuffer.LV.Length = 0U;

    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_RESET);

      status_code = StSafeA_ReceiveResponse(pStSafeA);
      if (status_code == STSAFEA_OK)
      {
        pStSafeA->MacCounter = 0;
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_GenerateRandom
  *          Generates a number of random bytes
  *
  * @param   pStSafeA       : STSAFE-A1xx object pointer.
  * @param   InRndSubject   : Random bytes type (STSAFEA_EPHEMERAL or STSAFEA_ENTITY_AUTHENTICATION).\n
  *                           Only used for STSAFE-A100.
  * @param   InRespDataLen  : Expected data length in bytes of generated random (pOutLVResponse->Data).\n
  *                           Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutLVResponse : LV structure pointer to be filled with the response data.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC          : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_GenerateRandom(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_RndSubject_t InRndSubject,
  uint8_t InRespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  uint16_t tmp_len;

#if defined(STSAFE_A110)
  (void)InRndSubject;
#endif /* STSAFE_A110 */

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
#if defined(STSAFE_A100)
    stsafea_assert_param((IS_STSAFEA_LEN(InRespDataLen)) || (InRndSubject == STSAFEA_ENTITY_AUTHENTICATION_RND));
#else
    stsafea_assert_param(IS_STSAFEA_LEN(InRespDataLen));
#endif /* STSAFE_A100 */

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_GENERATE_RANDOM, &InMAC);

#if defined(STSAFE_A100)
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InRndSubject;

    if (InRndSubject == STSAFEA_EPHEMERAL_RND)
    {
      pStSafeA->InOutBuffer.LV.Data[1] = InRespDataLen;
      pStSafeA->InOutBuffer.LV.Length = 2U;
    }
    else
    {
      pStSafeA->InOutBuffer.LV.Length = 1U;
    }
#else
    pStSafeA->InOutBuffer.LV.Data[0] = 0U;
    pStSafeA->InOutBuffer.LV.Data[1] = InRespDataLen;
    pStSafeA->InOutBuffer.LV.Length = 2U;
#endif /* STSAFE_A100 */

    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
#if defined(STSAFE_A100)
      tmp_len = (InRndSubject == STSAFEA_EPHEMERAL_RND) ? InRespDataLen : STSAFEA_RANDOM_ENTITY_AUTH_RESPONSE_LENGTH;
#else
      tmp_len = InRespDataLen;
#endif /* STSAFE_A100 */

      tmp_len += STSAFEA_R_MAC_LENGTH(InMAC);
      pStSafeA->InOutBuffer.LV.Length = tmp_len;

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_GENERATE_RANDOM);

      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        status_code = StSafeA_AssignLVResponse(pOutLVResponse, &pStSafeA->InOutBuffer.LV, InRespDataLen);
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_Hibernate
  *          This function is executed to put the STSAFE-A1xx device in hibernation.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @param   InWakeUpMode  : Wake up mode conditions.\n
  *                          Can be one of the following values:\n
  *                          @arg STSAFEA_WAKEUP_FROM_I2C_START_OR_RESET: Wake up from hibernate after I2C start condition or after reset.\n
  *                          @arg STSAFEA_WAKEUP_FROM_RESET: Wake up from hibernate after reset only.
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Hibernate(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InWakeUpMode,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_WAKEUP_MODE(InWakeUpMode));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_HIBERNATE, &InMAC);

    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InWakeUpMode;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_HIBERNATE);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }
  return status_code;
}

/**
  * @}
  */


/** @addgroup CORE_Exported_Functions_Group3 Data Partition Commands APIs
  *  @brief    STSAFE-A APIs related to data partition commands
  *
@verbatim
 ===============================================================================
              ##### Data Partition Commands #####
 ===============================================================================
[..]
    (+) Data Partition Query
    (+) Decrement
    (+) Read
    (+) Update
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_DataPartitionQuery
  *          Query command to retrieve data partition configuration.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer.
  * @param   InZoneMaxNum      : Expected number of zones.\n
  *                              Must correspond to the num of zones allocated for pOutDataPartition->pZoneInfoRecord, if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.\n
  *                              If unknown it's possible to specify 0 (zero). In this case the num of zones will be returned in pOutDataPartition->NumberOfZones and\n
  *                              the function can then be called once again with the right InZoneMaxNum value.\n
  *                              Specify 0xFF if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1.
  * @param   pOutDataPartition : StSafeA_DataPartitionBuffer_t structure pointer to be filled with the response data partition.\n
  *                              If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the internal StSafeA_ZoneInformationRecordBuffer_t structure array must have been properly allocated.\n
  *                              If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned to pOutDataPartition->pZoneInfoRecord.
  * @param   InMAC             : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_DataPartitionQuery(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InZoneMaxNum,
  StSafeA_DataPartitionBuffer_t *pOutDataPartition,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)         &&
      (pOutDataPartition != NULL)                    &&
      (IS_STSAFEA_CONDITIONAL_VALID_PTR(pOutDataPartition->pZoneInfoRecord)))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_DATA_PARTITION_CONFIGURATION;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_DATA_PARTITION_QUERY_MIN_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutDataPartition->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutDataPartition->NumberOfZones = pStSafeA->InOutBuffer.LV.Data[0];
#if (STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
        (void)InZoneMaxNum;
        pOutDataPartition->pZoneInfoRecord = (StSafeA_ZoneInformationRecordBuffer_t *)&pStSafeA->InOutBuffer.LV.Data[1];
#else
        if (pOutDataPartition->NumberOfZones > InZoneMaxNum)
        {
          /* Return an error so the application can read the correct number of zones,
             allocate the right space (if not under STSAFEA_USE_OPTIMIZATION_SHARED_RAM optimization),
             and call back this API with the right InZoneMaxNum parameter */
          return STSAFEA_INVALID_RESP_LENGTH;
        }

        (void)memcpy(pOutDataPartition->pZoneInfoRecord,
                     &pStSafeA->InOutBuffer.LV.Data[1],
                     (uint32_t)pOutDataPartition->Length - 1U);
#endif
        uint16_t uIdx;
        for (uint8_t i = 0; i < pOutDataPartition->NumberOfZones; i++)
        {
          uIdx = (uint16_t)(i * sizeof(StSafeA_ZoneInformationRecordBuffer_t)) + 1U;
          if (pOutDataPartition->pZoneInfoRecord[i].ZoneType == STSAFEA_ONE_WAY_COUNTER_PRESENCE)
          {
            (void)memmove(&pOutDataPartition->pZoneInfoRecord[i].ReadAccessCondition + 3U,
                          &pOutDataPartition->pZoneInfoRecord[i].ReadAccessCondition,
                          (uint32_t)pOutDataPartition->Length - uIdx - 3U);
            /* Add bytes for AC */
             pOutDataPartition->Length += 3U;
            /* Adjust little/big endian bytes */
            pOutDataPartition->pZoneInfoRecord[i].OneWayCounter =
                                  SWAP4BYTES(pOutDataPartition->pZoneInfoRecord[i].OneWayCounter);
          }
          else
          {
            if ((i + 1U) < pOutDataPartition->NumberOfZones)
            {
              (void)memmove(&pOutDataPartition->pZoneInfoRecord[i + 1U].Index,
                            &pOutDataPartition->pZoneInfoRecord[i].UpdateAccessCondition,
                            (uint32_t)pOutDataPartition->Length - uIdx - 5U);

              /* Add bytes for AC & counter */
              pOutDataPartition->Length += 7U;
            }
            /* One way counter set to 0 */
            pOutDataPartition->pZoneInfoRecord[i].OneWayCounter = 0;
            /* Copy data segment length */
            pOutDataPartition->pZoneInfoRecord[i].DataSegmentLength =
                                  pOutDataPartition->pZoneInfoRecord[i].ReadAccessCondition +
                                  ((uint16_t)pOutDataPartition->pZoneInfoRecord[i].UpdateAcChangeRight << 8U);
          }
          /* Adjust little/big endian bytes */
          pOutDataPartition->pZoneInfoRecord[i].DataSegmentLength =
                                  (uint16_t)SWAP2BYTES(pOutDataPartition->pZoneInfoRecord[i].DataSegmentLength);

          /* Adjust AC bytes */
          pOutDataPartition->pZoneInfoRecord[i].UpdateAccessCondition =
                                  pOutDataPartition->pZoneInfoRecord[i].ReadAcChangeRight & 0X07U;
          pOutDataPartition->pZoneInfoRecord[i].UpdateAcChangeRight   =
                                  (pOutDataPartition->pZoneInfoRecord[i].ReadAcChangeRight & 0X08U) >> 3;
          pOutDataPartition->pZoneInfoRecord[i].ReadAccessCondition   =
                                  (pOutDataPartition->pZoneInfoRecord[i].ReadAcChangeRight & 0X70U) >> 4;
          pOutDataPartition->pZoneInfoRecord[i].ReadAcChangeRight     =
                                  (pOutDataPartition->pZoneInfoRecord[i].ReadAcChangeRight & 0X80U) >> 7;
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_Decrement
  *          This command is applied to a counter zone and performs the following processing:\n
  *          - It changes the update access conditions and the update AC change right.\n
  *          - It decrements the one way counter with the amount given as parameter.\n
  *          - It updates data in its data segment with data buffer pointed.
  * @note    The change in "update AC change right" is irreversible and the \n
  *          "update access conditions" may only be changed to a stricter value
  *
  * @param   pStSafeA              : STSAFE-A1xx object pointer.
  * @param   InChangeACIndicator   : STSAFEA_FLAG_TRUE (1) if change, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewDecrementACRight : STSAFEA_FLAG_TRUE (1) to change decrement access condition right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewDecrementAC      : Must be more strict than current and equal to 000b if InChangeACIndicator is 0.\n
  *                                  Can be one of the following values:\n
  *                                  @arg STSAFEA_AC_ALWAYS (0) Access Condition always allowed. Also use this value if InChangeACIndicator is 0.\n
  *                                  @arg STSAFEA_AC_HOST   MAC or Auth AC must be fulfilled.\n
  *                                  @arg STSAFEA_AC_AUTH   A valid C-MAC must have been received on the command or on some previous command.\n
  *                                  @arg STSAFEA_AC_MAC    Command must have a valid C-MAC.
  * @param   InZoneIndex           : Specify the Zone Index to apply the command to.
  * @param   InOffset              : Offset in data segment where data, if any, are written.
  * @param   InAmount              : Must be strictly larger than 0.
  * @param   pInData               : LV structure pointer with input data.
  * @param   pOutDecrement         : StSafeA_DecrementBuffer_t structure pointer to be filled with the response data.\n
  *                                  Pointer must have been properly allocated.
  * @param   InMAC                 : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Decrement(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InChangeACIndicator,
  uint8_t InNewDecrementACRight,
  uint8_t InNewDecrementAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  uint32_t InAmount,
  StSafeA_LVBuffer_t *pInData,
  StSafeA_DecrementBuffer_t *pOutDecrement,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pOutDecrement != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_FLAG(InChangeACIndicator));
    stsafea_assert_param(IS_STSAFEA_FLAG(InNewDecrementACRight));
    stsafea_assert_param(IS_STSAFEA_AC(InNewDecrementAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_DECREMENT, &InMAC);

    /* Change AC indicator + New AC change right + New update AC */
    uint8_t tmp = (InChangeACIndicator & 0x01U) << STSAFEA_CMD_DECREMENT_HEADER_CHAC_POS;
    tmp |= (InNewDecrementACRight & 0x01U) << STSAFEA_CMD_DECREMENT_HEADER_NEWUPDRIGHT_POS;
    tmp |= (InNewDecrementAC & STSAFEA_AC_MSK) << STSAFEA_CMD_DECREMENT_HEADER_NEWUPDAC_POS;

    pStSafeA->InOutBuffer.LV.Data[0] = tmp;

    /* Zone index */
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)InZoneIndex;

    /* Zone offset */
    uint16_t offset = (uint16_t)SWAP2BYTES(InOffset);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[2], &offset, sizeof(offset));

    /* Amount */
    uint32_t amount = (uint32_t)SWAP4BYTES(InAmount);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[4], &amount, sizeof(amount));

    pStSafeA->InOutBuffer.LV.Length = 8U;

    /* Data */
    if ((pInData != NULL) && (pInData->Length > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length, pInData->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length],
                   pInData->Data,
                   pInData->Length);

      pStSafeA->InOutBuffer.LV.Length += pInData->Length;
    }

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_DECREMENT_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_DECREMENT);

      /* Read response */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutDecrement->Length = pStSafeA->InOutBuffer.LV.Length;
        (void)memcpy((uint8_t *)&pOutDecrement->OneWayCounter, pStSafeA->InOutBuffer.LV.Data,
                     sizeof(pOutDecrement->OneWayCounter));
        pOutDecrement->OneWayCounter = (uint32_t)SWAP4BYTES(pOutDecrement->OneWayCounter);
      }
    }
  }
  return status_code;
}

/**
  * @brief   StSafeA_Read
  *          This command read data from a data partition zone:
  *          - It may change the read access conditions before reading the data.\n
  *          - It checks the access conditions and only returns the data starting from the specified offset up to the zone boundary.\n
  * @note    Note that change in read AC change right is irreversible and that, if allowed, the read access conditions may only be changed to a stricter value.
  *
  * @param   pStSafeA            : STSAFE-A1xx object pointer.
  * @param   InChangeACIndicator : STSAFEA_FLAG_TRUE (1) if change, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewReadACRight    : STSAFEA_FLAG_TRUE (1) to change read access condition right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewReadAC         : Must be stricter than current and equal to 000b if InChangeACIndicator is 0.\n
  *                                Can be one of the following values:\n
  *                                @arg STSAFEA_AC_ALWAYS (0) Access Condition always allowed. Also use this value if InChangeACIndicator is 0.\n
  *                                @arg STSAFEA_AC_HOST   MAC or Auth AC must be fulfilled for the command.\n
  *                                @arg STSAFEA_AC_AUTH   A valid C-MAC must have been received on the command or on some previous command.\n
  *                                @arg STSAFEA_AC_MAC    Command must have a valid C-MAC.
  * @param   InZoneIndex         : Specify the Zone Index to apply the command to.
  * @param   InOffset            : Range supported is from 0 up to the length of the data segment.
  * @param   InAmount            : Must be strictly larger than 0.
  * @param   InRespDataLen       : Expected data length in bytes of read data (pOutLVResponse->Data). Take care to include the length of the counter if any.\n
  *                                Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutLVResponse      : LV structure pointer to be filled with the response data.\n
  *                                If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                                If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC               : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Read(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InChangeACIndicator,
  uint8_t InNewReadACRight,
  uint8_t InNewReadAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  uint16_t InAmount,
  uint16_t InRespDataLen,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  uint32_t length;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InRespDataLen));
    stsafea_assert_param(IS_STSAFEA_FLAG(InChangeACIndicator));
    stsafea_assert_param(IS_STSAFEA_AC(InNewReadAC));

    /* Build command Header and process CMAC */
    STSAFEA_CHECK_SIZE(InMAC, 0U, InRespDataLen);
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_READ, &InMAC);

    /* Change AC indicator + New AC change right + New update AC */
    pStSafeA->InOutBuffer.LV.Data[0] = (InChangeACIndicator == 0U) ? 0x00U :
                                       ((uint8_t)((InNewReadACRight == 0U) ? 0x10U : 0x18U) | (uint8_t)InNewReadAC);

    /* Zone index */
    pStSafeA->InOutBuffer.LV.Data[1] = InZoneIndex;

    /* Zone offset */
    uint16_t offset = (uint16_t)SWAP2BYTES(InOffset);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[2], &offset, sizeof(offset));

    /* Length */
    length = (uint16_t)SWAP2BYTES(InAmount);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[4], &length, sizeof(InAmount));
    pStSafeA->InOutBuffer.LV.Length = 6U;

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = InRespDataLen + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_READ);

      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        status_code = StSafeA_AssignLVResponse(pOutLVResponse, &pStSafeA->InOutBuffer.LV, InRespDataLen);
      }
    }
  }
  return status_code;
}

/**
  * @brief   StSafeA_Process_Update
  *          This command updates data through zone partition without counter:
  *          - It may start by changing the update access conditions before updating the data in a zone with no one-way counter.\n
  *          - It carries out both operations in a single atomic operation.
  * @note    Note that changing the update AC change right is irreversible and that, if allowed, the update access conditions may only be changed to a stricter value.
  *
  * @param   pStSafeA            : STSAFE-A1xx object pointer.
  * @param   InAtomicity         : STSAFEA_FLAG_TRUE (1) if update will be done in atomic way, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InChangeACIndicator : STSAFEA_FLAG_TRUE (1) if change, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewUpdateACRight  : STSAFEA_FLAG_TRUE (1) to change update access condition right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewUpdateAC       : Must be stricter than current and equal to 000b if InChangeACIndicator is 0. Can be one of the following values:
  *                                @arg STSAFEA_AC_ALWAYS (0) Access Condition always allowed. Also use this value if InChangeACIndicator is 0
  *                                @arg STSAFEA_AC_HOST   MAC or Auth AC must be fulfilled
  *                                @arg STSAFEA_AC_AUTH   A valid C-MAC must have been received on the command or on some previous command
  *                                @arg STSAFEA_AC_MAC    Command must have a valid C-MAC
  * @param   InZoneIndex         : Specify the Zone Index to apply the command to.
  * @param   InOffset            : offset in data segment, from where data are written.
  * @param   pInLVData           : LV structure pointer containing the data to be written.
  * @param   InMAC               : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Process_Update(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InAtomicity,
  uint8_t InChangeACIndicator,
  uint8_t InNewUpdateACRight,
  uint8_t InNewUpdateAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  StSafeA_LVBuffer_t *pInLVData,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_FLAG(InAtomicity));
    stsafea_assert_param(IS_STSAFEA_FLAG(InChangeACIndicator));
    stsafea_assert_param(IS_STSAFEA_FLAG(InNewUpdateACRight));
    stsafea_assert_param(IS_STSAFEA_AC(InNewUpdateAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_UPDATE, &InMAC);

    /* If InChangeACIndicator is false then InNewUpdateACRight and InNewUpdateAC must be Zero. Let the STSAFE Hw to manage it */
    uint8_t tmp = (InAtomicity & 0x01U) << STSAFEA_CMD_UPDATE_HEADER_ATOM_POS;
    tmp |= (InChangeACIndicator & 0x01U) << STSAFEA_CMD_UPDATE_HEADER_CHAC_POS;
    tmp |= (InNewUpdateACRight & 0x01U) << STSAFEA_CMD_UPDATE_HEADER_NEWUPDRIGHT_POS;
    tmp |= (InNewUpdateAC & STSAFEA_AC_MSK) << STSAFEA_CMD_UPDATE_HEADER_NEWUPDAC_POS;
    pStSafeA->InOutBuffer.LV.Data[0] = tmp;

    /* Zone index */
    pStSafeA->InOutBuffer.LV.Data[1] = InZoneIndex;

    /* Zone offset */
    uint16_t offset = (uint16_t)SWAP2BYTES(InOffset);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[2], &offset, sizeof(offset));

    pStSafeA->InOutBuffer.LV.Length = 4U;

    /* Data */
    if ((pInLVData != NULL) && (pInLVData->Length > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length, pInLVData->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length],
                   pInLVData->Data,
                   pInLVData->Length);

      pStSafeA->InOutBuffer.LV.Length += pInLVData->Length;
    }

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_UPDATE);

      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_Update
  *          This command updates data through zone partition without counter:
  *          - It may start by changing the update access conditions before updating the data in a zone with no one-way counter.\n
  *          - It carries out both operations in a single atomic operation.
  * @note    Note that changing the update AC change right is irreversible and that, if allowed, the update access conditions may only be changed to a stricter value.
  *
  * @param   pStSafeA            : STSAFE-A1xx object pointer.
  * @param   InAtomicity         : STSAFEA_FLAG_TRUE (1) if update will be done in atomic way, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InChangeACIndicator : STSAFEA_FLAG_TRUE (1) if change, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewUpdateACRight  : STSAFEA_FLAG_TRUE (1) to change update access condition right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewUpdateAC       : Must be stricter than current and equal to 000b if InChangeACIndicator is 0.\n
  *                                Can be one of the following values:\n
  *                                @arg STSAFEA_AC_ALWAYS (0) Access Condition always allowed. Also use this value if InChangeACIndicator is 0.\n
  *                                @arg STSAFEA_AC_HOST   MAC or Auth AC must be fulfilled.\n
  *                                @arg STSAFEA_AC_AUTH   A valid C-MAC must have been received on the command or on some previous command.\n
  *                                @arg STSAFEA_AC_MAC    Command must have a valid C-MAC.
  * @param   InZoneIndex         : Specify the Zone Index to apply the command to.
  * @param   InOffset            : offset in data segment, from where data are written.
  * @param   pInLVData           : LV structure pointer containing the data to be written.
  * @param   InMAC               : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_Update(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InAtomicity,
  uint8_t InChangeACIndicator,
  uint8_t InNewUpdateACRight,
  uint8_t InNewUpdateAC,
  uint8_t InZoneIndex,
  uint16_t InOffset,
  StSafeA_LVBuffer_t *pInLVData,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  if ((InAtomicity == STSAFEA_FLAG_TRUE) && (pInLVData->Length > STSAFEA_ATOMICITY_BUFFER_SIZE))
  {
		uint8_t i;
    StSafeA_LVBuffer_t pLVData;

    pLVData.Length = STSAFEA_ATOMICITY_BUFFER_SIZE;
		pLVData.Data = pInLVData->Data;

    for (i = 0; i < (pInLVData->Length / STSAFEA_ATOMICITY_BUFFER_SIZE); i++)
    {
      status_code = StSafeA_Process_Update(pStSafeA, InAtomicity, InChangeACIndicator, InNewUpdateACRight,
                                           InNewUpdateAC, InZoneIndex, InOffset, &pLVData, InMAC);
      InOffset += STSAFEA_ATOMICITY_BUFFER_SIZE;
      pLVData.Data += STSAFEA_ATOMICITY_BUFFER_SIZE;
      if (status_code != STSAFEA_OK)
      {
        break;
      }
    }

    if (status_code == STSAFEA_OK)
    {
      pLVData.Length = pInLVData->Length % STSAFEA_ATOMICITY_BUFFER_SIZE;
      if (pLVData.Length != 0U)
      {
        status_code = StSafeA_Process_Update(pStSafeA, InAtomicity, InChangeACIndicator, InNewUpdateACRight,
                                             InNewUpdateAC, InZoneIndex, InOffset, &pLVData, InMAC);
      }
    }
  }
  else
  {
    status_code = StSafeA_Process_Update(pStSafeA, InAtomicity, InChangeACIndicator, InNewUpdateACRight,
                                         InNewUpdateAC, InZoneIndex, InOffset, pInLVData, InMAC);
  }

  return status_code;
}

/**
  * @}
  */


/** @addgroup CORE_Exported_Functions_Group4 Private and Public Key Commands APIs
  *  @brief    STSAFE-A APIs related to private and public key commands
  *
@verbatim
 ===============================================================================
              ##### Private and Public Key Commands  #####
 ===============================================================================
[..]
    (+) Generate Key Pair
    (+) Generate Signature
    (+) Verify Message Signature
    (+) Establish Key
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_GenerateKeyPair
  *          This command instructs the STSAFE-A1xx to generate a key-pair in a private key slot.
  * @note    If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the pOutPubX & pOutPubY LV structure pointers must have been properly allocated. \n
  *          If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1, pointers inside pStSafeA->InOutBuffer will be assigned.
  *
  * @param   pStSafeA                  : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum              : Slot number in the private key table.\n
  *                                      Can be one of the following values:\n
  *                                      @arg STSAFEA_KEY_SLOT_0: Slot 0 in private key table.\n
  *                                      @arg STSAFEA_KEY_SLOT_1: Slot 1 in private key table.
  * @param   InUseLimit                : Maximum number of times the key can be used (0xFFFF : no limit).
  * @param   InChangeACIndicator       : STSAFEA_FLAG_TRUE (1) if change, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InNewDecrementACRight     : STSAFEA_FLAG_TRUE (1) to change decrement access condition right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InChangeAuthFlagsRight    : STSAFEA_FLAG_TRUE (1) to allow changing authorization right, STSAFEA_FLAG_FALSE (0) if not.
  * @param   InAuthorizationFlags      : A combination of the following values:\n
  *                                      @arg STSAFEA_PRVKEY_MODOPER_AUTHFLAG_CMD_RESP_SIGNEN.\n
  *                                      @arg STSAFEA_PRVKEY_MODOPER_AUTHFLAG_MSG_DGST_SIGNEN.\n
  *                                      @arg STSAFEA_PRVKEY_MODOPER_AUTHFLAG_KEY_ESTABLISHEN.
  * @param   InCurveId                  : Curve identifier.
  * @param   InPubXYLen                 : Expected data length in bytes of public key X & Y coordinates (pOutPubX->Data & pOutPubY->Data).\n
  *                                       Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutPointReprensentationId : byte pointer to be filled with point representation identifier and must have been properly allocated.
  * @param   pOutPubX                   : LV structure pointer to be filled with public key X coordinate.
  *                                       If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                                       If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   pOutPubY                   : LV structure pointer to be filled with public key Y coordinate.
  *                                       If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                                       If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC                      : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_GenerateKeyPair(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint16_t InUseLimit,
  uint8_t InChangeAuthFlagsRight,
  uint8_t InAuthorizationFlags,
  StSafeA_CurveId_t InCurveId,
  uint16_t InPubXYLen,
  uint8_t *pOutPointReprensentationId,
  StSafeA_LVBuffer_t *pOutPubX,
  StSafeA_LVBuffer_t *pOutPubY,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)              &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutPubX) &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutPubY) &&
      (pOutPointReprensentationId != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_FLAG(InChangeAuthFlagsRight));

    uint16_t tmp_len;
    uint16_t tmp_use_limit;

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_GENERATE_KEY, &InMAC);

    /* Attribute Tag */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_PRIVATE_KEY_SLOT;

    /* Slot number */
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)InKeySlotNum;

    /* Use limit */
    tmp_use_limit = InUseLimit;

#ifdef STSAFEA_KEY_SLOT_EPHEMERAL
    if (InKeySlotNum == STSAFEA_KEY_SLOT_EPHEMERAL)
    {
      tmp_use_limit = 1;
    }
#endif /* STSAFEA_KEY_SLOT_EPHEMERAL */
    tmp_use_limit = (uint16_t)SWAP2BYTES(tmp_use_limit);
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[2], &tmp_use_limit, sizeof(uint16_t));

    /* Mode of operation */
    tmp_use_limit = (InChangeAuthFlagsRight == 0x0U) ?
                                  0x0000U : STSAFEA_PRIVATE_KEY_MODE_OF_OPERATION_CHANGE_RIGHT_MASK;
    tmp_use_limit |= InAuthorizationFlags & STSAFEA_PRIVATE_KEY_MODE_OF_OPERATION_AUTHORIZATION_FLAGS_MASK;
    tmp_use_limit = (uint16_t)SWAP2BYTES(tmp_use_limit);

    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[4], &tmp_use_limit, sizeof(tmp_use_limit));

    /* Curve ID */
    uint16_t tmp_curve_len = STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId);
    tmp_use_limit = (uint16_t)SWAP2BYTES(tmp_curve_len);
    tmp_len = 8U + tmp_curve_len;

    /* Curve identifier */
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[8], STSAFEA_GET_ECC_CURVE_OID(InCurveId), tmp_curve_len);
    /* Curve identifier size */
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[6], &tmp_use_limit, sizeof(uint16_t));
    /* Update Frame Length */
    pStSafeA->InOutBuffer.LV.Length = tmp_len;


    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_POINT_REPRESENTATION_ID_LEN +
                             2U * (STSAFEA_LENGTH_SIZE + (uint16_t)STSAFEA_GET_XYRS_LEN_FROM_CURVE(InCurveId)) +
                             STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_GENERATE_KEY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        status_code = STSAFEA_INVALID_RESP_LENGTH;
        if (STSAFEA_GET_XYRS_LEN_FROM_CURVE(InCurveId) <= InPubXYLen)
        {
          uint16_t uIdx = 1U;
          /* Assign Representation Id */
          if (pStSafeA->InOutBuffer.LV.Length >= uIdx)
          {
            *pOutPointReprensentationId = pStSafeA->InOutBuffer.LV.Data[0];
          }

          /* Assign Key X */
          if (pStSafeA->InOutBuffer.LV.Length > (uIdx + sizeof(pOutPubX->Length)))
          {
            /* Assign the received response to the output parameter */
            status_code = StSafeA_AssignLVBuffer(pOutPubX, &pStSafeA->InOutBuffer.LV.Data[uIdx], InPubXYLen);

            uIdx += (uint16_t)sizeof(pOutPubX->Length) + pOutPubX->Length ;
          }

          /* Assign Key Y*/
          if ((pStSafeA->InOutBuffer.LV.Length > (uIdx + sizeof(pOutPubY->Length))) && (status_code == STSAFEA_OK))
          {
            /* Assign the received response to the output parameter */
            status_code = StSafeA_AssignLVBuffer(pOutPubY, &pStSafeA->InOutBuffer.LV.Data[uIdx], InPubXYLen);
          }
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_GenerateSignature
  *          This command returns the ECDSA signature over a message digest.
  * @note    If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the pOutSignR & pOutSignS LV structure pointers must have been properly allocated. \n
  *          If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1, pointers inside pStSafeA->InOutBuffer will be assigned.
  *
  * @param   pStSafeA         : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum     : Slot number in the private key table.\n
  *                             Can be one of the following values:\n
  *                             @arg STSAFEA_KEY_SLOT_0: Slot 0 in private key table.\n
  *                             @arg STSAFEA_KEY_SLOT_1: Slot 1 in private key table.
  * @param   pInDigest        : Pointer to message digest bytes array.
  * @param   InDigestType     : Digest type.
  * @param   InSignRSLen      : Expected data length in bytes of ECDSA signature's part X & Y (pOutSignR->Data & pOutSignS->Data).\n
  *                             Can be one of the following values:\n
  *                             @arg STSAFEA_XYRS_ECDSA_SHA256_LENGTH.\n
  *                             @arg STSAFEA_XYRS_ECDSA_SHA384_LENGTH.\n
  *                             Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutSignR        : LV structure pointer to be filled with the ECDSA signature R.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   pOutSignR        : LV structure pointer to be filled with the ECDSA signature S.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC            : MAC authenticating command/response
  * @param   InHostEncryption : Encryption type.\n
  *                             Can be one of the following values:\n
  *                             @arg STSAFEA_ENCRYPTION_NONE: No encryption needed.\n
  *                             @arg STSAFEA_ENCRYPTION_RESPONSE: Response will be generated encrypted.\n
  *                             @arg STSAFEA_ENCRYPTION_COMMAND: Command is encrypted before sending.\n
  *                             @arg STSAFEA_ENCRYPTION_COMMAND_RESPONSE: Both command and response are encrypted.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_GenerateSignature(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  const uint8_t *pInDigest,
  StSafeA_HashTypes_t InDigestType,
  uint16_t InSignRSLen,
  StSafeA_LVBuffer_t *pOutSignR,
  StSafeA_LVBuffer_t *pOutSignS,
  uint8_t InMAC,
  uint8_t InHostEncryption)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

#if defined(STSAFE_A100)
  STSAFEA_UNUSED_VAR(InHostEncryption);
#endif /* STSAFE_A100 */

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)                &&
      IS_STSAFEA_CONDITIONAL_VALID_PTR(pInDigest)           &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutSignR)  &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutSignS))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InSignRSLen));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_ENCRYPT(InHostEncryption));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_GENERATE_SIGNATURE, &InMAC);

    /* Private key slot number */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InKeySlotNum;

    /* Digest size */
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)((((InDigestType == STSAFEA_SHA_384) ?
                                                   STSAFEA_SHA_384_LENGTH : STSAFEA_SHA_256_LENGTH) & 0xFF00U) >> 8);
    pStSafeA->InOutBuffer.LV.Data[2] = ((InDigestType == STSAFEA_SHA_384) ?
                                        STSAFEA_SHA_384_LENGTH : STSAFEA_SHA_256_LENGTH);

    /* Digest */
#ifdef WORKAROUND_GENERATE_SIGNATURE
    uint8_t DigestLength = (InDigestType == STSAFEA_SHA_384) ? STSAFEA_SHA_384_LENGTH : STSAFEA_SHA_256_LENGTH;
    uint8_t i, sum = 0;
    for (i = 0; i < DigestLength; i++)
    {
      sum |= pInDigest[i];
    }
    if (sum == 0U)
    {
      return STSAFEA_INVALID_PARAMETER;
    }

    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[3],
                 pInDigest,
                 DigestLength);
#else
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[3],
                 pInDigest,
                 ((InDigestType == STSAFEA_SHA_384) ? STSAFEA_SHA_384_LENGTH : STSAFEA_SHA_256_LENGTH));
#endif

    /* Length */
    pStSafeA->InOutBuffer.LV.Length = (uint16_t)((InDigestType == STSAFEA_SHA_384) ?
                                                 STSAFEA_SHA_384_LENGTH : STSAFEA_SHA_256_LENGTH) + 3U;

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
#if defined(STSAFE_A110)
    /* Encrypt data */
    if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_COMMAND) == (uint8_t)STSAFEA_ENCRYPTION_COMMAND)
    {
      status_code = StSafeA_DataEncryption(pStSafeA);
    }
    else
    {
      status_code = STSAFEA_OK;
    }

    if (status_code == STSAFEA_OK)
#endif /* STSAFE_A110 */
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
    {
      /* Transmit command */
      status_code = StSafeA_TransmitCommand(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        uint16_t tmp_len = (InDigestType == STSAFEA_SHA_384) ? STSAFEA_LENGTH_CMD_RESP_GENERATE_SIGNATURE_SHA_384 :
                           STSAFEA_LENGTH_CMD_RESP_GENERATE_SIGNATURE_SHA_256;
        /* Set response length */
        pStSafeA->InOutBuffer.LV.Length = tmp_len + STSAFEA_R_MAC_LENGTH(InMAC); ;

        /* Wait for the command processing. Then check for the response */
        StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_GENERATE_SIGNATURE);

        /* Read response */
        status_code = StSafeA_ReceiveResponse(pStSafeA);

        if (status_code == STSAFEA_OK)
        {
#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
#if defined(STSAFE_A110)
          /* Decrypt data */
          if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_RESPONSE) == (uint8_t)STSAFEA_ENCRYPTION_RESPONSE)
          {
            status_code = StSafeA_DataDecryption(pStSafeA);
          }

          if (status_code == STSAFEA_OK)
#endif /* STSAFE_A110 */
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
          {
            /* Assign the received response to the output parameter */
            status_code = StSafeA_AssignLVBuffer(pOutSignR, &pStSafeA->InOutBuffer.LV.Data[0], InSignRSLen);
            if (status_code == STSAFEA_OK)
            {
              status_code = StSafeA_AssignLVBuffer(
                                  pOutSignS,
                                  &pStSafeA->InOutBuffer.LV.Data[STSAFEA_LENGTH_SIZE + pOutSignR->Length],
                                  InSignRSLen);
            }
          }
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_VerifyMessageSignature
  *          This command is executed for Message authentication.
  * @note    Note that this command doesn't require a signature session to be ongoing.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @param   InCurveId     : Curve identifier.
  * @param   pInPubX       : LV structure pointer defining public key X coordinate.
  * @param   pInPubY       : LV structure pointer defining public key X coordinate.
  * @param   pInRSignature : LV structure pointer defining R of ECDSA signature.
  * @param   pInSSignature : LV structure pointer defining S of ECDSA signature.
  * @param   pInDigest     : Pointer to message digest bytes array.
  * @param   pOutRespVerifySignature : StSafeA_VerifySignatureBuffer_t structure pointer to be filled with\n
  *                                    the signature validity response.\n
  *                                    Pointer must have been properly allocated.
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_VerifyMessageSignature(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_CurveId_t InCurveId,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  const StSafeA_LVBuffer_t *pInSignR,
  const StSafeA_LVBuffer_t *pInSignS,
  const StSafeA_LVBuffer_t *pInDigest,
  StSafeA_VerifySignatureBuffer_t *pOutRespVerifySignature,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)       &&
      (pInPubX != NULL)                            &&
      (pInPubY != NULL)                            &&
      (pInSignR != NULL)                           &&
      (pInSignS != NULL)                           &&
      (pInDigest != NULL)                          &&
      (pOutRespVerifySignature != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_VERIFY_SIGNATURE, &InMAC);

    /* Verify signature [message authentication] */
    pStSafeA->InOutBuffer.LV.Data[0] = 0x00U;

    /* Curve ID */
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)((STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId) & 0xFF00U) >> 8);
    pStSafeA->InOutBuffer.LV.Data[2] = (uint8_t)(STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId) & 0x00FFU);

    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[3],
                 STSAFEA_GET_ECC_CURVE_OID(InCurveId),
                 STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId));

    pStSafeA->InOutBuffer.LV.Length = (0x0001U + STSAFEA_ECC_CURVE_OID_LEN +
                                      (uint16_t)STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId));

    /* Public key */
    pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = STSAFEA_POINT_REPRESENTATION_ID;
    pStSafeA->InOutBuffer.LV.Length++;

    /* X coordinate */
    if ((pInPubX->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubX->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubX->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInPubX->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubX->Data,
                   pInPubX->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubX->Length) + pInPubX->Length;
    }

    /* Y coordinate */
    if ((pInPubY->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubY->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubY->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInPubY->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubY->Data,
                   pInPubY->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubY->Length) + pInPubY->Length;
    }

    /* Signature */
    if (pInSignR->Length > 0U)
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInSignR->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInSignR->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInSignR->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInSignR->Data,
                   pInSignR->Length);

      pStSafeA->InOutBuffer.LV.Length += 2U + pInSignR->Length;
    }

    if (pInSignS->Length > 0U)
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInSignS->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInSignS->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInSignS->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInSignS->Data,
                   pInSignS->Length);

      pStSafeA->InOutBuffer.LV.Length += 2U + pInSignS->Length;
    }

    /* Hash */
    if ((pInDigest->Length > 0U))
    {
      /* Digest size */
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInDigest->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInDigest->Length & 0x00FFU);

      /* Digest */
      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInDigest->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInDigest->Data,
                   pInDigest->Length);

      pStSafeA->InOutBuffer.LV.Length += 2U + pInDigest->Length;
    }

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_VERIFY_MSG_SIGNATURE_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_VERIFY_MSG_SIGNATURE);

      /* Read response */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutRespVerifySignature->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutRespVerifySignature->SignatureValidity = pStSafeA->InOutBuffer.LV.Data[0];
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_EstablishKey
  *          This command establish shared secret between 2 hosts by using asymmetric cryptography.
  * @note    Note that this command doesn't require an asymmetric crypto session to be ongoing.
  *
  * @param   pStSafeA             : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum         : Slot number in the private key table.\n
  *                                 Can be one of the following values:\n
  *                                 @arg STSAFEA_KEY_SLOT_0: Slot 0 in private key table.\n
  *                                 @arg STSAFEA_KEY_SLOT_1: Slot 1 in private key table.
  * @param   pInPubX              : LV structure pointer defining public key X coordinate.
  * @param   pInPubY              : LV structure pointer defining public key X coordinate.
  * @param   InSharedSecretLength : Expected data length in bytes of computed shared secret (pOutSharedSecret->SharedKey.Data).\n
  *                                 Can be one of the following values:\n
  *                                 @arg STSAFEA_XYRS_ECDSA_SHA256_LENGTH.\n
  *                                 @arg STSAFEA_XYRS_ECDSA_SHA384_LENGTH.\n
  *                                 Used if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.
  * @param   pOutSharedSecret     : StSafeA_SharedSecretBuffer_t structure pointer to be filled with the response data, coded as length and key.\n
  *                                 If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the StSafeA_SharedSecretBuffer_t SharedKey structure must have been properly allocated.\n
  *                                 If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned for the SharedKey.
  * @param   InMAC                : MAC authenticating command/response.
  * @param   InHostEncryption     : Encryption type.\n
  *                                 Can be one of the following values:\n
  *                                 @arg STSAFEA_ENCRYPTION_NONE: No encryption needed.\n
  *                                 @arg STSAFEA_ENCRYPTION_RESPONSE: Response will be generated encrypted.\n
  *                                 @arg STSAFEA_ENCRYPTION_COMMAND: Command is encrypted before sending.\n
  *                                 @arg STSAFEA_ENCRYPTION_COMMAND_RESPONSE: Both command and response are encrypted.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_EstablishKey(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  uint16_t InSharedSecretLength,
  StSafeA_SharedSecretBuffer_t *pOutSharedSecret,
  uint8_t InMAC,
  uint8_t InHostEncryption)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)              &&
      (pInPubX != NULL)                                   &&
      (pInPubY != NULL)                                   &&
      (pOutSharedSecret != NULL)                          &&
      IS_STSAFEA_CONDITIONAL_VALID_PTR(&pOutSharedSecret->SharedKey))
  {
#if defined(STSAFE_A100)
    STSAFEA_UNUSED_VAR(InHostEncryption);
#endif /* STSAFE_A100 */
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_ENCRYPT(InHostEncryption));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_ESTABLISH_KEY, &InMAC);

    /* Private key slot number */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InKeySlotNum;
    /* Point representation identifier */
    pStSafeA->InOutBuffer.LV.Data[1] = STSAFEA_POINT_REPRESENTATION_ID;
    pStSafeA->InOutBuffer.LV.Length = 2U;

    /* X public key */
    if ((pInPubX->Length > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length, pInPubX->Length + STSAFEA_LENGTH_SIZE);

      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubX->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubX->Length & 0x00FFU);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubX->Data,
                   (uint32_t)pInPubX->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubX->Length) + pInPubX->Length;
    }

    /* Y public key */
    if ((pInPubY->Length > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length, pInPubY->Length + STSAFEA_LENGTH_SIZE);

      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubY->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubY->Length & 0x00FFU);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubY->Data,
                   (uint32_t)pInPubY->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubY->Length) + pInPubY->Length;
    }

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
#if defined(STSAFE_A110)
    /* Encrypt data */
    if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_COMMAND) == (uint8_t)STSAFEA_ENCRYPTION_COMMAND)
    {
      status_code = StSafeA_DataEncryption(pStSafeA);
    }
    else
    {
      status_code = STSAFEA_OK;
    }

    if (status_code == STSAFEA_OK)
#endif /* STSAFE_A110 */
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
    {
      /* Transmit command */
      status_code = StSafeA_TransmitCommand(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Set response length */
        pStSafeA->InOutBuffer.LV.Length = InSharedSecretLength +
                                          STSAFEA_LENGTH_SIZE +
                                          STSAFEA_R_MAC_LENGTH(InMAC);

        /* Wait for the command processing. Then check for the response */
        StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_ESTABLISH_KEY);

        /* Read response */
        status_code = StSafeA_ReceiveResponse(pStSafeA);

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
        if (status_code == STSAFEA_OK)
        {
#if defined(STSAFE_A100)
          status_code = StSafeA_DataDecryption(pStSafeA);
#else
          /* Decrypt data */
          if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_RESPONSE) ==
                                  (uint8_t)STSAFEA_ENCRYPTION_RESPONSE)
          {
            status_code = StSafeA_DataDecryption(pStSafeA);
          }
#endif /* STSAFE_A100 */
        }
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

        if (status_code == STSAFEA_OK)
        {
          pOutSharedSecret->Length = pStSafeA->InOutBuffer.LV.Length;

          /* Assign the internal StSafeA_SharedSecretBuffer_t LVBuffer */
          status_code = StSafeA_AssignLVBuffer(&pOutSharedSecret->SharedKey,
                                               pStSafeA->InOutBuffer.LV.Data,
                                               InSharedSecretLength);
        }
      }
    }
  }

  return status_code;
}

/**
  * @}
  */


/** @addgroup CORE_Exported_Functions_Group5 Administrative Commands APIs
  *  @brief    STSAFE-A APIs related to administrative commands
  *
@verbatim
 ===============================================================================
              ##### Administrative Commands  #####
 ===============================================================================
[..]
    (+) Product Data Query
    (+) I2C Parameter Query
    (+) Life Cycle State Query
    (+) Host Key Slot Query
    (+) Put Attribute
    (+) Delete Password
    (+) Verify Password
    (+) RawCommand
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_ProductDataQuery
  *          Query command to retrieve product data.
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer.
  * @param   pOutProductData   : StSafeA_ProductDataBuffer_t structure pointer to be filled with the response product data.\n
  *                              The pointer must have been properly allocated.
  * @param   InMAC             : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_ProductDataQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_ProductDataBuffer_t *pOutProductData,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)  &&
      (pOutProductData != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_PRODUCT_DATA;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_PRODUCT_DATA_QUERY_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutProductData->Length = pStSafeA->InOutBuffer.LV.Length;

        uint8_t i = 0;

        /* Mask Identification */
        pOutProductData->MaskIdentificationTag    = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->MaskIdentificationLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->MaskIdentification,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->MaskIdentificationLength);
        i += pOutProductData->MaskIdentificationLength;

        /* ST Number */
        pOutProductData->STNumberTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->STNumberLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->STNumber,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->STNumberLength);
        i += pOutProductData->STNumberLength;

        /* Input output buffer length */
        pOutProductData->InputOutputBufferSizeTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->InputOutputBufferSizeLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(&pOutProductData->InputOutputBufferSize,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->InputOutputBufferSizeLength);
        pOutProductData->InputOutputBufferSize = (uint16_t)SWAP2BYTES(pOutProductData->InputOutputBufferSize);
        i += 2U;

        /* Atomicity buffer length */
        pOutProductData->AtomicityBufferSizeTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->AtomicityBufferSizeLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(&pOutProductData->AtomicityBufferSize,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->AtomicityBufferSizeLength);
        pOutProductData->AtomicityBufferSize = (uint16_t)SWAP2BYTES(pOutProductData->AtomicityBufferSize);
        i += 2U;

        /* Size of non-volatile memory */
        pOutProductData->NonVolatileMemorySizeTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->NonVolatileMemorySizeLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(&pOutProductData->NonVolatileMemorySize,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->NonVolatileMemorySizeLength);
        pOutProductData->NonVolatileMemorySize = (uint16_t)SWAP2BYTES(pOutProductData->NonVolatileMemorySize);
        i += 2U;

        /* Test date */
        pOutProductData->TestDateTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->TestDateLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(&pOutProductData->TestDateSize,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->TestDateLength);
        pOutProductData->TestDateSize = (uint16_t)SWAP2BYTES(pOutProductData->TestDateSize);
        i += 2U;

        /* Internal product version */
        pOutProductData->InternalProductVersionTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->InternalProductVersionLength = pStSafeA->InOutBuffer.LV.Data[++i];
        pOutProductData->InternalProductVersionSize = pStSafeA->InOutBuffer.LV.Data[++i];

        /* Module date */
        pOutProductData->ModuleDateTag = pStSafeA->InOutBuffer.LV.Data[++i];
        pOutProductData->ModuleDateLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(&pOutProductData->ModuleDateSize,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->ModuleDateLength);
        pOutProductData->ModuleDateSize = (uint16_t)SWAP2BYTES(pOutProductData->ModuleDateSize);
        i += 2U;

#if (STSAFE_A110)
        /* Firmware delivery traceability */
        pOutProductData->FirmwareDeliveryTraceabilityTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->FirmwareDeliveryTraceabilityLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->FirmwareDeliveryTraceability,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->FirmwareDeliveryTraceabilityLength);
        i += pOutProductData->FirmwareDeliveryTraceabilityLength;

        /* Blackbox delivery traceability */
        pOutProductData->BlackboxDeliveryTraceabilityTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->BlackboxDeliveryTraceabilityLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->BlackboxDeliveryTraceability,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->BlackboxDeliveryTraceabilityLength);
        i += pOutProductData->BlackboxDeliveryTraceabilityLength;

        /* Perso ID */
        pOutProductData->PersoIdTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->PersoIdLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->PersoId,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->PersoIdLength);
        i += pOutProductData->PersoIdLength;

        /* Perso generation batch ID */
        pOutProductData->PersoGenerationBatchIdTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->PersoGenerationBatchIdLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->PersoGenerationBatchId,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->PersoGenerationBatchIdLength);
        i += pOutProductData->PersoGenerationBatchIdLength;

        /* Perso date */
        pOutProductData->PersoDateTag = pStSafeA->InOutBuffer.LV.Data[i];
        pOutProductData->PersoDateLength = pStSafeA->InOutBuffer.LV.Data[++i];
        (void)memcpy(pOutProductData->PersoDate,
                     &pStSafeA->InOutBuffer.LV.Data[++i],
                     pOutProductData->PersoDateLength);
//        i += pOutProductData->PersoDateLength;
#endif /* STSAFE_A110 */
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_I2cParameterQuery
  *          Query command to retrieve I2C address and low power mode configuration.
  *
  * @param   pStSafeA         : STSAFE-A1xx object pointer
  * @param   pOutI2CParamData : StSafeA_I2cParameterBuffer_t structure pointer to be filled with the response data.\n
  *                             The pointer must have been properly allocated.
  * @param   InMAC            : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_I2cParameterQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_I2cParameterBuffer_t *pOutI2CParamData,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)  &&
      (pOutI2CParamData != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */

    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_I2C_PARAMETER;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_I2C_PARAMETERS_QUERY_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutI2CParamData->Length               = pStSafeA->InOutBuffer.LV.Length;
        pOutI2CParamData->I2cAddress           = pStSafeA->InOutBuffer.LV.Data[0] >> 1;
        pOutI2CParamData->LockConfig           = pStSafeA->InOutBuffer.LV.Data[1]  & STSAFEA_I2C_LOCK_MSK;   /* 1-bit: the 8th  */
        pOutI2CParamData->LowPowerModeConfig   = pStSafeA->InOutBuffer.LV.Data[1]  & STSAFEA_I2C_LPMODE_MSK;   /* 2-bits : the 6th and 7th */
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_LifeCycleStateQuery
  *          Query command to retrieve the life cycle state product status.
  *
  * @param   pStSafeA           : STSAFE-A1xx object pointer.
  * @param   pOutLifeCycleState : StSafeA_LifeCycleStateBuffer_t structure pointer to be filled with the response data.\n
  *                               The pointer must have been properly allocated.
  * @param   InMAC              : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_LifeCycleStateQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_LifeCycleStateBuffer_t *pOutLifeCycleState,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pOutLifeCycleState != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_LIFE_CYCLE_STATE;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_LIFE_CYCLE_STATE_QUERY_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutLifeCycleState->Length          = pStSafeA->InOutBuffer.LV.Length;
        pOutLifeCycleState->LifeCycleStatus = pStSafeA->InOutBuffer.LV.Data[0];
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_HostKeySlotQuery
  *          Query command to retrieve the host key information (presence & host C-MAC counter).
  *
  * @param   pStSafeA          : STSAFE-A1xx object pointer
  * @param   pOutHostKeySlot   : StSafeA_HostKeySlotBuffer_t structure pointer to be filled with the response data.\n
  *                              The pointer must have been properly allocated.
  * @param   InMAC             : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_HostKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_HostKeySlotBuffer_t *pOutHostKeySlot,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pOutHostKeySlot != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_HOST_KEY_SLOT;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_HOST_KEY_SLOT_QUERY_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutHostKeySlot->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutHostKeySlot->HostKeyPresenceFlag = pStSafeA->InOutBuffer.LV.Data[0];

        /* Only 3-bytes for MAC Counter */
        pOutHostKeySlot->HostCMacSequenceCounter = STSAFEA_HOST_CMAC_INVALID_COUNTER;
        if (pOutHostKeySlot->HostKeyPresenceFlag != 0U)
        {
          pOutHostKeySlot->HostCMacSequenceCounter  = ((uint32_t)pStSafeA->InOutBuffer.LV.Data[1]) << 16;
          pOutHostKeySlot->HostCMacSequenceCounter |= ((uint32_t)pStSafeA->InOutBuffer.LV.Data[2]) << 8;
          pOutHostKeySlot->HostCMacSequenceCounter |= pStSafeA->InOutBuffer.LV.Data[3];
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_PutAttribute
  *          This command is executed to put attributes in the STSAFE-A1xx device like
  *          keys, password, I2C parameters according to the attribute TAG.
  *
  * @param   pStSafeA       : STSAFE-A1xx object pointer.
  * @param   InAttributeTag : Attribute tag.
  * @param   pInData        : pointer to attribute value (bytes array).
  * @param   InDataSize     : attribute value size in bytes.
  * @param   InMAC          : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_PutAttribute(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InAttributeTag,
  const uint8_t *pInData,
  uint16_t InDataSize,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pInData != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_TAG(InAttributeTag));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_PUT_ATTRIBUTE, &InMAC);

    /* Attribute Tag */
    pStSafeA->InOutBuffer.LV.Data[0] = InAttributeTag;
    /* Use limit */
    if (InDataSize > 0U)
    {
      STSAFEA_CHECK_SIZE(InMAC, 1U, InDataSize);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[1], pInData, InDataSize);
    }
    pStSafeA->InOutBuffer.LV.Length = 1U + InDataSize;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_PUT_ATTRIBUTE);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_DeletePassword
  *          This command instructs the STSAFE-A1xx to delete the password form its slot.
  *
  * @param   pStSafeA    : STSAFE-A1xx object pointer.
  * @param   InMAC       : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_DeletePassword(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));


    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_DELETE_KEY, &InMAC);
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_PASSWORD_SLOT;
    pStSafeA->InOutBuffer.LV.Length = 1U;


    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_DELETE_KEY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_VerifyPassword
  *          This command performs password verification
  *          and remembers the outcome of the verification for future command authorization.
  *
  * @param   pStSafeA            : STSAFE-A1xx object pointer.
  * @param   pInPassword         : Pointer to password bytes array (16 bytes length).
  * @param   pOutVerifyPassword  : StSafeA_VerifyPasswordBuffer_t structure pointer to be filled with response data.\n
  *                                The pointer must have been properly allocated.
  * @param   InMAC               : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_VerifyPassword(
  StSafeA_Handle_t *pStSafeA,
  const uint8_t *pInPassword,
  StSafeA_VerifyPasswordBuffer_t *pOutVerifyPassword,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      (pInPassword != NULL)                  &&
      (pOutVerifyPassword != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_VERIFY_PASSWORD, &InMAC);

    /* Build command data */
    /* Password */
    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[0], pInPassword, STSAFEA_PASSWORD_SIZE);

    pStSafeA->InOutBuffer.LV.Length = STSAFEA_PASSWORD_SIZE;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_VERIFY_PASSWORD_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_VERIFY_PASSWORD);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        pOutVerifyPassword->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutVerifyPassword->VerificationStatus = pStSafeA->InOutBuffer.LV.Data[0];
        pOutVerifyPassword->RemainingTries     = pStSafeA->InOutBuffer.LV.Data[1];;

      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_RawCommand
  *          This functions is used to execute a raw command and receive the related response.
  *
  * @param   pStSafeA       : STSAFE-A1xx object pointer.
  * @param   pInRawCommand  : Pointer to the TLV structure containing the command to send to STSAFE-A device.
  * @param   InRespDataLen  : Expected response length. Specify 0xFF if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1.
  * @param   pOutLVResponse : LV structure pointer to be filled with the response data.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   DelayMs        : Waiting time in ms after the command has been sent, before checking for the received response.
  * @param   InMAC          : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_RawCommand(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_TLVBuffer_t *pInRawCommand,
  uint16_t InRespDataLen,
  StSafeA_TLVBuffer_t *pOutTLVResponse,
  uint32_t DelayMs,
  uint8_t InMAC)
{

  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)                  &&
      (pInRawCommand != NULL)                                 &&
      (pOutTLVResponse != NULL)                               &&
      IS_STSAFEA_CONDITIONAL_VALID_PTR(&pOutTLVResponse->LV))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InRespDataLen));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, pInRawCommand->Header, &InMAC);

    /* Build command */
    STSAFEA_CHECK_SIZE(InMAC, 0U, pInRawCommand->LV.Length);
    pStSafeA->InOutBuffer.LV.Length = pInRawCommand->LV.Length;
    (void)memcpy(pStSafeA->InOutBuffer.LV.Data, pInRawCommand->LV.Data, pInRawCommand->LV.Length);

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(DelayMs);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        pOutTLVResponse->Header = pStSafeA->InOutBuffer.Header;
        status_code = StSafeA_AssignLVResponse(&pOutTLVResponse->LV, &pStSafeA->InOutBuffer.LV,  InRespDataLen);
      }
    }
  }

  return status_code;
}

/**
  * @}
  */


/** @addgroup CORE_Exported_Functions_Group6 Local Envelope Commands APIs
  *  @brief    STSAFE-A APIs related to local envelope commands APIs
  *
@verbatim
 ===============================================================================
              ##### Local Envelope Commands  #####
 ===============================================================================
[..]
    (+) Local Envelope Key Slot Query
    (+) Generate Local Envelope Key
    (+) Wrap Local Envelope
    (+) Unwrap Local Envelope
@endverbatim
  * @{
  */
/**
  * @brief   StSafeA_LocalEnvelopeKeySlotQuery
  *          Query command to retrieve local envevelope keys information (slot number, presence & key length) for the available key slots.
  *
  * @param   pStSafeA                                    : STSAFE-A1xx object pointer
  * @param   pOutLocalEnvelopeKeyTable                   : StSafeA_LocalEnvelopeKeyTableBuffer_t structure pointer to be filled with response data.\n
  *                                                        The pointer must have been properly allocated.
  * @param   pOutLlocalEnvelopeKeySlot0InformationRecord : StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t structure pointer to be filled with local envelope key slot 0 information.
  *                                                        The pointer must have been properly allocated.
  * @param   pOutLlocalEnvelopeKeySlot1InformationRecord : StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t structure pointer to be filled with local envelope key slot 1 information.
  *                                                        The pointer must have been properly allocated.
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_LocalEnvelopeKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_LocalEnvelopeKeyTableBuffer_t *pOutLocalEnvelopeKeyTable,
  StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t *pOutLlocalEnvelopeKeySlot0InformationRecord,
  StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t *pOutLlocalEnvelopeKeySlot1InformationRecord,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)                 &&
      (pOutLocalEnvelopeKeyTable != NULL)                    &&
      (pOutLlocalEnvelopeKeySlot0InformationRecord != NULL)  &&
      (pOutLlocalEnvelopeKeySlot1InformationRecord != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_LOCAL_ENVELOPE_QUERY_MIN_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        uint8_t idx = 0;
        pOutLocalEnvelopeKeyTable->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutLocalEnvelopeKeyTable->NumberOfSlots = pStSafeA->InOutBuffer.LV.Data[idx];

        if (pOutLocalEnvelopeKeyTable->NumberOfSlots != 0U)
        {
          /* Fill the Slot-0 Information record */
          idx++;
          pOutLlocalEnvelopeKeySlot0InformationRecord->SlotNumber   = pStSafeA->InOutBuffer.LV.Data[idx];
          idx++;
          pOutLlocalEnvelopeKeySlot0InformationRecord->PresenceFlag = pStSafeA->InOutBuffer.LV.Data[idx];
          if (pOutLlocalEnvelopeKeySlot0InformationRecord->PresenceFlag != 0U)
          {
            idx++;
            pOutLlocalEnvelopeKeySlot0InformationRecord->KeyLength = pStSafeA->InOutBuffer.LV.Data[idx];
          }

          /* Fill the Slot-1 Information record */
          idx++;
          pOutLlocalEnvelopeKeySlot1InformationRecord->SlotNumber   = pStSafeA->InOutBuffer.LV.Data[idx];
          idx++;
          pOutLlocalEnvelopeKeySlot1InformationRecord->PresenceFlag = pStSafeA->InOutBuffer.LV.Data[idx];
          if (pOutLlocalEnvelopeKeySlot1InformationRecord->PresenceFlag != 0U)
          {
            idx++;
            pOutLlocalEnvelopeKeySlot1InformationRecord->KeyLength = pStSafeA->InOutBuffer.LV.Data[idx];
          }
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_GenerateLocalEnvelopeKey
  *          This command instructs the STSAFE-A1xx to generate a key in a local envelope key slot.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum  : Slot number in the KSK table.\n
  *                          Can be one of the following values:\n
  *                          @arg STSAFEA_KEY_SLOT_0: Slot 0 in the KSK table.\n
  *                          @arg STSAFEA_KEY_SLOT_1: Slot 1 in the KSK table.
  * @param   InKeyType     : AES Key type.\n
  *                          Can be one of the following values:\n
  *                          @arg STSAFEA_KEY_TYPE_AES_128: AES-128 bits key.\n
  *                          @arg STSAFEA_KEY_TYPE_AES_256: AES-256 bits key.
  * @param   pInSeed       : Seed pointer. It's optional and can be empty.
  * @param   InSeedSize    : Seed size in bytes. Should be 0 is seed is empty.
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_GenerateLocalEnvelopeKey(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t InKeyType,
  uint8_t *pInSeed,
  uint16_t InSeedSize,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_KEY_TYPE(InKeyType));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_GENERATE_KEY, &InMAC);

    /* Attribute Tag */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE;

    /* Slot number */
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)InKeySlotNum;

    /* AES key types */
    pStSafeA->InOutBuffer.LV.Data[2] = (uint8_t)InKeyType;

    /*Seed (can be optional, so the related pointer is not checked at the beginning)*/
    uint16_t seed_size = InSeedSize;
    if ((seed_size > 0U)  && (pInSeed != NULL))
    {
      STSAFEA_CHECK_SIZE(InMAC, 3U, seed_size);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[3], pInSeed, seed_size);
    }

    pStSafeA->InOutBuffer.LV.Length = 3U + seed_size;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_GENERATE_KEY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_WrapLocalEnvelope
  *          This command is used to wrap data and typically working keys that are entirely
  *          managed by the host, with a local envelope key and the [AES key wrap] algorithm.
  *
  * @param   pStSafeA         : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum     : Local envelope key slot number.\n
  *                             Can be one of the following values:\n
  *                             @arg STSAFEA_KEY_SLOT_0: Slot 0 in local envelope key table.\n
  *                             @arg STSAFEA_KEY_SLOT_1: Slot 1 in local envelope key table.
  * @param   pInData          : Pointer to data bytes array to wrap.
  * @param   InDataSize       : Data size (non-zero, multiple of 8 bytes and maximum 480 bytes).
  * @param   pOutLVResponse   : LV structure pointer to be filled with the wrapped local envelope response data.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                             If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC            : MAC authenticating command/response.
  * @param   InHostEncryption : Encryption type.
  *                             Can be one of the following values:\n
  *                             @arg STSAFEA_ENCRYPTION_NONE: No encryption needed.\n
  *                             @arg STSAFEA_ENCRYPTION_RESPONSE: Response will be generated encrypted.\n
  *                             @arg STSAFEA_ENCRYPTION_COMMAND: Command is encrypted before sending.\n
  *                             @arg STSAFEA_ENCRYPTION_COMMAND_RESPONSE: Both command and response are encrypted.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_WrapLocalEnvelope(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t *pInData,
  uint16_t InDataSize,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC,
  uint8_t InHostEncryption)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

#if defined(STSAFE_A100)
  STSAFEA_UNUSED_VAR(InHostEncryption);
#endif /* STSAFE_A100 */

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_ENCRYPT(InHostEncryption));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_WRAP_LOCAL_ENVELOPE, &InMAC);

    /* Build command data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InKeySlotNum;

    if ((pInData != NULL) && (InDataSize > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, 1U, InDataSize);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[1], pInData, InDataSize);
    }
    pStSafeA->InOutBuffer.LV.Length = InDataSize + 1U;

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
#if defined(STSAFE_A100)
    status_code = StSafeA_DataEncryption(pStSafeA);
#else
    /* Encrypt data */
    if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_COMMAND) == (uint8_t)STSAFEA_ENCRYPTION_COMMAND)
    {
      status_code = StSafeA_DataEncryption(pStSafeA);
    }
    else
    {
      status_code = STSAFEA_OK;
    }
#endif /* STSAFE_A100 */
    if (status_code == STSAFEA_OK)
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */
    /* Transmit */
    {
      status_code = StSafeA_TransmitCommand(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Set response length */
        pStSafeA->InOutBuffer.LV.Length = InDataSize +
                                          STSAFEA_WRAP_UNWRAP_ENVELOPE_ADDITIONAL_RESPONSE_LENGTH +
                                          STSAFEA_R_MAC_LENGTH(InMAC);

        /* Wait for the command processing. Then check for the response */
        StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_WRAP_LOCAL_ENVELOPE);

        /* Receive */
        status_code = StSafeA_ReceiveResponse(pStSafeA);

        if (status_code == STSAFEA_OK)
        {
          /* Local Envelope response data is 8-bytes longer than the working key (see User Manual).
            Need to compensate the input length before the assignment function call.
            WARNING: Must be clear at App level to allocate additional 8-Bytes to avoid overflow!*/

          /* Assign the internal StSafeA_SharedSecretBuffer_t LVBuffer */
          status_code = StSafeA_AssignLVResponse(pOutLVResponse, &pStSafeA->InOutBuffer.LV, InDataSize + 8U);
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_UnwrapLocalEnvelope
  *          This command is used to unwrap a local envelope with a local envelope key.
  *          The result can be a working key that is further used by the host.
  *
  * @param   pStSafeA            : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum        : Local envelope key slot number.\n
  *                                Can be one of the following values:\n
  *                                @arg STSAFEA_KEY_SLOT_0: Slot 0 in local envelope key table.\n
  *                                @arg STSAFEA_KEY_SLOT_1: Slot 1 in local envelope key table.
  * @param   pInLocalEnvelope    : Pointer to data bytes array to wrap.
  * @param   InLocalEnvelopeSize : Data size (multiple of 8 bytes between 16 and 488 bytes inclusive).
  * @param   pOutLVResponse      : LV structure pointer to be filled with the working key response data.\n
  *                                If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                                If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC               : MAC authenticating command/response.
  * @param   InHostEncryption    : Encryption type.
  *                                Can be one of the following values:\n
  *                                @arg STSAFEA_ENCRYPTION_NONE: No encryption needed.\n
  *                                @arg STSAFEA_ENCRYPTION_RESPONSE: Response will be generated encrypted.\n
  *                                @arg STSAFEA_ENCRYPTION_COMMAND: Command is encrypted before sending.\n
  *                                @arg STSAFEA_ENCRYPTION_COMMAND_RESPONSE: Both command and response are encrypted.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_UnwrapLocalEnvelope(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t *pInLocalEnvelope,
  uint16_t InLocalEnvelopeSize,
  StSafeA_LVBuffer_t *pOutLVResponse,
  uint8_t InMAC,
  uint8_t InHostEncryption)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

#if defined(STSAFE_A100)
  STSAFEA_UNUSED_VAR(InHostEncryption);
#endif /* STSAFE_A100 */

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));
    stsafea_assert_param(IS_STSAFEA_ENCRYPT(InHostEncryption));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE, &InMAC);

    /* Build command data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)InKeySlotNum;

    if ((pInLocalEnvelope != NULL) && (InLocalEnvelopeSize > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, 1U, InLocalEnvelopeSize);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[1], pInLocalEnvelope, InLocalEnvelopeSize);
    }
    pStSafeA->InOutBuffer.LV.Length = InLocalEnvelopeSize + 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = InLocalEnvelopeSize +
                                        STSAFEA_WRAP_UNWRAP_ENVELOPE_ADDITIONAL_RESPONSE_LENGTH +
                                        STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_UNWRAP_LOCAL_ENVELOPE);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
      if (status_code == STSAFEA_OK)
      {
#if defined(STSAFE_A100)          
        status_code = StSafeA_DataDecryption(pStSafeA);
#else
        /* Decrypt data */
        if (((uint8_t)InHostEncryption & (uint8_t)STSAFEA_ENCRYPTION_RESPONSE) == (uint8_t)STSAFEA_ENCRYPTION_RESPONSE)
        {
          status_code = StSafeA_DataDecryption(pStSafeA);
        }
#endif /* STSAFE_A100 */
      }
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        status_code = StSafeA_AssignLVResponse(pOutLVResponse, &pStSafeA->InOutBuffer.LV,  InLocalEnvelopeSize);
      }
    }
  }

  return status_code;
}

/**
  * @}
  */


/**
  * @}
  */


/* Private functions ---------------------------------------------------------*/
/** @addtogroup CORE_Private_Functions
  * @{
  */

/**
  * @brief   StSafeA_TransmitCommand
  *          Static function to transmit the already prepared command to the STSAFE-A device.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_TransmitCommand(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  if (pStSafeA != NULL)
  {
    status_code = StSafeA_MAC_SHA_PrePostProcess(pStSafeA, STSAFEA_MAC_SHA_PRE_PROCESS);
    if (status_code == STSAFEA_OK)
    {
      status_code = StSafeA_Transmit(&pStSafeA->InOutBuffer, pStSafeA->CrcSupport);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_ReceiveResponse
  *          Static function to receive the esponse from the STSAFE-A device, after a command has been transmitted.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_ReceiveResponse(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  if (pStSafeA != NULL)
  {
    status_code = StSafeA_Receive(&pStSafeA->InOutBuffer, pStSafeA->CrcSupport);
    if (status_code == STSAFEA_OK)
    {
      pStSafeA->MacCounter ++;

      status_code = StSafeA_MAC_SHA_PrePostProcess(pStSafeA, STSAFEA_MAC_SHA_POST_PROCESS);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_AssignLVResponse
  *          Static function used to assign the  LV structure from the received response.
  *
  * @param   pDestLVBuffer : Pointer to destination LV structure to be filled.
  * @param   pSrcLVBuffer  : Pointer to source LV structure to fill from.
  * @param   ExpectedLen   : Expected data length.
  *                          If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, must correspond to the memory allocated for pDestLVBuffer->Data.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_AssignLVResponse(StSafeA_LVBuffer_t *pDestLVBuffer,
                                                       StSafeA_LVBuffer_t *pSrcLVBuffer,
                                                       uint16_t ExpectedLen)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  if ((pDestLVBuffer != NULL)  && (pSrcLVBuffer != NULL))
  {
    /* Check if the length value received by STSAFE is what it was expected */
    if (pSrcLVBuffer->Length > ExpectedLen)
    {
      return STSAFEA_INVALID_RESP_LENGTH;
    }

    status_code = STSAFEA_OK;

    /* The length value received by STSAFE is assigned anyway, to allow the application to re-try with the correct parameter */
    pDestLVBuffer->Length = pSrcLVBuffer->Length;

    /* Set the Data of the LVBuffer according to the selected Shared Ram optimization */
    StSafeA_SetLVData(pDestLVBuffer, pSrcLVBuffer->Data, pDestLVBuffer->Length);
  }
  return status_code;
}

/**
  * @brief   StSafeA_AssignLVBuffer
  *          Static function used to assign the  LV structure from a bytes array.
  *
  * @param   pDestLVBuffer : Pointer to destination LV structure to be filled.
  * @param   pDataBuffer   : Pointer to source bytes array to fill from.
  * @param   ExpectedLen   : Expected data length.
  *                          If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, must correspond to the memory allocated for pDestLVBuffer->Data.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
static StSafeA_ResponseCode_t StSafeA_AssignLVBuffer(StSafeA_LVBuffer_t *pDestLVBuffer,
                                                     uint8_t *pDataBuffer,
                                                     uint16_t ExpectedLen)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;
  if ((pDestLVBuffer != NULL)  && (pDataBuffer != NULL))
  {
    /* The length value is assigned anyway, to allow the application to re-try with the correct parameter, in case */
    pDestLVBuffer->Length = (((uint16_t)pDataBuffer[0U]) << 8) + pDataBuffer[1U];

    /* Check if the length value is what it was expected */
    if (pDestLVBuffer->Length > ExpectedLen)
    {
      return STSAFEA_INVALID_RESP_LENGTH;
    }
    else
    {
      status_code = STSAFEA_OK;
    }

    /* Set the Data of the LVBuffer according to the selected Shared Ram optimization */
    StSafeA_SetLVData(pDestLVBuffer, &pDataBuffer[2U], pDestLVBuffer->Length);
  }
  return status_code;
}

/**
  * @brief   StSafeA_SetLVData
  *          Static function used to set the LV structure data according to the STSAFEA_USE_OPTIMIZATION_SHARED_RAM configuration.\n
  *         Used by StSafeA_AssignLVResponse and StSafeA_AssignLVBuffer functions.
  *
  * @param   pDestLVBuffer : Pointer to destination LV structure to be filled.
  * @param   pSrcLVData    : Pointer to source bytes array to fill from.
  * @param   Length        : If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, must correspond to the memory allocated for pDestLVBuffer->Data.
  * @retval  none
  */
static void StSafeA_SetLVData(StSafeA_LVBuffer_t *pDestLVBuffer, uint8_t *pSrcLVData,  uint16_t Length)
{
#if (STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
  STSAFEA_UNUSED_VAR(Length);
  /* In this case the direct pointer assignment allows the best memory optimization.
     But up to the caller to manage this memory content properly, copying it to a local buffer if needed */
  pDestLVBuffer->Data = pSrcLVData;
#else
  /* In this case a copy from buffer to buffer is done. It's expected that the
     application have been already allocated the right memory for the pDestLVData.
     Only the expected length of data is copied, but the actual length provided by STSAFE is
     returned, so up to the application to re-allocate properly and re-try  */
  if ((Length > 0U) && (pDestLVBuffer->Data != NULL))
  {
    (void)memcpy(pDestLVBuffer->Data, pSrcLVData, Length);
  }
#endif /* STSAFEA_USE_OPTIMIZATION_SHARED_RAM */
}

/**
  * @brief   StSafeA_BuildCommandHeaderCMAC
  *          Static function used to Build the Command Header and set the C-MAC command field.
  *
  * @param   pStSafeA    : STSAFE-A1xx object pointer.
  * @param   CommandCode : Command code.
  * @param   pMAC        : Pointer to MAC authenticating command/response.
  * @retval  none
  */
static void StSafeA_BuildCommandHeaderCMAC(StSafeA_Handle_t *pStSafeA, uint8_t CommandCode, uint8_t *pMAC)
{
  if ((pStSafeA != NULL) && (pMAC != NULL))
  {
#if (STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
    *pMAC = STSAFEA_MAC_NONE;
#else
    /* Get CMAC counter value */
    if ((*pMAC & STSAFEA_CMD_HEADER_SCHN_HOSTEN) != 0x00U)
    {
      StSafeA_GetHostMacSequenceCounter(pStSafeA);
    }
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

    pStSafeA->InOutBuffer.Header = (CommandCode | (*pMAC & STSAFEA_CMD_HEADER_MAC_MSK));
  }
}

#if (!STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT)
/**
  * @brief   StSafeA_GetHostMacSequenceCounter
  *          Static function used to retrieve the Host MAC sequence counter and update it's value into the pStSafeA handler pointer.
  *
  * @param   pStSafeA    : STSAFE-A1xx object pointer.
  * @retval  none
  */
static void StSafeA_GetHostMacSequenceCounter(StSafeA_Handle_t *pStSafeA)
{
  StSafeA_HostKeySlotBuffer_t host_key_slot;

  if (pStSafeA != NULL)
  {
    if ((StSafeA_HostKeySlotQuery(pStSafeA, &host_key_slot, STSAFEA_MAC_NONE) == STSAFEA_OK)
        && (host_key_slot.HostKeyPresenceFlag != 0U))
    {
      pStSafeA->HostMacSequenceCounter = host_key_slot.HostCMacSequenceCounter;
    }
  }
}
#endif /* STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT */

/**
  * @}
  */


#if defined(STSAFE_A100)

/* STSAFE-A100 Private functions ---------------------------------------------*/
/** @addtogroup STSAFE_A100_CORE_Exported_Functions
  * @{
  */

/** @addgroup STSAFE_A100_CORE_Exported_Functions_Group1 Signature Session Commands APIs
  *  @brief    STSAFE-A100 APIs related to signature session commands
  *
@verbatim
 ===============================================================================
              ##### Signature Session Commands  #####
 ===============================================================================
[..]
    (+) Start Signature Session
    (+) Get Signature
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_StartSignatureSession
  *          This function is executed to start an asymmetric cryptographic session.\n
  *          This command notifies the chip that a signature computation must be started.\n
  *          There can be only a single asymmetric cryptographic session on going at any moment.
  * @note    This feature is available for STSAFE-A100 only.
  *
  * @param   pStSafeA      : STSAFE-A1xx object pointer.
  * @param   InKeySlotNum  : Slot number in the private key table. Can be one of the following values:
  *                          @arg STSAFEA_KEY_SLOT_0: Slot 0 in private key table
  *                          @arg STSAFEA_KEY_SLOT_1: Slot 1 in private key table
  * @param   InMAC         : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_StartSignatureSession(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InKeySlotNum,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_KEY_SLOT(InKeySlotNum));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_START_SESSION, &InMAC);

    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_SESSION_SIGNATURE;
    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)InKeySlotNum;
    pStSafeA->InOutBuffer.LV.Length = 2U;

    pStSafeA->HashObj.HashCtx = NULL;
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Setup HASH context */
      StSafeA_InitHASH(pStSafeA);

      if (pStSafeA->HashObj.HashCtx != NULL)
      {
        /* Wait for the command processing. Then check for the response */
        StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_START_SESSION);

        status_code = StSafeA_ReceiveResponse(pStSafeA);
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_GetSignature
  *          This command returns the ECDSA signature over the command response sequence since the start of a signature session.
  * @note    Note that this feature is available for STSAFE-A100 only. This command require a signature session to be ongoing.
  *
  * @param   pStSafeA        : STSAFE-A1xx object pointer.
  * @param   pInChallenge    : Pointer to challenge bytes array. Might be empty.
  * @param   InChallengeSize : Challenge size in bytes.
  * @param   InSignRSLen     : Expected response length.\n
  *                            Can be one of the following values:\n
  *                            @arg STSAFEA_XYRS_ECDSA_SHA256_LENGTH.\n
  *                            @arg STSAFEA_XYRS_ECDSA_SHA384_LENGTH.
  * @param   pOutSignR       : LV structure pointer to be filled with the ECDSA signature R.\n
  *                            If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                            If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   pOutSignR       : LV structure pointer to be filled with the ECDSA signature S.\n
  *                            If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                            If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC           : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_GetSignature(
  StSafeA_Handle_t *pStSafeA,
  const uint8_t *pInChallenge,
  uint16_t InChallengeSize,
  uint16_t InSignRSLen,
  StSafeA_LVBuffer_t *pOutSignR,
  StSafeA_LVBuffer_t *pOutSignS,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InSignRSLen));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_GET_SIGNATURE, &InMAC);

    /* Challenge (challenge might be empty)*/
    if ((pInChallenge != NULL) && (InChallengeSize > 0U))
    {
      STSAFEA_CHECK_SIZE(InMAC, 0U, InChallengeSize);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[0], pInChallenge, InChallengeSize);
      pStSafeA->InOutBuffer.LV.Length = InChallengeSize;
    }
    else
    {
      pStSafeA->InOutBuffer.LV.Length = 0U;
    }

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    /* Compute HASH */
    StSafeA_ComputeHASH(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = ((pStSafeA->HashObj.HashType == STSAFEA_SHA_384) ?
                                         STSAFEA_LENGTH_CMD_RESP_GET_SIGNATURE_SHA_384 :
                                         STSAFEA_LENGTH_CMD_RESP_GET_SIGNATURE_SHA_256);

      pStSafeA->InOutBuffer.LV.Length += STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_GET_SIGNATURE);

      /* Read response */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        /* Assign the received response to the output parameter */
        status_code = StSafeA_AssignLVBuffer(pOutSignR, &pStSafeA->InOutBuffer.LV.Data[0], InSignRSLen);
        if (status_code == STSAFEA_OK)
        {
          status_code = StSafeA_AssignLVBuffer(pOutSignS, &pStSafeA->InOutBuffer.LV.Data[STSAFEA_LENGTH_SIZE + pOutSignR->Length], InSignRSLen);
        }
      }
    }
  }

  return status_code;
}
/**
  * @}
  */


/** @addtogroup STSAFE_A100_CORE_Exported_Functions_Group2 Entity Authentication Commands APIs
  *  @brief    STSAFE-A APIs related to entity authentication commands
  *
@verbatim
 ===============================================================================
              ##### Entity authentication Commands  #####
 ===============================================================================
[..]
    (+) Public Key Slot Query
    (+) Put Public Key Slot
    (+) Verify Entity Signature
@endverbatim
  * @{
  */

/**
  * @brief   StSafeA_PublicKeySlotQuery
  *          Query command to retrieve public key information (presence & curve ID).
  * @note    If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the pOutPubX, pOutPubY, pOutCurveId LV structure pointers must have been properly allocated. \n
  *          If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 pointers inside pStSafeA->InOutBuffer will be assigned.
  *
  * @param   pStSafeA       : STSAFE-A1xx object pointer
  * @param   pOutPubKeySlot : StSafeA_PublicKeySlotBuffer_t structure pointer to be filled with presence, point representation Id.
  * @param   InPubXYLen     : Public key X,Y coordinate expected length.\n
  *                           Can be one of the following values:\n
  *                           @arg STSAFEA_XYRS_ECDSA_SHA256_LENGTH.\n
  *                           @arg STSAFEA_XYRS_ECDSA_SHA384_LENGTH.
  * @param   InCurveIdLen   : Expected data length of curve identifier string (pOutCurveId->Data).\n
  *                           Can be one of the following values:\n
  *                           @arg STSAFEA_ECC_CURVE_OID_NIST_P_256_LEN.\n
  *                           @arg STSAFEA_ECC_CURVE_OID_NIST_P_384_LEN.\n
  *                           @arg STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_256_LEN.\n
  *                           @arg STSAFEA_ECC_CURVE_OID_BRAINPOOL_P_384_LEN.
  * @param   pOutPubX       : LV structure pointer to be filled with public key X coordinate.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   pOutPubY       : LV structure pointer to be filled with public key Y coordinate.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   pOutCurveId    : LV structure pointer to be filled with curve identifier.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the LV structure must have been properly allocated.\n
  *                           If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned.
  * @param   InMAC          : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_PublicKeySlotQuery(
  StSafeA_Handle_t *pStSafeA,
  StSafeA_PublicKeySlotBuffer_t *pOutPubKeySlot,
  uint16_t InPubXYLen,
  uint16_t InCurveIdLen,
  StSafeA_LVBuffer_t *pOutPubX,
  StSafeA_LVBuffer_t *pOutPubY,
  StSafeA_LVBuffer_t *pOutCurveId,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)                  &&
      (pOutPubKeySlot != NULL)                                &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutPubX)     &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutPubY)     &&
      IS_STSAFEA_CONDITIONAL_LVBUFFER_VALID_PTR(pOutCurveId))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));
    stsafea_assert_param(IS_STSAFEA_LEN(InPubXYLen));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_PUBLIC_KEY_SLOT;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_PUBLIC_KEY_SLOT_QUERY_MIN_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        uint16_t uIdx = 0;
        pOutPubKeySlot->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutPubKeySlot->PresenceFlag = pStSafeA->InOutBuffer.LV.Data[uIdx];
        uIdx++;

        if ((pOutPubKeySlot->Length > 1U) && (pOutPubKeySlot->PresenceFlag != 0U))
        {
          /* Assign the PointRepresentationId */
          pOutPubKeySlot->PointRepresentationId = pStSafeA->InOutBuffer.LV.Data[uIdx];
          uIdx++;

          /* The following input params are checked here so the API can be called
             with the following NULL just to check the PresenceFlag */
          if ((pOutPubX == NULL) || (pOutPubY == NULL) || (pOutCurveId == NULL))
          {
            status_code = STSAFEA_INVALID_PARAMETER;
          }
          else
          {
            /* Assign the PubX Key output parameter */
            status_code = StSafeA_AssignLVBuffer(pOutPubX, &pStSafeA->InOutBuffer.LV.Data[uIdx], InPubXYLen);
            uIdx += (uint16_t)sizeof(pOutPubX->Length) + pOutPubX->Length;

            /* Assign the PubY Key output parameter */
            if (status_code == STSAFEA_OK)
            {
              status_code = StSafeA_AssignLVBuffer(pOutPubY, &pStSafeA->InOutBuffer.LV.Data[uIdx], InPubXYLen);
              uIdx += (uint16_t)sizeof(pOutPubY->Length) + pOutPubY->Length;
            }

            /* Assign the CurveId output parameter */
            if (status_code == STSAFEA_OK)
            {
              status_code = StSafeA_AssignLVBuffer(pOutCurveId, &pStSafeA->InOutBuffer.LV.Data[uIdx], InCurveIdLen);
            }
          }
        }
      }
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_PutPublicKeySlot
  *          This command is executed to fill the public key slot.
  *
  * @param   pStSafeA     : STSAFE-A1xx object pointer.
  * @param   InCurveId    : Curve identifier.
  * @param   pInPubX      : LV structure pointer defining public key X coordinate.
  * @param   pInPubY      : LV structure pointer defining public key X coordinate.
  * @param   InMAC        : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_PutPublicKeySlot(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_CurveId_t InCurveId,
  const StSafeA_LVBuffer_t *pInPubX,
  const StSafeA_LVBuffer_t *pInPubY,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) &&
      IS_STSAFEA_LVBUFFER_VALID_PTR(pInPubX) &&
      IS_STSAFEA_LVBUFFER_VALID_PTR(pInPubY))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_PUT_ATTRIBUTE, &InMAC);

    /* Attribute Tag */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_PUBLIC_KEY_SLOT;

    /* Curve ID */

    pStSafeA->InOutBuffer.LV.Data[1] = (uint8_t)((STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId) & 0xFF00U) >> 8);
    pStSafeA->InOutBuffer.LV.Data[2] = (uint8_t)(STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId) & 0x00FFU);

    (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[3],
                 STSAFEA_GET_ECC_CURVE_OID(InCurveId),
                 STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId));   /* Curve identifier */

    pStSafeA->InOutBuffer.LV.Length = 0x0001U + STSAFEA_ECC_CURVE_OID_LEN + STSAFEA_GET_ECC_CURVE_OID_LEN(InCurveId);

    /* Public key */
    pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = STSAFEA_POINT_REPRESENTATION_ID;
    pStSafeA->InOutBuffer.LV.Length++;

    /* X coordinate */
    if ((pInPubX->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubX->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubX->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInPubX->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubX->Data,
                   pInPubX->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubX->Length) + pInPubX->Length;
    }

    /* Y coordinate */
    if ((pInPubY->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] = (uint8_t)((pInPubY->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] = (uint8_t)(pInPubY->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInPubY->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInPubY->Data,
                   pInPubY->Length);

      pStSafeA->InOutBuffer.LV.Length += (uint16_t)sizeof(pInPubY->Length) + pInPubY->Length;
    }

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_PUT_ATTRIBUTE);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);
    }
  }

  return status_code;
}

/**
  * @brief   StSafeA_VerifyEntitySignature
  *          This command is executed for Entity authentication.
  * @note    Note that this command doesn't require a signature session to be ongoing.
  *
  * @param   pStSafeA                : STSAFE-A1xx object pointer.
  * @param   pInRSignature           : LV structure pointer defining R of ECDSA signature.
  * @param   pInSSignature           : LV structure pointer defining S of ECDSA signature.
  * @param   pOutRespVerifySignature : StSafeA_VerifySignatureBuffer_t structure pointer to be filled with the signature validity response.
  *                                    The pointer must have been properly allocated.
  * @param   InMAC                   : MAC authenticating command/response.
  * @retval  STSAFEA_OK if success, an error code otherwise.
  */
StSafeA_ResponseCode_t StSafeA_VerifyEntitySignature(
  StSafeA_Handle_t *pStSafeA,
  const StSafeA_LVBuffer_t *pInRSignature,
  const StSafeA_LVBuffer_t *pInSSignature,
  StSafeA_VerifySignatureBuffer_t *pOutRespVerifySignature,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA)       &&
      IS_STSAFEA_LVBUFFER_VALID_PTR(pInRSignature) &&
      IS_STSAFEA_LVBUFFER_VALID_PTR(pInSSignature) &&
      (pOutRespVerifySignature != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_VERIFY_SIGNATURE, &InMAC);

    /* Verify signature [entity authentication] */
    pStSafeA->InOutBuffer.LV.Data[0] = 0x01U;

    /* Signature offset */
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Signature */
    if ((pInRSignature->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] =
                                  (uint8_t)((pInRSignature->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] =
                                  (uint8_t)(pInRSignature->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInRSignature->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInRSignature->Data,
                   pInRSignature->Length);

      pStSafeA->InOutBuffer.LV.Length += 2U + pInRSignature->Length;
    }


    if ((pInSSignature->Length > 0U))
    {
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length] =
                                  (uint8_t)((pInSSignature->Length & 0xFF00U) >> 8);
      pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 1U] =
                                  (uint8_t)(pInSSignature->Length & 0x00FFU);

      STSAFEA_CHECK_SIZE(InMAC, pStSafeA->InOutBuffer.LV.Length + STSAFEA_LENGTH_SIZE, pInSSignature->Length);
      (void)memcpy(&pStSafeA->InOutBuffer.LV.Data[pStSafeA->InOutBuffer.LV.Length + 2U],
                   pInSSignature->Data,
                   pInSSignature->Length);

      pStSafeA->InOutBuffer.LV.Length += 2U + pInSSignature->Length;
    }

    /* Transmit command */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_VERIFY_ENTITY_SIGNATURE_RESPONSE_LENGTH + STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_VERIFY_ENTITY_SIGNATURE);

      /* Read response */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {

        pOutRespVerifySignature->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutRespVerifySignature->SignatureValidity = pStSafeA->InOutBuffer.LV.Data[0];
      }
    }
  }

  return status_code;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* STSAFE_A100 */

#if defined(STSAFE_A110)

/* STSAFE-A110 Private functions ---------------------------------------------*/
/** @addtogroup STSAFE_A110_CORE_Exported_Functions
  * @{
  */

/** @addtogroup CORE_Exported_Functions_Group1 Administrative Commands APIs
  *  @brief    STSAFE-A APIs related to administrative commands
  *
@verbatim
 ===============================================================================
              ##### Administrative Commands  #####
 ===============================================================================
[..]
    (+) Command Authorization Configuration Query
@endverbatim
  * @{
  */
/**
  * @brief   StSafeA_CommandAuthorizationConfigurationQuery
  *          Query command to retrieve command authorization configuration.
  * @note    This command is valid for STSAFE-A110 only. If called on STSAFE-A100 will return STSAFEA_INCONSISTENT_COMMAND_DATA.
  *
  * @param   pStSafeA           : STSAFE-A1xx object pointer.
  * @param   InCmdAuthRecordNum : Expected number of authorization record.\n
  *                              Must correspond to the num of auth. record allocated for pOutCmdAuthConfig->pCommandAuthorizationRecord, if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0.\n
  *                              If unknown it's possible to specify 0 (zero). In this case the num of auth record will be returned in pOutCmdAuthConfig->CommandAuthorizationRecordNumber and
  *                              the function can then be called once again with the right InCmdAuthRecordNum value.\n
  *                              Specify 0xFF if STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1
  * @param   pOutDataPartition : StSafeA_CommandAuthorizationConfigurationBuffer_t structure pointer to be filled with the response data.\n
  *                              If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 0, the internal StSafeA_CommandAuthorizationRecordBuffer_t structure array must have been properly allocated.\n
  *                              If STSAFEA_USE_OPTIMIZATION_SHARED_RAM = 1 a pointer inside pStSafeA->InOutBuffer will be assigned to pOutCmdAuthConfig->pCommandAuthorizationRecord;
  * @param   InMAC             : MAC authenticating command/response
  * @retval  STSAFEA_OK if success, an error code otherwise
  */
StSafeA_ResponseCode_t StSafeA_CommandAuthorizationConfigurationQuery(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InCmdAuthRecordNum,
  StSafeA_CommandAuthorizationConfigurationBuffer_t *pOutCmdAuthConfig,
  uint8_t InMAC)
{
  StSafeA_ResponseCode_t status_code = STSAFEA_INVALID_PARAMETER;

  /* Check null pointers */
  if (IS_STSAFEA_HANDLER_VALID_PTR(pStSafeA) && (pOutCmdAuthConfig != NULL))
  {
    /* Check parameters */
    stsafea_assert_param(IS_STSAFEA_MAC(InMAC));

    /* Build command Header and process CMAC */
    StSafeA_BuildCommandHeaderCMAC(pStSafeA, STSAFEA_CMD_QUERY, &InMAC);

    /* Build command Data */
    pStSafeA->InOutBuffer.LV.Data[0] = (uint8_t)STSAFEA_TAG_COMMAND_AUTHORIZATION_CONFIGURATION;
    pStSafeA->InOutBuffer.LV.Length = 1U;

    /* Transmit */
    status_code = StSafeA_TransmitCommand(pStSafeA);

    if (status_code == STSAFEA_OK)
    {
      /* Set response length */
      pStSafeA->InOutBuffer.LV.Length = STSAFEA_CMD_AUTH_CONFIG_QUERY_MIN_RESPONSE_LENGTH +
                                        STSAFEA_R_MAC_LENGTH(InMAC);

      /* Wait for the command processing. Then check for the response */
      StSafeA_Delay(STSAFEA_MS_WAIT_TIME_CMD_QUERY);

      /* Receive */
      status_code = StSafeA_ReceiveResponse(pStSafeA);

      if (status_code == STSAFEA_OK)
      {
        uint8_t uIdx = 0U;
        pOutCmdAuthConfig->Length = pStSafeA->InOutBuffer.LV.Length;
        pOutCmdAuthConfig->ChangeRight = pStSafeA->InOutBuffer.LV.Data[uIdx];
        uIdx++;
        pOutCmdAuthConfig->CommandAuthorizationRecordNumber = pStSafeA->InOutBuffer.LV.Data[uIdx];
        uIdx++;

        if (pOutCmdAuthConfig->CommandAuthorizationRecordNumber > InCmdAuthRecordNum)
        {
          /* Return an error so the application can read the correct number of Records,
             allocate the right space (if not under STSAFEA_USE_OPTIMIZATION_SHARED_RAM optimization),
             and call back this API with the right InCmdAuthRecordNum parameter */
          status_code = STSAFEA_INVALID_RESP_LENGTH;
        }
        else
        {
#if (STSAFEA_USE_OPTIMIZATION_SHARED_RAM)
          /* In this case the direct pointer assignment allows the best memory optimization.
             But up to the caller to manage this memory content properly, copying it to a local buffer if needed */
          pOutCmdAuthConfig->pCommandAuthorizationRecord =
                                  (StSafeA_CommandAuthorizationRecordBuffer_t *)&pStSafeA->InOutBuffer.LV.Data[uIdx];
#else
          /* In this case a copy from buffer to buffer is done. It's expected that the
            application have been already allocated the right memory for the StSafeA_CommandAuthorizationRecordBuffer[] */
          if (pOutCmdAuthConfig->CommandAuthorizationRecordNumber > 0U)
          {
            if (pOutCmdAuthConfig->pCommandAuthorizationRecord != NULL)
            {
              uint8_t record_max_len = (uint8_t)sizeof(StSafeA_CommandAuthorizationRecordBuffer_t);
              for (uint8_t i = 0; i < pOutCmdAuthConfig->CommandAuthorizationRecordNumber; i++)
              {
                pOutCmdAuthConfig->pCommandAuthorizationRecord[i].CommandCode =
                                  pStSafeA->InOutBuffer.LV.Data[uIdx];
                pOutCmdAuthConfig->pCommandAuthorizationRecord[i].CommandAC =
                                  pStSafeA->InOutBuffer.LV.Data[uIdx + 1U];
                pOutCmdAuthConfig->pCommandAuthorizationRecord[i].HostEncryptionFlags =
                                  pStSafeA->InOutBuffer.LV.Data[uIdx + 2U];
                uIdx += record_max_len;
              }
            }
            else
            {
              status_code = STSAFEA_INVALID_PARAMETER;
            }
          }
#endif /* STSAFEA_USE_OPTIMIZATION_SHARED_RAM */
        }
      }
    }
  }

  return status_code;
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* STSAFE_A110 */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
