/**
  ******************************************************************************
  * @file    iot_flash_config.c
  * @author  MCD Application Team
  * @brief   configuration in flash memory.
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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "flash.h"
#include "iot_flash_config.h"
#include "msg.h"
#include "net_connect.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define PEM_READ_BUFFER_SIZE  8192  /* Max size which can be got from the terminal in a single getInputString(). */

/* Private macros ------------------------------------------------------------*/
#ifdef ENABLE_TRACE_FLASH
#define TRACE_FLASH msg_info
#else
#define TRACE_FLASH(...)
#endif

/* Private variables ---------------------------------------------------------*/
/** Do not zero-initialize the static user configuration.
 *  Otherwise, it must be entered manually each time the device FW is updated by STLink.
 */

/* to workaround a limitation of SFMI tools which support a single section      */
/* so do not mark structure as _no_init                                         */
#ifdef __ICCARM__ /* IAR */
extern void __ICFEDIT_region_FIXED_LOC_start__;
const  user_config_t    *lUserConfigPtr = &__ICFEDIT_region_FIXED_LOC_start__;

#elif defined (__CC_ARM ) /* Keil / armcc */
user_config_t __uninited_region_start__ __attribute__((section("UNINIT_FIXED_LOC"), zero_init));
const  user_config_t    *lUserConfigPtr = &__uninited_region_start__;

#elif defined (__GNUC__) /* GNU compiler */
user_config_t __uninited_region_start__ __attribute__((section("UNINIT_FIXED_LOC")));
const  user_config_t    *lUserConfigPtr = &__uninited_region_start__;

#endif

/* Private function prototypes -----------------------------------------------*/
int CaptureAndFlashPem(char *pem_name, char const *flash_addr);
/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Get a line from the console (user input).
 * @param  Out:  inputString   Pointer to buffer for input line.
 * @param  In:   len           Max length for line.
 * @retval Number of bytes read from the terminal.
 */
int getInputString(char *inputString, size_t len)
{
  size_t currLen = 0;
  int c = 0;

  c = getchar();

  while ((c != EOF) && ((currLen + 1) < len) && (c != '\r') && (c != '\n') )
  {
    if (c == '\b')
    {
      if (currLen != 0)
      {
        --currLen;
        inputString[currLen] = 0;
        printf(" \b");
      }
    }
    else
    {
      if (currLen < (len-1))
      {
        inputString[currLen] = c;
      }

      ++currLen;
    }
    c = getchar();
  }
  if (currLen != 0)
  { /* Close the string in the input buffer... only if a string was written to it. */
    inputString[currLen] = '\0';
  }
  if (c == '\r')
  {
    c = getchar(); /* assume there is '\n' after '\r'. Just discard it. */
  }

  return currLen;
}

#ifdef USE_C2C
/**
 * @brief  Check whether the C2C parameters are present in Flash memory.
 *         Returns the parameters if present.
 * @param  Out:  oper_ap_code       SIM operator Access Point code
 * @param  Out:  username           username
 * @param  Out:  password           password
 * @param  Out:  use_internal_sim  Use the embedded SIM rather than the external slot
 * @retval   0 if the parameters are present in Flash.
 *          -1 if the parameters are not present in Flash.
 */
int checkC2cCredentials(const char ** const oper_ap_code, const char ** const username, const char ** const password, bool * use_internal_sim)
{
  bool is_soapc_present = 0;

  if (lUserConfigPtr->c2c_config.magic == USER_CONF_MAGIC)
  {
    is_soapc_present = true;
    if (oper_ap_code == NULL)
    {
      return -2;
    }

    *use_internal_sim = lUserConfigPtr->c2c_config.use_internal_sim;
    *oper_ap_code = lUserConfigPtr->c2c_config.oper_ap_code;
    *username = lUserConfigPtr->c2c_config.username;
    *password = lUserConfigPtr->c2c_config.password;
  }

  return (is_soapc_present) ? 0 : -1;
}


