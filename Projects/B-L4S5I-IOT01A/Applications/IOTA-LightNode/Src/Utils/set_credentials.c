/**
  ******************************************************************************
  * @file    set_credentials.c
  * @author  MCD Application Team
  * @brief   set the device connection credentials
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include <stdint.h>
#include "main.h"
#include "iot_flash_config.h"
#include "net_connect.h"

/* Global variables ----------------------------------------------------------*/
extern int32_t es_wifi_driver(net_if_handle_t * pnetif);
net_if_driver_init_func device_driver_ptr=&es_wifi_driver;
net_wifi_credentials_t wifi_cred;

/* Function prototypes -----------------------------------------------*/
int32_t set_network_credentials(net_if_handle_t *pnetif);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Prompt and set the credentials for the network interface
  * @param  pnetif     Pointer to the network interface

  * @retval NET_OK               in case of success
  *         NET_ERROR_FRAMEWORK  if unable to set the parameters
  */
int32_t set_network_credentials(net_if_handle_t *pnetif)
{
  const char *ssid_ptr;
  const char *psk_ptr;
  uint8_t sec_mode;
  bool skip_reconf = false;

  printf("\n*** WIFI connection ***\n\n");

  skip_reconf = (checkWiFiCredentials(&ssid_ptr, &psk_ptr, &sec_mode) == NET_OK);

  if (skip_reconf == true)
  {
    printf("Push the User button (Blue) within the next 5 seconds if you want to update"
           " the WiFi network configuration.\n\n");

    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }

  if (skip_reconf == false)
  {
    printf("Your WiFi parameters need to be entered to proceed.\n");
    do
    {
      updateWiFiCredentials();
    } while (checkWiFiCredentials(&ssid_ptr, &psk_ptr, &sec_mode) != NET_OK);
  }

  wifi_cred.psk = psk_ptr;
  wifi_cred.security_mode = sec_mode;
  wifi_cred.ssid = ssid_ptr;

  if (net_wifi_set_credentials(pnetif, &wifi_cred) != NET_OK)
  {
    msg_error("Can not set the Wi-Fi credentials\n");
    return NET_ERROR_FRAMEWORK;
  }
  return NET_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
