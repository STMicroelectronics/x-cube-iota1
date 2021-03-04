/**
  ******************************************************************************
  * @file    stm32l475e_iot01_qspi.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l475e_iot01_qspi.c driver.
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
#ifndef STM32L475E_IOT01_QSPI_H
#define STM32L475E_IOT01_QSPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l475e_iot01_conf.h"
#include "stm32l475e_iot01_errno.h"
#include "../Components/mx25r6435f/mx25r6435f.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @addtogroup STM32L475E_IOT01_QSPI
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup STM32L475E_IOT01_QSPI_Exported_Types STM32L475E_IOT01 QSPI Exported Types
  * @{
  */
typedef enum {
  QSPI_ACCESS_NONE = 0,          /*!<  Instance not initialized,              */
  QSPI_ACCESS_INDIRECT,          /*!<  Instance use indirect mode access      */
  QSPI_ACCESS_MMP                /*!<  Instance use Memory Mapped Mode read   */
} QSPI_Access_t;

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pQSPI_CallbackTypeDef  pMspInitCb;
  pQSPI_CallbackTypeDef  pMspDeInitCb;
}BSP_QSPI_Cb_t;
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 1) */

typedef struct
{
  uint32_t MemorySize;
  uint32_t ClockPrescaler;
  uint32_t SampleShifting;
} MX_QSPI_InitTypeDef;
/**
  * @}
  */

/** @defgroup STM32L475E_IOT01_QSPI_Exported_Types STM32L475E_IOT01 QSPI Exported Types
  * @{
  */
#define BSP_QSPI_Info_t                MX25R6435F_Info_t
#define BSP_QSPI_Interface_t           MX25R6435F_Interface_t
#define BSP_QSPI_Transfer_t            MX25R6435F_Transfer_t
#define BSP_QSPI_Erase_t               MX25R6435F_Erase_t

typedef struct
{
  QSPI_Access_t              IsInitialized;  /*!<  Instance access Flash method     */
  BSP_QSPI_Interface_t       InterfaceMode;  /*!<  Flash Interface mode of Instance */
} QSPI_Ctx_t;

typedef struct
{
  BSP_QSPI_Interface_t       InterfaceMode;      /*!<  Current Flash Interface mode */
} BSP_QSPI_Init_t;
  /**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup STM32L475E_IOT01_QSPI_Exported_Constants STM32L475E_IOT01 QSPI Exported Constants
  * @{
  */


/* Definition for QSPI clock resources */
#define QSPI_CLK_ENABLE()                 __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()                __HAL_RCC_QSPI_CLK_DISABLE()

#define QSPI_CLK_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_D0_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_D1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_D2_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_D3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()

#define QSPI_FORCE_RESET()                __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()              __HAL_RCC_QSPI_RELEASE_RESET()

/* Definition for QSPI Pins */
/* QSPI_CLK */
#define QSPI_CLK_PIN                      GPIO_PIN_10
#define QSPI_CLK_GPIO_PORT                GPIOE
#define QSPI_CLK_PIN_AF                   GPIO_AF10_QUADSPI
/* QSPI_CS */
#define QSPI_CS_PIN                       GPIO_PIN_11
#define QSPI_CS_GPIO_PORT                 GPIOE
#define QSPI_CS_PIN_AF                    GPIO_AF10_QUADSPI
/* QSPI_D0 */
#define QSPI_D0_PIN                       GPIO_PIN_12
#define QSPI_D0_GPIO_PORT                 GPIOE
#define QSPI_D0_PIN_AF                    GPIO_AF10_QUADSPI
/* QSPI_D1 */
#define QSPI_D1_PIN                       GPIO_PIN_13
#define QSPI_D1_GPIO_PORT                 GPIOE
#define QSPI_D1_PIN_AF                    GPIO_AF10_QUADSPI
/* QSPI_D2 */
#define QSPI_D2_PIN                       GPIO_PIN_14
#define QSPI_D2_GPIO_PORT                 GPIOE
#define QSPI_D2_PIN_AF                    GPIO_AF10_QUADSPI
/* QSPI_D3 */
#define QSPI_D3_PIN                       GPIO_PIN_15
#define QSPI_D3_GPIO_PORT                 GPIOE
#define QSPI_D3_PIN_AF                    GPIO_AF10_QUADSPI

