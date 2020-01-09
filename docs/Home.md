<img style="margin: 10px 10px; float:right; width:20%" src="_media/boiler.svg" alt="EMS-ESP Logo"></img>

[![version](https://img.shields.io/github/release/proddy/EMS-ESP.svg?label=Latest%20Release)](https://github.com/proddy/EMS-ESP/blob/master/CHANGELOG.md)
[![release-date](https://img.shields.io/github/release-date/proddy/EMS-ESP.svg?label=Released)](https://github.com/proddy/EMS-ESP/commits/master)
[![license](https://img.shields.io/github/license/proddy/EMS-ESP.svg)](LICENSE)
[![travis](https://travis-ci.com/proddy/EMS-ESP.svg?branch=master)](https://travis-ci.com/proddy/EMS-ESP)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/b8880625bdf841d4adb2829732030887)](https://app.codacy.com/app/proddy/EMS-ESP?utm_source=github.com&utm_medium=referral&utm_content=proddy/EMS-ESP&utm_campaign=Badge_Grade_Settings)
[![downloads](https://img.shields.io/github/downloads/proddy/EMS-ESP/total.svg)](https://github.com/proddy/EMS-ESP/releases)

**EMS-ESP** is a project to build an electronic controller circuit using an Espressif ESP8266 microcontroller to communicate with EMS (Energy Management System) based Boilers and Thermostats from the Bosch range and compatibles such as Buderus, Nefit, Junkers etc.

Currently [this is the list](Supported-EMS-Devices) of EMS heating devices that have been tested. 

### What's New?

<!-- tabs:start -->

#### **Version 1.9.5 beta**
**January 6 2020**

Happy new year everyone! 1.9.5 has been in development for a while now but it's almost ready with a few new features and many bug fixes and improvements such as:

- Changing heating settings on Junkers thermostats
- Supporting multiple thermostat heat controllers
- MM10 Mixer support
- SM200 Solar module support

Thanks to everyone for providing feedback.

Also make sure you check out the full [ChangeLog](https://github.com/proddy/EMS-ESP/blob/dev/CHANGELOG.md).

Paul.

#### **Version 2.0**
**January 6 2020**

The next major version is in the works. Here are some of the changes:
- A re-write of the Web interface. Websocket authentication was problematic in many browsers. The new version moves to a cleaner React and TypeScript
- Tighter security in the Web and Telnet, using SSL/TLS and tokens.
- Improved Telnet console with different levels of admin control and multi-client.
- Refactoring the code to make it easier to read, using more C++11 features now in the Arduino core libraries
 
<!-- tabs:end -->

### Community Support
Go to [Gitter](https://gitter.im/EMS-ESP/community) for quick feedback, questions and troubleshooting.

### Acknowledgments

Kudos out to the following people who have open-sourced their work and helped me build this project:

 **susisstrolch** - One of the first working versions of the EMS bridge circuit I found designed for specifically for the ESP8266. I borrowed Juergen's [schematic](https://github.com/susisstrolch/EMS-ESP12) and parts of his code ideas for reading telegrams.

 **bbqkees** - Kees built a small SMD commercial grade circuit board which is available for purchase on his [website](https://bbqkees-electronics.nl/).

 **EMS Wiki** - A comprehensive [reference](https://emswiki.thefischer.net/doku.php?id=wiki:ems:telegramme) (in German) for the EMS bus which is a little outdated, not always 100% accurate and sadly no longer maintained.


## DISCLAIMER

!> This code and libraries were developed from information gathered on the internet and many hours of reverse engineering the communications between the EMS bus and thermostats. It is **not** based on any official documentation or supported libraries from Buderus/Junkers/Nefit (and associated companies) and therefore there are no guarantees whatsoever regarding the safety of your devices and/or their settings, or the accuracy of the information provided.
