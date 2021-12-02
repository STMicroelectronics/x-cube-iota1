/**
  ******************************************************************************
  * @file    stsafea_service.h
  * @author  SMD/AME application teams
  * @version V3.2.0
  * @brief   Header file of STSAFE-A Middleware Service module.
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STSAFEA_SERVICE_H
#define STSAFEA_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stsafeaxxx.h"

/** @addtogroup STSAFE_A1XX_CORE_MODULES
  * @{
  */


/** @addtogroup SERVICE
  * @{
  */


/* Exported types ------------------------------------------------------------*/
/** @addtogroup SERVICE_Exported_Types
  * @{
  */
typedef struct
{
  int32_t  (* IOInit)(void);
  int32_t  (* BusInit)(void);
  int32_t  (* BusDeInit)(void);
  int32_t  (* BusRecv)(uint16_t address, uint8_t *, uint16_t);
  int32_t  (* BusSend)(uint16_t address, uint8_t *, uint16_t);
  void     (* TimeDelay)(uint32_t msDelay);
  int32_t  (*CrcInit)(void);
  uint32_t (*CrcCompute)(uint8_t *pData1, uint16_t Length1, uint8_t *pData2, uint16_t Length2);
  uint16_t DevAddr;
} STSAFEA_HW_t;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup SERVICE_Error_Constants Generic Low Level error definitions to maintain platform independency
  * @{
  */
#define STSAFEA_BUS_OK   ( 0)
#define STSAFEA_BUS_ERR  (-1)
#define STSAFEA_BUS_NACK ( 1)

/**
  * @}
  */

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/** @addtogroup SERVICE_Exported_Functions
  * @{
  */

/** @addtogroup SERVICE_Exported_Functions_Group1 Initialization / Configuration APIs
  *  @brief    STSAFE-A Service APIs related to initialization and configuration
  *
@verbatim
 ===============================================================================
              ##### Initialization/Configuration functions #####
 ===============================================================================

@endverbatim
  * @{
  */
int8_t StSafeA_HW_Init(void);
/**
  * @}
  */


/** @addtogroup SERVICE_Exported_Functions_Group2 Low Level operation APIs
  *  @brief    STSAFE-A Service APIs related to low level operations
  *
@verbatim
 ===============================================================================
              ##### Low Level operation functions #####
 ===============================================================================

@endverbatim
  * @{
  */

StSafeA_ResponseCode_t StSafeA_Transmit(StSafeA_TLVBuffer_t *pTLV_Buffer, uint8_t CrcSupport);
StSafeA_ResponseCode_t StSafeA_Receive(StSafeA_TLVBuffer_t *pTLV_Buffer, uint8_t CrcSupport);
void                   StSafeA_Delay(uint32_t msDelay);
/**
  * @}
  */


/**
  * @}
  */


/* Interface weak functions ***************************************************/
/** @addtogroup SERVICE_Interface_Functions
  *  @brief    These functions MUST be implemented at application layer
  * @{
  */
int8_t StSafeA_HW_Probe(void  *pCtx);
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

#endif /* STSAFEA_SERVICE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
