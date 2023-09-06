# Changelog

# [3.6.1]

## **IMPORTANT! BREAKING CHANGES**

- shower_data MQTT topic shows duration is seconds (was previously an English string)

## Added

- rssi in Network Status Page
- boiler nominal power from telegram 0x04
- boiler energy for heating and dhw stored in nvs
- analogsensor counter value stored in nvs

## Fixed

- issue in espMqttClient on low mem
- mqtt subscription to scheduler and custom
- HA configuration for scheduler and custom

## Changed

- mqtt free mem check 60k
- small cosmetic changes to Searching in Customization web page
- update to espressif32@6.4.0
