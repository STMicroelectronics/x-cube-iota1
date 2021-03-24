 /**
  ******************************************************************************
  * @file    timingSystem.c
  * @author  Central LAB
  * @brief   Wrapper to STM32 timing
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

/* Includes ------------------------------------------------------------------*/
#include <time.h>
#include "timingSystem.h"

#if defined (__CC_ARM)
extern const int yytab[2][12];
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

#if defined (__IAR_SYSTEMS_ICC__)
#if _DLIB_TIME_USES_64
__time64_t __time64(__time64_t * pointer)
{
  return (__time64_t)TimingSystemGetSystemTime();
}
#else
__time32_t __time32(__time32_t * pointer)
{
  return (__time32_t)TimingSystemGetSystemTime();
}
#endif
#elif defined (__CC_ARM)

time_t time(time_t * pointer)
{
  return TimingSystemGetSystemTime();
}

/** @brief gmtime conversion for ARM compiler
* @param  time_t *timer : File pointer to time_t structure
* @retval struct tm * : date in struct tm format
 */
struct tm *gmtimeMDK(register const time_t *timer)
{
  static struct tm br_time;
  register struct tm *timep = &br_time;
  time_t time = *timer;
  register unsigned long dayclock, dayno;
  int year = EEPOCH_YR;

  dayclock = (unsigned long)time % SSECS_DAY;
  dayno = (unsigned long)time / SSECS_DAY;

  timep->tm_sec = dayclock % 60;
  timep->tm_min = (dayclock % 3600) / 60;
  timep->tm_hour = dayclock / 3600;
  timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
  while (dayno >= YYEARSIZE(year)) {
    dayno -= YYEARSIZE(year);
    year++;
  }
  timep->tm_year = year - YYEAR0;
  timep->tm_yday = dayno;
  timep->tm_mon = 0;
  while (dayno >= yytab[LLEAPYEAR(year)][timep->tm_mon]) {
    dayno -= yytab[LLEAPYEAR(year)][timep->tm_mon];
    timep->tm_mon++;
  }
  timep->tm_mday = dayno + 1;
  timep->tm_isdst = 0;
  return timep;
}

struct tm * gmtime(const time_t *p)
{
  return gmtimeMDK(p);
}


#elif defined (__GNUC__)
time_t time(time_t * pointer)
{
  return TimingSystemGetSystemTime();
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