/**
 * @brief  Write the C2C parameters to the Flash memory.
 * @param  None
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int updateC2cCredentials(void)
{
  c2c_config_t c2c_config;
  int ret = 0;

  memset(&c2c_config, 0, sizeof(c2c_config_t));

/*
  Build this code if you want the user to choose the SIM slot.
  By default, the external slot is used.
*/
#if MANUAL_SIM_SELECTION
  {
    char c;
    do
    {
        printf("\rSelect the SIM slot (0 - External, 1 - Internal): \b");
        c = getchar();
    }
    while ( (c < '0')  || (c > '1'));
    c2c_config.use_internal_sim = (bool) (c - '0');

    printf("\nYou have selected the %s SIM.\n",
               (c2c_config.use_internal_sim == true) ? "internal" : "external");
  }
#endif

  printf("\nEnter Sim Operator Access Point Code (e.g. EM or ESEYE1 etc): ");
  getInputString(c2c_config.oper_ap_code, USER_CONF_C2C_SOAPC_MAX_LENGTH);
  printf("\nYou have entered <%s> as the Sim Operator Access Point Code.\n", c2c_config.oper_ap_code);

  printf("\nEnter the username (it can be NULL) (max 16 char):  ");
  getInputString(c2c_config.username, USER_CONF_C2C_USERID_MAX_LENGTH);
  printf("\nYou have entered <%s> as the username.\n", c2c_config.username);

  printf("\nEnter the password (it can be NULL) (max 16 char):  ");
  getInputString(c2c_config.password, USER_CONF_C2C_PSW_MAX_LENGTH);
  printf("\nYou have entered <%s> as the password.\n", c2c_config.password);

  c2c_config.magic = USER_CONF_MAGIC;

  ret = FLASH_update((uint32_t)&lUserConfigPtr->c2c_config, &c2c_config, sizeof(c2c_config_t));

  if (ret < 0)
  {
    msg_error("Failed updating the C2C configuration in Flash.\n");
  }

  msg_info("\n");
  return ret;
}
#endif /* USE_C2C */

#ifdef USE_WIFI
/**
 * @brief  Check whether the Wifi parameters are present in Flash memory.
 *         Returns the parameters if present.
 * @param  Out:  ssid              Wifi SSID.
 * @param  Out:  psk               Wifi security password.
 * @param  Out:  security_mode     See @ref wifi_network_security_t definition.
 * @retval   0 if the parameters are present in Flash.
 *          -1 if the parameters are not present in Flash.
 */
int checkWiFiCredentials(const char ** const ssid, const char ** const psk, uint8_t * const security_mode)
{
  bool is_ssid_present = 0;

  if (lUserConfigPtr->wifi_config.magic == USER_CONF_MAGIC)
  {
    is_ssid_present = true;
    if ((ssid == NULL) ||(psk == NULL) || (security_mode == NULL))
    {
      return -2;
    }
    *ssid = lUserConfigPtr->wifi_config.ssid;
    *psk = lUserConfigPtr->wifi_config.psk;
    *security_mode = lUserConfigPtr->wifi_config.security_mode;
  }

  return (is_ssid_present) ? 0 : -1;
}


/**
 * @brief  Write the Wifi parameters to the Flash memory.
 * @param  None
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int updateWiFiCredentials(void)
{
  wifi_config_t wifi_config;
  int ret = 0;

  memset(&wifi_config, 0, sizeof(wifi_config_t));

  printf("\nEnter SSID: ");

  getInputString(wifi_config.ssid, USER_CONF_WIFI_SSID_MAX_LENGTH);
  msg_info("You have entered %s as the ssid.\n", wifi_config.ssid);

  printf("\n");
  char c;
  do
  {
      printf("\rEnter Security Mode (0 - Open, 1 - WEP, 2 - WPA, 3 - WPA2): ");
      c = getchar();
  }
  while ( (c < '0')  || (c > '3'));
  wifi_config.security_mode = c - '0';
  msg_info("\nYou have entered %d as the security mode.\n", wifi_config.security_mode);

  if (wifi_config.security_mode != 0)
  {
    printf("\nEnter password: ");
    getInputString(wifi_config.psk, sizeof(wifi_config.psk));
  }

  wifi_config.magic = USER_CONF_MAGIC;

  ret = FLASH_update((uint32_t)&lUserConfigPtr->wifi_config, &wifi_config, sizeof(wifi_config_t));

  if (ret < 0)
  {
    msg_error("Failed updating the wifi configuration in Flash.\n");
  }

  printf("\n");
  return ret;
}
#endif /* USE_WIFI */


