/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_azure_rtos.h"
#include "stm32u5xx_hal_rtc.h" 
#include <time.h>
/* USER CODE END Includes */


#include "sensors.h"
#ifdef L2SEC_WITH_STSAFE
#include "stsafe.h"
#endif /* L2SEC_WITH_STSAFE */
#include "menu.h"
#include "serial_comm.h"
#include "terminal_manager.h"


/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
TX_THREAD AppMainThread;
TX_THREAD AppHTTPThread;
TX_THREAD AppSNTPThread;

TX_SEMAPHORE Semaphore;

NX_PACKET_POOL AppPool;
TX_BYTE_POOL *byte_pool;

ULONG IpAddress;
ULONG NetMask;
NX_IP IpInstance;

NX_DNS DNSClient;
NX_DHCP DHCPClient;
NX_SNTP_CLIENT SntpClient;

TX_EVENT_FLAGS_GROUP SntpFlags;
NX_TCP_SOCKET TCPSocket;

CHAR *pointer;
CHAR  buffer[64];
/* RTC handler declaration */
extern RTC_HandleTypeDef hrtc;

ULONG current_time;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FW_VERSION_NAME "X-CUBE-IOTA1"
#define FW_VERSION_MAJOR 3
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 0
#define FW_VERSION_DATE "01-12-2022"

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
static void display_rtc_time(RTC_HandleTypeDef *hrtc);
static void rtc_time_update(NX_SNTP_CLIENT *client_ptr);
static void iota_client_run(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
UINT  iface_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID App_Main_Thread_Entry(ULONG thread_input);
static VOID App_SNTP_Thread_Entry(ULONG thread_input);
static VOID App_HTTP_Thread_Entry(ULONG thread_input);

static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);
static VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time);
/* USER CODE END PFP */



/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
  nx_system_initialize();
  /* USER CODE END App_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN MX_NetXDuo_Init */
#if (USE_STATIC_ALLOCATION == 1)
  printf("\n");
  printf("*************************************************************\n");
  printf("***   STM32 IoT Discovery kit for STM32U585AI MCU\n");
  printf("***   IOTA Client (with L2Sec protocol)\n");
  printf("***   FW name: %s\n", FW_VERSION_NAME);
  printf("***   FW version: %d.%d.%d - %s\n",
         FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH, FW_VERSION_DATE);
  printf("*************************************************************\n");
  printf("\n");
  
  /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }  
  /* Create the Packet pool to be used for packet allocation */
  ret = nx_packet_pool_create(&AppPool, "Main Packet Pool", PAYLOAD_SIZE, pointer, NX_PACKET_POOL_SIZE);
  
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }
  

  /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, 4*DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create the main NX_IP instance */
  ret = nx_ip_create(&IpInstance, "Main Ip instance", NULL_ADDRESS, NULL_ADDRESS, &AppPool, nx_driver_emw3080_entry,
                     pointer, 4*DEFAULT_MEMORY_SIZE, DEFAULT_MAIN_PRIORITY);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  
  /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /*  Enable the ARP protocol and provide the ARP cache size for the IP instance */
  ret = nx_arp_enable(&IpInstance, (VOID *)pointer, DEFAULT_MEMORY_SIZE);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

   
  /* Enable the ICMP */
  ret = nx_icmp_enable(&IpInstance);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  
  /* Enable the UDP protocol required for  DHCP communication */
  ret = nx_udp_enable(&IpInstance);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  
  /* Enable the TCP protocol */
  ret = nx_tcp_enable(&IpInstance);
  
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }


  /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, THREAD_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }  
  /* Create the main thread */
  ret = tx_thread_create(&AppMainThread, "App Main thread", App_Main_Thread_Entry, 0, pointer, THREAD_MEMORY_SIZE,
                         DEFAULT_MAIN_PRIORITY, DEFAULT_MAIN_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }


  /* Allocate the memory for SNTP client thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, 6*DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* create the SNTP client thread */
  ret = tx_thread_create(&AppSNTPThread, "App SNTP Thread", App_SNTP_Thread_Entry, 0, pointer, 6*DEFAULT_MEMORY_SIZE,
                         DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }


  /* Allocate the memory for HTTP CLIENT thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, THREAD_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* create the HTTP CLIENT thread */
  ret = tx_thread_create(&AppHTTPThread, "App HTTP Thread", App_HTTP_Thread_Entry, 0, pointer, THREAD_MEMORY_SIZE,
                         DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

     
  /* create the DHCP client */
  ret = nx_dhcp_create(&DHCPClient, &IpInstance, "DHCP Client");
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }



  /* Create the event flags. */
  ret = tx_event_flags_create(&SntpFlags, "SNTP event flags");
  /* Check for errors */
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }



  /* set DHCP notification callback  */
  tx_semaphore_create(&Semaphore, "App Semaphore", 0);

