/**
  **********************************************************************************************************************
  * @file    sensors.h
  * @author  MCD Application Team
  * @brief   Header for sensors.c module
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

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef SENSORS_H
#define SENSORS_H

/* Includes ----------------------------------------------------------------------------------------------------------*/


/* Exported types ----------------------------------------------------------------------------------------------------*/
/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------------------------------------------- */
int temp_sensor_start(void);
int temp_sensor_stop(void);
int temp_sensor_read(float *value);
int press_sensor_start(void);
int press_sensor_stop(void);
int press_sensor_read(float *value);
int humid_sensor_start(void);
int humid_sensor_stop(void);
int humid_sensor_read(float *value);
int sensors_start(void);
int sensor_data_to_json(char *buffer, int buf_len);

/* Private defines ---------------------------------------------------------------------------------------------------*/

#endif /* SENSORS_H */
