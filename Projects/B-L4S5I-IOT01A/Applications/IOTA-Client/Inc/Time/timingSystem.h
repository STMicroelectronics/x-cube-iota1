/**
  ******************************************************************************
  * @file    timingSystem.h
  * @author  SRA/Central LAB
  * @brief   Header file for timingSystem.c
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


#ifndef TIMING_AGENT_H
#define TIMING_AGENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <time.h>

/** @addtogroup PROJECTS
 * @{
 */
 
/** @addtogroup B-L4S5I-IOT01A
 * @{
 */
 
/** @addtogroup APPLICATIONS
 * @{
 */

/** @addtogroup IOTA_Client
 * @{
 */

/** @addtogroup TIME
 * @{
 */

/** @defgroup TIME_EXPORTED_MACROS Exported Macros
 * @{
 */
/* Exported macros -----------------------------------------------------------*/
#define YYEAR0           1900                    /* the first year */
#define EEPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SSECS_DAY        (24UL * 60UL * 60UL)
#define LLEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YYEARSIZE(year)  (LLEAPYEAR(year) ? 366 : 365)

/**
 * @}
 */

/** @addtogroup TIME_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */
  
/* Exported functions --------------------------------------------------------*/
extern void TimingSystemInitialize(void);
extern int TimingSystemSetSystemTime(time_t epochTimeNow);
extern time_t TimingSystemGetSystemTime(void);
/* MDK */
extern struct tm *gmtimeMDK(register const time_t *timer);

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

