# espMqttClient

MQTT client library for the Espressif devices ESP8266 and ESP32 on the Arduino framework.
Aims to be a non-blocking, fully compliant MQTT 3.1.1 client.

Copy of <https://github.com/bertmelis/espMqttClient>

Based on Version 1.7.0 - <https://github.com/bertmelis/espMqttClient/tree/v1.7.0>

with additional changes to support EMS-ESP such as compiling with Tasmota and not using `SecureWifiClient` in these two files:

```
 src/espMqttClient.cpp
 src/Transport/ClientSecureSync.h
 src/Config.h
```

# License

This library is released under the MIT Licence. A copy is included in the repo.
Parts of this library, most notably the API, are based on [Async MQTT client for ESP8266 and ESP32](https://github.com/marvinroger/async-mqtt-client).
