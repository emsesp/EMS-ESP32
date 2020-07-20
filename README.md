# EMS-ESP version 2.0

## **Breaking changes**

- MQTT base has been removed. The hostname is only used.

## **New Features in v2**

- A new web interface using React and TypeScript that's now secure as each URL endpoint is protected by issuing a JWT which is then sent using Bearer Authentication. Using a Captive Portal in AP mode or connecting to a local wifi network.

<img src="media/web_settings.PNG" width=70% height=70%>
<img src="media/web_status.PNG" width=70% height=70%>
<img src="media/web_devices.PNG" width=70% height=70%>
<img src="media/web_mqtt.PNG" width=70% height=70%>

- A new console. Like in version 1.9 it works with both Serial and Telnet but with a more intuitive Linux shell like behavior. It supports multiple connections and has basic security to prevent any changes to EMS-ESP. A full list of commands is below, here are the key ones:
    * `help` lists the commands and keywords
    * some commands take you into a new context, a bit like a sub-menu. e.g. `system`, `thermostat`. Use `help` to show which commands this context has and `exit` or CTRL-D to get back to the root.
    * To change a setting use the `set` command. Typing `set` shows the current settings.
    * `show` shows the data specific to the which context you're in.
    * `su` to switch to Admin which enables more commands such as most of the `set` commands. The default password is "ems-esp-neo" which can be changed with `passwd` from the system menu. When in Admin mode the command prompt switches from `$` to `#`.
    * `log` sets the logging level. `log off` disables logging. Use `log debug` for debugging commands and actions.
    * `watch` will output the incoming Rx telegrams to the console. You can also show these in its 'raw' data format and also watch a particular ID.
    * CTRL-D to exit, CTRL-U to remove line, TAB to auto-complete 
  
- There is no "serial mode" anymore like with version 1.9. When the Wifi cannot connect to the SSID it will automatically enter a "safe" mode where the Serial console is activated automatically (baud 115200). Note Serial is always available on the ESP32 because it has multiple UARTs.

- The onboard LED behaves like in 1.9. A solid LED means good connection and EMS data is coming in. A slow pulse means either the WiFi or the EMS bus is not connected yet. A very fast pulse is when the system is booting up and configuring itself, which typically takes 5 seconds.

- Built to work with both EMS8266 and ESP32

- Extended MQTT to use MQTT discovery on Home Assistant.

- For debugging there is an offline mode where the code can be compiled and executed standalone without uploading to an ESP controller. Use `make` (based off GNUMakefile).

## **Uploading the firmware**

- If you're not using PlatformIO, use the command-line and Python. You can download Python from https://www.python.org/downloads/. Make sure you also get:
  - `esptool`, install using the command `pip install esptool`
  - and for OTA updates later, `espota` from https://github.com/esp8266/Arduino/blob/master/tools/espota.py

- Grab the latest firmware binary from https://github.com/proddy/EMS-ESP/releases/tag/travis-v2-build
- Uploading directly via USB.
  
  For ESP8266: `esptool.py -p <COM PORT> -b 921600 write_flash 0x00000 <firmware.bin>`
  
  For ESP32: `esptool.py --chip esp32 --port "COM6" --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 XX\.platformio\packages\framework-arduinoespressif32\tools\sdk\bin\bootloader_dio_40m.bin 0x8000 XX\.pio\build\esp32\partitions.bin 0xe000 XX\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin 0x10000  <firmware.bin>`
- Uploading over WiFi: `espota.py --debug --progress --port 8266 --auth ems-esp-neo -i <IP address> -f <firmware.bin>`

## **Setting EMS-ESP up for the first time**

 - Connect to the Access Point called ems-esp using the WPA password `ems-esp-neo`. When you see the captive portal sign-in with username `admin` and password `admin`. Set the WiFi credentials and restart the ESP.
 - When it connects to your wifi network you can use either the Web UI to further configure the other settings or using Telnet. If using the Telnet console refer to the full set of commands below. To change any settings you need to be admin which can be accessed via the `su` command. The password here is the same as the JWT secret from the Web UI, default `ems-esp-neo`.
 - First thing to check is if Tx is working. Use `show ems` and if you have a large number of Tx fails try changing the Tx Mode using `set tx_mode` as admin. Typical values are 1 for EMS1.0, 2 for EMS2.0/EMS+ and 3 for Heatronics.

## **List of console commands**

