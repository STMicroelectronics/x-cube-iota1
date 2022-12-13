# IOTA-Client Application

## Application Description

This application shows how to create an IOTA Client application, running on 
a STM32 microcontroller, and enabling use cases especially related to machine-to-machine secure communication.
It relies on the security algorithms for encryption, hashing, message authentication,
and digital signing provided by Mbed crypto and libsodium.
It also introduces a sample application demonstrating the L2Sec, a Layer 2 lightweight security protocol
for building and sending to the Tangle an encrypted, authenticated message stream.

The current version of this application implements the IOTA protocol version 1.5
(Chrysalis). For details about Chrysalis, please refer to the official [IOTA Documentation website](https://wiki.iota.org/introduction/welcome/).

In the IOTA-Client application, the STM32 Discovery device provides the
following full functionality set:

 1. Get node info
 2. Send an indexation message including sensor data
 3. Enter the L2Sec example submenu to run the L2Sec application:
    * Send only
    * Send and receive
	
    Note: Enter the number of messages to be sent, otherwise a default number (defined by the constant N_MESSAGES in the l2sec_example.c source file) is used.
    Each message contains the following application data: [DeviceName,Timestamp,Temperature,Humidity]

The application runs on the B-U585I-IOT02A Discovery board
and connects to the Internet through the attached WiFi interface.

### <b>Keywords</b>

IOTA, L2SEC, RTOS, Network, ThreadX, NetXDuo, WIFI, TCP, MXCHIP, UART

## Hardware and Software environment

  - This example runs on STM32U585xx devices with a WiFi module (MXCHIP:EMW3080) with this configuration :

    + MXCHIP Firmware 2.1.11

    + Bypass mode 

    + SPI mode used as interface

    The EMW3080B MXCHIP Wi-Fi module firmware and the way to update your board with it are available at <https://www.st.com/en/development-tools/x-wifi-emw3080b.html>

    Before using this project, you shall update your B-U585I-IOT02A board with the EMW3080B firmware version 2.1.11.

    To achieve this, follow the instructions given at the above link, using the EMW3080updateV2.1.11RevC.bin flasher under the V2.1.11/SPI folder.

  - This application has been tested with B-U585I-IOT02A (MB1551-U585AI) boards Revision: RevC and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

## How to use it?

**WARNING**: before opening the project with any toolchain be sure your folder
    installation path is not too in-depth since the toolchain may report errors
    after building.

In order to make the program work, you must do the following :
	
 - Open your preferred toolchain
 
  This firmware has been successfully tested with:

	* IAR Embedded Workbench for ARM (EWARM) V9.20.1 + STLink/V2
	* RealView Microcontroller Development Kit (MDK-ARM) V5.37.0 + ST-LINK/V2
	* STM32CubeIDE V1.10.1 + ST-LINK/V2

 - On <code> Core/Inc/mx_wifi_conf.h </code> , Edit your Wifi Settings (WIFI_SSID,WIFI_PASSWORD)  

 - Rebuild all files and load your image into target memory

 - Run the application
 
   Once the inizialization is complete, the user is prompted to enter an option to test the corresponding function.

