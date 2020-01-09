### Web UI is showing blank pages after logging in

Due to an issue in how web socket authentication is supported in some browsers, the web interface will currently not work with
* Safari
* Any browser on an iOS or iPadOS device (so iPhones and iPads)

and the Dashboard will show an empty page.
  
A workaround is to use another browser like Chrome or Firefox until the issue has been resolved in the 3rd party library. See [here](https://github.com/me-no-dev/ESPAsyncWebServer/issues/575) and [here](https://bugs.webkit.org/show_bug.cgi?id=80362) for further details on the bug.

### Web UI is slow or not responding

Check that you have compiled the firmware with the correct ESP8266 board. In PlatformIO look for `board =` in the `[env]` section. There are noticeable differences between flash chips on certain branded and clone ESP8266 development boards. Best let PlatformIO figure out which one to use (e.g. dio, dout, qio).

Also certain ESP8266 boards (like from DO-IT) have been problematic. Check that first before logging an issue.

### Can't connect via WiFi to the EMS-ESP

The EMS-ESP is probably in Access Point mode. Look for a wifi SSID 'ems-esp', connect to this and open a browser to 'https://192.168.4.1'. If you have configured a WiFi client your router would have appointed an IP address via DHCP. You should be able to connect via https://ems-esp or https://ems-esp.local.

If you're still unable to connect to the EMS-ESP, hook it up to the USB and open a serial monitor to the COM port to see what is happening. The serial output will always automatically be switched off when there is a connection to the EMS bus. To force it on at bootup, re-compile the code by adding `-DFORCE_SERIAL` to the `general_flags` in `platformio.ini`. Note make sure you re-compile without this flag when putting the EMS-ESP back into production use.

### EMS Bus shows as disconnected

The on-board blue LED will flash if there is no connection with the EMS bus. If the LED is disabled re-enable it using the telnet command `set led` or the web interface. The two possible reasons why the bus shows as disabled or disconnected:

1) The board is running in Serial mode. This will be shown in the telnet session and you can use `set serial off` to disable it or via the web interface.
2) There is no Rx data coming in. The connection to the EMS bus (2-wires or the jack) is faulty or connected to the wrong GPIO pins on the ESP. Rx is D7 and Tx is D8.

### ESP is unstable and frequently reboots

Well, that sucks. Sorry!

Here are some first things to check

* Check you're using a genuine ESP8266 (like the Wemos D1 or Nodemcuv2). There have been reports of some esp8266 clones (noticeably from DO-IT) which are problematic. See this message from [bbqkees](https://github.com/proddy/EMS-ESP/issues/173#issuecomment-528791079).
* Check that the settings can be saved after a set command. Check with typing `set` into the telnet session.
* Can you reproduce the problem using the official firmware .bin files from the GitHub Release page?
* If you compiled the firmware yourself
  1) Check if you can you reproduce this using the precompiled firmware from the GitHub Releases page
  2) Check that your `platformio.ini` file is up-to-date with the example one provided
  3) Check that your platformio environment up-to-date with the latest libraries
* Did it work before, in an earlier version? Report back which one.

Then try the following to isolate the root cause of the problem:

* Use `system` to see how many times it has restarted and what the exception cause was. Note, if the EMS-ESP restarts more than 10 times within a 60-second window of booting it is considered unstable and Serial mode will automatically be enabled to help debug the root cause. This will be shown in the system command too.
* Don't use the web interface, only the Telnet with logging off (`set log n`). Does it still fail? Then try with logging on and see where it fails. Capture the logs and screenshots.
* If compiled with the `debug` target the firmware will capture the stack dump of a crash. Use `crash dump` to output the stack, copy this to a file in scripts/stackdmp.txt (you may need to clean up the formatting) and run the `analyze_stackdmp.py` file to see where in the code the crash occurred.
* Perhaps it's the Tx logic, which can be disabled using `set listen_mode on` (or via the web). See next session below for Tx issues.
* Perhaps the MQTT is failing, which can be disabled with `set publish_time 0` or `set mqtt_enabled false` (or via the web).
* Perhaps the UART low-level code to read the signals from the circuit is causing interference somewhere. Detach the ESP8266 from the circuit (or gateway module) and start it without any of the GPIOs attached to see if it is any more stable.
* As a last resort try wiping the ESP8266 and uploading the firmware again. Use `pio run -t erase` or `esptool.py erase_flash`.

### Tx is just not working

First make sure 'listen_mode' is not on as this disables writing to the EMS bus (called Tx). Verify this with the `set` command.

A quick test is to manually send a command with verbose logging enabled, for example, this will request the version information from the boiler on Device ID 0x08:

```
log v
send 0B 88 02 00 20
```

Note for Junkers use `send 8B 88 02 00 20`

Then try a different Tx setting with `set tx_mode`. 1 is the default, 2 works well for EMS+ and 3 is Junkers specific. With some Bosch devices 2 works better.

If you see errors pop up in the Telnet session like `** Warning, we missed the bus - Rx non-idle!` and have other components using the +5V power line of the ESP8266 (for example external sensors) then disconnect and try again and this may be causing noise on the line.
