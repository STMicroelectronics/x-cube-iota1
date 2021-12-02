---
pagetitle: Release Notes for ES_WIFI Component Driver
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> <mark>ES_WIFI Component Driver</mark>
Copyright &copy; 2017 STMicroelectronics\
Microcontrollers Division - Application Team

[![ST logo](../../../../_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

Licensed by ST under BSD 3-Clause license (the \"License\"). You may
not use this package except in compliance with the License. You may
obtain a copy of the License at:

[https://opensource.org/licenses/BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

# Purpose

This directory contains the ES_WIFI component driver.

:::

::: {.col-sm-12 .col-lg-8}
# Update History
::: {.collapse}
<input type="checkbox" id="collapse-section27" checked aria-hidden="true">
<label for="collapse-section27" aria-hidden="true">V1.8.0 / 30-March-2021</label>
<div>

## Main Changes

- Fix lock management when es_wifi driver is used from different thread (not recommended)
- Do not lock continously driver resources while waiting for server connection

</div>
:::
::: {.collapse}
<input type="checkbox" id="collapse-section26" checked aria-hidden="true">
<label for="collapse-section26" aria-hidden="true">V1.7.0 / 30-March-2021</label>
<div>

## Main Changes

- Fix UDP socket management, pseudo server mode was corrupted as well as socket reported port information
- Fix HW timeout, removing useless and confusing API 
- Add specific ATCMD api to send low level command (used by network library)

</div>
:::
::: {.collapse}
<input type="checkbox" id="collapse-section25" checked aria-hidden="true">
<label for="collapse-section25" aria-hidden="true">V1.6.0 / 09-Apr-2020</label>
<div>

## Main Changes

- Add country code definition in configuration file to allow higher WiFi channels support
- Replaced strtok() by strtok_r() to be thread-safe
- Add Sock info and Peer info functions

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section24" aria-hidden="true">
<label for="collapse-section24" aria-hidden="true">V1.5.0 / 21-Nov-2019</label>
<div>

## Main Changes

- Add partial support of new Inventek Module for Tiny Shark board
- Fix  MISRA deviation
- Improve embedded TLS support API

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section23"  aria-hidden="true">
<label for="collapse-section23" aria-hidden="true">V1.4.0 / 03-July-2019</label>
<div>

## Main Changes

- Set a default value to read and write time-out when non blocking
- Add ES_WIFI_MAX_SO_TIMEOUT
- Apply new template for sources
- New release notes format

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section22" aria-hidden="true">
<label for="collapse-section22" aria-hidden="true">V1.3.0 / 10-Sept-2018</label>
<div>

## Main Changes

- Add support of TLS connection based on internal WIFI module support
- Ability to list Access Point one by one from Inventek firmware version 3.5.2.5

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section20" aria-hidden="true">
<label for="collapse-section20" aria-hidden="true">V1.2.0 / 24-July-2018</label>
<div>

## Main Changes

- Add parameter to ES_WIFI_Ping() API to get the time for each ping.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section19" aria-hidden="true">
<label for="collapse-section19" aria-hidden="true">V1.1.2 / 07-June-2018</label>
<div>

## Main Changes

- Fix in UDP support along new Inventek FW versions >= C3.5.2.4.STM.RC11

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section185" aria-hidden="true">
<label for="collapse-section185" aria-hidden="true">V1.1.1 / 27-February-2018</label>
<div>

## Main Changes

- Improve TCP server command
- Improve Robustness on potential memory overflows

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section18" aria-hidden="true">
<label for="collapse-section18" aria-hidden="true">V1.1.0 / 05-February-2018</label>
<div>

## Main Changes

- Support for UDP sockets
- Spport for projects using RTOS
- Fix issue on never ending reads on stuffing bits

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section17" aria-hidden="true">
<label for="collapse-section17" aria-hidden="true">V1.0.1 / 31-August-2017</label>
<div>

## Main Changes

- Corrections for ES_WIFI WiFi component.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section16" aria-hidden="true">
<label for="collapse-section16" aria-hidden="true">V1.0.0 / 23-February-2017</label>
<div>

## Main Changes

- First official release of ES_WIFI WiFi component.

</div>
:::


:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [http://www.st.com/STM32](http://www.st.com/STM32)
</footer>
