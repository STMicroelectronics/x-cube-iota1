/**
  ******************************************************************************
  * @file    stm32l475e_iot01_env_sensors.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the 
  *          environmental sensors mounted on the B-L475E-IOT01 board.
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
#include "stm32l475e_iot01_env_sensors.h"
#include "stm32l475e_iot01_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS STM32L475E_IOT01 ENV_SENSOR
  * @{
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Private_Defines STM32L475E_IOT01 ENV_SENSOR Private Defines
  * @{
  */
#define TemperatureIdx  0U
#define PressureIdx     1U
#define HumidityIdx     2U
/**
  * @}
  */ 

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Exported_Variables STM32L475E_IOT01 ENV_SENSOR Exported Variables
  * @{
  */
/* Environmental sensor context */
ENV_SENSOR_Ctx_t Env_Sensor_Ctx[ENV_SENSOR_INSTANCES_NBR] = {{0}, {0}};

/* Environmental sensor component object */
void *Env_Sensor_CompObj[ENV_SENSOR_INSTANCES_NBR] = {0, 0};

/* Environmental sensor common driver */
ENV_SENSOR_CommonDrv_t *Env_Sensor_Drv[ENV_SENSOR_INSTANCES_NBR] = {0, 0};

/* Environmental sensor function driver */
ENV_SENSOR_FuncDrv_t *Env_Sensor_FuncDrv[ENV_SENSOR_INSTANCES_NBR][ENV_SENSOR_FUNCTIONS_NBR] = {{0, 0, 0},
                                                                                                {0, 0, 0}};
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Private_Function_Prototypes STM32L475E_IOT01 ENV_SENSOR Private Function Prototypes
  * @{
  */
static int32_t HTS221_Probe(uint32_t Functions);
static int32_t LPS22HB_Probe(uint32_t Functions);
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Exported_Functions STM32L475E_IOT01 ENV_SENSOR Exported Functions
  * @{
  */

/**
  * @brief  Initialize the environmental sensor.
  * @param  Instance Environmental sensor instance.
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE and/or ENV_PRESSURE for instance 1
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && ((Functions & ENV_PRESSURE) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && ((Functions & ENV_HUMIDITY) != 0U))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    /* Probe the motion sensor */
    if (Instance == 0U)
    {
      if (HTS221_Probe(Functions) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    else /* Instance = 1 */
    {
      if (LPS22HB_Probe(Functions) != BSP_ERROR_NONE)
      {
        status = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    
    if (status == BSP_ERROR_NONE)
    {
      /* Store current initialized functions */
      Env_Sensor_Ctx[Instance].Functions |= Functions;
    }
  }
  return status;
}

/**
  * @brief  De-initialize environmental sensor.
  * @param  Instance Environmental sensor instance.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance)
{
  int32_t status = BSP_ERROR_NONE;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Env_Sensor_Ctx[Instance].Functions != 0U)
  {
    if (Env_Sensor_Drv[Instance]->DeInit(Env_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Reset current initialized functions */
      Env_Sensor_Ctx[Instance].Functions = 0U;
    }
  }
  else
  {
    /* Nothing to do (not initialized) */
  }
  return status;
}

/**
  * @brief  Get environmental sensor capabilities.
  * @param  Instance Environmental sensor instance.
  * @param  Capabilities Pointer to environmental sensor capabilities.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= ENV_SENSOR_INSTANCES_NBR) || (Capabilities == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Env_Sensor_Ctx[Instance].Functions == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    if (Env_Sensor_Drv[Instance]->GetCapabilities(Env_Sensor_CompObj[Instance], Capabilities) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Read environmental sensor ID.
  * @param  Instance Environmental sensor instance.
  * @param  Id Pointer to environmental sensor ID.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t status = BSP_ERROR_NONE;

  if ((Instance >= ENV_SENSOR_INSTANCES_NBR) || (Id == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if (Env_Sensor_Ctx[Instance].Functions == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Read the environmental sensor ID */
    if (Env_Sensor_Drv[Instance]->ReadID(Env_Sensor_CompObj[Instance], Id) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Enable one environmental sensor function.
  * @param  Instance Environmental sensor instance.
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE or ENV_PRESSURE for instance 1
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (Function == ENV_PRESSURE))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Function == ENV_HUMIDITY))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Env_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Enable the environmental sensor function */
    index = (Function == ENV_TEMPERATURE) ? TemperatureIdx : ((Function == ENV_PRESSURE) ? PressureIdx : HumidityIdx);
    if(Env_Sensor_FuncDrv[Instance][index]->Enable(Env_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Disable one environmental sensor function.
  * @param  Instance Environmental sensor instance.
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE or ENV_PRESSURE for instance 1
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (Function == ENV_PRESSURE))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Function == ENV_HUMIDITY))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Env_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Disable the environmental sensor function */
    index = (Function == ENV_TEMPERATURE) ? TemperatureIdx : ((Function == ENV_PRESSURE) ? PressureIdx : HumidityIdx);
    if (Env_Sensor_FuncDrv[Instance][index]->Disable(Env_Sensor_CompObj[Instance]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get output data rate from one environmental sensor function.
  * @param  Instance Environmental sensor instance.
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE or ENV_PRESSURE for instance 1
  * @param  Odr Pointer to output data rate.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= ENV_SENSOR_INSTANCES_NBR) || (Odr == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (Function == ENV_PRESSURE))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Function == ENV_HUMIDITY))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Env_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Get the environmental sensor output data rate */
    index = (Function == ENV_TEMPERATURE) ? TemperatureIdx : ((Function == ENV_PRESSURE) ? PressureIdx : HumidityIdx);
    if (Env_Sensor_FuncDrv[Instance][index]->GetOutputDataRate(Env_Sensor_CompObj[Instance], Odr) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Set output data rate of one environmental sensor function.
  * @param  Instance Environmental sensor instance.
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE or ENV_PRESSURE for instance 1
  * @param  Odr Output data rate.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if (Instance >= ENV_SENSOR_INSTANCES_NBR)
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (Function == ENV_PRESSURE))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Function == ENV_HUMIDITY))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Env_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /* Set the environmental sensor output data rate */
    index = (Function == ENV_TEMPERATURE) ? TemperatureIdx : ((Function == ENV_PRESSURE) ? PressureIdx : HumidityIdx);
    if (Env_Sensor_FuncDrv[Instance][index]->SetOutputDataRate(Env_Sensor_CompObj[Instance], Odr) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}

