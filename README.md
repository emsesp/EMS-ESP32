# EMS-ESP-Boiler

EMS-ESP-Boiler is an controller running on an ESP8266 to communicate with EMS (Energy Management System) based Boilers from the Bosch range. This includes the Buderus and Nefit ranger of boilers, heaters and thermostats.

There are 3 parts to this project, first the design of the circuit, second the code to deploy to an ESP8266 based microcontroller and lastly settings for Home Assistant to monitor data and issue direct commands via MQTT.

[![version](https://img.shields.io/badge/version-1.0-brightgreen.svg)](CHANGELOG.md)
[![branch](https://img.shields.io/badge/branch-dev-orange.svg)](https://github.org/xoseperez/espurna/tree/dev/)
[![license](https://img.shields.io/github/license/xoseperez/espurna.svg)](LICENSE)

- [EMS-ESP-Boiler](#ems-esp-boiler)
  - [Introduction](#introduction)
  - [Supported boilers](#supported-boilers)
  - [Acknowledgments](#acknowledgments)
  - [ESP8266 device compatibility](#esp8266-device-compatibility)
  - [Getting Started](#getting-started)
  - [Debugging](#debugging)
  - [Building the Circuit](#building-the-circuit)
  - [Known Issues](#known-issues)
  - [To Do](#to-do)
  - [How the EMS works](#how-the-ems-works)
    - [IDs](#ids)
    - [1. EMS Polling](#1-ems-polling)
    - [2. EMS Broadcasting](#2-ems-broadcasting)
    - [3. EMS Sending](#3-ems-sending)
  - [The Code](#the-code)
    - [Supported EMS Types](#supported-ems-types)
    - [Customizing](#customizing)
    - [MQTT](#mqtt)
    - [Shower Logic](#shower-logic)
  - [Home Assistant Configuration](#home-assistant-configuration)
- [Building the firmware](#building-the-firmware)
    - [Using pre-built firmware's](#using-pre-built-firmwares)
    - [Using PlatformIO](#using-platformio)
    - [Using ESPurna](#using-espurna)
    - [Your comments and feedback](#your-comments-and-feedback)


## Introduction

My original intention for this home project with to build my own smart thermostat for my Nefit Trendline boiler and then have it controlled via [Home Assistant](https://www.home-assistant.io/) on my mobile phone. I had a few ESP32s and ESP8266s lying around from previous IoT projects and building a specific circuit to decode the EMS messages was a nice challenge into more complex electronic circuits. I then began adding new features such as timing how long the shower was running for and triggering an alarm (actually a shot of cold water!) after a certain duration.

## Supported boilers

Most Bosch branded boilers that support the Logamatic EMS (and EMS+) bus protocols work with this design. Which are Nefit, Buderus, Worcester and Junkers.

## Acknowledgments

First a big thanks and my appreciation to the following people for their own open source related projects:

 **susisstrolch** - Probably the first working version of the EMS bridge circuit I could find designed for the ESP8266. I borrowed Juergen's schematic and parts of his code logic. https://github.com/susisstrolch/EMS-ESP12

 **bbqkees** - Kees built a circuit and wrote some sample Arduino code to read from the EMS and push messages to Domoticz. His SMD board is also available to purchase from him directly. https://github.com/bbqkees/Nefit-Buderus-EMS-bus-Arduino-Domoticz

 **EMS Wiki** - A comprehensive reference for decoding the EMS telegrams, which I found not always to be 100% accurate. https://emswiki.thefischer.net/doku.php?id=wiki:ems:telegramme. It's in German but Google Translate can help there.


## ESP8266 device compatibility

I've tested the code and circuit with a few ESP8266 development boards such as the Wemos D1 Mini, Wemos D1 Mini Pro, Nodemcu0.9 and Nodemcu2 boards.


## Getting Started

1. Either build the circuit below or purchase a ready built board from bbqkees via his [GitHub](https://github.com/bbqkees/Nefit-Buderus-EMS-bus-Arduino-Domoticz) page or the [Domoticz forum](http://www.domoticz.com/forum/viewtopic.php?f=22&t=22079&start=20).
2. Get an ESP8266 dev board and connect the 2 EMS output lines from the boiler to the circuit and the Rx/Tx out to pins D7 and D8 on the ESP. The EMS connection can either be the 12-15V AC direct from the thermostat bus line or from the 3.5" Service Jack at the front. Again bbqkees has a nice explanation [here](https://github.com/bbqkees/Nefit-Buderus-EMS-bus-Arduino-Domoticz/tree/master/Documentation).
3. Optionally connect the three LEDs to show Rx and Tx traffic and Error codes to pins D1, D2, D3 respectively. I use 220 Ohm pull-down resistors. These pins are configurable in ``boiler.ino``. This is furtherr explained in the **code** section below.
3. Build and upload the firmware to the ESP8266 device. I used Platformio with Visual Studio. Do make sure you set the MQTT and WiFi credentials correctly and if you're not using MQTT leave the MQTT_IP blank. The firmware supports OTA too with the default hostname as 'boiler' (or 'boiler.' depending on your OS and how the mdns resolves hostnames).
4. Power the ESP either via USB or direct into the 5v vin pin from an external power 5V volts supply with min 400mA.
5. Attach the 3v3 out on the ESP8266 to the DC power line on the EMS circuit as indicated in the schematics.
6. When your ESP device has booted, telnet (port 23) to the IP. If using DHCP figure the IP out or use USB and the serial terminal on the COM port to see. If everything is working you should see the messages appear in the window as shown in the next section. I use a Telnet client that comes with Linux distro on Windows 10 in developer mode but you can also use [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) or any other software.

## Debugging

Use the telnet client to inform you of all activity and errors real-time. This is an example of the telnet output:

![Telnet](doc/telnet/telnet_example.JPG)

If you hit 'q' and Enter, it will toggle verbose logging showing you more detailed messages. I use ANSI colors with white text for info messages, green for well formatted telegram packages (which have validated CRC checks), red for corrupt packages and yellow for send responses.

![Telnet](doc/telnet/telnet_verbose.JPG)

To see the current values of the Boiler and its parameters type 's' and hit Enter. Watch out for unsuccessful telegram packets in the #CrcErrors line. 

![Telnet](doc/telnet/telnet_stats.JPG)

Commands can be issued directly to the EMS bus typing in a letter followed by an optional parameter and pressing Enter. Supported commands are:
* **r** to send a read command to all devices to fetch values. The 2nd parameter is the type. For example 'r 33' will request type UBAParameterWW and bring back the Warm Water temperatures (not the heating) from the Boiler
* **t** set the thermostat temperature to the given value
* **w** to adjust the temperature of the warm water from the boiler
* **a** to turn the warm water on and off
* **p** to toggle the Polling response on/off. It's not necessary to have Polling enabled to work
* **T** to toggle thermostat reading on/off
* **S** to toggle the Shower Timer functionality on/off

*Disclaimer: be careful when sending values to the boiler. If in doubt you can always reset the boiler to its original factory settings by following the instructions in the user guide. On my **Nefit Trendline HRC30** that is done by pressing the Home and Menu buttons simultaneously, selecting factory settings from the scroll menu and lastly pressing the Reset button.

## Building the Circuit

The EMS circuit is really all credit to the hard work many people have done before me, noticeably *susisstrolch* with his ESP8266 [version](https://github.com/susisstrolch/EMS-ESP8266_12-PCB/tree/newmaster/Schematics/EMS-ESP8266-12).

I've included a prototype boards you can build yourself on a breadboard. One part for only reading values from the Boiler and an extension with the write logic so you can also send commands.

We need the Rx/Tx of the ESP8266 for flashing, so the code in ``emsuart.cpp`` switches the UART pins to use RX1 and TX1 (GPIO13/D7 and GPIO15/D8 respectively). This also prevents any bogus stack data being sent to EMS bus when the ESP8266 decides to crash like after a Watch Dog Reset.

The breadboard layout was done using [DIY Layout Creator](https://github.com/bancika/diy-layout-creator) and sources files are included in this repo.

![Breadboard Circuit](doc/schematics/breadboard.png)

The schematic used (as designed by [susisstrolch](https://github.com/susisstrolch/EMS-ESP8266_12-PCB)):

![Schematic](doc/schematics/circuit.png)

*Optionally I've also added 2 polyfuses between the EMS and the two inductors*

And here's an early prototype circuit using a NodeMcu2 with the additional LEDs added and 5v buck converter to power the ESP8266 also from the EMS line.

![Breadboard](doc/schematics/breadboard_example.png)

And lastly if you don't fancy building the circuit [bbqkees](http://www.domoticz.com/forum/memberlist.php?mode=viewprofile&u=1736) can sell you one which looks like the photo below running on a Wemos D1 Mini:

![WemosD1](doc/schematics/wemos_kees.png)



## Known Issues

In the source code:

* Very infrequently an EMS write command is not sent, probably due to a collision somewhere in the UART between an incoming Rx and a Poll. The retries in the code fix that but it is annoying nevertheless and does need fixing.
* I've seen a few duplicate telegrams being processed. Again, it's harmless and not a big issue. But its still a bug.

In the circuit:

* Powering the circuit of the ESP's 3v3 line is very stable when there is a steady 5v going to VIN. There are stability issues and some noise when using a buck step-down converter to power the ESP from the EMS bus line (which is around 15V AC).

## To Do

Here's my top things I'm still working on:

* Make an ESPurna version. ESPurna is a lovely framework that takes care of the WiFi, MQTT, web server, telnet & debugging.
* Complete the ESP32 version. It's surprisingly a lot easier doing the UART code on an ESP32 with the ESP-IDF framework. The first beta version is working.
* Find a better way to control the 3-way valve to switch the warm water off quickly rather than deactivating the warm water heater.


## How the EMS works

Packages are sent to the EMS "bus" from the Boiler and any other compatible connected devices via serial TTL transmission. The protocol is 9600 baud, 8N1 (8 bytes, no parity, 1 stop bit). Each package is terminated with a break signal `<BRK>`, a 11-bit long low signal of zeros.

A package can be a single byte (see Polling below) or a string of 6 or more bytes making up an actual data telegram. A telegram is always in the format:

``[src] [dest] [type] [offset] [data] [crc] <BRK>``

I reference the first 4 bytes as the *header* in this document.

### IDs

Each device has a unique ID.

The Boiler has an ID of 0x08 (type MC10) and also referred to as the Bus Master or UBA.

My thermostat, which is a* Moduline 300* uses the RC20 protocol and has an ID 0x17. If you're using an RC30 or RC35 type thermostat such as the newer Moduline 300s or 400s use 0x10 and make adjustments in the code as appropriate. bbqkees did a nice write-up on his github page [here](https://github.com/bbqkees/Nefit-Buderus-EMS-bus-Arduino-Domoticz/blob/master/README.md).

Our circuit acts as a service key and thus uses an ID 0x0B. This ID is reserved for special devices intended for installation engineers for maintenance work. 

### 1. EMS Polling
The bus master (boiler) sends out a poll request every second by sending out a sequential list of all possible IDs as a single byte followed by the break signal. The ID always has its high 7th bit set so in the code we're looking for 1 byte messages matching the format `[dest|0x80] <BRK>`.

Any connected device can respond to a Polling call with an acknowledging by sending back a single byte with its own ID. In our case we would listen for a `[0x8B] <BRK>` (meaning us) and then send back `[0x0B] <BRK>` to say we're alive and ready.

Polling is also the trigger to start transmitting any packages queued for sending. It must be done within 200ms or the bus master will time out.

### 2. EMS Broadcasting

When a device is broadcasting to everyone there is no specific destination needed. `[dest]` is always 0x00.

The Boiler (ID 0x08) will send out these broadcast telegrams regularly:

Type | Description (see [here](https://emswiki.thefischer.net/doku.php?id=wiki:ems:telegramme)) | Data length (excluding header) | Frequency
--- | --- | --- | --- |
0x34 | UBAMonitorWWMessage | 19 bytes | 10 seconds
0x18 | UBAMonitorFast | 25 bytes | 10 seconds
0x19 | UBAMonitorSlow | 22 bytes | every minute
0x1c | UBAWartungsmelding | 27 bytes | every minute
0x2a | status, specific to boiler type | - | 10 seconds

And a thermostat (ID 0x17 for a RC20) would broadcast these messages regularly:

Type | Description 
--- | --- | 
0x06 | time on thermostat Y,M,H,D,M,S,wd

Refer to the code in `ems.cpp` for further explanation on how to parse these message types and also reference the EMS Wiki.

### 3. EMS Sending

Telegram packets can only be sent after the Boiler sends a poll to the sending device. The response can be a read command to request data or a write command to send data. At the end of the transmission a poll response is sent from the client (`<ID> <BRK>`) to say we're all done and free up the bus for other clients.

When doing a request to read data the `[src]` is our device (0x0B) and the `[dest]` has it's 7-bit set. Say we were requesting data from the thermostat we would use `[dest] = 0x97` since RC20 has an ID of 0x17.

When doing a write request, the 7th bit is masked in the `[dest]`. After this write request the destination device will send either a single byte 0x01 for success or 0x04 for fail.

Every telegram sent is echo'd back to Rx.

 ## The Code

 *Disclaimer*: This code here is really for reference only, I don't expect anyone to use as is since it's highly tailored to my environment and my needs. Most of the code however is self explanatory with comments here and there. If you wish to make some changes start with the `defines` and `const` sections at the top of `boiler.ino`.

 The code is built on the Arduino framework and is dependent on these external libraries:

 * Time http://playground.arduino.cc/code/time
 * PubSubClient http://pubsubclient.knolleary.net
 * ArduinoJson https://github.com/bblanchon/ArduinoJson
 * Ticker https://github.com/sstaub/Ticker

 `src\emsuart.cpp` handles the low level UART read and write logic. You shouldn't need to touch this. All receive commands from the EMS bus are handled asynchronously using a circular buffer via an interrupt. A separate function processes the buffer and extracts the telegrams. Since we don't send too many write commands this is done sequentially. I couldn't use the standard Arduino Serial implementation because of the 11-bit break signal causes a frame-error which gets ignored. 
 
 `src\ems.cpp` is the logic to read the EMS packets (telegrams), validates them and process them based on the type.

 `src\boiler.ino` is the Arduino code for the ESP8266 that kicks it all off. This is where we have specific logic such as the code to monitor and alert on the Shower timer and light up the LEDs.

 `lib\ESPHelper` is my customized version of [ESPHelper](https://github.com/ItKindaWorks/ESPHelper) with added Telnet support and some other minor tweaking.

 ### Supported EMS Types

`ems.cpp` defines callback functions that handle all the broadcast types plus these extra types:

Device |  Type | Description | What
--- | --- | --- | --- |
Boiler (0x08) | 0x33 | UBAParameterWW | selected & desired warm water temp
Boiler (0x08) | 0x14 | UBATotalUptimeMessage | 
Boiler (0x08) | 0x15 | UBAMaintenanceSettingsMessage | 
Boiler (0x08) | 0x16 | UBAParametersMessage | 
Thermostat (0x17) | 0xA8 | RC20Temperature | setting temperature and operating modes
Thermostat (0x17) | 0xA3 | RCOutdoorTempMessage |  
Thermostat (0x17) | 0x91 | RC20StatusMessage | set & current room temperatures

 ### Customizing

 Most of the changes will be done in `boiler.ino` and `ems.cpp`. 
 * To add new handlers for data types, create a callback function and add to the `EMS_Types` at the top of the file `ems.cpp` and the DEFINES in `ems.h`
 * To change your thermostat type set `EMS_ID_THERMOSTAT` in `ems.h`. The default is 0x17 for an RC20.
 * The DEFINES `BOILER_THERMOSTAT_ENABLED`, `BOILER_SHOWER_ENABLED` and `BOILER_SHOWER_TIMER` enabled the thermostat logic, the shower logic and the shower timer alert logic respectively. 1 is on and 0 is off.

### MQTT

When the ESP8266 boots it will send a start signal via MQTT. This is picked up by Home Assistant and sends a notification informing me that the device has booted. Useful for knowing when the ESP gets reset.

I'm using the standard PubSubClient client so make sure you set `-DMQTT_MAX_PACKET_SIZE=300` as the default package size is 128 and our JSON messages are around 220 bytes.

I run Mosquitto on my Raspberry PI 3 as the MQTT broker.

The boiler data is collected and sent as a single JSON object to MQTT TOPIC `home/boiler/boiler_data`. Example payload:

`{"wWCurTmp":"43.0","wWHeat":"on","curFlowTemp":"51.7","retTemp":"48.0","burnGas":"off","heatPmp":"off","fanWork":"off","ignWork":"off","wWCirc":"off","selBurnPow":"0","curBurnPow":"0","sysPress":"1.6","boilTemp":"54.7","pumpMod":"4"}`
home/boiler/boiler_data

The temperature values of the thermostat as two separate topics `home/boiler/thermostat_currtemp` and `home/boiler/thermostat_seltemp`

Values sent from HA to set the temperature come in via the subscribed topic `home/boiler/thermostat_temp`

These topics can be configured in the `TOPIC_*` defines in `boiler.ino`. Make sure you change the HA configuration too to match.

### Shower Logic

Checking whether the shower is running was tricky. We know when the warm water is on and being heated but need to distinguish between the central heating, shower, hot tap and bath. I found via trial and error the Selected Burner Max Power is between 80% and 115% when the shower is running and fixed at 75% if the central heating is on. Furthermore the Selected Flow Impulsion is 80 C for the heating.

There is other logic in the code to compensate for ramp up and whether the shower is turned off and back on again quickly within a 10 second window.

## Home Assistant Configuration

This is what my HA configuration looks like:

**configuration.yaml**

    ...
    automation: !include automations.yaml
    input_number: !include input_number.yaml
    group: !include groups.yaml
    sensor: !include sensors.yaml


**sensors.yaml**

    - platform: mqtt
      state_topic: 'home/boiler/thermostat_currtemp'
      name: 'Boiler Thermostat Current Temperature'
      unit_of_measurement: '°C'

    - platform: mqtt
      state_topic: 'home/boiler/thermostat_seltemp'
      name: 'Boiler Thermostat Set Temperature'
      unit_of_measurement: '°C'

    - platform: mqtt
      state_topic: 'home/boiler/showertime'
      name: 'Last shower duration'
      force_update: true

    - platform: template
      sensors:
        showertime_time:
          value_template: '{{ as_timestamp(states.sensor.last_shower_duration.last_updated) | int | timestamp_custom("%-I:%M %P on %a %-d %b") }}'

    - platform: mqtt
      state_topic: 'home/boiler/boiler_data'
      name: 'Warm Water current temperature'
      unit_of_measurement: '°C'
      value_template: '{{ value_json.wWCurTmp }}'

      ..etc..etc 

**automations.yaml**

    - id: thermostat_temp
      alias: 'Adjust Thermostat Temperature'
      trigger:
        platform: state
        entity_id: input_number.thermostat_temp
      action:
        service: mqtt.publish
        data_template:
          topic: 'home/boiler/thermostat_temp'
          payload: >
            {{ states.input_number.thermostat_temp.state }}

    - id: boiler_shower
      alias: Alert shower time
      trigger:
        platform: mqtt
        topic: home/boiler/showertime
      action:
        - service: notify.boiler_notify
          data_template:
            title: "Shower finished!"
            message: 'Shower duration was {{trigger.payload}} at {{states.sensor.time.state}}'

**input_number.yaml**

    thermostat_temp:
      name: Set thermostat temperature
      icon: mdi:temperature-celsius
      min: 10
      max: 25
      step: 0.5
      unit_of_measurement: "°C" 
      mode: slider

**groups.yaml**

    boiler:
    name: Boiler
    view: no
    entities:
       - sensor.boiler_boottime
       - sensor.warm_water_current_temperature
       ..etc..etc..

    shower:
    name: Shower
    view: no
    entities:
       - sensor.last_shower_duration
       - sensor.showertime_time

    thermostat:
      name: Thermostat
      view: no
      entities:
        - sensor.thermostat_current_temperature
        - sensor.thermostat_set_temperature
        - input_number.thermostat_temp

And in Home Assistant looks like:

![Home Assistant panel)](doc/ha/ha.png) 

![Home Assistant iPhone notify)](doc/ha/ha_notify.jpg) 


# Building the firmware

### Using pre-built firmware's

In the `/firmware` folder, if there are pre-built versions you can upload using esptool (https://github.com/espressif/esptool) bootloader. On Windows, follow these instructions:

1. Check if you have python 2.7 installed. If not [download it](https://www.python.org/downloads/) and make sure you add Python to the windows PATH so it'll recognize .py files.
2. Install the ESPTool by running `pip install esptool` from a command prompt.
3. Connect the ESP via USB, figure out the COM port.
4. run `esptool.py -p <com> write_flash 0x00000 <firmware>` where firmware is the .bin file and \<com\> is the com port, e.g. COM3

### Using PlatformIO

There are two ways to compile and build the firmware yourself.

The first method is a standalone version which uses a modified version of [ESPHelper](https://github.com/ItKindaWorks/ESPHelper) for the WiFi, OTA and MQTT handling with the Telnet server code.

To compile, using PlatformIO create a project and modify your `platformio.ini` to include these build flags:

`WIFI_SSID, WIFI_PASSWORD, MQTT_IP, MQTT_USER, MQTT_PASS`

If you're not using MQTT keep MQTT_IP empty (`MQTT_IP=""`)

Here's an example part from `platformio.ini`:

```
[env:nodemcuv2]
board = nodemcuv2
platform = espressif8266
framework = arduino
lib_deps =
  Time
  PubSubClient
  ArduinoJson
  Ticker
upload_speed = 921600
build_flags = '-DWIFI_SSID="<my_ssid>"' '-DWIFI_PASSWORD="<my_password>"' '-DMQTT_IP="<broker_ip>"' '-DMQTT_USER="<broker_username>"' '-DMQTT_PASS="<broker_password>"' -DMQTT_MAX_PACKET_SIZE=300
; comment out next line if using USB and not OTA
upload_port = "boiler."
```
	
### Using ESPurna

*Note: This is still work in progress. The ESPurna code for the HTML config is still to be added.* 

[ESPurna](https://github.com/xoseperez/espurna/wiki) is framework that handles most of the tedious tasks of building IoT devices so you can focus on the functionality you need.

If you're using Windows follow these steps. We'll be using the free Visual Studio Code and PlatformIO. Similar steps also work on Linux distributions.

- First download Git: https://git-scm.com/download/win (install using the default settings)
- Visual Studio Code (VSC): https://code.visualstudio.com/docs/?dv=win
- Install node.js and npm (LTS version): https://nodejs.org/en/download

restart your PC to pick up the new PATH settings and start Visual Studio Code. It should now detect Git. Now go and search for and install these following VSC extensions:

- PlatformIO IDE
- GitLens

and hit **reload** to activate them all.

Next download espurna by cloning the git repository from https://github.com/xoseperez/espurna.git. Either from a terminal using 'git clone' or the GUI interface. From VSC open the folder ``espurna\code``
 - open a terminal window (ctrl-`) 
 - Install the node modules: ``npm install --only=dev``
 - Build the web interface:  ``node node_modules/gulp/bin/gulp.js``

PlatformIO should detect and set some things up for you. Build and Deploy as you normally would in PlatformIO.
 	
If you run into issues refer to official ESPurnas setup instructions [here](https://github.com/xoseperez/espurna/wiki/Build-and-update-from-Visual-Studio-Code-using-PlatformIO).

Next copy the files custom.h, index.html, boiler.ino and the esp*.cpp/h files from the espurna directory to the code directory and build.
	
### Your comments and feedback

Any comments or suggestions are very welcome. You can contact me at **dev** at **derbyshire** dot **nl** or creating a GitHub issue.	

