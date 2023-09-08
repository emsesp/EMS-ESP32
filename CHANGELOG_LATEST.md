# Changelog

## [3.6.1]

## **IMPORTANT! BREAKING CHANGES**

- `shower_data` MQTT topic shows duration is seconds (was previously a full english sentence)

## Added

- show WiFi rssi in Network Status Page

## Fixed

- Issue in espMqttClient causing a memory leak when MQTT broker is disconnected due to network unavailability [#1264](https://github.com/emsesp/EMS-ESP32/issues/1264)
- Using MQTT enum values correctly formatted in MQTT Discovery [#1280](https://github.com/emsesp/EMS-ESP32/issues/1280)

## Changed

- mqtt free mem check set to 60 kb
- small cosmetic changes to Searching in Customization web page
- updated to espressif32@6.4.0
