# ![logo](media/EMS-ESP_logo_dark.png)

**EMS-ESP** is an open-source firmware for the Espressif ESP8266 and ESP32 microcontroller that communicates with **EMS** (Energy Management System) based equipment from manufacturers like Bosch, Buderus, Nefit, Junkers, Worcester and Sieger.

This project is the specifically for the ESP32. Compared with the previous ESP8266 (version 2) release it has the following enhancements:

- Ethernet Support
- Pre-configured circuit board layouts
- Supports writing EMS values directly from within Web UI
- Mock API server for faster offline development and testing
- Improved API and MQTT commands
- Improvements to Dallas temperature sensors
- Embedded log tracing in the Web UI

[![version](https://img.shields.io/github/release/emsesp/EMS-ESP32.svg?label=Latest%20Release)](https://github.com/emsesp/EMS-ESP32/blob/main/CHANGELOG.md)
[![release-date](https://img.shields.io/github/release-date/emsesp/EMS-ESP32.svg?label=Released)](https://github.com/emsesp/EMS-ESP32/commits/main)
[![license](https://img.shields.io/github/license/emsesp/EMS-ESP32.svg)](LICENSE)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=emsesp_EMS-ESP32&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=emsesp_EMS-ESP32)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/9441142f49424ef891e8f5251866ee6b)](https://www.codacy.com/gh/emsesp/EMS-ESP32/dashboard?utm_source=github.com&utm_medium=referral&utm_content=emsesp/EMS-ESP32&utm_campaign=Badge_Grade)
[![downloads](https://img.shields.io/github/downloads/emsesp/EMS-ESP32/total.svg)](https://github.com/emsesp/EMS-ESP32/releases)
[![chat](https://img.shields.io/discord/816637840644505620.svg?style=flat-square&color=blueviolet)](https://discord.gg/3J3GgnzpyT)

If you like **EMS-ESP**, please give it a star, or fork it and contribute!

[![GitHub stars](https://img.shields.io/github/stars/emsesp/EMS-ESP32.svg?style=social&label=Star)](https://github.com/emsesp/EMS-ESP32/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/emsesp/EMS-ESP32.svg?style=social&label=Fork)](https://github.com/emsesp/EMS-ES32P/network)
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/paypalme/prderbyshire/2)

Note, EMS-ESP requires a small hardware circuit that can convert the EMS bus data to be read by the microcontroller. These can be ordered at <https://bbqkees-electronics.nl> or contact the contributors that can provide the schematic and designs.

<img src="media/gateway-integration.jpg" width=40%>

---

# **Features**

- A multi-user secure web interface to change settings and monitor incoming data
- A console, accessible via Serial and Telnet for more advanced monitoring
- Native support for Home Assistant and Domoticz via [MQTT Discovery](https://www.home-assistant.io/docs/mqtt/discovery/)
- Can run standalone as an independent WiFi Access Point or join an existing WiFi network
- Easy first-time configuration via a web Captive Portal
- Support for more than [100 EMS devices](https://emsesp.github.io/docs/#/Supported-EMS-Devices) (boilers, thermostats, solar modules, mixer modules, heat pumps, gateways)

## **Demo**

See a demo [here](https://ems-esp.derbyshire.nl). Log in with any username/password.

# **Screenshots**

## Web Interface

|                                    |                                  |
| ---------------------------------- | -------------------------------- |
| <img src="media/web_settings.png"> | <img src="media/web_status.png"> |
| <img src="media/web_devices.png">  | <img src="media/web_mqtt.png">   |
| <img src="media/web_edit.png">     | <img src="media/web_log.png">    |

## Telnet Console

<img src="media/console.png" width=80% height=80%>

## In Home Assistant

<img src="media/ha_lovelace.png" width=80% height=80%>
  
# **Installing**

Refer to the [official documentation](https://emsesp.github.io/docs) to how to install the firmware and configure it. The documentation is being constantly updated as new features and settings are added.

You can choose to use an pre-built firmware image or compile the code yourself:

- [Uploading a pre-built firmware build](https://emsesp.github.io/docs/#/Uploading-firmware)
- [Building the firmware from source code and flashing manually](https://emsesp.github.io/docs/#/Building-firmware)

# **Support Information**

If you're looking for support on **EMS-ESP** there are some options available:

## Documentation

- [Official EMS-ESP Documentation](https://emsesp.github.io/docs): For information on how to build and upload the firmware
- [FAQ and Troubleshooting](https://emsesp.github.io/docs/#/Troubleshooting): For information on common problems and solutions. See also [BBQKees's wiki](https://bbqkees-electronics.nl/wiki/gateway/troubleshooting.html)

## Support Community

- [Discord Server](https://discord.gg/3J3GgnzpyT): For support, troubleshooting and general questions. You have better chances to get fast answers from members of the community
- [Search in Issues](https://github.com/emsesp/EMS-ESP32/issues): You might find an answer to your question by searching current or closed issues

## Developer's Community

- [Bug Report](https://github.com/emsesp/EMS-ESP32/issues/new?template=bug_report.md): For reporting Bugs
- [Feature Request](https://github.com/emsesp/EMS-ESP32/issues/new?template=feature_request.md): For requesting features/functions
- [Troubleshooting](https://github.com/emsesp/EMS-ESP32/issues/new?template=questions---troubleshooting.md): As a last resort, you can open new _Troubleshooting & Question_ issue on GitHub if the solution could not be found using the other channels. Just remember: the more info you provide the more chances you'll have to get an accurate answer

# **Contributors ✨**

EMS-ESP is a project originally created and owned by [proddy](https://github.com/proddy). Key contributors are:

<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center">
    <a href="https://github.com/MichaelDvP"><img src="https://avatars.githubusercontent.com/u/59284019?v=3?s=100" width="100px;" alt=""/><br /><sub><b>MichaelDvP</b></sub></a><br /></a> <a href="https://github.com/emsesp/EMS-ESP/commits?author=MichaelDvP" title="v2 Commits">v2</a>
    <a href="https://github.com/emsesp/EMS-ESP32/commits?author=MichaelDvP" title="v3 Commits">v3</a>
    </td>

  </tr>
</table>
<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

You can also contribute to EMS-ESP by

- providing Pull Requests (Features, Fixes, suggestions)
- testing new released features and report issues on your EMS equipment
- contributing to missing [Documentation](https://emsesp.github.io/docs)

# **Libraries used**

- [esp8266-react](https://github.com/rjwats/esp8266-react) by @rjwats for the framework that provides the core of the Web UI
- [uuid-\*](https://github.com/nomis/mcu-uuid-console) from @nomis. The console, syslog, telnet and logging are based off these open source libraries
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) for JSON
- [AsyncMqttClient](https://github.com/marvinroger/async-mqtt-client) for the MQTT client, with custom modifications from @bertmelis and @proddy
- ESPAsyncWebServer and AsyncTCP for the Web server and TCP backends, with custom modifications for performance

# **License**

This program is licensed under GPL-3.0
