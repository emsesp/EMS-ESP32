/*
 * my_config.h
 *
 * All configurations and customization's go here
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#pragma once

#include "ems.h"

// MQTT base name
#define MQTT_BASE "home" // all MQTT topics are prefix with this string, in the format <MQTT_BASE>/<app name>/<topic>

// MQTT general settings
#define MQTT_TOPIC_START "start"
#define MQTT_TOPIC_START_PAYLOAD "start"
#define MQTT_WILL_TOPIC "status"            // for last will & testament topic name
#define MQTT_WILL_ONLINE_PAYLOAD "online"   // for last will & testament payload
#define MQTT_WILL_OFFLINE_PAYLOAD "offline" // for last will & testament payload
#define MQTT_RETAIN false
#define MQTT_KEEPALIVE 120 // 2 minutes
#define MQTT_QOS 1

// MQTT for thermostat
#define TOPIC_THERMOSTAT_DATA "thermostat_data"         // for sending thermostat values to MQTT
#define TOPIC_THERMOSTAT_CMD_TEMP "thermostat_cmd_temp" // for received thermostat temp changes via MQTT
#define TOPIC_THERMOSTAT_CMD_MODE "thermostat_cmd_mode" // for received thermostat mode changes via MQTT
#define THERMOSTAT_CURRTEMP "thermostat_currtemp"       // current temperature
#define THERMOSTAT_SELTEMP "thermostat_seltemp"         // selected temperature
#define THERMOSTAT_MODE "thermostat_mode"               // mode

// MQTT for boiler
#define TOPIC_BOILER_DATA "boiler_data"                // for sending boiler values to MQTT
#define TOPIC_BOILER_TAPWATER_ACTIVE "tapwater_active" // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE "heating_active"   // if heating is on
#define TOPIC_BOILER_WWACTIVATED "wwactivated"         // for receiving MQTT message to change water on/off
#define TOPIC_BOILER_CMD_WWTEMP "boiler_cmd_wwtemp"    // for received boiler wwtemp changes via MQTT

// shower time
#define TOPIC_SHOWERTIME "showertime"           // for sending shower time results
#define TOPIC_SHOWER_TIMER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT "shower_coldshot" // used to trigger a coldshot from an MQTT command

// default values for shower logic on/off
#define BOILER_SHOWER_TIMER 1      // enable (1) to monitor shower time
#define BOILER_SHOWER_ALERT 0      // enable (1) to send alert of cold water when shower time limit has exceeded
#define SHOWER_MAX_DURATION 420000 // in ms. 7 minutes, before trigger a shot of cold water

////////////////////////////////////////////////////////////////////////////////////////////////////
// THESE DEFAULT VALUES CAN ALSO BE SET AND STORED WITHTIN THE APPLICATION (see 'set' command)    //
// ALTHOUGH YOU MAY ALSO HARDCODE THEM HERE BUT THEY WILL BE OVERWRITTEN WITH NEW RELEASE UPDATES //
////////////////////////////////////////////////////////////////////////////////////////////////////

// Set LED pin used for showing ems bus connection status. Solid is connected, Flashing is error
// can be either the onboard LED on the ESP8266 (LED_BULLETIN) or external via an external pull-up LED
// (e.g. D1 on a bbqkees' board
// can be enabled and disabled via the 'set led'
// pin can be set by 'set led_gpio'
#define EMSESP_LED_GPIO LED_BUILTIN

// set this if using an external temperature sensor like a DS18B20
// D5 is the default on bbqkees' board
#define EMSESP_DALLAS_GPIO D5

// By default the EMS bus will be scanned for known devices based on product ids in ems_devices.h
// You can override the Thermostat and Boiler types here
#define EMSESP_BOILER_TYPE EMS_ID_NONE
#define EMSESP_THERMOSTAT_TYPE EMS_ID_NONE