/**
 * @brief  Get one PEM string (ASCII format of TLS certificates and keys) from the console (user input).
 * @param  Out: key_read_buffer    Destination buffer.
 * @param  In:  max_len            Maximum length to be written to the destination buffer.
 * @retval Number of characters read into the output buffer.
 */
int enterPemString(char * read_buffer, size_t max_len)
{
  int i = 0;
  int read_len = 0;
  bool eof = false;
  read_len = getInputString(&read_buffer[i], max_len);

  while ( (read_len >= 0) && (i < max_len) && !eof )
  {
    i += read_len;
    read_buffer[i++] = '\n';
    read_len = getInputString(&read_buffer[i], max_len);
    eof = (strncmp(&read_buffer[i], "-----END",8) == 0);
    if (eof)
    {
        i += read_len;
        read_buffer[i++] = '\n';
        read_len = getInputString(&read_buffer[i], max_len);
        if (read_len != 0) eof =false;
    }
  }

  if (i >= max_len)
  {
   msg_error("Certificate is too long , allocated size is %d\n",max_len);
   return 0;
  }
  read_buffer[++i] = '\0';
  return i;
}


/**
 * @brief  Ask user and write the TLS certificates and key to the Flash memory.
 * @param  pem_name pointer to certificate
 * @param  flash_addr addrress in flash
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int CaptureAndFlashPem(char *pem_name, char const *flash_addr)
{
  char * key_read_buffer = NULL;
  int    ret = 0;
  key_read_buffer = malloc(PEM_READ_BUFFER_SIZE);
  if (key_read_buffer == NULL)
  {
    msg_error("Could not allocate %d bytes for the console readbuffer.\n", PEM_READ_BUFFER_SIZE);
    return -1;
  }

  memset(key_read_buffer, 0, PEM_READ_BUFFER_SIZE);
  printf("\nEnter your %s: \n",pem_name);
  enterPemString(key_read_buffer, PEM_READ_BUFFER_SIZE);

  msg_info("read: --->\n%s\n<---\n", key_read_buffer);

  /* Write to Flash. */
  TRACE_FLASH("writing to %lx\n", flash_addr);
  ret = FLASH_update((uint32_t)flash_addr, key_read_buffer, strlen(key_read_buffer) + 1);  /* Append the closing \0*/

  free(key_read_buffer);

  return ret;
 }

/**
 * @brief Warning:
 *        The device key pair should only be updated if the application uses it.
 *        The connection security mode may chosen by the user during the device configuration.
 *        Therefore, updateTLSCredentials() must be called only after the IoT device configuration has been selected.
 * @param None
 * @retval 0 success <0 failure
 */
int updateTLSCredentials(void)
{
  int ret = 0;

  if ( (checkTLSRootCA() == -1)
      || dialog_ask("\nDo you want to update the root CA certificate(s)? [y/n]\n") )
  {
    printf("\nUpdating TLS security credentials.\n");
    printf("\nEnter the x509 certificates or keys as per the following format:\n");
    printf("-----BEGIN CERTIFICATE-----\n");
    printf("YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n");
    printf("R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n");
    printf(".......\n");
    printf("-----END CERTIFICATE-----\n");
    printf("-----BEGIN CERTIFICATE-----\n");
    printf("YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n");
    printf(".......\n");
    printf("-----END CERTIFICATE-----\n");
    printf("\n.......\n");

    ret = CaptureAndFlashPem("root CA",lUserConfigPtr->tls_root_ca_cert);
    if (ret == 0)
    {
      uint64_t magic = USER_CONF_MAGIC;
      ret = FLASH_update((uint32_t)&lUserConfigPtr->ca_tls_magic, &magic, sizeof(uint64_t));
    }
  }

  if (ret < 0)
  {
    msg_error("Failed updating the TLS configuration in Flash.\n");
  }

  return ret;
}


/**
 * @brief  Check if TLS root CA certificates are present in Flash memory.
 * @param  None
 * @retval 0 Configured,
           -1 Not configured.
 */
int checkTLSRootCA(void)
{
  return (lUserConfigPtr->ca_tls_magic == USER_CONF_MAGIC) ? 0 : -1;
}

