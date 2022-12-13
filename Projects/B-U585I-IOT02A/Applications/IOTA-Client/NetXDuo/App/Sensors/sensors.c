/**
  **********************************************************************************************************************
  * @file    sensors.c
  * @author  MCD Application Team
  * @brief   This file implements the web server sensors services
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include "b_u585i_iot02a_env_sensors.h"


extern RTC_HandleTypeDef hrtc;

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/
static time_t make_timestamp();

/* Functions prototypes ----------------------------------------------------------------------------------------------*/


/**
  * @brief  Start temperature sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int temp_sensor_start(void)
{
  float outputDataRate = 0;
  int   status         = BSP_ERROR_NONE;

  /* Initialize function */
  status |= BSP_ENV_SENSOR_Init(0, ENV_TEMPERATURE);

  /* Enable function */
  status |= BSP_ENV_SENSOR_Enable(0, ENV_TEMPERATURE);

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(0, ENV_TEMPERATURE, 12.5f);
  status |= BSP_ENV_SENSOR_GetOutputDataRate(0, ENV_TEMPERATURE, &outputDataRate);
  if (outputDataRate != 12.5f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(0, ENV_TEMPERATURE, 7.0f);
  status |= BSP_ENV_SENSOR_GetOutputDataRate(0, ENV_TEMPERATURE, &outputDataRate);
  if (outputDataRate != 7.0f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  return status;
}

/**
  * @brief  Stop temperature sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int temp_sensor_stop(void)
{
  int status = BSP_ERROR_NONE;

  /* De-Initialize function */
  status |= BSP_ENV_SENSOR_DeInit(0);

  return status;
}

/**
  * @brief  Read temperature sensor acquisition value.
  * @param  None
  * @retval BSP status
  */
int temp_sensor_read(float *value)
{
  int   status = BSP_ERROR_NONE;

  /* Get value function */
  status |= BSP_ENV_SENSOR_GetValue(0, ENV_TEMPERATURE, value);

  return status;
}

/**
  * @brief  Start pressure sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int press_sensor_start(void)
{
  float outputDataRate = 0;
  int   status         = BSP_ERROR_NONE;

  /* Initialize function */
  status |= BSP_ENV_SENSOR_Init(1, ENV_PRESSURE);

  /* Enable function */
  status |= BSP_ENV_SENSOR_Enable(1, ENV_PRESSURE);

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(1, ENV_PRESSURE, 75.0f);
  status |= BSP_ENV_SENSOR_GetOutputDataRate(1, ENV_PRESSURE, &outputDataRate);
  if (outputDataRate != 75.0f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(1, ENV_PRESSURE, 50.0f);
  status |= BSP_ENV_SENSOR_GetOutputDataRate(1, ENV_PRESSURE, &outputDataRate);
  if (outputDataRate != 50.0f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  return status;
}

/**
  * @brief  Stop pressure sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int press_sensor_stop(void)
{
  int status = BSP_ERROR_NONE;

  /* De-Initialize function */
  status |= BSP_ENV_SENSOR_DeInit(1);

  return status;
}

/**
  * @brief  Read pressure sensor acquisition value.
  * @param  None
  * @retval BSP status
  */
int press_sensor_read(float *value)
{
  int   status = BSP_ERROR_NONE;

  /* Get value function */
  status |= BSP_ENV_SENSOR_GetValue(1, ENV_PRESSURE, value);

  return status;
}

/**
  * @brief  Start humidity sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int humid_sensor_start(void)
{
  float outputDataRate = 0;
  int   status         = BSP_ERROR_NONE;

  /* Initialize function */
  status |= BSP_ENV_SENSOR_Init(0, ENV_HUMIDITY);

  /* Enable function */
  status |= BSP_ENV_SENSOR_Enable(0, ENV_HUMIDITY);

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(0, ENV_HUMIDITY, 12.5f);

  status |= BSP_ENV_SENSOR_GetOutputDataRate(0, ENV_HUMIDITY, &outputDataRate);
  if (outputDataRate != 12.5f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* GetOutputDataRate and SetOutputDataRate functions */
  status |= BSP_ENV_SENSOR_SetOutputDataRate(0, ENV_HUMIDITY, 7.0f);
  status |= BSP_ENV_SENSOR_GetOutputDataRate(0, ENV_HUMIDITY, &outputDataRate);
  if (outputDataRate != 7.0f)
  {
    status |= BSP_ERROR_UNKNOWN_FAILURE;
  }

  return status;
}

/**
  * @brief  Stop humidity sensor acquisition.
  * @param  None
  * @retval BSP status
  */
int humid_sensor_stop(void)
{
  int status = BSP_ERROR_NONE;

  /* De-Initialize function */
  status |= BSP_ENV_SENSOR_DeInit(0);

  return status;
}

/**
  * @brief  Read humidity sensor acquisition value.
  * @param  None
  * @retval BSP status
  */
int humid_sensor_read(float *value)
{
  int   status = BSP_ERROR_NONE;

  /* Get value function */
  status |= BSP_ENV_SENSOR_GetValue(0, ENV_HUMIDITY, value);

  return status;
}

/**
  * @brief  Start temperature, pressure and humidity sensors acquisition.
  * @param  None
  * @retval BSP status
  */
int sensors_start(void)
{
  int   status = BSP_ERROR_NONE;

  /* De-Initialize function */
  status |= BSP_ENV_SENSOR_DeInit(0);
  status |= BSP_ENV_SENSOR_DeInit(1);

  /* Start sensors */
  status |= press_sensor_start();
  status |= humid_sensor_start();
  status |= temp_sensor_start();

  return status;
}


static time_t make_timestamp()
{
  RTC_TimeTypeDef currentTime;
  RTC_DateTypeDef currentDate;
  time_t timestamp;
  struct tm currTime;
  
  HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);
  
  currTime.tm_year = currentDate.Year + 100;  // In fact: 2000 + 18 - 1900
  currTime.tm_mday = currentDate.Date;
  currTime.tm_mon  = currentDate.Month - 1;
  
  currTime.tm_hour = currentTime.Hours;
  currTime.tm_min  = currentTime.Minutes;
  currTime.tm_sec  = currentTime.Seconds;
  
  timestamp = mktime(&currTime);
  
  return(timestamp);
}

int sensor_data_to_json(char *buffer, int buf_len)
{
  char device[] = "B-U585I-IOT02A";
  float_t Temp, Humi;
  int size;
  int32_t rc = 0;

  if(buffer != NULL)
  {
    humid_sensor_read(&Humi);
    temp_sensor_read(&Temp);
    
    time_t timestamp = make_timestamp();
#ifdef __ARMCC_VERSION
    size = snprintf(buffer, buf_len, "{\"Device\":\"%s\",\"time\":%u,\"temp\":%.2f,\"humi\":%.2f}", device, timestamp, Temp, Humi);
#else
    size = snprintf(buffer, buf_len, "{\"Device\":\"%s\",\"time\":%llu,\"temp\":%.2f,\"humi\":%.2f}", device, timestamp, Temp, Humi);
#endif /* __ARMCC_VERSION */
    
    if (size <= 0) {
      rc = -1;
    }
  } else {
    rc = -1;
  }

  return rc;
}


