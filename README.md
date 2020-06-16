# EMS-ESP version 2.0 (alpha)

*Warning: this is still in development and not all features have been fully tested.*

---

## **New Features in v2**

- A new web interface using React and TypeScript that's now secure as each URL endpoint is protected by issuing a JWT which is then sent using Bearer Authentication. Using a Captive Portal in AP mode or connecting to a local wifi network.
  
- A new console. Like in version 1.9 it works with both Serial and Telnet but with a rich set of commands and more intuitive with behavior similar to a Linux-style shell. It supports multiple connections and commands that alter the settings or interact directly with EMS devices are secure behind an admin password. A full list of commands is below, here are the key ones:
    * `help` lists the commands and keywords
    * some commands take you into a new context, a bit like a sub-menu. e.g. `system`, `mqtt`, `thermostat`. Use `help` to show which commands this context has and `exit` or CTRL-D to get back to the root.
    * To change a setting use the `set` command. Typing `set` shows the current settings.
    * `show` shows the data specific to the which context you're in.
    * `su` to switch to Admin which enables more commands such as most of the `set` commands. The default password is "neo" which can be changed with `passwd` from the system menu. When in Admin mode the command prompt switches from `$` to `#`.
    * `log` sets the logging level. `log off` disables logging. Use `log debug` for debugging commands and actions.
    * `watch` from the ems context, will output the incoming Rx telegrams to the console. You can also show these in its 'raw' data format and also watch a particular ID.
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
- Uploading directly via USB: `esptool.py -p <COM PORT> -b 921600 write_flash 0x00000 <firmware.bin>`
- Uploading over WiFi: `espota.py --debug --progress --port 8266 --auth neo -i <IP address> -f <firmware.bin>`

## **Setting EMS-ESP up for the first time**

 - connect the ESP8266/ESP32 via USB. If you're coming from version 1.9 all the settings will be erased. If you want a full clean system do a `esptool.py erase_flash` first
 - enter USB via the serial/com/tty port with baud 115200, with something like putty
 - type `su` and use the default password *neo*
 - type `system` to go to the system sub-menu/context
 - type `help` if you want see the available commands
 - type `set` to see the current settings
 - use `set wifi ssid` to change the SSID and password. Remember TAB auto-completes commands if you're not sure of the next parameter.
 - CTRL-D to get back to the root
 - use `mqtt` to enter the MQTT context. Using the same approach, use `help` and `set` command to set the MQTT IP and any credentials. Then CTRL-D to get back to the root.
 - reboot and next time use the Telnet via WiFi to connect as the serial mode will be disabled. The EMS-ESP should have a solid LED or a slow pulsing LED.
 - Telnet to the IP to get back into the console.
 - If Tx is not working you will see errors show up in red. Use `ems` to change to the EMS context and `set` to see the default values. Try different `tx_mode` settings. 1 is default for EMS 1.0. EMS 2.0 is 2 and HT3/Bosch/Junkers is 3. Then a `refresh` to see if it's working. There is no need to reboot.
 - `show` to show all values.

## **List of console commands**

```
common commands available in all contexts:
  exit
  help
  log [level]
  su
  set
  show

(top root level)
	refresh
	show version
	ems (enters a context)
	mqtt (enters a context)
	system (enters a context)
	boiler (enters a context)
	thermostat (enters a context)

ems
    scan devices [deep]
	send telegram <"XX XX ...">
	set bus_id <device ID>
	set read_only <on | off>
	set tx_mode <n>
	refresh
	show devices
	show emsbus
	show values
	watch <on | off | raw> [ID]

mqtt
	publish
	reconnect
	set base <name>
	set enabled <on | off>
	set heartbeat <on | off>
	set ip <IP address>
	set format <single | nested | ha>
	set password
	set publish_time <seconds>
	set qos <n>
	set username [name]

system
	format
	passwd
	restart
	set syslog host [IP address]
	set syslog level [level]
	set syslog mark [seconds]
	set wifi hostname <name>
	set wifi password
	set wifi ssid <name>
	wifi disconnect
	wifi reconnect
	wifi scan

boiler
	change comfort <hot |eco | intelligent>
	change flowtemp <degrees>
	change wwactive <on | off>
	change wwcirculation <on | off>
	change wwonetime <on | off>
	change wwtemp <degrees>
	read <type ID>
	set shower alert <on | off>
	set shower timer <on | off>

thermostat
	change mode <mode> [heating circuit]
	change temp <degrees> [heating circuit]
	read <type ID>
	set master [device ID]
```
  
----------

### **Basic Design Principles**

- The core services like telnet, logging and shell are based off the libraries from @nomis. I also adopted his general design pattens such as making everything as asynchronous as possible so that no one operation should starve another operation of it's time to execute (https://isocpp.org/wiki/faq/ctors#static-init-order).
- All EMS devices (e.g. boiler, thermostat, solar modules, mixing units etc) are derived from a factory base class and each class handles its own registering of telegram and mqtt handlers. This makes the EMS device code easier to manage and we can extend with new telegrams types and features.

### **Things to tidy up in code later**

- Replace vectors of class objects with shared pointers and use emplace_back since it instantiates during construction. It may have a performance gain.
- Make more use of comparison operators in the Telegram class e.g. the compare like "friend inline bool operator==(const Telegram & lhs, const Telegram & rhs)"
- Add real unit tests using platformio's test bed (https://docs.platformio.org/en/latest/plus/pio-remote.html)
- See if it's easier to use timers instead of millis() based timers, using https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/BlinkPolledTimeout/BlinkPolledTimeout.ino