```
common commands available in all contexts:
  exit
  help
  log [level]
  watch <on | off | raw> [ID]
  su

(from the root)
	set
	refresh
	system (enters a context)
	boiler (enters a context)
	thermostat (enters a context)
	scan devices [deep]
	send telegram <"XX XX ...">
	set bus_id <device ID>
	set tx_mode <n>
	show
	show devices
	show ems
	show values

system
	set
	show
	show mqtt
	passwd
	restart
	set wifi hostname <name>
	set wifi password
	set wifi ssid <name>

boiler
	comfort <hot |eco | intelligent>
	flowtemp <degrees>
	wwactive <on | off>
	wwcirculation <on | off>
	wwonetime <on | off>
	wwtemp <degrees>
	read <type ID>

thermostat
	set
	set master [device ID]
	mode <mode> [heating circuit]
	temp <degrees> [heating circuit]
	read <type ID>
```
  
----------

### **Basic Design Principles**

- The core services like telnet, logging and shell are based off the libraries from @nomis. I also adopted his general design pattens such as making everything as asynchronous as possible so that no one operation should starve another operation of it's time to execute (https://isocpp.org/wiki/faq/ctors#static-init-order).
- All EMS devices (e.g. boiler, thermostat, solar modules, mixing units etc) are derived from a factory base class and each class handles its own registering of telegram and mqtt handlers. This makes the EMS device code easier to manage and we can extend with new telegrams types and features.

### **Things to tidy up in code later**

- Replace vectors of class objects with shared pointers and use emplace_back since it instantiates during construction. It may have a performance gain.
- Add real unit tests using platformio's [pio-remote](https://docs.platformio.org/en/latest/plus/pio-remote.html) test bed.
- See if it's easier to use timers instead of millis() based timers, using [polledTimeout](https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/BlinkPolledTimeout/BlinkPolledTimeout.ino).
- Port over to ESP-IDF. The Arduino SDK is showing its limitations

### **Features to add**

- Multi-language. German, Dutch, French
- Click on a device in the Web UI shows it's details
- Publish time can be customized per device (solar, mixing etc)
- add homeassistant mqtt discovery for Boiler as well

### **Customizing the Web UI**

The Web is based off Rick's awesome [esp8266-react](https://github.com/rjwats/esp8266-react/) framework. These are the files that are modified:

**`interface:`**
  * `.env` project name and project path to ems-esp
  * `.env.development` CORS URL
 
**`interface/public:`**
  * `app/manifest.json` ems-esp name
  * `index.html` ems-esp name
  * `app/icon.png` 256x256 PNG
  * `favicon.ico` replaced

**`interface/src:`**
  * `CustomMuiTheme.tsx` colors for dark mode
  * `interface/src/wifi/WifiSettingsController.tsx` rename esp8266-react

**`interface/src/project:`**
  * `ProjectRouting.tsx` removed demo, added paths to ems-esp/status, ems-esp/settings and *
  * `DemoProject.tsx` remove /demo/ and changed title, renamed to EMSESP.tsx
  * `ProjectMenu.tsx` title change, added /ems-esp/settings
  * `DemoInformation.tsx` removed file
  * `types.ts` add variables
  * added all custom files starting with EMSESP*

**`interface/src/mqtt:`**
  * `types.ts` added mqtt_fails
  * `MqttStatusForm.tsx` added MQTT Publish Errors
  * `MqttStatus.ts` added function mqttPublishHighlight
  * `MqttSettingsForm.tsx` added publish time, qos, format

**`interface/src/authentication:`**
  * `AuthenticationWrapper.tsx` commented out features.security because we added version
  * `AuthenticationContext.tsx` added version
  * `MqttSettingsForm.tsx` added publish time, qos, format

**`interface/src/components:`**
  * `MenuAppBar.tsx` added version to toolbar

**`interface/src/system:`**
  * `types.ts` added uptime and free_mem
  * `SystemStatusForm.tsx` added system uptime, % free mem

**`lib/framework`:**
  * `SystemStatus.h` added #include <LittleFS.h>, #include <uuid/log.h>, #include "../../src/system.h"
  * `SystemStatus.cpp` added LittleFS.info(fs_info); root["uptime"], root["free_mem"]
  * Commented out all `Serial.print`'s in all files
  * `MqttStatus.h` added #include "../../src/mqtt.h"
  * `MqttStatus.cpp` added root["mqtt_fails"]
  * `SecuritySettingsService.cpp` added version to the JWT payload
  * `SecuritySettingsService.h` #include "../../src/version.h"
  * `WiFiSettingsService.cpp` added WiFi.setOutputPower(20.0f)
  * `features.ini`: -D FT_NTP=0
  * `platformio.ini` using our own version
  * `factory_settings.ini` modified with `ems-esp-neo` as password and `ems-esp` everywhere else


 UI customization links:

  * icons: https://material-ui.com/components/material-icons/
  * colors: https://material-ui.com/customization/color/
  * tables: https://material-ui.com/components/tables/#dense-table


