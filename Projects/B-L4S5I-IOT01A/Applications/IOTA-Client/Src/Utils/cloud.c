/**
  ******************************************************************************
  * @file    cloud.c
  * @author  MCD Application Team
  * @brief   cloud application common functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017-2021 STMicroelectronics International N.V.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "net_connect.h"
#include "timedate.h"
#include "cloud.h"
#include "sensors_data.h"
#include "stsafe.h"

/* Private function prototypes -----------------------------------------------*/
static void IOTA_Error_Handler(int errorCode);

static void hnet_notify (void *context,
                         uint32_t event_class,
                         uint32_t event_id,
                         void *event_data);

extern net_if_driver_init_func device_driver_ptr;
extern int32_t set_network_credentials(net_if_handle_t *pnetif);

net_if_handle_t netif;

const net_event_handler_t net_handler = { hnet_notify, &netif };

/* Private defines -----------------------------------------------------------*/
#define IOTA_TIMEDATE_TLS_VERIFICATION_IGNORE  /**< Accept to connect to a server which is not verified by TLS */

#define STATE_TRANSITION_TIMEOUT        100000

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  IOTA_CLIENT_WIFI_INITIALIZATION_ERROR      = -2,
  IOTA_CLIENT_MAC_ADDRESS_ERROR              = -3,
  IOTA_CLIENT_WIFI_CONNECTION_ERROR          = -4,
  IOTA_CLIENT_IP_ADDRESS_ERROR               = -5,
  IOTA_CLIENT_CONNECTION_ERROR               = -6,
  IOTA_CLIENT_TIMEDATE_ERROR                 = -7,
  IOTA_CLIENT_C2C_INITIALIZATION_ERROR       = -8
} IOTA_CLIENT_Error_t;

/* Exported functions --------------------------------------------------------*/

bool dialog_ask(char *s)
{
  char console_yn;
  do
  {
    printf("%s", s);
    console_yn = getchar();
    printf("\b");
  }
  while ((console_yn != 'y') && (console_yn != 'n') && (console_yn != '\n'));
  if (console_yn == 'y')
  {
    return true;
  }
  return false;
}

int platform_init(void)
{
  bool skip_reconf = false;

  printf("\n");
  printf("*************************************************************\n");
  printf("***   STM32 IoT Discovery kit for STM32L4S5VI MCU\n");
  printf("***   %s IOTA Client\n", FW_VERSION_NAME);
  printf("***   FW version: %d.%d.%d - %s\n",
         FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH, FW_VERSION_DATE);
  printf("*************************************************************\n");


  printf("\n*** Board personalization ***\n\n");
  /* Network initialization */

  if (net_if_init(&netif, device_driver_ptr, &net_handler) != NET_OK )
  {
    msg_error("Can not set-up the network interface structure\n");
    return NET_ERROR_FRAMEWORK;
  }
  else
  {
    if (net_if_wait_state(&netif, NET_STATE_INITIALIZED, STATE_TRANSITION_TIMEOUT) != NET_OK )
    {
      msg_error("The network interface can not be initialized\n");
      return NET_ERROR_FRAMEWORK;
    }
  }

  set_network_credentials(&netif);

  if ( net_if_start (&netif) != NET_OK )
  {
    msg_error("The network interface can not be started\n");
    return NET_ERROR_FRAMEWORK;
  }

  if(net_if_connect (&netif) != NET_OK)
  {
    msg_error("Unable to connect netif\n");
    return -1;
  }
  else
  {
    net_if_wait_state(&netif,NET_STATE_CONNECTED,STATE_TRANSITION_TIMEOUT);
  }

  /* End of network initialization */
  
  /* Security and cloud parameters definition */
  /* Define, or allow to update if the user button is pushed. */

  skip_reconf = ((checkTLSRootCA() == 0) || !app_needs_root_ca());

  if ( (skip_reconf == true) && (app_needs_root_ca() ))
  {
    printf("Push the User button (Blue) within the next 5 seconds if you want to update "
           "the device connection parameters.\n\n");
    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }

  if (skip_reconf == false)
  {
    if (app_needs_root_ca())
    {
      updateTLSCredentials();
    }
  }
  /* End of security and cloud parameters definition */

  printf("Setting the RTC from the network time.\n");
#ifdef IOTA_TIMEDATE_TLS_VERIFICATION_IGNORE
  if (setRTCTimeDateFromNetwork(true) != TD_OK)
#else   /* IOTA_TIMEDATE_TLS_VERIFICATION_IGNORE */
  if ((setRTCTimeDateFromNetwork(false) != TD_OK) && (setRTCTimeDateFromNetwork(true) != TD_OK))
#endif  /* IOTA_TIMEDATE_TLS_VERIFICATION_IGNORE */
  {
    IOTA_Error_Handler(IOTA_CLIENT_TIMEDATE_ERROR);
    return -1;
  }

  int res = init_sensors();
  if (0 != res)
  {
    msg_error("init_sensors returned error : %d\n", res);
  }

  /* Initialize STSAFE-A */
  if(stsafe_init() != 0)
  {
    msg_error("Unable to init STSAFE\n");
    return -1;
  }

  return 0;
}

