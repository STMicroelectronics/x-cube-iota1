/**
  ******************************************************************************
  * @file    stm32l475e_iot01_qspi.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the MX25R6435F QSPI
  *          memory mounted on the B-L475E-IOT01 board.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) This driver is used to drive the MX25R6435F Quad Flash external memory mounted 
       on STM32L475E-IOT01 discovery board.

   (#) This driver need specific component driver (MX25R6435F) to be included with.

   (#) MX25R6435F Initialization steps:
       (++) Initialize the QPSI external memory using the BSP_QSPI_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            QSPI interface with the external memory.

   (#) MX25R6435F Quad Flash memory operations
       (++) QSPI memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_QSPI_Read()/BSP_QSPI_Write().
       (++) The function BSP_QSPI_GetInfo() returns the configuration of the QSPI memory.
            (see the QSPI memory data sheet)
       (++) Perform erase block operation using the function BSP_QSPI_Erase_Block() and by
            specifying the block address. You can perform an erase operation of the whole
            chip by calling the function BSP_QSPI_Erase_Chip().
       (++) The function BSP_QSPI_GetStatus() returns the current status of the QSPI memory.
            (see the QSPI memory data sheet)
       (++) The memory access can be configured in memory-mapped mode with the call of
            function BSP_QSPI_EnableMemoryMapped(). To go back in indirect mode, the
            function BSP_QSPI_DisableMemoryMapped() should be used.
       (++) The erase operation can be suspend and resume with using functions
            BSP_QSPI_SuspendErase() and BSP_QSPI_ResumeErase()
       (++) It is possible to put the memory in deep power-down mode to reduce its consumption.
            For this, the function BSP_QSPI_EnterDeepPowerDown() should be called. To leave
            the deep power-down mode, the function BSP_QSPI_LeaveDeepPowerDown() should be called.
       (++) The function BSP_QSPI_ReadID() returns the identifier of the memory
            (see the QSPI memory data sheet)
       (++) The configuration of the interface between peripheral and memory is done by
            the function BSP_QSPI_ConfigFlash(), three modes are possible :
            - SPI : instruction, address and data on one line
            - STR QPI : instruction, address and data on four lines with sampling on one edge of clock
            - DTR QPI : instruction, address and data on four lines with sampling on both edges of clock

  @endverbatim
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
#include "stm32l475e_iot01_qspi.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L475E_IOT01
  * @{
  */

/** @defgroup STM32L475E_IOT01_QSPI STM32L475E_IOT01 QSPI
  * @{
  */

/* Exported variables --------------------------------------------------------*/
/** @addtogroup STM32L475E_IOT01_QSPI_Exported_Variables STM32L475E_IOT01 QSPI Exported Variables
  * @{
  */
QSPI_HandleTypeDef hqspi[QSPI_INSTANCES_NUMBER] = {0};
QSPI_Ctx_t Qspi_Ctx[QSPI_INSTANCES_NUMBER] = {{QSPI_ACCESS_NONE, MX25R6435F_SPI_MODE}};
/**
  * @}
  */