/**
  * @brief  Get environmental sensor value.
  * @param  Instance Environmental sensor instance.
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance 0
  *         - ENV_TEMPERATURE or ENV_PRESSURE for instance 1
  * @param  Value Pointer to environmental sensor value.
  * @retval BSP status.
  */
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value)
{
  int32_t status = BSP_ERROR_NONE;
  uint8_t index;

  if ((Instance >= ENV_SENSOR_INSTANCES_NBR) || (Value == NULL))
  {
    status = BSP_ERROR_WRONG_PARAM;
  }
  else if ((Instance == 0U) && (Function == ENV_PRESSURE))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Instance == 1U) && (Function == ENV_HUMIDITY))
  {
    status = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if ((Env_Sensor_Ctx[Instance].Functions & Function) == 0U)
  {
    status = BSP_ERROR_NO_INIT;
  }
  else
  {
    /*  Get environmental sensor value */
    index = (Function == ENV_TEMPERATURE) ? TemperatureIdx : ((Function == ENV_PRESSURE) ? PressureIdx : HumidityIdx);
    if (Env_Sensor_FuncDrv[Instance][index]->GetValue(Env_Sensor_CompObj[Instance], Value) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return status;
}
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_ENV_SENSORS_Private_Functions STM32L475E_IOT01 ENV_SENSOR Private Functions
  * @{
  */
/**
  * @brief  Probe the HTS221 environmental sensor driver.
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_HUMIDITY
  * @retval BSP status.
  */
static int32_t HTS221_Probe(uint32_t Functions)
{
  int32_t                status = BSP_ERROR_NONE;
  HTS221_IO_t            IOCtx;
  uint8_t                hts221_id;
  static HTS221_Object_t HTS221_Obj;

  /* Configure the environmental sensor driver */
  IOCtx.BusType     = HTS221_I2C_BUS;
  IOCtx.Address     = HTS221_I2C_ADDRESS;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (HTS221_RegisterBusIO(&HTS221_Obj, &IOCtx) != HTS221_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (HTS221_ReadID(&HTS221_Obj, &hts221_id) != HTS221_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if (hts221_id != HTS221_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Env_Sensor_CompObj[0] = &HTS221_Obj;
    Env_Sensor_Drv[0]     = (ENV_SENSOR_CommonDrv_t *) &HTS221_COMMON_Driver;
    if (Env_Sensor_Drv[0]->Init(Env_Sensor_CompObj[0]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if ((Functions & ENV_TEMPERATURE) != 0U)
      {
        Env_Sensor_FuncDrv[0][TemperatureIdx] = (ENV_SENSOR_FuncDrv_t *) &HTS221_TEMP_Driver;
      }
      if ((Functions & ENV_HUMIDITY) != 0U)
      {
        Env_Sensor_FuncDrv[0][HumidityIdx] = (ENV_SENSOR_FuncDrv_t *) &HTS221_HUM_Driver;
      }
    }
  }

  return status;
}

/**
  * @brief  Probe the LPS22HB environmental sensor driver.
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_PRESSURE
  * @retval BSP status.
  */
static int32_t LPS22HB_Probe(uint32_t Functions)
{
  int32_t                 status = BSP_ERROR_NONE;
  LPS22HB_IO_t            IOCtx;
  uint8_t                 lps22hb_id;
  static LPS22HB_Object_t LPS22HB_Obj;

  /* Configure the environmental sensor driver */
  IOCtx.BusType     = LPS22HB_I2C_BUS;
  IOCtx.Address     = LPS22HB_I2C_ADD_H;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if (LPS22HB_RegisterBusIO(&LPS22HB_Obj, &IOCtx) != LPS22HB_OK)
  {
    status = BSP_ERROR_BUS_FAILURE;
  }
  else if (LPS22HB_ReadID(&LPS22HB_Obj, &lps22hb_id) != LPS22HB_OK)
  {
    status = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if (lps22hb_id != LPS22HB_ID)
  {
    status = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    Env_Sensor_CompObj[1] = &LPS22HB_Obj;
    Env_Sensor_Drv[1]     = (ENV_SENSOR_CommonDrv_t *) &LPS22HB_COMMON_Driver;
    if (Env_Sensor_Drv[1]->Init(Env_Sensor_CompObj[1]) < 0)
    {
      status = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if ((Functions & ENV_TEMPERATURE) != 0U)
      {
        Env_Sensor_FuncDrv[1][TemperatureIdx] = (ENV_SENSOR_FuncDrv_t *) &LPS22HB_TEMP_Driver;
      }
      if ((Functions & ENV_PRESSURE) != 0U)
      {
        Env_Sensor_FuncDrv[1][PressureIdx] = (ENV_SENSOR_FuncDrv_t *) &LPS22HB_PRESS_Driver;
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
