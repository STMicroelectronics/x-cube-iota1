/**
  ******************************************************************************
  * @file    stm32l475e_iot01_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef STM32L475E_IOT01_CONF_H
#define STM32L475E_IOT01_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* COM define */
#define USE_BSP_COM_FEATURE                  1U

#define USE_COM_LOG                          0U

#define USE_ENV_SENSOR_HTS221_0              1U
#define USE_ENV_SENSOR_LPS22HB_0             1U

#define USE_MOTION_SENSOR_LSM6DSL_0          1U
#define USE_MOTION_SENSOR_LIS3MDL_0          1U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY          0x0FU

/* I2C2 Frequency in Hz  */
#define BUS_I2C2_FREQUENCY                  100000U /* Frequency of I2C2 = 100 KHz*/

/* SPI1 Baud rate in bps  */
#define BUS_SPI1_BAUDRATE                   16000000U /* baud rate of SPIn = 16 Mbps */

#ifdef __cplusplus
}
#endif

#endif /* STM32L475E_IOT01_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
