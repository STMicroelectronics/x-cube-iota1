/**
  ******************************************************************************
  * @file    b_u585i_iot02a_crc.h
  * @author  SMD Application Team
  * @brief   B-U585I-IOT02A CRC
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_U585I_IOT02A_CRC_H
#define B_U585I_IOT02A_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "b_u585i_iot02a_conf.h"

/** @addtogroup CRC
  * @{
  */


/** @addtogroup B_U585I_IOT02A
  * @{
  */


/** @addtogroup B_U585I_IOT02A_CRC
  * @{
  */


/** @defgroup B_U585I_IOT02A_CRC_Exported_Constants STM32L4xx NUCLEO CRC Exported Constants
  * @{
  */
/**
  * @}B_U585I_IOT02A_CRC_Exported_Constants
  */


/** @defgroup B_U585I_IOT02A_CRC_Private_Types STM32L4xx NUCLEO CRC Private types
  * @{
  */


/**
  * @}B_U585I_IOT02A_CRC_Private_Types
  */


/** @defgroup B_U585I_IOT02A_LOW_LEVEL_Exported_Variables LOW LEVEL Exported Constants
  * @{
  */


/**
  * @}B_U585I_IOT02A_LOW_LEVEL_Exported_Variables
  */


/** @addtogroup B_U585I_IOT02A_CRC_Exported_Functions
  * @{
  */
int32_t CRC16X25_Init(void);
uint32_t CRC_Compute(uint8_t *pData1, uint16_t Length1, uint8_t *pData2, uint16_t Length2);
/**
  * @}B_U585I_IOT02A_CRC_Exported_Functions
  */


/**
  * @}B_U585I_IOT02A_CRC
  */

/**
  * @}B_U585I_IOT02A
  */


/**
  * @}CRC
  */


#ifdef __cplusplus
}
#endif

#endif /* B_U585I_IOT02A_CRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