#endif
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID App_Main_Thread_Entry(ULONG thread_input)
{
  UINT ret;

  ret = nx_ip_address_change_notify(&IpInstance, ip_address_change_notify_callback, NULL);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }
  
  ret = nx_dhcp_start(&DHCPClient);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }
  
  /* wait until an IP address is ready */
  if(tx_semaphore_get(&Semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  ret = nx_ip_address_get(&IpInstance, &IpAddress, &NetMask);
  
  if (ret != TX_SUCCESS)
  {
    Error_Handler();
  }
  
  PRINT_IP_ADDRESS(IpAddress);
  
  /* the network is correctly initialized, start the SNTP CLIENT thread */
  tx_thread_resume(&AppSNTPThread);
  /* this thread is not needed any more, relinquish it */
  tx_thread_relinquish();
  
  return;
}

/**
* @brief  IP Address change callback.
* @param ip_instance: NX_IP instance registered for this callback.
* @param ptr: VOID* user data pointer
* @retval none
*/
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
  tx_semaphore_put(&Semaphore);
}

/* This application defined handler for notifying SNTP time update event.  */
static VOID time_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time)
{
  NX_PARAMETER_NOT_USED(time_update_ptr);
  NX_PARAMETER_NOT_USED(local_time);

  tx_event_flags_set(&SntpFlags, SNTP_UPDATE_EVENT, TX_OR);
}

/**
* @brief  DNS Create Function.
* @param dns_ptr
* @retval ret
*/
UINT dns_create(NX_DNS *dns_ptr)
{
  UINT ret = NX_SUCCESS;

  /* Create a DNS instance for the Client */
  ret = nx_dns_create(dns_ptr, &IpInstance, (UCHAR *)"DNS Client");
  
  if (ret)
  {
    Error_Handler();
  }
  
  /* Initialize DNS instance with the DNS server Address */
  ret = nx_dns_server_add(dns_ptr, USER_DNS_ADDRESS);
  if (ret)
  {
    Error_Handler();
  }

  return ret;
}

/**
* @brief  SNTP thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
/* Define the client thread.  */
static void App_SNTP_Thread_Entry(ULONG info)
{
  UINT ret;
  ULONG  fraction;
  ULONG  events = 0;
  UINT   server_status;
  NXD_ADDRESS sntp_server_ip;
  NX_PARAMETER_NOT_USED(info);

  sntp_server_ip.nxd_ip_version = 4;

  /* Create a DNS client */
  ret = dns_create(&DNSClient);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Look up SNTP Server address. */
  ret = nx_dns_host_by_name_get(&DNSClient, (UCHAR *)SNTP_SERVER_NAME,
                                &sntp_server_ip.nxd_ip_address.v4, DEFAULT_TIMEOUT);

    /* Check for error. */
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

   /* Create the SNTP Client */
  ret = nx_sntp_client_create(&SntpClient, &IpInstance, iface_index, &AppPool, NX_NULL, NX_NULL, NX_NULL);

  /* Check for error. */
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Setup time update callback function. */
  nx_sntp_client_set_time_update_notify(&SntpClient, time_update_callback);

  /* Use the IPv4 service to set up the Client and set the IPv4 SNTP server. */
  ret = nx_sntp_client_initialize_unicast(&SntpClient, sntp_server_ip.nxd_ip_address.v4);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Run whichever service the client is configured for. */
  ret = nx_sntp_client_run_unicast(&SntpClient);

  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }
  else
  {
    PRINT_CNX_SUCC();
  }

  /* Wait for a server update event. */
  tx_event_flags_get(&SntpFlags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &events, PERIODIC_CHECK_INTERVAL);

  if (events == SNTP_UPDATE_EVENT)
  {
    /* Check for valid SNTP server status. */
    ret = nx_sntp_client_receiving_updates(&SntpClient, &server_status);

    if ((ret != NX_SUCCESS) || (server_status == NX_FALSE))
    {
      /* We do not have a valid update. */
      Error_Handler();
    }
    /* We have a valid update.  Get the SNTP Client time.  */
    ret = nx_sntp_client_get_local_time_extended(&SntpClient, &current_time, &fraction, NX_NULL, 0);
    /* take off 70 years difference */
    current_time -= EPOCH_TIME_DIFF;

    ret = nx_sntp_client_utility_display_date_time(&SntpClient,buffer,64);

    if (ret != NX_SUCCESS)
    {
      printf("Internal error with getting local time 0x%x\n", ret);
      Error_Handler();
    }
    else
    {
      printf("\nSNTP update :\n");
      printf("%s\n\n",buffer);
    }
  }
  else
  {
    Error_Handler();
  }

  /* Set Current time from SNTP TO RTC */
  rtc_time_update(&SntpClient);

  /* We can stop the SNTP service if for example we think the SNTP server has stopped sending updates */
  ret = nx_sntp_client_stop(&SntpClient);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* When done with the SNTP Client, we delete it */
  ret = nx_sntp_client_delete(&SntpClient);
  if (ret != NX_SUCCESS)
  {
    Error_Handler();
  }

  /* Display RTC time  */
  display_rtc_time(&hrtc);

  /* the network is correctly initialized, start the HTTP CLIENT thread */
  tx_thread_resume(&AppHTTPThread);
  /* this thread is not needed any more, relinquish it */
