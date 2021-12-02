/**
  ******************************************************************************
  * @file    stm32l475e_iot01_bus.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for B-L475E-IOT01 bus.
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
#ifndef STM32L475E_IOT01_BUS_H
#define STM32L475E_IOT01_BUS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_conf.h"
#include "stm32l475e_iot01_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @addtogroup STM32L475E_IOT01_BUS
  * @{
  */
   
/** @defgroup STM32L475E_IOT01_BUS_Exported_Types STM32L475E_IOT01 BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
} BSP_SPI_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_BUS_Exported_Constants STM32L475E_IOT01 BUS Exported Constants
  * @{
  */
#define BUS_I2C2                        I2C2
#define BUS_I2C2_CLK_ENABLE()           __HAL_RCC_I2C2_CLK_ENABLE()
#define BUS_I2C2_CLK_DISABLE()          __HAL_RCC_I2C2_CLK_DISABLE()
#define BUS_I2C2_FORCE_RESET()          __HAL_RCC_I2C2_FORCE_RESET()
#define BUS_I2C2_RELEASE_RESET()        __HAL_RCC_I2C2_RELEASE_RESET()
#define BUS_I2C2_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C2_SCL_GPIO_PIN           GPIO_PIN_10
#define BUS_I2C2_SCL_GPIO_PORT          GPIOB
#define BUS_I2C2_SCL_GPIO_AF            GPIO_AF4_I2C2
#define BUS_I2C2_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C2_SDA_GPIO_PIN           GPIO_PIN_11
#define BUS_I2C2_SDA_GPIO_PORT          GPIOB
#define BUS_I2C2_SDA_GPIO_AF            GPIO_AF4_I2C2
#define BUS_I2C2_TIMEOUT                10000U

#define BUS_SPI1                        SPI1
#define BUS_SPI1_CLK_ENABLE()           __HAL_RCC_SPI1_CLK_ENABLE()
#define BUS_SPI1_CLK_DISABLE()          __HAL_RCC_SPI1_CLK_DISABLE()
#define BUS_SPI1_FORCE_RESET()          __HAL_RCC_SPI1_FORCE_RESET()
#define BUS_SPI1_RELEASE_RESET()        __HAL_RCC_SPI1_RELEASE_RESET()
#define BUS_SPI1_SCK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_SPI1_SCK_GPIO_PIN           GPIO_PIN_5
#define BUS_SPI1_SCK_GPIO_PORT          GPIOA
#define BUS_SPI1_SCK_GPIO_AF            GPIO_AF5_SPI1
#define BUS_SPI1_MOSI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_SPI1_MOSI_GPIO_PIN          GPIO_PIN_7
#define BUS_SPI1_MOSI_GPIO_PORT         GPIOA
#define BUS_SPI1_MOSI_GPIO_AF           GPIO_AF5_SPI1
#define BUS_SPI1_MISO_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_SPI1_MISO_GPIO_PIN          GPIO_PIN_6
#define BUS_SPI1_MISO_GPIO_PORT         GPIOA
#define BUS_SPI1_MISO_GPIO_AF           GPIO_AF5_SPI1
#define BUS_SPI1_TIMEOUT                10000U
/**
  * @}
  */

/** @addtogroup STM32L475E_IOT01_BUS_Exported_Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c2;
extern SPI_HandleTypeDef hbus_spi1;
/**
  * @}
  */

/** @addtogroup STM32L475E_IOT01_BUS_Exported_Functions
  * @{
  */
int32_t BSP_I2C2_Init(void);
int32_t BSP_I2C2_DeInit(void);
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials);
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C2_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C2_RegisterMspCallbacks(BSP_I2C_Cb_t *Callbacks);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */
HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

int32_t BSP_SPI1_Init(void);
int32_t BSP_SPI1_DeInit(void);
int32_t BSP_SPI1_Send(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI1_Recv(uint8_t *pData, uint16_t Length);
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI1_RegisterDefaultMspCallbacks(void);
int32_t BSP_SPI1_RegisterMspCallbacks(BSP_SPI_Cb_t *Callbacks);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef* hSpi, uint32_t baudrate_presc);

int32_t BSP_GetTick(void);
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

#endif /* STM32L475E_IOT01_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
