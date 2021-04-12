/**
  ******************************************************************************
  * @file    vl53l0x_proximity.h
  * @author  MCD Application Team
  * @brief   Header for vl53l0x_proximity.c file
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
#ifndef VL53L0X_PROXIMITY_H
#define VL53L0X_PROXIMITY_H

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

/** @defgroup VL53l0x_Proximity VL53l0x Proximity
 * @{
 */

/** @defgroup VL53l0x_Proximity_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/* Defines -------------------------------------------------------------------*/
#define PROXIMITY_I2C_ADDRESS         ((uint16_t)0x0052)
#define VL53L0X_ID                    ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC

void VL53L0X_PROXIMITY_MspInit(void);
uint16_t VL53L0X_PROXIMITY_GetDistance(void);
void VL53L0X_PROXIMITY_Init(void);

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

#endif /* VL53L0X_PROXIMITY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
