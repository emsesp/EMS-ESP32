# espMqttClient

MQTT client library for the Espressif devices ESP8266 and ESP32 on the Arduino framework.
Aims to be a non-blocking, fully compliant MQTT 3.1.1 client.

![platformio](https://github.com/bertmelis/espMqttClient/actions/workflows/build_platformio.yml/badge.svg)
![cpplint](https://github.com/bertmelis/espMqttClient/actions/workflows/cpplint.yml/badge.svg)
![cppcheck](https://github.com/bertmelis/espMqttClient/actions/workflows/cppcheck.yml/badge.svg)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/bertmelis/library/espMqttClient.svg)](https://registry.platformio.org/libraries/bertmelis/espMqttClient)

# Features

- MQTT 3.1.1 compliant library
- Sending and receiving at all QoS levels
- TCP and TCP/TLS using standard WiFiClient and WiFiClientSecure connections
- Virtually unlimited incoming and outgoing payload sizes
- Readable and understandable code
- Fully async clients available via [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) or [ESPAsnycTCP](https://github.com/me-no-dev/ESPAsyncTCP) (no TLS supported)
- Supported platforms:
  - Espressif ESP8266 and ESP32 using the Arduino framework
- Basic Linux compatibility*. This includes WSL on Windows

    > Linux compatibility is mainly for automatic testing. It relies on a quick and dirty Arduino-style `Client` with a POSIX TCP client underneath and Arduino-style `IPAddress` class. These are lacking many features needed for proper Linux support.

# Documentation

See [documentation](https://www.emelis.net/espMqttClient/) and the [examples](examples/).

## Limitations

### MQTT 3.1.1 Compliancy

Outgoing messages and session data are not stored in non-volatile memory. Any events like loss of power or sudden resets result in loss of data. Despite this limitation, one could still consider this library as fully complaint based on the non normative remark in point 4.1.1 of the specification.

### Non-blocking

This library aims to be fully non-blocking. It is however limited by the underlying `WiFiClient` library which is part of the Arduino framework and has a blocking `connect` method. This is not an issue on ESP32 because the call is offloaded to a separate task. On ESP8266 however, connecting will block until succesful or until the connection timeouts.

If you need a fully asynchronous MQTT client, you can use `espMqttClientAsync` which uses AsyncTCP/ESPAsyncTCP under the hood. These underlying libraries do not support TLS (anymore). I will not provide support TLS for the async client.

# Bugs and feature requests

Please use Github's facilities to get in touch.

# About this library

This client wouldn't exist without [Async-mqtt-client](https://github.com/marvinroger/async-mqtt-client). It has been my go-to MQTT client for many years. It was fast, reliable and had features that were non-existing in alternative libraries. However, the underlying async TCP libraries are lacking updates, especially updates related to secure connections. Adapting this library to use up-to-date TCP clients would not be trivial. I eventually decided to write my own MQTT library, from scratch. 

The result is an almost non-blocking library with no external dependencies. The library is almost a drop-in replacement for the async-mqtt-client except a few parameter type changes (eg. `uint8_t*` instead of `char*` for payloads).

# License

This library is released under the MIT Licence. A copy is included in the repo.
Parts of this library, most notably the API, are based on [Async MQTT client for ESP8266 and ESP32](https://github.com/marvinroger/async-mqtt-client).
