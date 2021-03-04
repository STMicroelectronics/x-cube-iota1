/**
  ******************************************************************************
  * @file    timingSystem.h
  * @author  Central LAB
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

#include <time.h>

extern void TimingSystemInitialize(void);
extern int TimingSystemSetSystemTime(time_t epochTimeNow);
extern time_t TimingSystemGetSystemTime(void);

#define YYEAR0           1900                    /* the first year */
#define EEPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SSECS_DAY        (24UL * 60UL * 60UL)
#define LLEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YYEARSIZE(year)  (LLEAPYEAR(year) ? 366 : 365)

/* MDK */
extern struct tm *gmtimeMDK(register const time_t *timer);

#endif

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

