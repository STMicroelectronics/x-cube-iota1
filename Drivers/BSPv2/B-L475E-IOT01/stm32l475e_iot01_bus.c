/**
  ******************************************************************************
  * @file    stm32l475e_iot01_bus.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage bus
  *          available on B-L475E-IOT01 board (MB1297) from
  *          STMicroelectronics.
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
#include "stm32l475e_iot01_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_BUS STM32L475E_IOT01 BUS
  * @{
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Marcos STM32L475E_IOT01 BUS Private Marcos
  * @{
  */
#define DIV_ROUND_CLOSEST(x, d)  (((x) + ((d) / 2U)) / (d))
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Defines STM32L475E_IOT01 BUS Private Defines
  * @{
  */
enum i2c_speed {
  I2C_SPEED_STANDARD, /* 100 kHz */
  I2C_SPEED_FAST, /* 400 kHz */
  I2C_SPEED_FAST_PLUS, /* 1 MHz */
};
#define I2C_ANALOG_FILTER_DELAY_MIN 50U  /* ns */
#define I2C_ANALOG_FILTER_DELAY_MAX 260U /* ns */
#define VALID_PRESC_NBR             100U
#define PRESC_MAX                   16U
#define NSEC_PER_SEC                1000000000UL
#define SCLDEL_MAX                  16U
#define SDADEL_MAX                  16U
#define SCLH_MAX                    256U
#define SCLL_MAX                    256U
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Structures STM32L475E_IOT01 BUS Private Structures
  * @{
  */
struct i2c_specs_s
{
  uint32_t rate;
  uint32_t rate_min;
  uint32_t rate_max;
  uint32_t fall_max;
  uint32_t rise_max;
  uint32_t hddat_min;
  uint32_t vddat_max;
  uint32_t sudat_min;
  uint32_t l_min;
  uint32_t h_min;
};

struct i2c_setup_s
{
  uint32_t rise_time;
  uint32_t fall_time;
  uint32_t dnf;
  uint32_t analog_filter;
};

struct i2c_timings_s
{
  uint8_t presc;
  uint8_t scldel;
  uint8_t sdadel;
  uint8_t sclh;
  uint8_t scll;
};
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Variables STM32L475E_IOT01 BUS Private Variables
  * @{
  */
static uint32_t I2c2InitCounter = 0;
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
static uint32_t Bus_IsI2c2MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

static uint32_t Spi1InitCounter = 0;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
static uint32_t Bus_IsSpi1MspCbValid = 0;
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Exported_Variables STM32L475E_IOT01 BUS Exported Variables
  * @{
  */
I2C_HandleTypeDef hbus_i2c2 = {0};
SPI_HandleTypeDef hbus_spi1 = {0};
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Function_Prototypes STM32L475E_IOT01 BUS Private Function Prototypes
  * @{
  */
static void     I2C2_MspInit(I2C_HandleTypeDef *hI2c);
static void     I2C2_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t  I2C2_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C2_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static uint32_t I2C_GetTiming(uint32_t clock_src_hz, uint32_t i2cfreq_hz);

static void     SPI1_MspInit(SPI_HandleTypeDef* hSpi);
static void     SPI1_MspDeInit(SPI_HandleTypeDef* hSpi);
static uint32_t SPI_GetPrescaler(uint32_t clock_src_hz, uint32_t baudrate_mbps);
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Exported_Functions STM32L475E_IOT01 BUS Exported Functions
  * @{
  */

/**
  * @brief  Initialize BSP I2C2.
  * @retval BSP status.
  */
int32_t BSP_I2C2_Init(void)
{
  int32_t status = BSP_ERROR_NONE;

  hbus_i2c2.Instance = BUS_I2C2;

  if (I2c2InitCounter == 0U)
  {
    if (HAL_I2C_GetState(&hbus_i2c2) == HAL_I2C_STATE_RESET)
    {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      /* Init the I2C1 Msp */
      I2C2_MspInit(&hbus_i2c2);

      if (MX_I2C2_Init(&hbus_i2c2, I2C_GetTiming(HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C2), BUS_I2C2_FREQUENCY)) != HAL_OK)
      {
        status = BSP_ERROR_BUS_FAILURE;
      }
#else
      if (Bus_IsI2c2MspCbValid == 0U)
      {
        if (BSP_I2C2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          status = BSP_ERROR_MSP_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        if (MX_I2C2_Init(&hbus_i2c2, I2C_GetTiming(HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C2), BUS_I2C2_FREQUENCY)) != HAL_OK)
        {
          status = BSP_ERROR_BUS_FAILURE;
        }
      }
#endif
    }
  }
  if (I2c2InitCounter < 0xFFFFFFFFU)
  {
    I2c2InitCounter++;
  }
  return status;
}

/**
  * @brief  DeInitialize BSP I2C2.
  * @retval BSP status.
  */
int32_t BSP_I2C2_DeInit(void)
{
  int32_t status = BSP_ERROR_NONE;

  if (I2c2InitCounter > 0U)
  {
    I2c2InitCounter--;
    if (I2c2InitCounter == 0U)
    {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      I2C2_MspDeInit(&hbus_i2c2);
#endif

      /* De-Init the I2C */
      if (HAL_I2C_DeInit(&hbus_i2c2) != HAL_OK)
      {
        status = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  Write 8bit values in registers of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg     The target register start address to write.
  * @param  pData   Pointer to data buffer.
  * @param  Length  Number of data.
  * @retval BSP status.
  */
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C2_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length);
}

/**
  * @brief  Read a 8bit values in registers of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg     The target register start address to read.
  * @param  pData   Pointer to data buffer.
  * @param  Length  Number of data.
  * @retval BSP status
  */
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C2_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length);
}

/**
  * @brief  Write 16bit values in registers of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg     The target register start address to write.
  * @param  pData   Pointer to data buffer.
  * @param  Length  Number of data.
  * @retval BSP status.
  */
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C2_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length);
}

/**
  * @brief  Read a 16bit values in registers of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg     The target register start address to read.
  * @param  pData   Pointer to data buffer.
  * @param  Length  Number of data.
  * @retval BSP status
  */
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C2_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length);
}

/**
  * @brief  Checks if target device is ready for communication.
  * @param  DevAddr  Target device address.
  * @param  Trials   Number of trials.
  * @retval BSP status
  */
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t status = BSP_ERROR_NONE;

  if (HAL_I2C_IsDeviceReady(&hbus_i2c2, DevAddr, Trials, BUS_I2C2_TIMEOUT) != HAL_OK)
  {
    status = BSP_ERROR_BUSY;
  }

  return status;
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register Default I2C2 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C2_RegisterDefaultMspCallbacks(void)
{
  int32_t ret = BSP_ERROR_NONE;

  __HAL_I2C_RESET_HANDLE_STATE(&hbus_i2c2);

  /* Register default MspInit/MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hbus_i2c2, HAL_I2C_MSPINIT_CB_ID, I2C2_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_I2C_RegisterCallback(&hbus_i2c2, HAL_I2C_MSPDEINIT_CB_ID, I2C2_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    Bus_IsI2c2MspCbValid = 1U;
  }

  /* BSP status */
  return ret;
}

/**
  * @brief Register I2C2 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C2 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C2_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  __HAL_I2C_RESET_HANDLE_STATE(&hbus_i2c2);

  /* Register MspInit/MspDeInit Callbacks */
  if (HAL_I2C_RegisterCallback(&hbus_i2c2, HAL_I2C_MSPINIT_CB_ID, Callback->pMspI2cInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_I2C_RegisterCallback(&hbus_i2c2, HAL_I2C_MSPDEINIT_CB_ID, Callback->pMspI2cDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    Bus_IsI2c2MspCbValid = 1U;
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

/**
  * @brief  MX I2C2 initialization.
  * @param  hI2c I2C handle.
  * @param  timing I2C timing.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hI2c, uint32_t timing)
{
  hI2c->Init.Timing           = timing;
  hI2c->Init.OwnAddress1      = 0;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  return HAL_I2C_Init(hI2c);
}

/**
  * @brief  Initialize BSP SPI1.
  * @retval BSP status.
  */
int32_t BSP_SPI1_Init(void)
{
  int32_t status = BSP_ERROR_NONE;

  hbus_spi1.Instance  = BUS_SPI1;

  if (Spi1InitCounter == 0U)
  {
    if (HAL_SPI_GetState(&hbus_spi1) == HAL_SPI_STATE_RESET)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
      /* Init the SPI Msp */
      SPI1_MspInit(&hbus_spi1);

      /* Init the SPI */
      if (MX_SPI1_Init(&hbus_spi1, SPI_GetPrescaler(HAL_RCC_GetPCLK2Freq(), BUS_SPI1_BAUDRATE)) != HAL_OK)
      {
        status = BSP_ERROR_BUS_FAILURE;
      }
#else
      if(Bus_IsSpi1MspCbValid == 0U)
      {
        if(BSP_SPI1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }

      if (status == BSP_ERROR_NONE)
      {
        if (MX_SPI1_Init(&hbus_spi1, SPI_GetPrescaler(HAL_RCC_GetPCLK2Freq(), BUS_SPI1_BAUDRATE)) != HAL_OK)
        {
          status = BSP_ERROR_BUS_FAILURE;
        }
      }
#endif
    }
  }
  if (Spi1InitCounter < 0xFFFFFFFFU)
  {
    Spi1InitCounter++;
  }  
  return status;
}

/**
  * @brief  DeInitialize BSP SPI1.
  * @retval BSP status.
  */
int32_t BSP_SPI1_DeInit(void)
{
  int32_t status = BSP_ERROR_NONE;

  if (Spi1InitCounter > 0U)
  {
    Spi1InitCounter--;
    if (Spi1InitCounter == 0U)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
      SPI1_MspDeInit(&hbus_spi1);
#endif

      /* De-Init the SPI */
      if(HAL_SPI_DeInit(&hbus_spi1) == HAL_OK)
      {
        status = BSP_ERROR_BUS_FAILURE;
      }
    }
  }

  return status;
}

/**
  * @brief  Send data through SPI BUS.
  * @param  pData  Pointer to data buffer.
  * @param  Length Length of data in byte.
  * @retval BSP status.
  */
int32_t BSP_SPI1_Send(uint8_t *pData, uint16_t Length)
{
  int32_t status = BSP_ERROR_UNKNOWN_FAILURE;

  if (HAL_SPI_Transmit(&hbus_spi1, pData, Length, BUS_SPI1_TIMEOUT) == HAL_OK)
  {
    status = BSP_ERROR_NONE;
  }
  return status;
}

/**
  * @brief  Receive data from SPI BUS
  * @param  pData  Pointer to data buffer.
  * @param  Length Length of data in byte.
  * @retval BSP status.
  */
int32_t  BSP_SPI1_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t status = BSP_ERROR_UNKNOWN_FAILURE;

  if (HAL_SPI_Receive(&hbus_spi1, pData, Length, BUS_SPI1_TIMEOUT) == HAL_OK)
  {
    status = BSP_ERROR_NONE;
  }
  return status;
}

/**
  * @brief  Send and receive data to/from SPI BUS (Full duplex).
  * @param  pTxData Pointer to data buffer to send.
  * @param  pRxData Pointer to data buffer to receive.
  * @param  Length Length of data in byte.
  * @retval BSP status.
  */
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length)
{
  int32_t status = BSP_ERROR_UNKNOWN_FAILURE;

  if (HAL_SPI_TransmitReceive(&hbus_spi1, pTxData,pRxData, Length, BUS_SPI1_TIMEOUT) == HAL_OK)
  {
    status = BSP_ERROR_NONE;
  }
  return status;
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default SPI1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterDefaultMspCallbacks(void)
{
  int32_t status = BSP_ERROR_NONE;

  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi1);

  /* Register default MspInit/MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbus_spi1, HAL_SPI_MSPINIT_CB_ID, SPI1_MspInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_SPI_RegisterCallback(&hbus_spi1, HAL_SPI_MSPDEINIT_CB_ID, SPI1_MspDeInit) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    Bus_IsSpi1MspCbValid = 1U;
  }

  /* BSP status */
  return status;
}

/**
  * @brief Register SPI1 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterMspCallbacks(BSP_SPI_Cb_t *Callbacks)
{
  int32_t status = BSP_ERROR_NONE;

  __HAL_SPI_RESET_HANDLE_STATE(&hbus_spi1);

  /* Register MspInit/MspDeInit Callbacks */
  if (HAL_SPI_RegisterCallback(&hbus_spi1, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_SPI_RegisterCallback(&hbus_spi1, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    Bus_IsSpi1MspCbValid = 1;
  }

  /* BSP status */
  return status;
}
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  MX SPI1 initialization.
  * @param  hSpi SPI handle.
  * @param  baudrate_presc Baudrate prescaler.
  * @retval HAL status.
  */
__weak HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef* hSpi, uint32_t baudrate_presc)
{
  hSpi->Init.Mode              = SPI_MODE_MASTER;
  hSpi->Init.Direction         = SPI_DIRECTION_2LINES;
  hSpi->Init.DataSize          = SPI_DATASIZE_8BIT;
  hSpi->Init.CLKPolarity       = SPI_POLARITY_LOW;
  hSpi->Init.CLKPhase          = SPI_PHASE_1EDGE;
  hSpi->Init.NSS               = SPI_NSS_SOFT;
  hSpi->Init.BaudRatePrescaler = baudrate_presc;
  hSpi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hSpi->Init.TIMode            = SPI_TIMODE_DISABLE;
  hSpi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hSpi->Init.CRCPolynomial     = 7;

  return HAL_SPI_Init(hSpi);
}

/**
  * @brief  Provide a tick value in millisecond.
  * @retval Tick value.
  */
int32_t BSP_GetTick(void)
{
  uint32_t ret;
  ret = HAL_GetTick();
  return (int32_t)ret;
}
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Private_Functions STM32L475E_IOT01 BUS Private Functions
  * @{
  */

/**
  * @brief  Initialize I2C2 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  BUS_I2C2_SDA_GPIO_CLK_ENABLE();
  BUS_I2C2_SCL_GPIO_CLK_ENABLE();

  /* Configure I2C SCL as alternate function */
  gpio_init_structure.Pin       = BUS_I2C2_SCL_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = BUS_I2C2_SCL_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SCL_GPIO_PORT, &gpio_init_structure);

  /* Configure I2C SDA as alternate function */
  gpio_init_structure.Pin       = BUS_I2C2_SDA_GPIO_PIN;
  gpio_init_structure.Alternate = BUS_I2C2_SDA_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  BUS_I2C2_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  BUS_I2C2_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  BUS_I2C2_RELEASE_RESET();
}

