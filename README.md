<div align="center">
  <p align="center">
    <a href="#">
      <img src="https://raw.githubusercontent.com/emsesp/EMS-ESP32/dev/media/favicon/android-chrome-512x512.png" height="100px" />
    </a>
  </p>
</div>

<h1 align="center">EMS-ESP</h1>

<p align="center">
  <a href="https://emsesp.org">
    <img src="https://img.shields.io/badge/Website-0077b5?style=for-the-badge&logo=googlehome&logoColor=white" alt="Website" />
  </a>
  <a href="https://github.com/emsesp/EMS-ESP32/blob/dev/CONTRIBUTING.md">
    <img src="https://img.shields.io/badge/Contribute-ff4785?style=for-the-badge&logo=git&logoColor=white" alt="Contribute" />
  </a>
  <a href="https://emsesp.org">
    <img src="https://img.shields.io/badge/Documentation-0077b5?style=for-the-badge&logo=googledocs&logoColor=white" alt="Guides" />
  </a>
  <a href="https://discord.gg/GP9DPSgeJq">
    <img src="https://img.shields.io/badge/Discord-7289da?style=for-the-badge&logo=discord&logoColor=white" alt="Discord" />
  </a>
  <a href="https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md">
    <img src="https://img.shields.io/badge/Changelog-6c5ce7?style=for-the-badge&logo=git&logoColor=white" alt="Changelog" />
  </a>
</p>

[![version](https://img.shields.io/github/release/emsesp/EMS-ESP32.svg?label=Latest%20Release)](https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md)
[![release-date](https://img.shields.io/github/release-date/emsesp/EMS-ESP32.svg?label=Released)](https://github.com/emsesp/EMS-ESP32/commits/main)
[![license](https://img.shields.io/github/license/emsesp/EMS-ESP32.svg)](LICENSE)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=emsesp_EMS-ESP32&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=emsesp_EMS-ESP32)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/9441142f49424ef891e8f5251866ee6b)](https://app.codacy.com/gh/emsesp/EMS-ESP32/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![downloads](https://img.shields.io/github/downloads/emsesp/EMS-ESP32/total.svg)](https://github.com/emsesp/EMS-ESP32/releases)
[![chat](https://img.shields.io/discord/816637840644505620.svg?style=flat-square&color=blueviolet)](https://discord.gg/GP9DPSgeJq)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/emsesp/EMS-ESP32)

[![GitHub stars](https://img.shields.io/github/stars/emsesp/EMS-ESP32.svg?style=social&label=Star)](https://github.com/emsesp/EMS-ESP32/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/emsesp/EMS-ESP32.svg?style=social&label=Fork)](https://github.com/emsesp/EMS-ESP32/network)
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/paypalme/prderbyshire/2)

**EMS-ESP** is an open-source firmware for the Espressif ESP32 microcontroller to communicate with **EMS** (Energy Management System) compatible equipment from manufacturers such as Bosch, Buderus, Nefit, Junkers, Worcester, Sieger, elm.leblanc and iVT.

It requires a small circuit to interface with the EMS bus which can be purchased from <https://bbqkees-electronics.nl>. These gateways are tested thoroughly and certified to work with EMS-ESP.


## 📦&nbsp; **Key Features**

- Compatible with EMS, EMS+, EMS2, EMS Plus, Logamatic EMS, Junkers 2-wire, Heatronic 3 and 4
- Supporting over 120 different EMS compatible devices such as thermostats, boilers, heat pumps, mixing units, solar modules, connect modules, ventilation units, switches and more
- Easy to add external Temperature and Analog sensors that are attached to GPIO pins on the ESP32 board
- A multi-user, multi-language web interface to change settings and monitor incoming data
- A simple to use console, accessible via Serial/USB or Telnet for advanced operations and detailed monitoring
- Native integration with Home Assistant, Domoticz, openHAB and Modbus
- Easy setup and install with automatic updates
- Simulation of remote thermostats
- Localized in 11 languages, and customizable to rename any device or sensor
- Extendable by adding own custom EMS entities
- Expandable via adding user-built external modules
- A powerful Scheduler to automate tasks and trigger events based data changes
- A Notification service to alert you of important events

## 🚀&nbsp; **Installing**

Head over to the [Installation Guide](https://emsesp.org/Installing) section of the documentation for instructions on how to install EMS-ESP.

## 📋&nbsp; **Documentation**

Visit [emsesp.org](https://emsesp.org) for more details on how to setup and configure EMS-ESP. You'll also find more a collection of example configuarations, Frequently Asked Questions and Troubleshooting tips.

## 💬&nbsp; **Getting Support**

To chat with the community reach out on our [Discord Server](https://discord.gg/GP9DPSgeJq).

If you find an issue or have a request, see the [Getting Support](https://emsesp.org/Support/) section of the documentation. Note if you are using a non-BBQKees EMS gateway, you may need to contact the manufacturer for support.

## 🎥&nbsp; **Live Demo**

To see a live demo go to [demo.emsesp.org](https://demo.emsesp.org). Pick a language and use any username and password to log in. Note whast you're seeing is static example data so not all features are operational.

## 💖&nbsp; **Contributors**

EMS-ESP is a project originally created by [proddy](https://github.com/proddy) and maintained by the ems-esp community.

If you like **EMS-ESP**, please give it a ✨ on GitHub, or even better fork it and contribute. You can also offer a small donation. This is an open-source project maintained by volunteers, and your support is greatly appreciated.

## 📦&nbsp; **Building**

See the [Building the firmware](https://emsesp.org/Building) guide in the documentation for instructions on how to build EMS-ESP from this source code.

## 📢&nbsp; **Libraries used**

- [esp8266-react](https://github.com/rjwats/esp8266-react) originally by @rjwats for the core framework that provides the Web UI, which has been heavily modified
- [uuid-\*](https://github.com/nomis/mcu-uuid-console) from @nomis. The console, syslog, telnet and logging are based off these awesome open source libraries
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for all the JSON processing
- [espMqttClient](https://github.com/bertmelis/espMqttClient) for the MQTT client
- [ESPAsyncWebServer](https://github.com/ESP32Async/ESPAsyncWebServer) and [AsyncTCP](https://github.com/ESP32Async/AsyncTCP) for the Web server

## 📜&nbsp; **License**

This program is licensed under GPL-3.0-or-later

## **Screenshots**

### Web Interface

|                                    |                                  |
| ---------------------------------- | -------------------------------- |
| ![Web Settings](media/web_settings.png) | ![Web Status](media/web_status.png) |
| ![Web Devices](media/web_devices.png)  | ![Web MQTT](media/web_mqtt.png)   |
| ![Web Edit](media/web_edit.png)     | ![Web Log](media/web_log.png)    |

### Telnet Console

![Console](media/console0.png)

### Home Assistant

![Home Assistant](media/ha_lovelace.png)