/**
 * @brief  Check whether the SEED is present in Flash memory.
 *         Returns the SEED if present.
 * @param  Out:  seed              SEED.
 * @retval   0 if the SEED is present in Flash.
 *          -1 if the SEED is not present in Flash.
 */
int checkDefaultSeed(const char ** const seed)
{
  bool is_seed_present = 0;

  if (lUserConfigPtr->iota_config.magic == USER_CONF_MAGIC)
  {
    is_seed_present = true;
    if ((seed == NULL))
    {
      return -2;
    }
    *seed = lUserConfigPtr->iota_config.seed;
  }

  return (is_seed_present) ? 0 : -1;
}

/**
 * @brief  Update the SEED in Flash memory.
 *         Returns the SEED if present.
 * @param  Out:  seed              SEED.
 * @retval   0 if the SEED is present in Flash.
 *          -1 if the SEED is not present in Flash.
 */
int updateDefaultSeed(char *seed)
{
  iota_config_t iota_config;
  int ret = 0;

  memset(&iota_config, 0, sizeof(iota_config_t));

  iota_config.magic = USER_CONF_MAGIC;
  (void)memcpy(iota_config.seed, seed, SEED_LENGTH); /* Copy new seed in the config struct */
  (void)memset(iota_config.seed+SEED_LENGTH, 0, 3); /* Padding data */

  ret = FLASH_update((uint32_t)&lUserConfigPtr->iota_config, &iota_config, sizeof(iota_config_t));

  if (ret < 0)
  {
    msg_error("Failed updating the SEED in Flash.\n");
  }

  printf("\n");
  return ret;
}

/**
 * @brief  Check whether the Tx parameters are present in Flash memory.
 *         Returns the parameters if present.
 * @param  Out:  full_node_url URL of full node.
 * @param  Out:  tx_interval Transaction interval [min].
 * @param  Out:  temp_thresh Temeprature threshold [degC].
 * @retval   0 if the parameters are present in Flash.
 *          -1 if the parameters are not present in Flash.
 */
int checkTxConfig(const char ** const full_node_url, const char ** const tx_interval, const char ** const temp_thresh)
{
  bool is_tx_config_present = 0;

  if (lUserConfigPtr->iota_tx_config.magic == USER_CONF_MAGIC)
  {
    is_tx_config_present = true;
    if ((full_node_url == NULL) || (tx_interval == NULL) || (temp_thresh == NULL))
    {
      return -2;
    }
    *full_node_url = lUserConfigPtr->iota_tx_config.full_node_url;
    *tx_interval = lUserConfigPtr->iota_tx_config.tx_interval;
    *temp_thresh = lUserConfigPtr->iota_tx_config.temp_thresh;
  }

  return (is_tx_config_present) ? 0 : -1;
}


/**
 * @brief  Write the Tx parameters to the Flash memory.
 * param None
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int updateTxConfig(void)
{
  iota_tx_config_t iota_tx_config;
  int ret = 0;

  memset(&iota_tx_config, 0, sizeof(iota_tx_config_t));

  printf("\nEnter Full Node URL (%s): ", FULL_NODE_URL);

  getInputString(iota_tx_config.full_node_url, MAX_FULL_NODE_URL_SIZE);
  msg_info("You have entered %s as the Full Node URL.\n", iota_tx_config.full_node_url);

  printf("\n");

  printf("\nEnter TX interval [min] (1): ");

  getInputString(iota_tx_config.tx_interval, TX_CONFIG_MAX_LENGTH);
  msg_info("You have entered %s as the TX interval.\n", iota_tx_config.tx_interval);

  printf("\n");

  printf("\nEnter Temperature threshold [degC] (18.5): ");

  getInputString(iota_tx_config.temp_thresh, TX_CONFIG_MAX_LENGTH);
  msg_info("You have entered %s as the TEMP THRESH.\n", iota_tx_config.temp_thresh);

  printf("\n");

  iota_tx_config.magic = USER_CONF_MAGIC;

  ret = FLASH_update((uint32_t)&lUserConfigPtr->iota_tx_config, &iota_tx_config, sizeof(iota_tx_config_t));

  if (ret < 0)
  {
    msg_error("Failed updating the TX configuration in Flash.\n");
  }

  printf("\n");
  return ret;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