void platform_deinit(void)
{
  /* Close IOTA Client demonstration */
  printf("\n*** IOTA Client demonstration completed ***\n\n");

  net_if_deinit(&netif);
}

/* Private functions ---------------------------------------------------------*/
static void hnet_notify (void *context,
                         uint32_t event_class,
                         uint32_t event_id,
                         void *event_data)
{
  net_if_handle_t *netif=context;

  if (NET_EVENT_STATE_CHANGE == event_class)
  {
    net_state_t new_state= (net_state_t) event_id;
    switch(new_state)
    {
      case NET_STATE_INITIALIZED:
        printf("- Network Interface initialized: \n");
        break;

      case NET_STATE_STARTING:
        printf("- Network Interface starting: \n");
        break;

      case NET_STATE_READY:
        printf("- Network Interface started: \n");
        printf("   - Device Name : %s. \n", netif->DeviceName );
        printf("   - Device ID : %s. \n", netif->DeviceID );
        printf("   - Device Version : %s. \n", netif->DeviceVer );
#ifndef STM32L496xx
        printf("   - MAC address: %02X:%02X:%02X:%02X:%02X:%02X. \n", netif->macaddr.mac[0], netif->macaddr.mac[1], netif->macaddr.mac[2], netif->macaddr.mac[3], netif->macaddr.mac[4], netif->macaddr.mac[5] );
#endif
        break;

      case NET_STATE_CONNECTING:
        printf("- Network Interface connecting: \n");
        break;

      case NET_STATE_CONNECTED:
        printf("- Network Interface connected: \n");
        printf("   - IP address :  %s. \n", net_ntoa(&netif->ipaddr));
        break;

      case NET_STATE_DISCONNECTING:
        printf("- Network Interface disconnecting\n");
        break;

      case NET_STATE_STOPPING:
        printf("- Network Interface stopping\n");
        break;

      case NET_STATE_DEINITIALIZED:
        printf("- Network Interface de-initialized\n");
        break;

      default:
        break;
    }
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void IOTA_Error_Handler(int errorCode)
{
  switch (errorCode)
  {
    case (IOTA_CLIENT_C2C_INITIALIZATION_ERROR):
    case (IOTA_CLIENT_WIFI_INITIALIZATION_ERROR):
    {
      printf("Error initializing the module!\n");

      break;
    }
    case (IOTA_CLIENT_MAC_ADDRESS_ERROR):
    {
      printf("Error detecting module!\n");

      break;
    }
    case (IOTA_CLIENT_WIFI_CONNECTION_ERROR):
    {
      printf("Error connecting to AP!\n");

      break;
    }
    case (IOTA_CLIENT_IP_ADDRESS_ERROR):
    {
      printf("Error retrieving IP address!\n");

      break;
    }
    case (IOTA_CLIENT_CONNECTION_ERROR):
    {
      printf("Error connecting to Cloud!\n");

      break;
    }
    case (IOTA_CLIENT_TIMEDATE_ERROR):
    {
      printf("Error initializing the RTC from the network time!\n");

      break;
    }
    default:
    {
      break;
    }
  }

  while (1)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(200);
  }
}
