# IOTA-Client Application

## License

This software package is licensed by ST under ST license SLA0055, the "License". You may not use this component except in compliance
with the License.

You may obtain a copy of the License at: [SLA0055](https://bit.ly/2JuvQ4F)

## Application Description

This application shows how to create an IOTA Client application, running on 
a STM32 microcontroller, for use cases especially related to the data and value
transaction among machines.

The current version of this application implements the IOTA protocol version 1.5
(Chrysalis). For details about Chrysalis, please refer to the official IOTA
Documentation website (https://chrysalis.docs.iota.org/).

In the IOTA-Client application, the STM32 Nucleo device provides the 
following functionalities:
 1. Send an indexation message
 2. Send a transaction message
 4. Test other functions
	* Get node info
	* Get output
	* Get balance
	* Get message
	* Find message
	* Message builder
	* Get tips
	* Outputs from address
	* Test response error
	* Send message
	* Test wallet
	* Test crypto

The application runs on the B-L4S5I-IOT01A-Discovery board
and connects to the Internet through the attached WiFi interface.

The connection to the Internet is configured by the user at application startup
thanks to the device console. The network connections are secured by TLS.

## Hardware and Software environment

* MCU board: B-L4S5I-IOT01A Discovery kit.
* A development PC for building the application, programming through ST-Link, and running the virtual console.

## How to use it?
### Application build and flash

* **WARNING**: before opening the project with any toolchain be sure your folder
    installation path is not too in-depth since the toolchain may report errors
    after building.
* Open and build the project with one of the supported development toolchains.
  This firmware has been successfully tested with:
	* IAR Embedded Workbench for ARM (EWARM) V8.50.9 + STLink/V2
	* RealView Microcontroller Development Kit (MDK-ARM) V5.32.0 + ST-LINK/V2
	* STM32CubeIDE V1.6.1 + ST-LINK/V2
* Program the STM32 board with the firmware binary: download the pre-built .bin file in folder
  *Projects\B-L4S5I-IOT01A\Applications\IOTA-Client\Binary* on your target device.
* Alternatively, you can program the STM32 board directly through one of the supported development toolchains.

### Application first launch

* Connect the board to your development PC through USB (ST-LINK USB port). Open the console through a serial terminal emulator (e.g. TeraTerm), select the ST-LINK COM port of your board and configure it with:
	* 8N1, 115200 bauds, no HW flow control;
	* set the line endings to AUTO (Transmit) and LF (receive).
* On the console:
	* Set the TLS root CA certificates:
		* Copy-paste the contents of Projects\B-L4S5I-IOT01A\Applications\IOTA-Client\usertrust_thetangle.pem.
    The device uses them to authenticate the remote hosts through TLS.
	* **Note**: The IOTA Client application requires that a concatenation of 2 CA certificates is provided:
		1. for the HTTPS server which is used to retrieve the current time and date at boot time (the "Usertrust" certificate).
		2. for the Tangle Full Node (the "Tangle" certificate).
        The concatenated string must end with an empty line. This is usertrust_thetangle.pem.
	* These parameters are then saved in flash and used as default for the next runs.
    * After the parameters are configured, it is possible to change them by restarting the board
      and pushing the User button (blue button) within 5 seconds.

### Application runtime

* The application makes an HTTPS request to retrieve the current time and date, and configures the RTC.
* The user is prompted to enter an option to test the corresponding function.
