/*
 * my_config.h
 * All configurations and customization's go here
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP-Boiler
 */

#pragma once

// these are set as -D build flags during compilation
// they can be set in platformio.ini or alternatively hard coded here
/*
#define WIFI_SSID "<my_ssid>"
#define WIFI_PASSWORD "<my_password>"
#define MQTT_IP "<broker_ip>"
#define MQTT_USER "<broker_username>"
#define MQTT_PASS "<broker_password>"
*/

// default values for shower logic on/off
#define BOILER_SHOWER_TIMER 1 // monitors how long a shower has taken
#define BOILER_SHOWER_ALERT 0 // send alert if showetime exceeded

// trigger settings to determine if hot tap water or the heating is active
#define EMS_BOILER_BURNPOWER_TAPWATER 100
#define EMS_BOILER_SELFLOWTEMP_HEATING 70

//define maximum settable tapwater temperature, not every installation supports 90 degrees
#define EMS_BOILER_TAPWATER_TEMPERATURE_MAX 60

// if using the shower timer, change these settings
#define SHOWER_PAUSE_TIME 15000     // in ms. 15 seconds, max time if water is switched off & on during a shower
#define SHOWER_MIN_DURATION 120000  // in ms. 2 minutes, before recognizing its a shower
#define SHOWER_MAX_DURATION 420000  // in ms. 7 minutes, before trigger a shot of cold water
#define SHOWER_OFFSET_TIME 5000     // in ms. 5 seconds grace time, to calibrate actual time under the shower
#define SHOWER_COLDSHOT_DURATION 10 // in seconds. 10 seconds for cold water before turning back hot water

// The LED for showing connection errors, either onboard or via an external pull-up LED
// can be disabled using -DNO_LED build flag in platformio.ini
#define BOILER_LED LED_BUILTIN // LED_BULLETIN for onboard LED
//#define BOILER_LED D1          // for external LED like on the latest bbqkees boards

// set this if using an external temperature sensor like a DS18B20
#define TEMPERATURE_SENSOR_PIN D7
