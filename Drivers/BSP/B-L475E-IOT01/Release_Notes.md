---
pagetitle: Release Notes for B-L475E-IOT01A Board Drivers
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> <mark>B-L475E-IOT01A Board Drivers</mark>
Copyright &copy; 2017 STMicroelectronics\
    
[![ST logo](../../../_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

Licensed by ST under BSD 3-Clause license (the \"License\"). You may
not use this package except in compliance with the License. You may
obtain a copy of the License at:

[https://opensource.org/licenses/BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

# Purpose

This directory contains the board drivers to demonstrate the capabilities of the B-L475E-IOT01A Kit.

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section6" checked aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">V2.0.0 / 29-March-2019</label>
<div>			

## Main Changes

### Component release

Official release of B-L475E-IOT01A board drivers in line with STM32Cube BSP drivers development guidelines (UM2298).  
  
  - The board drivers are composed of
    - board drivers configuration file: *stm32l475e_iot01_conf_template.h*
      - *stm32l475e_iot01_conf_template.h* file must be copied in user application as *stm32l475e_iot01_conf.h* with optional configuration update
    - board drivers error codes file: *stm32l475e_iot01_errno.h*
    - BSP Common drivers (*stm32l475e_iot01.h/stm32l475e_iot01.c*)
    - BSP Bus drivers (*stm32l475e_iot01_bus.h/stm32l475e_iot01_bus.c*)
    - BSP AUDIO_IN class (*stm32l475e_iot01_audio.h/stm32l475e_iot01_audio.c*)
    - BSP ENV_SENSOR class (*stm32l475e_iot01_env_sensors.h/stm32l475e_iot01_env_sensors.c*)
    - BSP MOTION_SENSOR class (*stm32l475e_iot01_motion_sensors.h/stm32l475e_iot01_motion_sensors.c*)
    - BSP QSPI class (*stm32l475e_iot01_qspi.h/stm32l475e_iot01_qspi.c*)

## Backward Compatibility

This release breaks compatibility with previous versions.

## Dependencies

This software release is compatible with:

-   BSP Common driver V6.0.0 or later

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">V1.1.3 / 27-July-2018</label>
<div>			

## Main Changes

### Component release

Release notes update to new format


</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V1.1.2 / 20-April-2018</label>
<div>			

## Main Changes

### Component release

Header functions description cleanup

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V1.1.1 / 13-October-2017</label>
<div>			

## Main Changes

### Component release

Fix compilation warning with armcc --gnu

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.1.0 / 21-April-2017</label>
<div>						

## Main Changes

### Component release

Add support of NFC via M24SR component.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 17-March-2017</label>
<div>			

## Main Changes

### Component release

First official release of B-L475E-IOT01 BSP drivers

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [http://www.st.com/STM32](http://www.st.com/STM32)
</footer>