/**
  * @brief  DeInitialize I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /* De-initialiaze I2C SCL and SDA */
  HAL_GPIO_DeInit(BUS_I2C2_SCL_GPIO_PORT, BUS_I2C2_SCL_GPIO_PIN);
  HAL_GPIO_DeInit(BUS_I2C2_SDA_GPIO_PORT, BUS_I2C2_SDA_GPIO_PIN);

  /* Disable I2C clock */
  BUS_I2C2_CLK_DISABLE();
}

/**
  * @brief  Write values in registers of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register start address to write.
  * @param  MemAddSize Size of internal memory address.
  * @param  pData      The target register values to be written.
  * @param  Length     Number of data.
  * @retval BSP status.
  */
static int32_t I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  int32_t  status = BSP_ERROR_NONE;
  uint32_t hal_error;

  if (HAL_I2C_Mem_Write(&hbus_i2c2, DevAddr, Reg, MemAddSize, pData, Length, BUS_I2C2_TIMEOUT) != HAL_OK)
  {
    hal_error = HAL_I2C_GetError(&hbus_i2c2);
    if ((hal_error & HAL_I2C_ERROR_BERR) != 0U)
    {
      status = BSP_ERROR_BUS_PROTOCOL_FAILURE;
    }
    else if ((hal_error & HAL_I2C_ERROR_ARLO) != 0U)
    {
      status = BSP_ERROR_BUS_ARBITRATION_LOSS;
    }
    else if ((hal_error & HAL_I2C_ERROR_AF) != 0U)
    {
      status = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else if (((hal_error & HAL_I2C_ERROR_TIMEOUT) != 0U) || ((hal_error & HAL_I2C_ERROR_SIZE) != 0U))
    {
      status = BSP_ERROR_BUS_TRANSACTION_FAILURE;
    }
    else
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }    
  }

  return status;
}