//  tx_thread_relinquish();
}

/* Function (set by user) to call when TLS needs the current time. */
ULONG nx_secure_tls_session_time_function(void)
{
  return (current_time);
}



/**
* @brief  HTTP thread entry.
* @param thread_input: thread user data
* @retval none
*/
static VOID App_HTTP_Thread_Entry(ULONG thread_input)
{
  INT status;
  
  NX_PARAMETER_NOT_USED(thread_input);
  
  status = sensors_start();
  if (status < 0) {
    printf("Error during sensors initialization! (ERR: %d)\n", status);
  }
#ifdef L2SEC_WITH_STSAFE
  status = stsafe_init();
  if (status < 0) {
    printf("Error during STSAFE initialization! (ERR: %d)\n", status);
  }
#endif /* L2SEC_WITH_STSAFE */
  
  iota_client_run();
  
  printf("\n-------------------------------------\n\tTEST ENDED\n-------------------------------------\n");
  
  return;
}

/* This function updates Time from SNTP to STM32 RTC */
static void rtc_time_update(NX_SNTP_CLIENT *client_ptr)
{
  RTC_DateTypeDef sdatestructure ={0};
  RTC_TimeTypeDef stimestructure ={0};
  struct tm ts;
  CHAR  temp[32] = {0};

  /* convert SNTP time (seconds since 01-01-1900 to 01-01-1970)

  EPOCH_TIME_DIFF is equivalent to 70 years in sec
  calculated with www.epochconverter.com/date-difference
  This constant is used to delete difference between :
  Epoch converter (referenced to 1970) and SNTP (referenced to 1900) */
  time_t timestamp = client_ptr->nx_sntp_current_server_time_message.receive_time.seconds - EPOCH_TIME_DIFF;

  /* convert time in yy/mm/dd hh:mm:sec */
  ts = *localtime(&timestamp);

  /* convert date composants to hex format */
  sprintf(temp, "%d", (ts.tm_year - 100));
  sdatestructure.Year = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_mon + 1);
  sdatestructure.Month = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_mday);
  sdatestructure.Date = strtol(temp, NULL, 16);
  /* dummy weekday */
  sdatestructure.WeekDay =0x00;

  if (HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* convert time composants to hex format */
  sprintf(temp,"%d", ts.tm_hour);
  stimestructure.Hours = strtol(temp, NULL, 16);
  sprintf(temp,"%d", ts.tm_min);
  stimestructure.Minutes = strtol(temp, NULL, 16);
  sprintf(temp, "%d", ts.tm_sec);
  stimestructure.Seconds = strtol(temp, NULL, 16);

  if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

}

/* This function displays time from RTC */
static void display_rtc_time(RTC_HandleTypeDef *hrtc)
{
  RTC_TimeTypeDef RTC_Time = {0};
  RTC_DateTypeDef RTC_Date = {0};

  HAL_RTC_GetTime(hrtc, &RTC_Time,RTC_FORMAT_BCD);
  HAL_RTC_GetDate(hrtc, &RTC_Date,RTC_FORMAT_BCD);

  printf("%02x-%02x-20%02x / %02x:%02x:%02x\n",\
        RTC_Date.Date, RTC_Date.Month, RTC_Date.Year,RTC_Time.Hours,RTC_Time.Minutes,RTC_Time.Seconds);
}

static void iota_client_run(void)
{
  terminal_print_frame("IOTA CLIENT NETX DUO", '+', '|', '~', WW, RED);

  serial_press_any();

  main_menu();
}

/* USER CODE END 1 */
