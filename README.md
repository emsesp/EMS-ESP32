# EMS-ESP version 2.0 (alpha)

*Warning: this is still in early stages of it's development. Not all features have been fully tested. Use at your own risk!*

Note: Version 2.0 is not backward compatible with v1.0. The File system structure is different. When coming from version 1.9.x its best if you first erase the flash on the ESP and upload using USB. `esptool.py erase_flash` will clean the flash and `esptool.py -p COM6 -b 921600 write_flash 0x00000 firmware.bin` is an example of how to upload the firmware over USB.

---

## Major changes since version 1.9.x

### **Design & coding principles**

- The code can be built and run without an ESP microcontroller, which is useful when testing and simulating handling of the different telegrams and devices. Make sure you have GNU make and g++ installed and use 'make' to build the image and execute the file `emsesp` (on linux).
- I used C++11 containers where I could (std::string, std::deque, std::list, std::multimap etc).
- The core is based off the great libraries from @nomis' and I adopted his general design pattens such as making everything as asynchronous as possible so that no one operation should starve another operation of it's time to execute (https://isocpp.org/wiki/faq/ctors#static-init-order).
- All EMS devices (e.g. boiler, thermostat, solar modules etc) are derived from a factory base class and each class handles its own registering of telegram and mqtt handlers. This makes the EMS device code easier to manage and extend with new telegrams types and features.
- Built to work with both EMS8266 and ESP32.
- Extended MQTT to use MQTT discovery on Home Assistant, just for the thermostat for now.

### **Features**

- A web interface built using React and TypeScript to be secure and cross-browser compatible. Each restful endpoint is protected and issues a JWT which is then sent using Bearer Authentication. Implements a Web captive portal. On first installs EMS-ESP starts an Access Point where system settings can be configured. Note, this is still in a separate repo and pending a merge into this project.
  
- A new console. Like in version 1.9 it works with both Serial and Telnet but a lot more intuitive, behaving similar to a Linux-style shell. It supports multiple connections and any commands that effect the behavior of EMS-ESP are secure behind an admin password. You can use TAB to auto-complete commands, ctrl-L, ctrl-U and the other typical console type shortcuts. ctrl-D to exit the current menu. Some other important commands:
    * `help` lists the commands and keywords
    * some commands take you into a new context, a bit like a sub-menu. e.g. `system`, `mqtt`, `thermostat`. Use `help` to show which commands this context has and `exit` or CTRL-D to get back to the root.
    * To change a setting use the `set` command. Typing `set` shows the current settings.
    * `show` shows the data specific to the context you're in.
    * `su` to switch to Admin which enables more commands such as most of the `set` commands. The default password is "neo" which can be changed with `passwd` from the system menu. When in Admin mode the command prompt switches from `$` to `#`.
    * `log` sets the logging. `log off` disables logging. Use `log trace` to see the telegram traffic and `log debug` for very verbose logging. To watch a specific telegram ID or device ID use `log trace [full|raw| [id]`.

- There is no "serial mode" anymore like with version 1.9. When the Wifi cannot connect to the SSID it will automatically enter a "safe" mode where the Serial console is activated, baud 115200. Note Serial is always available on the ESP32 because it has multiple UARTs.

- LED behaves like in 1.9. A solid LED means good connection and EMS data is coming in. A slow pulse means either the WiFi or the EMS bus is not connected. A very fast pulse is when the system is booting up and configuring itself.

### Setting up for the first time:

 - connect the ESP8266/ESP32 via USB and enter via the serial/com port with baud 115200
 - type `su` and use the default password *neo*
 - type `system` to go to the system sub-menu (which we call a context)
 - type `set` to see the current settings
 - use `set wifi` to change the SSID and password. Remember TAB auto-completes the command
 - CTRL-D to get back to the root
 - use `mqtt` to enter the MQTT menu. Same approach using the `set` command to set the MQTT IP and any credentials. CTRL-D to get back to the root.
 - use `ems` to change to the EMS BUS tx mode if you find Tx is not working.
 - reboot and next time use the Telnet via WiFi to connect as the serial mode will be disabled.

### Debugging
 - Turn on logging with either `log all` or `log trace` or `log debug`
 - Error messages are shown in the color red
 - type `show` from the main root to see if any data has come in
 - if not, go to the `ems` context and type `show` which will display some EMS bus stats
 - use the `refresh` command to fetch new data from the EMS bus
 - use `scan devices` or `scan devices deep` to locate devices on the EMS bus. If any are unknown please report back to the project so we can update our EMS device library.



# Full Console Commands

```
common commands available in all contexts:
  exit
  help
  log [level] [pretty | raw] [trace ID]
  su

(top root level)
	refresh
	show
	show version
	ems (enters a context)
	mqtt (enters a context)
	system (enters a context)
	boiler (enters a context)
	thermostat (enters a context)

ems
    scan devices [deep]
	send telegram <"XX XX ...">
	set
	set bus_id <device ID>
	set read_only <on | off>
	set tx_mode <n>
	refresh
	show
	show devices
	show emsbus
	show values

mqtt
	publish
	reconnect
	set
	set base <name>
	set enabled <on | off>
	set heartbeat <on | off>
	set ip <IP address>
	set format <single | nested | ha>
	set password
	set publish_time <seconds>
	set qos <n>
	set username [name]
	show

system
	format
	passwd
	restart
	set
	set syslog host [IP address]
	set syslog level [level]
	set syslog mark [seconds]
	set wifi hostname <name>
	set wifi password
	set wifi ssid <name>
	show
	test <name>
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
	set
	set shower alert <on | off>
	set shower timer <on | off>
	show

thermostat
	change mode <mode> [heating circuit]
	change temp <degrees> [heating circuit]
	read <type ID>
	set
	set master [device ID]
	show
```
  
----------

### **Known issues, bugs and improvements currently being worked on**

```
TODO Optimized ESP8266 and ESP32 UART code (via Michael)
TODO merge in the web code which has the Captive AP and better wifi reconnect logic. Use IPV6 and NTP from lwip2
```

### **To tidy up in code later**

```
TODO replace vectors of class objects with shared pointers and use emplace_back since it instantiates during construction. It may have a performance gain.
TODO make more use of comparison operators in the Telegram class e.g. the compare like "friend inline bool operator==(const Telegram & lhs, const Telegram & rhs)"
TODO replace read_value with C++ Templates per data type
TODO exit from serial should be prevented? Because you never can really exit, just close it.
TODO add real unit tests using platformio's test bed (https://docs.platformio.org/en/latest/plus/pio-remote.html)
TODO See if it's easier to use timers instead of millis() based timers, using https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/BlinkPolledTimeout/BlinkPolledTimeout.ino
```

