/**
  ******************************************************************************
  * @file    stm32l475e_iot01_motion_sensors.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the 
  *          motion sensors mounted on the B-L475E-IOT01 board.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_motion_sensors.h"
#include "stm32l475e_iot01_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS STM32L475E_IOT01 MOTION_SENSOR
  * @{
  */

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS_Private_Defines STM32L475E_IOT01 MOTION_SENSOR Private Defines
  * @{
  */
#define GyroIdx     0U
#define AcceleroIdx 1U
#define MagnetoIdx  2U
/**
  * @}
  */ 

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS_Exported_Variables STM32L475E_IOT01 MOTION_SENSOR Exported Variables
  * @{
  */
MOTION_SENSOR_Ctx_t        Motion_Sensor_Ctx[MOTION_SENSOR_INSTANCES_NBR] = {{0}, {0}};
void                      *Motion_Sensor_CompObj[MOTION_SENSOR_INSTANCES_NBR] = {0, 0};
MOTION_SENSOR_CommonDrv_t *Motion_Sensor_Drv[MOTION_SENSOR_INSTANCES_NBR] = {0, 0};
MOTION_SENSOR_FuncDrv_t   *Motion_Sensor_FuncDrv[MOTION_SENSOR_INSTANCES_NBR][MOTION_SENSOR_FUNCTIONS_NBR] = {{0, 0, 0},
                                                                                                              {0, 0, 0}};
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS_Private_Function_Prototypes STM32L475E_IOT01 MOTION_SENSOR Private Function Prototypes
  * @{
  */
static int32_t LSM6DSL_Probe(uint32_t Functions);
static int32_t LIS3MDL_Probe(uint32_t Functions);
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS_Exported_Functions STM32L475E_IOT01 MOTION_SENSOR Exported Functions
  * @{
  */
/**
  * @brief  Initialize the motion sensor.
  * @param  Instance Motion sensor instance.
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Functions & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Functions & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    /* Probe the motion sensor */
    if (Instance == 0U)
    {
      if (LSM6DSL_Probe(Functions) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      if (LIS3MDL_Probe(Functions) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    
    if (status == BSP_ERROR_NONE)
    {
      /* Store current initialized functions */
      Motion_Sensor_Ctx[Instance].Functions |= Functions;
    }
  }
  return status;
}

/**
  * @brief  De-initialize the motion sensor.
  * @param  Instance Motion sensor instance.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Motion_Sensor_Ctx[Instance].Functions != 0U)
  {
    if (Motion_Sensor_Drv[Instance]->DeInit(Motion_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Reset current initialized functions */
      Motion_Sensor_Ctx[Instance].Functions = 0U;
    }
  }
  else
  {
    /* Nothing to do (not initialized) */
  }
  return status;
}

/**
  * @brief  Get motion sensor capabilities.
  * @param  Instance Motion sensor instance.
  * @param  Capabilities Pointer to motion sensor capabilities.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetCapabilities(uint32_t Instance, MOTION_SENSOR_Capabilities_t *Capabilities)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Capabilities == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Motion_Sensor_Ctx[Instance].Functions == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Motion_Sensor_Drv[Instance]->GetCapabilities(Motion_Sensor_CompObj[Instance], Capabilities) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Read motion sensor ID.
  * @param  Instance Motion sensor instance.
  * @param  Id Pointer to motion sensor ID.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Id == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Motion_Sensor_Ctx[Instance].Functions == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Read the motion sensor ID */
    if (Motion_Sensor_Drv[Instance]->ReadID(Motion_Sensor_CompObj[Instance], Id) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Enable one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Enable the motion sensor function */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if(Motion_Sensor_FuncDrv[Instance][index]->Enable(Motion_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Disable one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Disable the motion sensor function */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->Disable(Motion_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get axes from one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Axes Pointer to motion sensor axes.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Axes == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the motion sensor axes */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->GetAxes(Motion_Sensor_CompObj[Instance], Axes) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get raw axes from one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Axes Pointer to motion sensor raw axes.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_AxesRaw_t *Axes)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Axes == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the motion sensor row axes */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->GetAxesRaw(Motion_Sensor_CompObj[Instance], Axes) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get sensitivity from one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Sensitivity Pointer to sensitivity.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float_t *Sensitivity)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Sensitivity == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the motion sensor sensitivity */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->GetSensitivity(Motion_Sensor_CompObj[Instance], Sensitivity) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get output data rate from one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Odr Pointer to output data rate.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Odr == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the motion sensor output data rate */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->GetOutputDataRate(Motion_Sensor_CompObj[Instance], Odr) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Set output data rate of one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Odr Output data rate.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Set the motion sensor output data rate */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->SetOutputDataRate(Motion_Sensor_CompObj[Instance], Odr) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get full scale value from one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Fullscale Pointer to full scale value.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= MOTION_SENSOR_INSTANCES_NBR) || (Fullscale == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the motion sensor full scale */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->GetFullScale(Motion_Sensor_CompObj[Instance], Fullscale) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Set full scale value of one motion sensor function.
  * @param  Instance Motion sensor instance.
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO or MOTION_ACCELERO for instance 0
  *         - MOTION_MAGNETO for instance 1
  * @param  Fullscale Full scale value.
  * @retval BSP status.
  */
int32_t BSP_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= MOTION_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Function & MOTION_MAGNETO) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Function & (MOTION_GYRO | MOTION_ACCELERO)) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Motion_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Set the motion sensor full scale */
    index = (Function == MOTION_GYRO) ? GyroIdx : ((Function == MOTION_ACCELERO) ? AcceleroIdx : MagnetoIdx);
    if (Motion_Sensor_FuncDrv[Instance][index]->SetFullScale(Motion_Sensor_CompObj[Instance], Fullscale) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_MOTION_SENSORS_Private_Functions STM32L475E_IOT01 MOTION_SENSOR Private Functions
  * @{
  */
/**
  * @brief  Probe the LSM6DSL motion sensor driver.
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status.
  */
static int32_t LSM6DSL_Probe(uint32_t Functions)
{
  int32_t                 status = BSP_ERROR_NONE;
  LSM6DSL_IO_t            IOCtx;
  uint8_t                 lsm6dsl_id;
  static LSM6DSL_Object_t LSM6DSL_Obj;

  /* Configure the motion sensor driver */
  IOCtx.BusType     = LSM6DSL_I2C_BUS;
  IOCtx.Address     = LSM6DSL_I2C_ADD_L;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (LSM6DSL_RegisterBusIO(&LSM6DSL_Obj, &IOCtx) != LSM6DSL_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (LSM6DSL_ReadID(&LSM6DSL_Obj, &lsm6dsl_id) != LSM6DSL_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if (lsm6dsl_id != LSM6DSL_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Motion_Sensor_CompObj[0] = &LSM6DSL_Obj;
    Motion_Sensor_Drv[0]     = (MOTION_SENSOR_CommonDrv_t *) &LSM6DSL_COMMON_Driver;
    if (Motion_Sensor_Drv[0]->Init(Motion_Sensor_CompObj[0]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if ((Functions & MOTION_GYRO) != 0U)
      {
        Motion_Sensor_FuncDrv[0][GyroIdx] = (MOTION_SENSOR_FuncDrv_t *) &LSM6DSL_GYRO_Driver;
      }
      if ((Functions & MOTION_ACCELERO) != 0U)
      {
        Motion_Sensor_FuncDrv[0][AcceleroIdx] = (MOTION_SENSOR_FuncDrv_t *) &LSM6DSL_ACC_Driver;
      }
    }
  }

  return status;
}

/**
  * @brief  Probe the LIS3MDL motion sensor driver.
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_MAGNETO
  * @retval BSP status.
  */
static int32_t LIS3MDL_Probe(uint32_t Functions)
{
  int32_t                 status = BSP_ERROR_NONE;
  LIS3MDL_IO_t            IOCtx;
  uint8_t                 lis3mdl_id;
  static LIS3MDL_Object_t LIS3MDL_Obj;

  /* Configure the motion sensor driver */
  IOCtx.BusType     = LIS3MDL_I2C_BUS;
  IOCtx.Address     = LIS3MDL_I2C_ADD_H;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (LIS3MDL_RegisterBusIO(&LIS3MDL_Obj, &IOCtx) != LIS3MDL_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (LIS3MDL_ReadID(&LIS3MDL_Obj, &lis3mdl_id) != LIS3MDL_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if (lis3mdl_id != LIS3MDL_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Motion_Sensor_CompObj[1] = &LIS3MDL_Obj;
    Motion_Sensor_Drv[1]     = (MOTION_SENSOR_CommonDrv_t *) &LIS3MDL_COMMON_Driver;
    if (Motion_Sensor_Drv[1]->Init(Motion_Sensor_CompObj[1]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if ((Functions & MOTION_MAGNETO) != 0U)
      {
        Motion_Sensor_FuncDrv[1][MagnetoIdx] = (MOTION_SENSOR_FuncDrv_t *) &LIS3MDL_MAG_Driver;
      }
    }
  }

  return status;
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
