## The main source files

`emsuart.cpp` handles the low level UART read and write logic to the bus. You shouldn't need to touch this. All receive commands from the EMS bus are handled asynchronously using a circular buffer via an interrupt. A separate function processes the buffer and extracts the telegrams.

`ems.cpp` is the logic to read the EMS data packets (telegrams), validates them and process them based on the type.

`ems-esp.cpp` is the Arduino code for the ESP8266 that kicks it all off. This is where we have specific logic such as the code to monitor and alert on the Shower timer and light up the LEDs.

`my_config.h` has all the custom settings tailored to your environment. Specific values here are also stored in the ESP's SPIFFs (File system).

`ems_devices.h` has all the configuration for the known EMS devices currently supported.

`MyESP.cpp` is my custom library to handle WiFi, MQTT and Telnet. Uses a modified version of [TelnetSpy](https://github.com/yasheena/telnetspy).

`ds18.*` are the Dallas libraries for any external temperature sensors.

## How the shower logic works

Checking whether the shower is running is tricky. We know when the warm water is on and being heated but need to distinguish between the central heating, shower, hot tap and even a bath tap. So this code is a little experimental.

There is other logic in the code to compensate for water heating up to shower temperature and whether the shower is turned off and back on again quickly within a 10 second window.