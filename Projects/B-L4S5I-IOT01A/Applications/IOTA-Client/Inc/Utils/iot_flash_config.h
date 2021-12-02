/**
  ******************************************************************************
  * @file    iot_flash_config.h
  * @author  MCD Application Team
  * @brief   Header for configuration
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

#ifndef iot_flash_config_H
#define iot_flash_config_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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

/** @defgroup FLASH_Config Flash Config
 * @{
 */

/** @defgroup FLASH_Config_EXPORTED_CONSTANTS Exported Constants
 * @{
 */

/* Exported constants --------------------------------------------------------*/
/**
 * @brief C2C config related macro: SOAPC
 */
#define USER_CONF_C2C_SOAPC_MAX_LENGTH  16
/**
 * @brief C2C config related macro: User ID
 */
#define USER_CONF_C2C_USERID_MAX_LENGTH 16
/**
 * @brief C2C config related macro: Password
 */
#define USER_CONF_C2C_PSW_MAX_LENGTH    16

/**
 * @brief WiFi config related macro: SSID
 */
#define USER_CONF_WIFI_SSID_MAX_LENGTH  32
/**
 * @brief WiFi config related macro: PSK
 */
#define USER_CONF_WIFI_PSK_MAX_LENGTH   64

/**
 * @brief User config: device name
 *
 * Must be large enough to hold a complete configuration string
 */
#define USER_CONF_DEVICE_NAME_LENGTH    300
/**
 * @brief User config: server name
 */
#define USER_CONF_SERVER_NAME_LENGTH    128
/**
 * @brief User config: TLS obj
 */
#define USER_CONF_TLS_OBJECT_MAX_SIZE   2048
/**
 * @brief User config: magic number for flash checking
 */
#define USER_CONF_MAGIC                 0x0123456789ABCDEFuLL

#ifndef SEED_LENGTH
/**
 * @brief IOTA Seed Length
 */
#define SEED_LENGTH                     81U
#endif /* SEED_LENGTH */
/**
 * @brief IOTA Full Node URL
 */
#define FULL_NODE_URL                   "https://nodes.thetangle.org:443/"
/**
 * @brief IOTA Full Node URL max size
 */
#define MAX_FULL_NODE_URL_SIZE          50
/**
 * @brief IOTA Transaction configuration max length
 */
#define TX_CONFIG_MAX_LENGTH            8

/**
 * @}
 */

/** @defgroup FLASH_Config_EXPORTED_TYPES Exported Types
 * @{
 */
/* Exported types ------------------------------------------------------------*/
/**
 * @brief C2C configuration structure
 */
typedef struct {
  uint64_t magic;                                     /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  bool use_internal_sim;                              /**< SIM slot selector. */
  char oper_ap_code[USER_CONF_C2C_SOAPC_MAX_LENGTH];  /**< C2C sim operator access point code. */
  char username[USER_CONF_C2C_USERID_MAX_LENGTH];     /**< C2C username. */
  char password[USER_CONF_C2C_PSW_MAX_LENGTH];        /**< C2C password. */
} c2c_config_t;

/**
 * @brief WiFi configuration structure
 * See @ref net_wifi_credentials_t definition.
 */
typedef struct {
  uint64_t magic;                                     /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char ssid[USER_CONF_WIFI_SSID_MAX_LENGTH];          /**< Wifi network SSID. */
  char psk[USER_CONF_WIFI_PSK_MAX_LENGTH];            /**< Wifi network PSK. */
  uint32_t security_mode;                             /**< Wifi network security mode. */
} wifi_config_t;

/**
 * @brief IOTA SEED configuration structure
 */
typedef struct {
  uint64_t magic;           /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char seed[SEED_LENGTH+3]; /**< Seed string - added 3 chars to make the len multiple of 4 */
} iota_config_t;

/**
 * @brief IOTA Transaction configuration structure
 */
typedef struct {
  uint64_t magic;                             /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char full_node_url[MAX_FULL_NODE_URL_SIZE]; /**< Full Node URL string. */
  char tx_interval[TX_CONFIG_MAX_LENGTH];     /**< Time interval between transactions. */
  char temp_thresh[TX_CONFIG_MAX_LENGTH];     /**< Temperature threshold. */
} iota_tx_config_t;

/**
 * @brief User configuration structure
 *
 * Static user configuration data which must survive reboot and firmware update.
 * Do not change the field order, due to firewall constraint the tls_device_key size must be placed at a 64 bit boundary.
 * Its size must also be multiple of 64 bits.
 *
 * Depending on the available board peripherals, the c2c_config and wifi_config fields may not be used.
 */
typedef struct {
  char tls_root_ca_cert[USER_CONF_TLS_OBJECT_MAX_SIZE * 3]; /**< Allow room for 3 root CA certificates. */
#ifdef USE_C2C
  c2c_config_t c2c_config;                                  /**< C2C configuration. */
#endif
#ifdef USE_WIFI
  wifi_config_t wifi_config;                                /**< C2C configuration. */
#endif
  iota_config_t iota_config;                                /**< IOTA Seed configuration. */
  iota_tx_config_t iota_tx_config;                          /**< IOTA Transaction configuration. */
  uint64_t ca_tls_magic;                                    /**< The USER_CONF_MAGIC magic word signals that the TLS root CA certificates strings
                                                                 (tls_root_ca_cert) are present in Flash. */
} user_config_t;

/**
 * @}
 */

/** @defgroup FLASH_Config_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */
/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Get one PEM string (ASCII format of TLS certificates and keys) from the console (user input).
 * @param  Out: read_buffer    Destination buffer.
 * @param  In:  max_len        Maximum length to be written to the destination buffer.
 * @retval Number of characters read into the output buffer.
 */
int enterPemString(char * read_buffer, size_t max_len);
/**
 * @brief  Get a line from the console (user input).
 * @param  Out:  inputString   Pointer to buffer for input line.
 * @param  In:   len           Max length for line.
 * @retval Number of bytes read from the terminal.
 */
int getInputString(char *inputString, size_t len);

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
int checkC2cCredentials(const char ** const oper_ap_code, const char ** const username, const char ** const password, bool * use_internal_sim);
/**
 * @brief  Write the C2C parameters to the Flash memory.
 * @param  None
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int updateC2cCredentials(void);
/**
 * @brief  Check whether the Wifi parameters are present in Flash memory.
 *         Returns the parameters if present.
 * @param  Out:  ssid              Wifi SSID.
 * @param  Out:  psk               Wifi security password.
 * @param  Out:  security_mode     See @ref wifi_network_security_t definition.
 * @retval   0 if the parameters are present in Flash.
 *          -1 if the parameters are not present in Flash.
 */
int checkWiFiCredentials(const char ** const ssid, const char ** const psk, uint32_t * const security_mode);
/**
 * @brief  Write the Wifi parameters to the Flash memory.
 * @param  None
 * @retval Error code
 *             0    Success
 *             <0   Unrecoverable error
 */
int updateWiFiCredentials(void);

/**
 * @brief Warning:
 *        The device key pair should only be updated if the application uses it.
 *        The connection security mode may chosen by the user during the device configuration.
 *        Therefore, updateTLSCredentials() must be called only after the IoT device configuration has been selected.
 * @param None
 * @retval 0 success <0 failure
 */
int updateTLSCredentials(void);
int checkTLSRootCA(void);

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
#endif /* iot_flash_config_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

