
## Hooking up the interface board

If you have an EMS Gateway board then follow the instructions on [BBQKees' wiki](https://bbqkees-electronics.nl/wiki/).

Things to check:

1. Any external Dallas temperature sensors go to D1
2. An external LED goes to D5
3. Check the EMS bus lines are connected the right way:

| EMS board | ESP8266 dev board |
| ----------|------------------ |
| Ground/G/J2|  GND/G |
| Rx/J2 | D7/GPIO13 * |
| Tx/J2 | D8/GPIO15 *|
| VC/J2 | 3v3 |

\* Note the firmware purposely switches the UART0 from the standard GPIOs (Rx 1 and Tx 3) to pins 13 and 15 to prevent Serial traffic (e.g. via a USB connection) interfere with the EMS line. See [the official document](https://arduino-esp8266.readthedocs.io/en/latest/reference.html#serial). 

## Adjusting the config settings

When the EMS-ESP is running for the first time it's likely to be Access Point mode with a SSID called ems-esp and IP of 192.168.4.1. Use a phone/pc/laptop and connect to AP's SSID. Then either use Telnet (port 23) to IP address 192.168.4.1 or type in this IP address in a web browser.

When using the web the default password is **admin**.

The first thing you probably want to do is set the wifi mode to Client and set the SSID and password dor your own network. Then restart the EMS-ESP device. If everything is working the software should detect the devices on the EMS bus and the LED should light up a solid blue. If not, use the Telnet to look any error messages.

The rest of the settings can be entered either via the Telnet interface (via `set`) or the web interface. The next settings you may want to change are the MQTT, `tx_mode` and `publish_time`.

## Adding external Temperature sensors

The code supports auto-detection of Dallas type temperature sensors. The default GPIO pin used on the ESP8266 is D5 but this can be configured in the setting menu (`set dallas_gpio`) or via the web. The Dallas chips DS1822, DS18S20, DS18B20, DS1825 are supported including their parasite varieties and can be daisy-chained on one line.  If you're using parasite make sure you set this using 'set dallas_parasite on'.

The values will be shown in the telnet session (`info`) and if MQTT is enabled sent as a single topic. See [MQTT](https://github.com/proddy/EMS-ESP/wiki/MQTT).
