/**
  ******************************************************************************
  * @file    iota_client.c
  * @author  SRA/Central LAB
  * @brief   Handle IOTA Task
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "iota_client.h"
#include "iota_conf.h"
#include "menu.h"
#include "serial_comm.h"
#include "terminal_manager.h"
#include "iot_flash_config.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

int iota_rng_raw( void *data, uint8_t *output, size_t len)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t random_number = 0;

  for (int i = 0; i < ((len + sizeof(uint32_t) - 1) / sizeof(uint32_t)); i++)
  {
    /* Data shall contain the pointer to the selected hrng instance */
    status = HAL_RNG_GenerateRandomNumber(data, &random_number);
    if (HAL_OK == status)
    {
      int jmax = ((len - i * sizeof(uint32_t)) >= sizeof(uint32_t)) ? sizeof(uint32_t) : len % sizeof(uint32_t);
      for (int j = 0; j < jmax; j++)
      {
        output[i * sizeof(uint32_t) + j] = ((uint8_t *) &random_number)[j];
      }
    }
    else
    {
      return -1;
    }
  }

  return 0;
}

/**
 * @brief  Require Root Certificate for TLS.
 * @param  None
 * @retval Always true
 */
bool app_needs_root_ca(void)
{
  return true;
}

void iota_client_run(void)
{
  terminal_print_frame("IOTA CLIENT", '+', '|', '~', WW, RED);
    
  serial_press_any();

  main_menu();
}
