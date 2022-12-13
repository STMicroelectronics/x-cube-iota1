
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos_config.h
  * @author  MCD Application Team
  * @brief   app_azure_rtos config header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_AZURE_RTOS_CONFIG_H
#define APP_AZURE_RTOS_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* Using static memory allocation via threadX Byte memory pools */

#define USE_STATIC_ALLOCATION	1

#define TX_APP_MEM_POOL_SIZE	1024

//#define NX_APP_MEM_POOL_SIZE	102400
#define NX_APP_MEM_POOL_SIZE	143360

#define CFG_HEAP_POOL_SIZE		102400

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/**
 * Allocate a block of memory of a specific size.
 *
 * @param size [IN] specifies the size in byte of the requested memory.
 * @return a pointer to the allocated memory if success, NULL otherwise.
 */
void *SysAlloc(size_t size);

/**
 * Release a block of memory.
 *
 * @param ptr [IN] specifies the start of the block of memory to release.
 */
void SysFree(void *ptr);

/**
 * Realloc a block of memory.
 *
 * @param ptr [IN] specifies the start of the block of memory to realloc.
 * @param size [IN] specifies the block memory size.
 */
void *SysRealloc(void *ptr, size_t size);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define malloc SysAlloc
#define free SysFree
#define realloc SysRealloc

/* USER CODE END PD */

#ifdef __cplusplus
}
#endif

#endif /* APP_AZURE_RTOS_CONFIG_H */

