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
 */
typedef struct {
  uint64_t magic;                                     /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char ssid[USER_CONF_WIFI_SSID_MAX_LENGTH];          /**< Wifi network SSID. */
  char psk[USER_CONF_WIFI_PSK_MAX_LENGTH];            /**< Wifi network PSK. */
  uint8_t security_mode;                              /**< Wifi network security mode. See @ref wifi_network_security_t definition. */
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
int enterPemString(char * read_buffer, size_t max_len);
int getInputString(char *inputString, size_t len);

int checkC2cCredentials(const char ** const oper_ap_code, const char ** const username, const char ** const password, bool * use_internal_sim);
int updateC2cCredentials(void);
int checkWiFiCredentials(const char ** const ssid, const char ** const psk, uint8_t * const security_mode);
int updateWiFiCredentials(void);

int updateTLSCredentials(void);
int checkTLSRootCA(void);

int checkDefaultSeed(const char ** const seed);
int updateDefaultSeed(char *seed);
int checkTxConfig(const char ** const full_node_url, const char ** const tx_interval, const char ** const temp_thresh);
int updateTxConfig(void);

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

