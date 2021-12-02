# IOTA-Client Application

## License

This software package is licensed by ST under ST license SLA0055, the "License". You may not use this component except in compliance
with the License.

You may obtain a copy of the License at: [SLA0055](https://bit.ly/2JuvQ4F)

## Application Description

This application shows how to create an IOTA Client application, running on 
a STM32 microcontroller, and enabling use cases especially related to machine-to-machine secure communication.
It relies on the security algorithms for encryption, hashing, message authentication,
and digital signing provided by STM32 cryptographic library.
It also introduces a sample application demonstrating the L2Sec, a Layer 2 lightweight security protocol
for building and sending to the Tangle an encrypted, authenticated message stream.

The current version of this application implements the IOTA protocol version 1.5
(Chrysalis). For details about Chrysalis, please refer to the official IOTA
Documentation website (https://chrysalis.docs.iota.org/).

In the IOTA-Client application, the STM32 Discovery device provides the
following full functionality set (L2SEC project configuration):
 1. Get node info
 2. Get a message payload
 3. Send a generic indexation message
 4. Send an indexation message including sensor data
 5. Send an encrypted indexation message
 6. Enter the L2Sec example submenu to run the L2Sec application:
    * Send only
	* Send and receive
	
	Note: Enter the number of messages to be sent, otherwise a default number (defined by the constant N_MESSAGES in the l2sec_example.c source file) is used.
          Each message contains the following application data: [DeviceName,Timestamp,Temperature,Humidity]

 7. Enter the STASFE example submenu to run some basic STSAFE demonstrations:
    * Echo
	* Wrap/Unwrap local envelope
 8. Other test functions to run some IOTA basic functions:
	* Get node info
	* Get output
	* Get balance
	* Get message
	* Find message
	* Message builder
	* Get tips
	* Get output from address
	* Response error
	* Send message
	* Get message metadata
	* Get message children
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
	* STM32CubeIDE V1.7.0 + ST-LINK/V2
* Select the build configuration:
    * L2SEC to run the respective example application relying on STASAFE security services and libsodium crypto support
	* CRYPTOLIB to include the cryptograhic middleware by ST
	* SODIUM to include the cryptograhic middleware by libsodium
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
		2. for the IOTA Node (the "Tangle" certificate).
        The concatenated string must end with an empty line. This is usertrust_thetangle.pem.
	* These parameters are then saved in flash and used as default for the next runs.
    * After the parameters are configured, it is possible to change them by restarting the board
      and pushing the User button (blue button) within 5 seconds.

### Application runtime

* The application makes an HTTPS request to retrieve the current time and date, and configures the RTC.
* The user is prompted to enter an option to test the corresponding function.
