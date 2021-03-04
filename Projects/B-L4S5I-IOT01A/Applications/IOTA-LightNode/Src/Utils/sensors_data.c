/**
  ******************************************************************************
  * @file    sensors_data.c
  * @author  MCD Application Team
  * @brief   Prepare data of B-L475E-IOT01 sensors.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sensors_data.h"

#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_env_sensors.h"
#include "stm32l475e_iot01_motion_sensors.h"
#include "vl53l0x_proximity.h"
#include "msg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define INSTANCE_TEMPERATURE_HUMIDITY 0
#define INSTANCE_TEMPERATURE_PRESSURE 1
#define INSTANCE_GYROSCOPE_ACCELEROMETER 0
#define INSTANCE_MAGNETOMETER 1
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  init_sensors
  * @param  none
  * @retval 0  in case of success
  *         <0 BSP error code in case of failure
  */
int init_sensors(void)
{
  int32_t ret = 0;

  /* For sensors combos, must do the two initializations first, then the enables */
  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_HUMIDITY, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Init(ENV_HUMIDITY) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_HUMIDITY, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Init(ENV_TEMPERATURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_HUMIDITY, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Enable(ENV_HUMIDITY) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_HUMIDITY, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Enable(ENV_TEMPERATURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_PRESSURE, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Init(ENV_PRESSURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_PRESSURE, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_ENV_SENSOR_Enable(ENV_PRESSURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_MAGNETOMETER, MOTION_MAGNETO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Init(MOTION_MAGNETO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_MAGNETOMETER, MOTION_MAGNETO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Enable(MOTION_MAGNETO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Init(MOTION_GYRO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_ACCELERO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Init(MOTION_ACCELERO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_ACCELERO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Enable(MOTION_ACCELERO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    msg_error("BSP_MOTION_SENSOR_Enable(MOTION_GYRO) returns %ld\n", ret);
    goto error;
  }

  VL53L0X_PROXIMITY_Init();

  error:

  return ret;
}

/**
  * @brief  fill the buffer with the sensor values
  * @param  none
  * @param Buffer is the char pointer for the buffer to be filled
  * @param Size size of the above buffer
  * @retval 0 in case of success
  *         -1 in case of failure
  */
int PrepareSensorsData(char * Buffer, int Size, char * deviceID)
{
  int rc = -1;

  return rc;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