/**
  * @brief  Read values in registers of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register start address to read.
  * @param  MemAddSize Size of internal memory address.
  * @param  pData      The target register values to be read.
  * @param  Length     Number of data.
  * @retval BSP status.
  */
static int32_t I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  int32_t  status = BSP_ERROR_NONE;
  uint32_t hal_error;

  if (HAL_I2C_Mem_Read(&hbus_i2c2, DevAddr, Reg, MemAddSize, pData, Length, BUS_I2C2_TIMEOUT) != HAL_OK)
  {
    hal_error = HAL_I2C_GetError(&hbus_i2c2);
    if ((hal_error & HAL_I2C_ERROR_BERR) != 0U)
    {
      status = BSP_ERROR_BUS_PROTOCOL_FAILURE;
    }
    else if ((hal_error & HAL_I2C_ERROR_ARLO) != 0U)
    {
      status = BSP_ERROR_BUS_ARBITRATION_LOSS;
    }
    else if ((hal_error & HAL_I2C_ERROR_AF) != 0U)
    {
      status = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else if (((hal_error & HAL_I2C_ERROR_TIMEOUT) != 0U) || ((hal_error & HAL_I2C_ERROR_SIZE) != 0U))
    {
      status = BSP_ERROR_BUS_TRANSACTION_FAILURE;
    }
    else
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }    
  }

  return status;
}