#define QSPI_INSTANCES_NUMBER         1U

/* Definition for QSPI modes */
#define BSP_QSPI_SPI_MODE             (BSP_QSPI_Interface_t)MX25R6435F_SPI_MODE       /* 1 Cmd Line, 1 Address Line and 1 Data Line   */
#define BSP_QSPI_DUAL_OUT_MODE        (BSP_QSPI_Interface_t)MX25R6435F_DUAL_OUT_MODE  /* 1 Cmd Line, 1 Address Line and 2 Data Lines  */
#define BSP_QSPI_DUAL_IO_MODE         (BSP_QSPI_Interface_t)MX25R6435F_DUAL_IO_MODE   /* 1 Cmd Line, 2 Address Lines and 2 Data Lines */
#define BSP_QSPI_QUAD_OUT_MODE        (BSP_QSPI_Interface_t)MX25R6435F_QUAD_OUT_MODE  /* 1 Cmd Line, 1 Address Line and 4 Data Lines  */
#define BSP_QSPI_QUAD_IO_MODE         (BSP_QSPI_Interface_t)MX25R6435F_QUAD_IO_MODE   /* 1 Cmd Line, 4 Address Lines and 4 Data Lines */

/* QSPI erase types */
#define BSP_QSPI_ERASE_4K             MX25R6435F_ERASE_4K
#define BSP_QSPI_ERASE_32K            MX25R6435F_ERASE_32K
#define BSP_QSPI_ERASE_64K            MX25R6435F_ERASE_64K
#define BSP_QSPI_ERASE_CHIP           MX25R6435F_ERASE_BULK

/* QSPI block sizes */
#define BSP_QSPI_BLOCK_4K             MX25R6435F_SECTOR_4K
#define BSP_QSPI_BLOCK_32K            MX25R6435F_BLOCK_32K
#define BSP_QSPI_BLOCK_64K            MX25R6435F_BLOCK_64K
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @addtogroup STM32L475E_IOT01_QSPI_Exported_Variables STM32L475E_IOT01 QSPI Exported Variables
  * @{
  */
extern QSPI_HandleTypeDef hqspi[QSPI_INSTANCES_NUMBER];
extern QSPI_Ctx_t Qspi_Ctx[QSPI_INSTANCES_NUMBER];
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L475E_IOT01_QSPI_Exported_Functions STM32L475E_IOT01 QSPI Exported Functions
  * @{
  */
int32_t BSP_QSPI_Init                        (uint32_t Instance, BSP_QSPI_Init_t *Init);
int32_t BSP_QSPI_DeInit                      (uint32_t Instance);
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
int32_t BSP_QSPI_RegisterMspCallbacks        (uint32_t Instance, BSP_QSPI_Cb_t *CallBacks);
int32_t BSP_QSPI_RegisterDefaultMspCallbacks (uint32_t Instance);
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 1) */
int32_t BSP_QSPI_Read                        (uint32_t Instance, uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_QSPI_Write                       (uint32_t Instance, uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_QSPI_Erase_Block                 (uint32_t Instance, uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize);
int32_t BSP_QSPI_Erase_Chip                  (uint32_t Instance);
int32_t BSP_QSPI_GetStatus                   (uint32_t Instance);
int32_t BSP_QSPI_GetInfo                     (uint32_t Instance, BSP_QSPI_Info_t* pInfo);
int32_t BSP_QSPI_EnableMemoryMappedMode      (uint32_t Instance);
int32_t BSP_QSPI_DisableMemoryMappedMode     (uint32_t Instance);
int32_t BSP_QSPI_ReadID                      (uint32_t Instance, uint8_t *Id);
int32_t BSP_QSPI_ConfigFlash                 (uint32_t Instance, BSP_QSPI_Interface_t Mode);
int32_t BSP_QSPI_SuspendErase                (uint32_t Instance);
int32_t BSP_QSPI_ResumeErase                 (uint32_t Instance);
int32_t BSP_QSPI_EnterDeepPowerDown          (uint32_t Instance);
int32_t BSP_QSPI_LeaveDeepPowerDown          (uint32_t Instance);

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_QSPI_Init(QSPI_HandleTypeDef *hQspi, MX_QSPI_InitTypeDef *Init);

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

#endif /* STM32L475E_IOT01_QSPI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
