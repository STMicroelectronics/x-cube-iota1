/**
  ******************************************************************************
  * @file    flash.h
  * @author  MCD Application Team
  * @brief   Management of the internal flash memory.
  *          Header for flash.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
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
#ifndef flash_H
#define flash_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

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

/** @addtogroup FLASH_Config_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/**
  * @brief  Update a chunk of the FLASH memory.
  * @note   The FLASH chunk must no cross a FLASH bank boundary.
  * @note   The source and destination buffers have no specific alignment constraints.
  * @param  In: dst_addr    Destination address in the FLASH memory.
  * @param  In: data        Source address. 
  * @param  In: size        Number of bytes to update.
  * @retval  0:  Success.
  *        !=0:  Failure.
  */
int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size);
int FLASH_Write(uint32_t uDestination, uint32_t *pSource, uint32_t uLength);
int FLASH_Erase_Size(uint32_t uStart, uint32_t uLength);

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

#endif /* flash_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

