---
pagetitle: Release Notes for B-U585I-IOT02A BSP Drivers
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---
::: {.row}
::: {.col-sm-12 .col-lg-4}

<center>
# Release Notes for <mark>B-U585I-IOT02A BSP Drivers</mark>

Copyright &copy; 2021 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>

# Purpose

These drivers provide a set of functions to manage:

- LEDs, push-button, joystick and COM ports
- OctoSPI NOR Flash memory
- OctoSPI PSRAM memory
- Camera
- Motion sensors (Acc, Gyro and Magno)
- Environmental sensors (Temperature, Humidity and Pressure)
- EEPROM memory
- USBPD Type C port
- Light sensor
- Audio IN
- Ranging Sensor

*Note that b_u585i_iot02a_conf_template.h file must be copied in user application as
b_u585i_iot02a_conf.h with optional configuration update.*

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section3" checked aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">__V1.1.0 / 16-February-2022__</label>
<div>

## Main Changes

- b_u585i_iot02a_audio: Add Audio IN functionality (reworked)
- b_u585i_iot02a_ranging_sensor: Add ranging sensor driver
- b_u585i_iot02a_usbpd_pwr: Correction for failed MQP certification test, and false OCP detection management
- MISRA-C 2012 coding rules compliance Formatting
- B-U585I-IOT02A_BSP_User_Manual.chm: Update with Audio IN and ranging sensor drivers

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V1.0.1 / 01-October-2021__</label>
<div>

## Main Changes

- b_u585i_iot02a_audio: remove Audio IN functionality (Under rework, to be re-integrated in the next release)
- b_u585i_iot02a_light_sensor: add new driver 
- b_u585i_iot02a_motion_sensors: Replace ISM330DLC with ISM330DHCX


</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.0.0 / 28-June-2021__</label>
<div>

## Main Changes

- First official release of __B-U585I-IOT02A__ BSP drivers in line with STM32Cube BSP drivers development guidelines (UM2298)

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [[www.st.com](http://www.st.com/STM32)]{style="font-color: blue;"}
</footer>