/* Private constants --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup STM32L475E_IOT01_QSPI_Private_Variables STM32L475E_IOT01 QSPI Private Variables
  * @{
  */
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
static uint32_t Qspi_IsMspCbValid[QSPI_INSTANCES_NUMBER] = {0};
#endif /* USE_HAL_QSPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32L475E_IOT01_QSPI_Private_Functions STM32L475E_IOT01 QSPI Private Functions
  * @{
  */
static void    QSPI_MspInit      (QSPI_HandleTypeDef *hQspi);
static void    QSPI_MspDeInit    (QSPI_HandleTypeDef *hQspi);
static int32_t QSPI_ResetMemory  (uint32_t Instance);
static int32_t QSPI_EnterQuadMode(uint32_t Instance);
static int32_t QSPI_ExitQuadMode (uint32_t Instance);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup STM32L475E_IOT01_QSPI_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @param  Init       QSPI Init structure
  * @retval BSP status
  */
int32_t BSP_QSPI_Init(uint32_t Instance, BSP_QSPI_Init_t *Init)
{
  int32_t ret;
  BSP_QSPI_Info_t pInfo;
  MX_QSPI_InitTypeDef qspi_init;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Qspi_Ctx[Instance].IsInitialized == QSPI_ACCESS_NONE)
    {
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 0)
      /* Msp QSPI initialization */
      QSPI_MspInit(&(hqspi[Instance]));
#else
      /* Register the QSPI MSP Callbacks */
      if(Qspi_IsMspCbValid[Instance] == 0UL)
      {
        if(BSP_QSPI_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#endif /* USE_HAL_QSPI_REGISTER_CALLBACKS */

      /* Get Flash informations of one memory */
      (void)MX25R6435F_GetFlashInfo(&pInfo);

      /* Fill config structure */
      qspi_init.ClockPrescaler = 3; /* QuadSPI clock = 80MHz / ClockPrescaler = 20MHz */
      qspi_init.MemorySize     = (uint32_t)POSITION_VAL((uint32_t)pInfo.FlashSize);
      qspi_init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;

      /* STM32 QSPI interface initialization */
      if (MX_QSPI_Init(&(hqspi[Instance]), &qspi_init) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      /* QSPI memory reset */
      else if (QSPI_ResetMemory(Instance) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Check if memory is ready */
      else if (MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Configure the memory */
      else if (BSP_QSPI_ConfigFlash(Instance, Init->InterfaceMode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  De-Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Qspi_Ctx[Instance].IsInitialized != QSPI_ACCESS_NONE)
    {
      /* Disable Memory mapped mode */
      if(Qspi_Ctx[Instance].IsInitialized == QSPI_ACCESS_MMP)
      {
        if(BSP_QSPI_DisableMemoryMappedMode(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_COMPONENT_FAILURE;
        }
      }

      /* Set default Qspi_Ctx values */
      Qspi_Ctx[Instance].IsInitialized = QSPI_ACCESS_NONE;
      Qspi_Ctx[Instance].InterfaceMode = BSP_QSPI_SPI_MODE;

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 0)
      QSPI_MspDeInit(&(hqspi[Instance]));
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 0) */

      /* Call the DeInit function to reset the driver */
      if (HAL_QSPI_DeInit(&(hqspi[Instance])) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Initializes the QSPI interface.
  * @param  hQspi          QSPI handle
  * @param  Init           QSPI config structure
  * @retval BSP status
  */
__weak HAL_StatusTypeDef MX_QSPI_Init(QSPI_HandleTypeDef *hQspi, MX_QSPI_InitTypeDef *Init)
{
    /* QuadSPI initialization */
  hQspi->Instance = QUADSPI;

  hQspi->Init.FifoThreshold      = 4;
  hQspi->Init.FlashSize          = Init->MemorySize; /* 64 MBits */
  hQspi->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_2_CYCLE;
  hQspi->Init.ClockMode          = QSPI_CLOCK_MODE_0;
  hQspi->Init.ClockPrescaler     = Init->ClockPrescaler;
  hQspi->Init.SampleShifting     = Init->SampleShifting;

  return HAL_QSPI_Init(hQspi);
}

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP QSPI Msp Callbacks
  * @param Instance      QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&(hqspi[Instance]), HAL_QSPI_MSP_INIT_CB_ID, QSPI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&(hqspi[Instance]), HAL_QSPI_MSP_DEINIT_CB_ID, QSPI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Qspi_IsMspCbValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP QSPI Msp Callback registering
  * @param Instance     QSPI Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterMspCallbacks (uint32_t Instance, BSP_QSPI_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&(hqspi[Instance]), HAL_QSPI_MSP_INIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&(hqspi[Instance]), HAL_QSPI_MSP_DEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Qspi_IsMspCbValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  Instance  QSPI instance
  * @param  pData     Pointer to data to be read
  * @param  ReadAddr  Read start address
  * @param  Size      Size of data to read
  * @retval BSP status
  */
int32_t BSP_QSPI_Read(uint32_t Instance, uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(MX25R6435F_Read(&(hqspi[Instance]), Qspi_Ctx[Instance].InterfaceMode, pData, ReadAddr, Size) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  Instance  QSPI instance
  * @param  pData     Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size      Size of data to write
  * @retval BSP status
  */
int32_t BSP_QSPI_Write(uint32_t Instance, uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t end_addr, current_size, current_addr;
  uint32_t data_addr;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Calculation of the size between the write address and the end of the page */
    current_size = MX25R6435F_PAGE_SIZE - (WriteAddr % MX25R6435F_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
      current_size = Size;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
    data_addr = (uint32_t)pData;

    /* Perform the write page by page */
    do
    {
      /* Check if Flash busy ? */
      if(MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Enable write operations */
      else if(MX25R6435F_WriteEnable(&(hqspi[Instance])) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Issue page program command */
        if(MX25R6435F_PageProgram(&(hqspi[Instance]), Qspi_Ctx[Instance].InterfaceMode, (uint8_t*)data_addr, current_addr, current_size) != MX25R6435F_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        
        if (ret == BSP_ERROR_NONE)
        {
          /* Configure automatic polling mode to wait for end of program */
          if (MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
          {
            ret = BSP_ERROR_COMPONENT_FAILURE;
          }
          else
          {
            /* Update the address and size variables for next page programming */
            current_addr += current_size;
            data_addr += current_size;
            current_size = ((current_addr + MX25R6435F_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25R6435F_PAGE_SIZE;
          }
        }
      }
    } while ((current_addr < end_addr) && (ret == BSP_ERROR_NONE));
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  Instance     QSPI instance
  * @param  BlockAddress Block address to erase
  * @param  BlockSize    Erase Block size
  * @retval BSP status
  */
int32_t BSP_QSPI_Erase_Block(uint32_t Instance, uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check Flash busy ? */
    if(MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if(MX25R6435F_WriteEnable(&(hqspi[Instance])) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Block Erase command */
    else if(MX25R6435F_BlockErase(&(hqspi[Instance]), BlockAddress, BlockSize) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_Erase_Chip(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check Flash busy ? */
    if(MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if(MX25R6435F_WriteEnable(&(hqspi[Instance])) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Chip erase command */
    else if(MX25R6435F_ChipErase(&(hqspi[Instance])) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads current status of the QSPI memory.
  * @param  Instance  QSPI instance
  * @retval QSPI memory status: whether busy or not
  */
int32_t BSP_QSPI_GetStatus(uint32_t Instance)
{
  static uint8_t reg[2];
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(MX25R6435F_ReadSecurityRegister(&(hqspi[Instance]), reg) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Check the value of the register */
    else if ((reg[0] & (MX25R6435F_SECR_P_FAIL | MX25R6435F_SECR_E_FAIL)) != 0U)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if ((reg[0] & (MX25R6435F_SECR_PSB | MX25R6435F_SECR_ESB)) != 0U)
    {
      ret = BSP_ERROR_QSPI_SUSPENDED;
    }
    else if(MX25R6435F_ReadStatusRegister(&(hqspi[Instance]), reg) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Check the value of the register */
    else if ((reg[0] & MX25R6435F_SR_WIP) != 0U)
    {
      ret = BSP_ERROR_BUSY;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  Instance  QSPI instance
  * @param  pInfo     pointer on the configuration structure
  * @retval BSP status
  */
int32_t BSP_QSPI_GetInfo(uint32_t Instance, BSP_QSPI_Info_t* pInfo)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    (void)MX25R6435F_GetFlashInfo(pInfo);
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_EnableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(MX25R6435F_EnableMemoryMappedMode(&(hqspi[Instance]), Qspi_Ctx[Instance].InterfaceMode) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else /* Update QSPI context if all operations are well done */
    {
      Qspi_Ctx[Instance].IsInitialized = QSPI_ACCESS_MMP;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Exit form memory-mapped mode
  *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_DisableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(Qspi_Ctx[Instance].IsInitialized != QSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE;
    }/* Abort MMP back to indirect mode */
    else if(HAL_QSPI_Abort(&(hqspi[Instance])) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else /* Update QSPI context if all operations are well done */
    {
      Qspi_Ctx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get flash ID 3 Bytes:
  *         Manufacturer ID, Memory type, Memory density
  * @param  Instance  QSPI instance
  * @param  Id Pointer to flash ID bytes
  * @retval BSP status
  */
int32_t BSP_QSPI_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MX25R6435F_ReadID(&(hqspi[Instance]), Id) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Flash to desired Interface mode. And this instance becomes current instance.
  *         If current instance running at MMP mode then this function doesn't work.
  *         Indirect -> Indirect
  * @param  Instance  QSPI instance
  * @param  Mode      QSPI mode
  * @retval BSP status
  */
int32_t BSP_QSPI_ConfigFlash(uint32_t Instance, BSP_QSPI_Interface_t Mode)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if MMP mode locked ************************************************/
    if(Qspi_Ctx[Instance].IsInitialized == QSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_QSPI_MMP_LOCK_FAILURE;
    }
    else
    {
      /* Setup Flash interface ***************************************************/
      switch(Qspi_Ctx[Instance].InterfaceMode)
      {
      case BSP_QSPI_QUAD_OUT_MODE :  /* 1-1-4 commands */
      case BSP_QSPI_QUAD_IO_MODE :   /* 1-4-4 commands */
        if((Mode != BSP_QSPI_QUAD_OUT_MODE) && (Mode != BSP_QSPI_QUAD_IO_MODE))
        {
          /* Exit Quad mode */
          ret = QSPI_ExitQuadMode(Instance);
        }
        break;
        
      case BSP_QSPI_SPI_MODE :       /* 1-1-1 commands, Power on H/W default setting */
      case BSP_QSPI_DUAL_OUT_MODE :  /* 1-1-2 commands */
      case BSP_QSPI_DUAL_IO_MODE :   /* 1-2-2 commands */
      default :
        if((Mode == BSP_QSPI_QUAD_OUT_MODE) || (Mode == BSP_QSPI_QUAD_IO_MODE))
        {
          /* Enter Quad mode */
          ret = QSPI_EnterQuadMode(Instance);
        }
        break;
      }
      
      /* Update QSPI context if all operations are well done */
      if(ret == BSP_ERROR_NONE)
      {
        /* Update current status parameter *****************************************/
        Qspi_Ctx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;
        Qspi_Ctx[Instance].InterfaceMode = Mode;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function suspends an ongoing erase command.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_SuspendErase(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check whether the device is busy (erase operation is in progress). */
  else if (BSP_QSPI_GetStatus(Instance) != BSP_ERROR_BUSY)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(MX25R6435F_Suspend(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else 
  {
    /* Latency when performing erase or program suspend : 60us */
    HAL_Delay(1);

    if (BSP_QSPI_GetStatus(Instance) != BSP_ERROR_QSPI_SUSPENDED)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function resumes a paused erase command.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_ResumeErase(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Check whether the device is busy (erase operation is in progress). */
  else if (BSP_QSPI_GetStatus(Instance) != BSP_ERROR_QSPI_SUSPENDED)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(MX25R6435F_Resume(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  /*
  When this command is executed, the status register write in progress bit is set to 1, and
  the flag status register program erase controller bit is set to 0. This command is ignored
  if the device is not in a suspended state.
  */
  else if (BSP_QSPI_GetStatus(Instance) != BSP_ERROR_BUSY)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function enter the QSPI memory in deep power down mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_EnterDeepPowerDown(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MX25R6435F_EnterPowerDown(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* ---          Memory takes 10us max to enter deep power down          --- */

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function leave the QSPI memory from deep power down mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_LeaveDeepPowerDown(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MX25R6435F_NoOperation(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* --- A NOP command is sent to the memory, as the nCS should be low for at least 20 ns --- */
  /* ---                  Memory takes 30us min to leave deep power down                  --- */

  /* Return BSP status */
  return ret;
}
/**
  * @}
  */

/** @addtogroup STM32L475I_IOT01_QSPI_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the QSPI MSP.
  * @param  hQspi QSPI handle
  * @retval None
  */
static void QSPI_MspInit(QSPI_HandleTypeDef *hQspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* hqspi unused argument(s) compilation warning */
  UNUSED(hQspi);

  /* Enable the QuadSPI memory interface clock */
  QSPI_CLK_ENABLE();

  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Enable GPIO clocks */
  QSPI_CLK_GPIO_CLK_ENABLE();
  QSPI_CS_GPIO_CLK_ENABLE();
  QSPI_D0_GPIO_CLK_ENABLE();
  QSPI_D1_GPIO_CLK_ENABLE();
  QSPI_D2_GPIO_CLK_ENABLE();
  QSPI_D3_GPIO_CLK_ENABLE();

  /* QuadSPI CS GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_CS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = QSPI_CS_PIN_AF;
  HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &GPIO_InitStruct);

  /* QuadSPI CLK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_CLK_PIN;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = QSPI_CLK_PIN_AF;
  HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &GPIO_InitStruct);

  /* QuadSPI D0 GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_D0_PIN;
  GPIO_InitStruct.Alternate = QSPI_D0_PIN_AF;
  HAL_GPIO_Init(QSPI_D0_GPIO_PORT, &GPIO_InitStruct);

  /* QuadSPI D1 GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_D1_PIN;
  GPIO_InitStruct.Alternate = QSPI_D1_PIN_AF;
  HAL_GPIO_Init(QSPI_D1_GPIO_PORT, &GPIO_InitStruct);

  /* QuadSPI D2 GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_D2_PIN;
  GPIO_InitStruct.Alternate = QSPI_D2_PIN_AF;
  HAL_GPIO_Init(QSPI_D2_GPIO_PORT, &GPIO_InitStruct);

  /* QuadSPI D3 GPIO pin configuration  */
  GPIO_InitStruct.Pin       = QSPI_D3_PIN;
  GPIO_InitStruct.Alternate = QSPI_D3_PIN_AF;
  HAL_GPIO_Init(QSPI_D3_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  De-Initializes the QSPI MSP.
  * @param  hQspi QSPI handle
  * @retval None
  */
static void QSPI_MspDeInit(QSPI_HandleTypeDef *hQspi)
{
  /* hqspi unused argument(s) compilation warning */
  UNUSED(hQspi);

  /* QuadSPI GPIO pins de-configuration  */
  HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);
  HAL_GPIO_DeInit(QSPI_CS_GPIO_PORT, QSPI_CS_PIN);
  HAL_GPIO_DeInit(QSPI_D0_GPIO_PORT, QSPI_D0_PIN);
  HAL_GPIO_DeInit(QSPI_D1_GPIO_PORT, QSPI_D1_PIN);
  HAL_GPIO_DeInit(QSPI_D2_GPIO_PORT, QSPI_D2_PIN);
  HAL_GPIO_DeInit(QSPI_D3_GPIO_PORT, QSPI_D3_PIN);

  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Disable the QuadSPI memory interface clock */
  QSPI_CLK_DISABLE();
}

/**
  * @brief  This function reset the QSPI memory.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_ResetMemory (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MX25R6435F_ResetEnable(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(MX25R6435F_ResetMemory(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Qspi_Ctx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;      /* After reset S/W setting to indirect access   */
    Qspi_Ctx[Instance].InterfaceMode = BSP_QSPI_SPI_MODE;         /* After reset H/W back to SPI mode by default  */

    /* After SWreset CMD, wait in case SWReset occurred during erase operation */
    HAL_Delay(MX25R6435F_RESET_MAX_TIME);
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function enables the quad mode of the memory.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_EnterQuadMode(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Read Status register */
  else if (MX25R6435F_ReadStatusRegister(&(hqspi[Instance]), &reg) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Enable write operations */
  else if (MX25R6435F_WriteEnable(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Write Status register (with Quad SPI protocol) */
  else
  {
    reg |= MX25R6435F_SR_QE;
    if (MX25R6435F_WriteStatusRegister(&(hqspi[Instance]), reg) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Wait that the configuration is effective and check that memory is ready */
      HAL_Delay(MX25R6435F_WRITE_REG_MAX_TIME);
      reg = 0;
      
      /* Check Flash busy ? */
      if (MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Check the configuration has been correctly done */
      else if (MX25R6435F_ReadStatusRegister(&(hqspi[Instance]), &reg) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if ((reg & MX25R6435F_SR_QE) == 0U)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }
  
  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function disables the quad mode of the memory.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_ExitQuadMode (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t reg;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  /* Read Status register */
  else if (MX25R6435F_ReadStatusRegister(&(hqspi[Instance]), &reg) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  /* Enable write operations */
  else if (MX25R6435F_WriteEnable(&(hqspi[Instance])) != MX25R6435F_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Write Status register (with SPI protocol) */
    reg &= ~(MX25R6435F_SR_QE);
    if (MX25R6435F_WriteStatusRegister(&(hqspi[Instance]), reg) != MX25R6435F_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      /* Wait that the configuration is effective and check that memory is ready */
      HAL_Delay(MX25R6435F_WRITE_REG_MAX_TIME);
      reg = 0;

      /* Check Flash busy ? */
      if (MX25R6435F_AutoPollingMemReady(&(hqspi[Instance])) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Check the configuration has been correctly done */
      else if (MX25R6435F_ReadStatusRegister(&(hqspi[Instance]), &reg) != MX25R6435F_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if ((reg & MX25R6435F_SR_QE) != 0U)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }

  /* Return BSP status */
  return ret;
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

