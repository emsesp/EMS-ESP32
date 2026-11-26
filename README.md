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

[![GitHub stars](https://img.shields.io/github/stars/emsesp/EMS-ESP32.svg?style=social&label=Star)](https://github.com/emsesp/EMS-ESP32/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/emsesp/EMS-ESP32.svg?style=social&label=Fork)](https://github.com/emsesp/EMS-ESP32/network)
[![ko-fi](https://img.shields.io/badge/Ko--fi-Support%20EMS--ESP-ff5e5b?logo=ko-fi&logoColor=white)](https://ko-fi.com/emsesp)

**EMS-ESP** is an open-source firmware for the Espressif ESP32 microcontroller to communicate with **EMS** (Energy Management System) compatible equipment from manufacturers such as Bosch, Buderus, Nefit, Junkers, Worcester, Sieger, elm.leblanc and iVT.

It requires a small circuit to interface with the EMS bus which can be purchased from <https://bbqkees-electronics.nl>. These gateways are tested thoroughly and certified to work with EMS-ESP.


## 📦&nbsp; **Key Features**

- Compatible with EMS, EMS+, EMS2, EMS Plus, Logamatic EMS, Junkers 2-wire, Heatronic 3 and 4
- Supports over 120 EMS devices, including thermostats, boilers, heat pumps, mixing units, solar modules, connect modules, ventilation units and switches
- Add your own temperature and analog sensors on the ESP32's GPIO pins
- A responsive, multi-user web interface for desktop, tablet and phone to monitor live data and change settings
- Native integration with Home Assistant (MQTT Discovery), Domoticz, openHAB, Modbus and a REST API
- Reports EMS devices that go offline, so integrations can mark their entities as unavailable
- Localized in 13 languages, and any device or sensor can be renamed
- Simulates remote room thermostats
- Add your own custom EMS entities, Commands and user-built modules
- A Scheduler to run tasks on a timer or when values change, and Notifications to alert you to important events
- Back up and restore your heating settings, such as set temperatures, heating curves and switch times
- Secure by design: hashed passwords, API keys with read or read-and-write access, MQTT over TLS and limits on writes to the boiler
- Signed firmware updates from the Web UI with automatic rollback, plus a Recovery mode for when things go wrong
- A console over Serial/USB, Telnet or the Web UI for advanced operations and detailed monitoring

## 🚀&nbsp; **Installing**

Head over to the [Installation Guide](https://emsesp.org/Installing) section of the documentation for instructions on how to install EMS-ESP.

## 📋&nbsp; **Documentation**

Visit [emsesp.org](https://emsesp.org) for more details on how to setup and configure EMS-ESP. You'll also find more a collection of example configurations, Frequently Asked Questions and Troubleshooting tips.

## 💬&nbsp; **Getting Support**

To chat with the community reach out on our [Discord Server](https://discord.gg/GP9DPSgeJq).

If you find an issue or have a request, see the [Getting Support](https://emsesp.org/Support/) section of the documentation. Note if you are using a non-BBQKees EMS gateway, you may need to contact the manufacturer for support.

## 🎥&nbsp; **Live Demo**

To see a live demo go to [demo.emsesp.org](https://demo.emsesp.org). Pick a language and use any username and password to log in. Note what you're seeing is static example data so not all features are operational.

## 💖&nbsp; **Contributors**

EMS-ESP is a project originally created by [proddy](https://github.com/proddy) and maintained by the ems-esp community.

If you like **EMS-ESP**, please give it a ✨ on GitHub, or even better fork it and contribute. This is an open-source project maintained by volunteers, and if it is useful to you, please consider [supporting it on Ko-fi](https://ko-fi.com/emsesp). One-off tips and monthly support both help keep the project going.

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