/**
  * @brief  Compute I2C timing according current I2C clock source and required I2C clock.
  * @param  clock_src_hz I2C clock source in Hz.
  * @param  i2cfreq_hz Required I2C clock in Hz.
  * @retval I2C timing or 0 in case of error.
  */
static uint32_t I2C_GetTiming(uint32_t clock_src_hz, uint32_t i2cfreq_hz)
{
  uint32_t ret = 0;
  uint32_t speed;
  uint32_t is_valid_speed = 0;
  uint32_t p_prev = PRESC_MAX;
  uint32_t i2cclk;
  uint32_t i2cspeed;
  uint32_t clk_error_prev;
  uint32_t tsync;
  uint32_t af_delay_min, af_delay_max;
  uint32_t dnf_delay;
  uint32_t clk_min, clk_max;
  int32_t  sdadel_min, sdadel_max;
  int32_t  scldel_min;
  struct   i2c_timings_s *s;
  struct   i2c_timings_s valid_timing[VALID_PRESC_NBR];
  uint16_t p, l, a, h;
  uint32_t valid_timing_nbr = 0;

  const struct i2c_specs_s i2c_specs[3] =
  { 
    [I2C_SPEED_STANDARD] =
    {
      .rate = 100000,
      .rate_min = 80000,
      .rate_max = 120000,
      .fall_max = 300,
      .rise_max = 1000,
      .hddat_min = 0,
      .vddat_max = 3450,
      .sudat_min = 250,
      .l_min = 4700,
      .h_min = 4000,
    },
    [I2C_SPEED_FAST] =
    {
      .rate = 400000,
      .rate_min = 320000,
      .rate_max = 400000,
      .fall_max = 300,
      .rise_max = 300,
      .hddat_min = 0,
      .vddat_max = 900,
      .sudat_min = 100,
      .l_min = 1300,
      .h_min = 600,
    },
    [I2C_SPEED_FAST_PLUS] =
    {
      .rate = 1000000,
      .rate_min = 800000,
      .rate_max = 1000000,
      .fall_max = 120,
      .rise_max = 120,
      .hddat_min = 0,
      .vddat_max = 450,
      .sudat_min = 50,
      .l_min = 500,
      .h_min = 260,
    }
  };
  const struct i2c_setup_s i2c_user_setup[3] =
  {
    [I2C_SPEED_STANDARD] =
    {
      .rise_time = 640,
      .fall_time = 20,
      .dnf = 0,
      .analog_filter = 1
    },
    [I2C_SPEED_FAST] =
    {
      .rise_time = 250,
      .fall_time = 100,
      .dnf = 0,
      .analog_filter = 1
    },
    [I2C_SPEED_FAST_PLUS] =
    {
      .rise_time = 60,
      .fall_time = 100,
      .dnf = 0,
      .analog_filter = 1
    }
  };

  for (speed = 0; speed <= (uint32_t) I2C_SPEED_FAST_PLUS; speed++)
  {
    if ((i2cfreq_hz >= i2c_specs[speed].rate_min) && (i2cfreq_hz <= i2c_specs[speed].rate_max))
    {
      is_valid_speed = 1;
      break;
    }
  }

  if ((is_valid_speed != 0U) && (speed  <= (uint32_t) I2C_SPEED_FAST_PLUS))
  {
    i2cclk = DIV_ROUND_CLOSEST(NSEC_PER_SEC, clock_src_hz);
    i2cspeed = DIV_ROUND_CLOSEST(NSEC_PER_SEC, i2cfreq_hz);
    clk_error_prev = i2cspeed;

    /* Analog and Digital Filters */
    af_delay_min = (i2c_user_setup[speed].analog_filter == 1U) ? I2C_ANALOG_FILTER_DELAY_MIN : 0U;
    af_delay_max = (i2c_user_setup[speed].analog_filter == 1U) ? I2C_ANALOG_FILTER_DELAY_MAX : 0U;
    dnf_delay    = i2c_user_setup[speed].dnf * i2cclk;
    sdadel_min   = (int32_t) i2c_user_setup[speed].fall_time - (int32_t) i2c_specs[speed].hddat_min - (int32_t) af_delay_min - (((int32_t) i2c_user_setup[speed].dnf + 3) * (int32_t) i2cclk);
    sdadel_max   = (int32_t) i2c_specs[speed].vddat_max - (int32_t) i2c_user_setup[speed].rise_time - (int32_t) af_delay_max - (((int32_t) i2c_user_setup[speed].dnf + 4) * (int32_t) i2cclk);
    scldel_min   = (int32_t) i2c_user_setup[speed].rise_time + (int32_t) i2c_specs[speed].sudat_min;
    if (sdadel_min < 0)
    {
      sdadel_min = 0;
    }
    if (sdadel_max < 0)
    {
      sdadel_max = 0;
    }

    /* Compute possible values for PRESC, SCLDEL and SDADEL */
    for (p = 0; p < PRESC_MAX; p++)
    {
      for (l = 0; l < SCLDEL_MAX; l++)
      {
        int32_t scldel = ((int32_t) l + 1) * ((int32_t) p + 1) * (int32_t) i2cclk;
        if (scldel < scldel_min)
        {
          continue;
        }
        for (a = 0; a < SDADEL_MAX; a++)
        {
          int32_t sdadel = (((int32_t) a * ((int32_t) p + 1)) + 1) * (int32_t) i2cclk;
          if (((sdadel >= sdadel_min) && (sdadel <= sdadel_max))&& (p != p_prev))
          {
            valid_timing[valid_timing_nbr].presc = (uint8_t) p;
            valid_timing[valid_timing_nbr].scldel = (uint8_t) l;
            valid_timing[valid_timing_nbr].sdadel = (uint8_t) a;
            p_prev = p;
            valid_timing_nbr ++;
            if(valid_timing_nbr >= VALID_PRESC_NBR)
            {
              /* max valid timing buffer is full, use only these values*/
              goto Compute_scll_sclh;
            }
          }
        }
      }
    }

    if (valid_timing_nbr == 0U)
    {
      return 0;
    }

Compute_scll_sclh:
    tsync = af_delay_min + dnf_delay + (2U * i2cclk);
    s = NULL;
    clk_max = NSEC_PER_SEC / i2c_specs[speed].rate_min;
    clk_min = NSEC_PER_SEC / i2c_specs[speed].rate_max;

    /*
    * Among Prescaler possibilities discovered above figures out SCL Low
    * and High Period. Provided:
    * - SCL Low Period has to be higher than Low Period of tehs SCL Clock
    *   defined by I2C Specification. I2C Clock has to be lower than
    *   (SCL Low Period - Analog/Digital filters) / 4.
    * - SCL High Period has to be lower than High Period of the SCL Clock
    *   defined by I2C Specification
    * - I2C Clock has to be lower than SCL High Period
    */
    for (uint32_t count = 0; count < valid_timing_nbr; count++)
    {
      uint32_t prescaler = ((uint32_t) valid_timing[count].presc + 1U) * i2cclk;
      for (l = 0; l < SCLL_MAX; l++)
      {
        uint32_t tscl_l = (((uint32_t) l + 1U) * prescaler) + tsync;
        if ((tscl_l < i2c_specs[speed].l_min) || (i2cclk >= ((tscl_l - af_delay_min - dnf_delay) / 4U)))
        {
          continue;
        }
        for (h = 0; h < SCLH_MAX; h++)
        {
          uint32_t tscl_h = (((uint32_t) h + 1U) * prescaler) + tsync;
          uint32_t tscl = tscl_l + tscl_h + i2c_user_setup[speed].rise_time + i2c_user_setup[speed].fall_time;
          if ((tscl >= clk_min) && (tscl <= clk_max) && (tscl_h >= i2c_specs[speed].h_min) && (i2cclk < tscl_h))
          {
            int32_t clk_error = (int32_t) tscl - (int32_t) i2cspeed;
            if (clk_error < 0)
            {
              clk_error = -clk_error;
            }
            /* save the solution with the lowest clock error */
            if (clk_error < (int32_t) clk_error_prev)
            {
              clk_error_prev = (uint32_t) clk_error;
              valid_timing[count].scll = (uint8_t) l;
              valid_timing[count].sclh = (uint8_t) h;
              s = &valid_timing[count];
            }
          }
        }
      }
    }
    if (s == NULL)
    {
      return 0;
    }
    ret = (((uint32_t) s->presc & 0xFU) << 28) | (((uint32_t) s->scldel & 0xFU) << 20) | (((uint32_t) s->sdadel & 0xFU) << 16) | (((uint32_t) s->sclh & 0xFFU) << 8) | (((uint32_t) s->scll & 0xFFU) << 0);
  }
  
  return ret;
}

