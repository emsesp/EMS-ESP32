# EMS-ESP version 2.0 (alpha)

*Warning: this is a snapshot the EMS-ESP2 development in still in early stages of development and not for production!*

---

## Major changes since version 1.9.x

### **Design & Coding principles**

- The code can be built and run without an ESP microcontroller, which helps with testing and simulating handling of telegrams.
- I used C++11 containers where I could (std::string, std::deque, std::list, std::multimap etc).
- The core is based off the great libraries from @nomis and adopted his general design pattens such as making everything as asynchronous as possible so that no one operation should starve another operation of it's time to execute (https://isocpp.org/wiki/faq/ctors#static-init-order).
- All EMS devices (e.g. boiler, thermostat, solar modules etc) are derived from a factory base class and each class handles its own registering of telegram and mqtt handlers. This makes the EMS device code easier to manage and extend with new telegrams types and features.
- Built to work with both EMS8266 and ESP32.

### **Features**

- A web interface built using React and TypeScript to be secure and cross-browser compatible. Each restful endpoint is protected and issues a JWT which is then sent using Bearer Authentication. Implements a Web captive portal. On first installs EMS-ESP starts an Access Point where system settings can be configured. Note, this is still in a separate repo and pending a merge into this project.
  
- A new console. Like 1.9.x it works with both Serial and Telnet but a lot more intuitive behaving like a Linux shell and secure. Multiple telnet sessions are supported now but watch out for slow connections and low memory. A password is need to change any settings. You can use TAB to auto-complete commands. Some key commands:
    * `help` lists the commands and keywords
    * some commands take you into a new context, a bit like a sub-menu. e.g. `system`, `mqtt`, `thermostat`. Use `help` to show which commands this context has and `exit` to get back to the root.
    * To change a setting use the `set` command. Typing `set` shows the current settings.
    * `show` shows the data specific to the context you're in.
    * `su` to switch to Admin which enables more commands such as most of the `set` commands. The default password is "neo". When in Admin mode the command prompt switches from `$` to `#`.
    * `log` sets the logging. `log off` disables logging. Use `log trace` to see the telegram traffic and `log debug` for very verbose logging. To watch a specific telegram ID or device ID use `log trace [id]`.

- There is no "serial mode" anymore. When the Wifi cannot connect to the SSID it will automatically enter a "safe" mode where the Serial console is activated. Note Serial is always available on the ESP32 because it has 2 UARTs.

- LED behaves like in 1.9. A solid LED means good connection and EMS data is coming in. A slow pulse means either the WiFi or the EMS bus is not connected. A very fast pulse is when the system is booting up and configuring itself.

- on a new install you will want to enter `su` and then go to the `system` context. Use `set wifi ...` to set the network up. Then go to the `mqtt` context to set the mqtt up.

  
----------

### **Fixes and Improvements to work on now**

```
TODO figure out why sometimes telnet on ESP32 (and sometimes ESP8266) has slow response times.
TODO Get the ESP32 UART code working.
TODO console auto-complete with 'set' command in the system context is not showing all commands, only the hostname.

```

### **Features to add next**

```
TODO finish porting over code for Solar, Mixing and Heat pump.
TODO implement 0xE9 telegram type for EMS+ boilers. This was a request #382 (https://github.com/proddy/EMS-ESP/issues/382)
```

### **To tidy up in code later**

```
TODO replace vectors of class objects with shared pointers and use emplace_back since it instantiates during construction. It may have a performance gain.
TODO decide if we really need to store the timestamp of each incoming Rx telegram.
TODO make more use of comparison operators in the Telegram class e.g. the compare like "friend inline bool operator==(const Telegram & lhs, const Telegram & rhs)"
TODO exit from serial should be prevented? Because you never can really exit, just close it.
TODO add real unit tests using platformio's test bed (https://docs.platformio.org/en/latest/plus/pio-remote.html)
TODO See if it's easier to use timers instead of millis() timers, using https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/BlinkPolledTimeout/BlinkPolledTimeout.ino
```

### **These features to add**

```
TODO merge in the web code
TODO merge in NTP code
TODO make ascii colors in the console optional?
TODO decide what to do with gateways, switches and other bogus EMS devices
TODO add MQTT subscribe topic to toggle on/off the shower alert and timer. If really needed.
TODO decide if I want to port over the shower one-shot cold water logic. Don't think its used.
```
