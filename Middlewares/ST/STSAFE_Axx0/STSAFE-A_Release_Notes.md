---
pagetitle: Release Notes for STSAFE-A1xx Middleware Component
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}

::: {.sectione .dark}
<center>
# <small>Release Notes for</small> **STSAFE-A1xx Middleware**
Copyright &copy; 2019 STMicroelectronics
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo target="_blank"}
</center>
:::
:::

# License

This software component is
licensed by ST under STSAFE DRIVER SOFTWARE LICENSE AGREEMENT (SLA0088), the "License"; You may not use this component except in compliance
with the License. You may obtain a copy of the License at:

[[SLA0088 License @ www.st.com](en.DM00594102.pdf){target="_blank"}]

# Purpose
The STSAFE-A1xx Middleware is a software component providing a complete set of APIs to access all the STSAFE-A110 device features from a Host Microcontroller. 

![](_htmresc/architecture_2.PNG)

The STSAFE-A110 is a highly secure solution that acts as a secure element providing authentication and data management services to a local or remote host. 
It consists of a full turnkey solution with a secure operating system running on the latest generation of secure microcontrollers. 
The STSAFE-A110 can be integrated in IoT (Internet of things) devices, smart-home, smart-city and industrial applications, consumer electronics devices, consumables and accessories.

**STSAFE-A110 Key Features**:

- Authentication (of peripherals, IoT and USB Type-C devices)

- Secure channel establishment with remote host including transport layer security (TLS) handshake 

- Signature verification service (secure boot and firmware upgrade) 

- Usage monitoring with secure counters

- Pairing and secure channel with host application processor 

- Wrapping and unwrapping of local or remote host envelopes

- On-chip key pair generation 




Here is the list of references to user documents:

