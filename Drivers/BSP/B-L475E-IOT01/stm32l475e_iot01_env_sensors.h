/**
  ******************************************************************************
  * @file    stm32l475e_iot01_env_sensors.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l475e_iot01_env_sensors.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L475E_IOT01_ENV_SENSORS_H
#define STM32L475E_IOT01_ENV_SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_conf.h"
#include "stm32l475e_iot01_errno.h"
#include "../Components/Common/env_sensor.h"
#include "../Components/lps22hb/lps22hb.h"
#include "../Components/hts221/hts221.h"
#include "math.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @addtogroup STM32L475E_IOT01_ENV_SENSORS
  * @{
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Exported_Types STM32L475E_IOT01 ENV_SENSOR Exported Types
  * @{
  */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Exported_Constants STM32L475E_IOT01 ENV_SENSOR Exported Constants
  * @{
  */
/* Two environmental sensors are mounted on the B-L475E-IOT01 board,
   so the number of environmental sensors instances is set to 2.
   Instance 0 corresponds to HTS221 (temperature and humidity capabilities).
   Instance 1 corresponds to LPS22HB (temperature and pressure capabilities). */
#define ENV_SENSOR_INSTANCES_NBR    2U

#define ENV_SENSOR_FUNCTIONS_NBR    3U

#define ENV_TEMPERATURE      1U
#define ENV_PRESSURE         2U
#define ENV_HUMIDITY         4U
/**
  * @}
  */

/** @addtogroup STM32L475E_IOT01_ENV_SENSORS_Exported_Variables
  * @{
  */
/* Environmental sensor context */
extern ENV_SENSOR_Ctx_t Env_Sensor_Ctx[ENV_SENSOR_INSTANCES_NBR];

/* Environmental sensor component object */
extern void *Env_Sensor_CompObj[ENV_SENSOR_INSTANCES_NBR];

/* Environmental sensor common driver */
extern ENV_SENSOR_CommonDrv_t *Env_Sensor_Drv[ENV_SENSOR_INSTANCES_NBR];

/* Environmental sensor function driver */
extern ENV_SENSOR_FuncDrv_t *Env_Sensor_FuncDrv[ENV_SENSOR_INSTANCES_NBR][ENV_SENSOR_FUNCTIONS_NBR];
/**
  * @}
  */

/** @addtogroup STM32L475E_IOT01_ENV_SENSORS_Exported_Functions
  * @{
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value);
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

#endif /* STM32L475E_IOT01_ENV_SENSORS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
