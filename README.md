| ![logo](https://github.com/proddy/EMS-ESP/raw/master/doc/ems gateway/logo-proddy-fw.jpg)
# EMS-ESP

[![version](https://img.shields.io/github/release/proddy/EMS-ESP.svg?label=Latest%20Release)](https://github.com/proddy/EMS-ESP/blob/master/CHANGELOG.md)
[![release-date](https://img.shields.io/github/release-date/proddy/EMS-ESP.svg?label=Released)](https://github.com/proddy/EMS-ESP/commits/master)
<br />
[![license](https://img.shields.io/github/license/proddy/EMS-ESP.svg)](LICENSE)
[![travis](https://travis-ci.com/proddy/EMS-ESP.svg?branch=master)](https://travis-ci.com/proddy/EMS-ESP)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/b8880625bdf841d4adb2829732030887)](https://app.codacy.com/app/proddy/EMS-ESP?utm_source=github.com&utm_medium=referral&utm_content=proddy/EMS-ESP&utm_campaign=Badge_Grade_Settings)
[![downloads](https://img.shields.io/github/downloads/proddy/EMS-ESP/total.svg)](https://github.com/proddy/EMS-ESP/releases)
<br />
[![gitter](https://img.shields.io/gitter/room/EMS-ESP/EMS-ESP.svg)](https://gitter.im/EMS-ESP/community)

EMS-ESP is a open-source system to communicate with **EMS** (Energy Management System) based boilers, thermostats and other modules from manufacturers like Bosch, Buderus, Nefit, Junkers and Sieger.

The code is written for the Espressif **ESP8266** microcontroller and supports a telnet console for real-time monitoring and configuration and customizable MQTT support for publishing the information to a home automation system such as Home Assistant or Domoticz.

####  Please reference the [Wiki](https://github.com/proddy/EMS-ESP/wiki) for further details and instructions on how to build and configure the firmware.

---

## Features

#### A web interface for easy configuration and real-time monitoring of the EMS bus

| ![web menu](https://github.com/proddy/EMS-ESP/raw/master/doc/web/system_status.PNG) | ![web menu](https://github.com/proddy/EMS-ESP/raw/master/doc/web/ems_dashboard.PNG) |
| -------------------------------------------------------------------------------- | -------------------------------------------------------------------------------- |

#### MQTT support for Home Assistant and Domoticz

![ha](https://github.com/proddy/EMS-ESP/raw/master/doc/home_assistant/ha.png)

#### Telnet for advanced configuration and verbose traffic logging

| ![telnet menu](https://github.com/proddy/EMS-ESP/raw/master/doc/telnet/telnet_menu.jpg) | ![telnet menu](https://github.com/proddy/EMS-ESP/raw/master/doc/telnet/telnet_stats.PNG) |
| --------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------- |

---

## Current list of supported EMS devices

### Thermostats:

* Buderus RC10, RC20, RC20F, RC30, R35, RC300, RC310, RC3000
* Buderus Logamatic TC100 (read-only)
* Nefit Moduline 100, 300, 400, 1010, 3000
* Nefit Moduline Easy (read-only)
* Bosch Easy, CW100 (read-only)
* Junkers FR10, FR100, FR110, FW100, FW120
* Sieger ES73

### Boilers:

* Buderus GBx72, GB162, GB152, Logamax U122, Logamax plus/GB192, Logano
* Bosch Condens 2500
* Worcester-Bosch Greenstar 550CDi
* Worcester Bosch Greenstar 24i
* Junkers Cerapur, Heatronic 3 boilers
* Nefit Proline, Trendline, Topline, Enviline, Smartline
* Sieger BK15 Boiler

### Solar Modules:

* Buderus SM10, SM50, SM100 Solar Module
* Junkers ISM1 Solar Module

### Mixing Modules:

* Buderus MM10, MM50, MM100 Mixer Module

### Heat Pump Modules:

* Buderus HeatPump Module

### Other devices:

* Generic Buderus MC10 Module
* Buderus WM10 Switch Module
* Buderus RFM20 Receiver
* Buderus BC10, BC25 Base Controller
* Buderus Web Gateway KM200
* Nefit Moduline Easy Connect
* Bosch Easy Connect
* EMS-OT OpenTherm converter
* Junkers Controller

## Compatible with EMS Gateway

Using BBQKees' [EMS Gateway](https://shop.hotgoodies.nl/ems/) board with integrated Wemos D1 ESP8266:

| ![on boiler](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/on-boiler.jpg) | ![kit](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/ems-kit-2.jpg) | ![basic circuit](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/ems-board-white.jpg) |
| ------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------- |

