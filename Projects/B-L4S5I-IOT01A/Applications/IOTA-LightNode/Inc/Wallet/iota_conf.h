/**
  ******************************************************************************
  * @file   iota_api_test.h
  * @author SRA/Central LAB
  * @brief  IOTA configuration file
  * 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under SLA0055, the "License";
  * You may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at www.st.com
  *
  ******************************************************************************
  */
  
#ifndef IOTA_CONF_H
#define IOTA_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_LightNode
 * @{
 */

/** @defgroup IOTA_CONF IOTA Configuration
 * @{
 */

/** @defgroup IOTA_CONF_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants---------------------------------------------------------*/
#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 1
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "30-11-2020 12:00:00 PM"

#define FW_VERSION_NAME   "FP-SNS-IOTA1"

#include <stdio.h>
#define iota_printf (void)printf
#define LINE_LENGTH   27

/**
 * If OFFLINE is defined only offline validation tests are executed.
 * For the IOTA Light Node normal behaviour keep it undefined.
 */
//#define OFFLINE

/**
 * If TESTING is defined validation tests are executed.
 * Otherwise the normal behaviour as IOTA Light Node is enabled.
 */
//#define TESTING
#define SEED_DYG "DS9KVRJREZSAZNIOQAREASQEOTHAKPNQE9GDDDVLCXLMRVXCHQCTYHRDLCRGQY9MKCQSKAKWCOPM9FOUR"
#define SEED_SIL "SILXOSRMIE9NTXFFXYJZE9WVFDQJONPSTYOCQEZUPKAMFCDXHJMFWXFEZ9OQHRVZRVJOULS9E9RQDUVIO"

#if defined(TESTING) || defined(OFFLINE)
/* In such a case only the SEED_DYG can be used */
#define TEST_SEED SEED_DYG
#else
/* Any of the above defined SEEDs can be used */
#define TEST_SEED SEED_SIL
#endif

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* IOTA_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