- [ STSAFE-A110 Datasheet
  ](https://www.st.com/resource/en/datasheet/stsafe-a110.pdf){target="_blank"} : Authentication state-of-the-art security for peripherals and IoT devices
:::

::: {.col-sm-12 .col-lg-8}
<center>
[![STSAFE-A logo](_htmresc/STSAFE_A_logo.png)](https://www.st.com/en/secure-mcus/stsafe-a110.html){.logo target="_blank"}

# Update History
</center>

<details open>
<summary>V3.3.1 / 8-July-2020</summary>

## Main Changes

### Maintenance release

This release implements a new version of the Middleware in order to:

- API change for read data partition.

- New communication bus timeout management

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  StSafeA_Read : API change to add amount of data to read command in order to differentiate it with expected response length
  StSafeA_SendBytes : Remove the bus DeInit + Init in case of driver's failure and change timeout value in milliseconds instead of number of loop
  StSafeA_ReceiveBytes : Remove the bus DeInit + Init in case of driver's failure and change timeout value in milliseconds instead of number of loop

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.40.2

- MDK-ARM       v5.29.0

- STM32CubeIDE  v1.3.0

- SW4STM32      v2.9.1

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is backward compatibility with previous version (V3.3.0) except for API change mentioned above

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.21.0
  
- Cryptographic Library X-CUBE-CRYPTOLIB
  V3.1.0 + PATCH_CRYPTOLIB V3.1.2
  
- STM32L4xx HAL drivers
  V1.11.1

\

</details>

<details open>
<summary>V3.3.0 / 29-April-2020</summary>

## Main Changes

### Maintenance release

This release implements a new version of the Middleware in order to:

- Modify service interface to provide MCU, IO Bus, Timing & CRC API definition at user space or anyway in the upper layers according to the hardware user choices.

- API change for signature generation. Both ECDSA signature's parts R & S are returned separately.

- Product data buffer type moved to products specific header file (stsafe-a100.h & stsafe-a110.h)

- API change for public key slot query.

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  StSafeA_HW_Probe : Update CRC16 X25 initialization
  StSafeA_GenerateKeyPair : API change (parameters swap)
  StSafeA_GenerateSignature : API change to return ECDSA signature's parts R & S separately
  StSafeA_GetSignature : API change to return ECDSA signature's parts R & S separately
  StSafeA_PublicKeySlotQuery : API change to add expected length of returned curve identifier
  STSAFEA_HOST_KEY_LENGTH : Value is not anymore in bits (128) but in bytes (16)

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.40.2

- MDK-ARM       v5.29.0

- STM32CubeIDE  v1.3.0

- SW4STM32      v2.9.1

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is backward compatibility with previous version (V3.2.0) except for API change mentioned above

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.21.0
  
- Cryptographic Library X-CUBE-CRYPTOLIB
  V3.1.0 + PATCH_CRYPTOLIB V3.1.2
  
- STM32L4xx HAL drivers
  V1.11.1

\

</details>

<details>
<summary>V3.2.0 / 17-March-2020</summary>

## Main Changes

### Maintenance release

This release implements a new version of the Middleware in order to:

- Add CRC16X25_Init & CRC_Compute to provide possibility to use HAL CRC peripheral like in STM32 family (By default software CRC computation is used)

- Modify StSafeA_Update to support atomic update with large buffer

- Add StSafeA_RndSubject_t type into STSAFE-A110's header file

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  CRC16X25_Init : HAL CRC16 X25 initialization. Remind to **add pointer to CRC16X25_Init through 'pCtx' object defined as parameter of StSafeA_HW_Probe** function (HwCtx->CrcInit = CRC16X25_Init).
  CRC_Compute : HAL CRC computation. Remind to **add pointer to CRC_Compute through 'pCtx" object defined as parameter of StSafeA_HW_Probe** function (HwCtx->CrcInit = CRC_Compute).
  StSafeA_Update command : Updated when command is used in an atomic way with payload size larger than atomic buffer length
  StSafeA_RndSubject_t type : added to stsafea110.h 

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.40.2

- MDK-ARM       v5.29.0

- STM32CubeIDE  v1.3.0

- SW4STM32      v2.9.1

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is backward compatibility with previous version (V3.1.1)

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.21.0
  
- Cryptographic Library X-CUBE-CRYPTOLIB
  V3.1.0 + PATCH_CRYPTOLIB V3.1.2
  
- STM32L4xx HAL drivers
  V1.11.1

\

</details>

<details>
<summary>V3.1.1 / 13-February-2020</summary>

## Main Changes

### Maintenance release

This release implements a new version of the Middleware in order to:

- Update I2C polling

- Update StSafeA_GenerateKeyPair command

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  I2C polling : different I2C polling duration for I2C transmit & I2C receive
  StSafeA_GenerateKeyPair command : optimize I2C response duration

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.42.2

- MDK-ARM       v5.29.0

- STM32CubeIDE  V1.2.1

- SW4STM32      v2.9.1

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is backward compatibility with previous version (V3.1.0)

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.17.0
  
- Cryptographic Library X-CUBE-CRYPTOLIB
  V3.1.0 + PATCH_CRYPTOLIB V3.1.2
  
- STM32L4xx HAL drivers
  V1.11.0

\

</details>

<details>
<summary>V3.1.0 / 16-September-2019</summary>

## Main Changes

### Maintenance release

This release implements a new version of the Middleware in order to:

- Clean STSAFE-A100/A110 unsupported commands

- Move "Key Slot Number" definitions into device specific include file

- Create STSAFE-A110 device specific include file

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Signature session : only supported for STSAFE-A100
  Entity signature verification : only supported for STSAFE-A100
  Public key slot : only supported for STSAFE-A100
  Signature generation, Key establishment, Wrap & Unwrap local envelope, Generate key pair : Updated to support STSAFE-A110's command access condition
  Command authorization configuration : only supported for STSAFE-A110
  StSafeA_RawCommand : Issue solved
  StSafeA_AssignLVResponse : Updated to return STSAFE-A1xx response length in case of response's length < expected response's length
  Add __weak macro definition for GCC compiler
  Add SWAP2BYTES & SWAP4BYTES macro definition for IAR, ARM, ARMV6 & GCC compilers
  __weak StSafeA_HostKeys_Init : Updated to return 0 when macro STSAFEA_USE_OPTIMIZATION_NO_HOST_MAC_ENCRYPT = 1 
  MISRA-C 2012 compliance

## Known Limitations
Outstanding bugs list

  Ticket                                                                            Headline
  --------------------------------------------------------------------------------- ----------------------------------------------------------
  ID: 573583                                                                         Failure when IAR Compiler High Optimization option is set. "initial_value" variable not reserved on the memory stack.

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.40.1

- MDK-ARM       v5.27.1

- SW4STM32      v2.9.1

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is backward compatibility with previous version (V3.0.0)

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.17.0
  
- Cryptographic Library X-CUBE-CRYPTOLIB
  V3.1.0 + PATCH_CRYPTOLIB V3.1.2
  
- STM32L4xx HAL drivers
  V1.10.0

\

</details>

<details>
<summary>V3.0.0 / 13-July-2019</summary>

## Main Changes

### Maintenance release

This is a major release implementing a completely new Middleware architecture that has been re-designed in order to:

- Improve Robustness and Security

- Improve Maintainability, Reliability, Scalability


**Therefore there is no backward compatibility with previous releases.**

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  New CORE module to simplify integration in customer applications
  New CRYPTO module to isolate the secure/trusted area in flash (eg. behind Firewall / TrustZone) increasing the security level and reducing the attack surface
  New SERVICE module to allow hardware abstraction and platform independence 
  MISRA-C 2012 compliance

## Known Limitations
Outstanding bugs list

  Ticket                                                                            Headline
  --------------------------------------------------------------------------------- ----------------------------------------------------------
  ID: 573583                                                                         Failure when IAR Compiler High Optimization option is set. "initial_value" variable not reserved on the memory stack.

## Development Toolchains and Compilers
The STSAFE-A1xx Middleware is easily portable on different toolchains.
It has been tested in applications developed using the following toolchains and compilers:

- IAR EWARM     v8.32

- MDK-ARM       v5.27

- SW4STM32      v2.9

## Supported Devices and Boards
The STSAFE-A1xx Middleware is easily portable on different STM32 microcontrollers.
It has been tested in applications developed for the following devices and boards:

- NUCLEO-L476RG        STM32L476

## Backward Compatibility

There is no backward compatibility with previous versions

## Dependencies

This software release is compatible with:

- MbedTLS 
  V2.14.0
  
- STM32 Crypto Library
  V3.0.0
  
- STM32L4xx HAL drivers
  V1.9.0

\

</details>

<details>
<summary>V2.3.8 / 10-July-2019</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Add command size check for StSafeA_GenerateRandom function
  Remove STSAFE_A_WEAK_COM macro
  Update endianness bytes swapping macro

\

</details>

<details>
<summary>V2.3.7 / 11-December-2019</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  CRC CCITT table correction
  Add StSafeA_SetI2cParameters function to stsafe_a_basic.h & .c

\

</details>

<details>
<summary>V2.3.6 / 08-March-2019</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Move definition of StSafeA_DataBuffer structure into stsafe_a_common.h
  Move StSafeA_DataPartitionQuery function into stsafe_a_data_partition.h & .c
  Move StSafeA_PublicKeySlotQuery function into stsafe_a_private_public_key.h & .c
  Add command size check in case of EXCHANGE_BUFFER_SIZE != EXCHANGE_BUFFER_DEFAULT_SIZE (0x1FB)

\

</details>

<details>
<summary>V2.3.5 / 08-February-2019</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Move STSAFE-A BSP files to STSAFE-A middleware
  Add SLA0088 license to STSAFE-A's source files
  Add check command size to ovoid buffer overflow

\

</details>

<details>
<summary>V2.3.4 / 11-December-2018</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Hotfix for StSafeA_VerifyEntitySignature & StSafeA_PublicKeySlotQuery
  Fix usage of STM32 HW byte swap
  Optimize StSafeA_Update

\

</details>

<details>
<summary>V2.3.3 / 20-July-2018</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Update "StSafeA_GenerateRandom" to solve issue with ephemeral random

\

</details>

<details>
<summary>V2.3.1 / 04-July-2018</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Adjust buffer size
  Add optimized bytes’ swap using ARM Instructions
  Minor "StSafeA_GenerateRandom" update

\

</details>

<details>
<summary>V2.3.0 / 25-June-2018</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Signature session functionality becomes optional (use STSAFE_A_SIGN_SESSION preprocessing flag to activate it)
  Update "StSafeA_WrapLocalEnvelope", "StSafeA_UnwrapLocalEnvelope", "StSafeA_GenerateSignature" and "StSafeA_EstablishKey" to manage Host Encryption (Available from STSAFE-A110)
  Update "StSafeA_RawCommand" to manage Host MAC
  Add "StSafeA_CommandAuthorizationConfigurationQuery"
  CRC computation optimization

\

</details>

<details>
<summary>V2.2.0 / 19-March-2018</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  "StSafeA_PutPublicKeySlot" function added
  Move weak function preprocessing definition to "stsafe_a_types.h"
  Add STSAFE_A_INVALID_PARAMETER to "StSafeA_ResponseCode" enumeration
  Remove 1 byte memory alignment for "stsafe_a" type
  Correct "StSafeA_Hibernate" issue 

\

</details>

<details>
<summary>V2.1.0 / 04-December-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  None

\

</details>

<details>
<summary>V2.0.0 / 21-November-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Update wrapper source code to add STSAFE-A110 device support

\

</details>

<details>
<summary>V1.5.0 / 30-August-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Correct typo issues 
  Update StSafeA_GetHostCMacSequenceCounter return to void type
  Update StSafeA_ComputeCMAC & StSafeA_ComputeRMAC to Remove dynamic allocation and Solve issue on RMAC computation with STM32 crypto library

\

</details>

<details>
<summary>V1.4.0 / 30-June-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Change __AUTHENTICATION__ compilation flag to __NO_HMAC__; this allows STSAFE-A wrapper usage without C/R MAC support

\

</details>

<details>
<summary>V1.3.0 / 28-March-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Bug fix in CMAC computation (memory corruption) : stsafe_a_tools.c
  Modify buffer allocation:  no more query with dynamic allocation, use static size instead : stsafe_a_configuration.c
  Add preprocessing flag commutation for AES table used for CRC computation: Static (flash) or dynamic (RAM) : stsafe_a_crc.c
  Modify template for I2C communication : stsafe_a_bus_conf_template.c

\

</details>

<details>
<summary>V1.2.0 / 25-January-2017</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Update format of Tag, Length, Value structures (new name is TLVBuffer)
  Update all wrapper sources file to be compliant with new TLVBuffer structure

\

</details>

<details>
<summary>V1.1.0 / 14-December-2016</summary>

## Main Changes

### Maintenance release

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  Rename stsafe_a_i2c_conf_template source (.c) & header (.h) files to stsafe_a_bus_conf_template source (.c) & header (.h) files into "Middlewares\ST\STSAFE_A\Template\I2C"
  Rename stsafe_a_i2c_conf source (.c) & header (.h) files to stsafe_a_bus_conf source (.c) & header (.h) files from all projects into "Projects\Multi"
  Update stsafe_a_configuration.c source file

\

</details>

<details>
<summary>V1.0.0 / 06-October-2016</summary>

## Main Changes

### First release 

<u>Additional features :</u>

  Headline
  ----------------------------------------------------------
  First official version for STSAFE-A100 devices

\

</details>




:::
:::

<footer class="sticky">
For complete documentation on **STSAFE-A110** ,
visit: [[STSAFE-A110 @ www.st.com](https://www.st.com/en/secure-mcus/stsafe-a110.html){target="_blank"}]

This release note uses up to date web standards and, for this reason, should not
be opened with Internet Explorer but preferably with popular browsers such as
Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
