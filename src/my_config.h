/*
 * my_config.h
 *
 * All configurations and customization's go here
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#pragma once

#include "ems.h"

// Set your wifi and mqtt params
// If set to NULL (default) you'll need to set them in AP mode using the 'set' command
// as these values are stored in SPIFFs for persisted
#define WIFI_SSID NULL
#define WIFI_PASSWORD NULL
#define MQTT_HOST NULL
#define MQTT_USER NULL
#define MQTT_PASS NULL

// All MQTT topics are prefixed with the following string
#define MQTT_BASE "home"
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_WILL "status" // for last will & testament
#define MQTT_RETAIN true
#define MQTT_KEEPALIVE 300
#define MQTT_QOS 1

// default values for shower logic on/off
#define BOILER_SHOWER_TIMER 1      // enable (1) to monitor shower time
#define BOILER_SHOWER_ALERT 0      // enable (1) to send alert of cold watewr when shower time limit has exceeded
#define SHOWER_MAX_DURATION 420000 // in ms. 7 minutes, before trigger a shot of cold water

// Set LED pin used for showing ems bus connection status. Solid is connected, Flashing is error
// can be either the onboard LED on the ESP8266 or external via an external pull-up LED (e.g. D1)
// can be enabled and disabled via the 'set led' command
#define BOILER_LED LED_BUILTIN // LED_BULLETIN for onboard LED

// set this if using an external temperature sensor like a DS18B20
#define TEMPERATURE_SENSOR_PIN D7

// By default the EMS bus will be scanned for known devices (EMS_MODEL_NONE).
// You can override this here by fixing the Boiler and Thermostat types.
// See ems.h for the list of recognized types. For example:
// boilers: EMS_MODEL_BK15, EMS_MODEL_UBA, EMS_MODEL_BC10, EMS_MODEL_MM10, EMS_MODEL_WM10
// thermostats: EMS_MODEL_ES73, EMS_MODEL_RC20, EMS_MODEL_RC30, EMS_MODEL_RC35, EMS_MODEL_EASY
#define MY_BOILER_MODELID EMS_MODEL_NONE
#define MY_THERMOSTAT_MODELID EMS_MODEL_NONE
//#define MY_BOILER_MODELID EMS_MODEL_UBA
//#define MY_THERMOSTAT_MODELID EMS_MODEL_RC20
