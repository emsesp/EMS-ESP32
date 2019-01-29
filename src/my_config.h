/*
 * my_config.h
 *
 * All configurations and customization's go here
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#pragma once

#include "ems.h"

// All MQTT topics are prefixed with the following string
#define MQTT_BASE "home"
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_WILL_TOPIC "status" // for last will & testament topic name
#define MQTT_WILL_PAYLOAD "0"    // for last will & testament payload
#define MQTT_RETAIN false
#define MQTT_KEEPALIVE 300
#define MQTT_QOS 1

// thermostat
#define TOPIC_THERMOSTAT_DATA "thermostat_data"         // for sending thermostat values
#define TOPIC_THERMOSTAT_CMD_TEMP "thermostat_cmd_temp" // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CMD_MODE "thermostat_cmd_mode" // for received thermostat mode changes
#define THERMOSTAT_CURRTEMP "thermostat_currtemp"       // current temperature
#define THERMOSTAT_SELTEMP "thermostat_seltemp"         // selected temperature
#define THERMOSTAT_MODE "thermostat_mode"               // mode

// boiler
#define TOPIC_BOILER_DATA "boiler_data"                // for sending boiler values
#define TOPIC_BOILER_TAPWATER_ACTIVE "tapwater_active" // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE "heating_active"   // if heating is on

// shower time
#define TOPIC_SHOWERTIME "showertime"           // for sending shower time results
#define TOPIC_SHOWER_TIMER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT "shower_coldshot" // used to trigger a coldshot from an MQTT command

// default values for shower logic on/off
#define BOILER_SHOWER_TIMER 1      // enable (1) to monitor shower time
#define BOILER_SHOWER_ALERT 0      // enable (1) to send alert of cold water when shower time limit has exceeded
#define SHOWER_MAX_DURATION 420000 // in ms. 7 minutes, before trigger a shot of cold water

//////////////////////////////////////////////////////////////////////////////////////////////////
// THESE DEFAULT VALUES CAN ALSO BE SET AND STORED WITHTIN THE APPLICATION (see 'set' command)  //
//////////////////////////////////////////////////////////////////////////////////////////////////

// Set your wifi and mqtt params
// If set to NULL (default) you'll need to set them in AP mode using the 'set' command
#define WIFI_SSID NULL
#define WIFI_PASSWORD NULL
#define MQTT_HOST NULL
#define MQTT_USER NULL
#define MQTT_PASS NULL

// Set LED pin used for showing ems bus connection status. Solid is connected, Flashing is error
// can be either the onboard LED on the ESP8266 (LED_BULLETIN) or external via an external pull-up LED (e.g. D1 on bbqkees' board)
// can be enabled and disabled via the 'set led' command
// pin can be set by 'set led_gpio'
#define EMSESP_LED_GPIO LED_BUILTIN

// set this if using an external temperature sensor like a DS18B20
// D5 is the default on bbqkees' board
#define EMSESP_DALLAS_GPIO D5

// By default the EMS bus will be scanned for known devices based on product ids in ems_devices.h
// You can override the Thermostat and Boiler types here
#define EMSESP_BOILER_TYPE EMS_ID_NONE
#define EMSESP_THERMOSTAT_TYPE EMS_ID_NONE