/**
  * @brief  Initialize SPI MSP.
  * @param  hSpi  SPI handle.
  * @retval None.
  */
static void SPI1_MspInit(SPI_HandleTypeDef* hSpi)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hSpi);

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  BUS_SPI1_SCK_GPIO_CLK_ENABLE();
  BUS_SPI1_MOSI_GPIO_CLK_ENABLE();
  BUS_SPI1_MISO_GPIO_CLK_ENABLE();

  /* Configure SPI CLK as alternate function */
  gpio_init_structure.Pin       = BUS_SPI1_SCK_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = BUS_SPI1_SCK_GPIO_AF;
  HAL_GPIO_Init(BUS_SPI1_SCK_GPIO_PORT, &gpio_init_structure);

  /* Configure SPI MOSI as alternate function */
  gpio_init_structure.Pin       = BUS_SPI1_MOSI_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = BUS_SPI1_MOSI_GPIO_AF;
  HAL_GPIO_Init(BUS_SPI1_MOSI_GPIO_PORT, &gpio_init_structure);

  /* Configure SPI MISO as alternate function */
  gpio_init_structure.Pin       = BUS_SPI1_MISO_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = BUS_SPI1_MISO_GPIO_AF;
  HAL_GPIO_Init(BUS_SPI1_MISO_GPIO_PORT, &gpio_init_structure);

  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  BUS_SPI1_CLK_ENABLE();

  /* Force the SPI peripheral clock reset */
  BUS_SPI1_FORCE_RESET();

  /* Release the SPI peripheral clock reset */
  BUS_SPI1_RELEASE_RESET();
}

