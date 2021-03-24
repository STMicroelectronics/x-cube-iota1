/**
  ******************************************************************************
  * @file    mbedtls_entropy.c
  * @author  MCD Application Team
  * @brief   This file provides code for the entropy collector.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "main.h" /* Include the HAL interface */
#include "net_conf.h"
#include "mbedtls_entropy.h"

#ifdef NET_MBEDTLS_HOST_SUPPORT


int mbedtls_rng_raw( void *data,
                    unsigned char *output, size_t len)
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

#endif /* NET_MBEDTLS_HOST_SUPPORT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
