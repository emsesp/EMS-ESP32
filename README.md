# EMS-ESP

EMS-ESP is a open-source system to communicate with **EMS** (Energy Management System) based boilers, thermostats and other modules from manufacturers like Bosch, Buderus, Nefit, Junkers and Sieger.

The code is writen for the Espressif **ESP8266** microcontroller and supports a telnet console for real-time monitoring and configuration and customizable MQTT support for publishing the information to a home automation system such as Home Assistant or Domoticz.

###  Please reference the [Wiki](https://github.com/proddy/EMS-ESP/wiki) for further details and instructions on how to build and configure the firmware.

---

**An example of the Home Assistant integration:**

![ha](https://github.com/proddy/EMS-ESP/raw/master/doc/home_assistant/ha.png)

**Using BBQKees' [EMS Gateway](https://shop.hotgoodies.nl/ems/) circuit:**

| ![on boiler](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/on-boiler.jpg) | ![kit](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/ems-kit-2.jpg) | ![basic circuit](https://github.com/proddy/EMS-ESP/raw/master/doc/ems%20gateway/ems-board-white.jpg) |
| - | - | - |

**Example of the EMS-ESP's telnet console:**

| ![telnet menu](https://github.com/proddy/EMS-ESP/raw/master/doc/telnet/telnet_menu.jpg) | ![telnet menu](https://github.com/proddy/EMS-ESP/raw/master/doc/telnet/telnet_stats.PNG) |
| - | - |

---

## The latest list of support EMS devices

### Thermostats:

* Sieger ES73
* RC10/Nefit Moduline 100
* RC20/Nefit Moduline 300
* RC20F
* RC30/Nefit Moduline 400
* RC35 (only a single HC)
* RC300/RC310
* Nefit Moduline 1010
* Junkers FR10
* TC100/Nefit Easy (read-only)
* Bosch Easy (read-only)
* Bosch CW100 (read-only)

### Boilers:

* Buderus GB172/Nefit Trendline
* Nefit Topline Compact/Buderus GB162
* Buderus Logamax U122
* Buderus Logamax plus/GB192
* Sieger BK15 Boiler/Nefit Smartline
* Bosch Condens 2500/Junkers Cerapur Comfort
* Nefit Proline

### Solar Modules:

* SM10 Solar Module
* SM100 Solar Module
* Junkers ISM1 Solar Module

### Other devices:

* MM10 Mixer Module
* MC10 Module
* WM10 Switch Module
* MM100 Mixing Module
* MM100 Mixing Module
* BC10/RFM20 Receiver
* BC10 Base Controller
* BC25 Base Controller
* Nefit Moduline Easy Connect
* Bosch Easy Connect
* EMS-OT OpenTherm converter
* Web Gateway KM200
* HeatPump Module