/**
  * @brief  DeInitialize SPI MSP.
  * @param  hSpi  SPI handler
  * @retval None
  */
static void SPI1_MspDeInit(SPI_HandleTypeDef* hSpi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hSpi);

  /* De-initialiaze SPI CLK, MOSI and MISO */
  HAL_GPIO_DeInit(BUS_SPI1_SCK_GPIO_PORT, BUS_SPI1_SCK_GPIO_PIN);
  HAL_GPIO_DeInit(BUS_SPI1_MOSI_GPIO_PORT, BUS_SPI1_MOSI_GPIO_PIN);
  HAL_GPIO_DeInit(BUS_SPI1_MISO_GPIO_PORT, BUS_SPI1_MISO_GPIO_PIN);

  /* Disable SPI clock */
  BUS_SPI1_CLK_DISABLE();
}

/**
  * @brief  Convert the SPI baudrate into prescaler.
  * @param  clock_src_hz : SPI source clock in HZ.
  * @param  baudrate_mbps : SPI baud rate in mbps.
  * @retval Prescaler dividor
  */
static uint32_t SPI_GetPrescaler( uint32_t clock_src_hz, uint32_t baudrate_mbps )
{
  uint32_t divisor = 0;
  uint32_t spi_clk = clock_src_hz;
  uint32_t presc = 0;

  static const uint32_t baudrate[]=
  {
    SPI_BAUDRATEPRESCALER_2,
    SPI_BAUDRATEPRESCALER_4,
    SPI_BAUDRATEPRESCALER_8,
    SPI_BAUDRATEPRESCALER_16,
    SPI_BAUDRATEPRESCALER_32,
    SPI_BAUDRATEPRESCALER_64,
    SPI_BAUDRATEPRESCALER_128,
    SPI_BAUDRATEPRESCALER_256,
  };

  while( spi_clk > baudrate_mbps)
  {
    presc = baudrate[divisor];
    if (++divisor > 7U)
      break;

    spi_clk= ( spi_clk >> 1);
  }

  return presc;
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
