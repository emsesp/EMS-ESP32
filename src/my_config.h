/*
 * my_config.h
 *
 * All configurations and customization's go here
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#pragma once

#include "ems.h"

// MQTT for thermostat
#define TOPIC_THERMOSTAT_DATA "thermostat_data"                    // for sending thermostat values to MQTT
#define TOPIC_THERMOSTAT_CMD_TEMP "thermostat_cmd_temp"            // for received thermostat temp changes via MQTT
#define TOPIC_THERMOSTAT_CMD_MODE "thermostat_cmd_mode"            // for received thermostat mode changes via MQTT
#define TOPIC_THERMOSTAT_CMD_HC "thermostat_cmd_hc"                // for received thermostat hc number changes via MQTT
#define TOPIC_THERMOSTAT_CMD_DAYTEMP "thermostat_daytemp"          // RC35 specific
#define TOPIC_THERMOSTAT_CMD_NIGHTTEMP "thermostat_nighttemp"      // RC35 specific
#define TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP "thermostat_holidayttemp" // RC35 specific
#define THERMOSTAT_CURRTEMP "thermostat_currtemp"                  // current temperature
#define THERMOSTAT_SELTEMP "thermostat_seltemp"                    // selected temperature
#define THERMOSTAT_HC "thermostat_hc"                              // which heating circuit number
#define THERMOSTAT_MODE "thermostat_mode"                          // mode
#define THERMOSTAT_DAYTEMP "thermostat_daytemp"                    // RC35 specific
#define THERMOSTAT_NIGHTTEMP "thermostat_nighttemp"                // RC35 specific
#define THERMOSTAT_HOLIDAYTEMP "thermostat_holidayttemp"           // RC35 specific
#define THERMOSTAT_HEATINGTYPE "thermostat_heatingtype"            // RC35 specific (3=floorheating)
#define THERMOSTAT_CIRCUITCALCTEMP "thermostat_circuitcalctemp"    // RC35 specific

// MQTT for boiler
#define TOPIC_BOILER_DATA "boiler_data"                 // for sending boiler values to MQTT
#define TOPIC_BOILER_TAPWATER_ACTIVE "tapwater_active"  // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE "heating_active"    // if heating is on
#define TOPIC_BOILER_WWACTIVATED "wwactivated"          // for receiving MQTT message to change water on/off
#define TOPIC_BOILER_CMD_WWTEMP "boiler_cmd_wwtemp"     // for received boiler wwtemp changes via MQTT
#define TOPIC_BOILER_CMD_COMFORT "boiler_cmd_comfort"   // for received boiler ww comfort setting via MQTT
#define TOPIC_BOILER_CMD_FLOWTEMP "boiler_cmd_flowtemp" // for received boiler flowtemp value via MQTT

// MQTT for SM10/SM100 Solar Module
#define TOPIC_SM_DATA "sm_data"            // topic name
#define SM_COLLECTORTEMP "collectortemp"   // collector temp
#define SM_BOTTOMTEMP "bottomtemp"         // bottom temp
#define SM_PUMPMODULATION "pumpmodulation" // pump modulation
#define SM_PUMP "pump"                     // pump active
#define SM_ENERGYLASTHOUR "energylasthour" // energy last hour
#define SM_ENERGYTODAY "energytoday"       // energy today
#define SM_ENERGYTOTAL "energytotal"       // energy total
#define SM_PUMPWORKMIN "pumpWorkMin"       // Total minutes

// MQTT for HP (HeatPump)
#define TOPIC_HP_DATA "hp_data"            // topic name
#define HP_PUMPMODULATION "pumpmodulation" // pump modulation
#define HP_PUMPSPEED "pumpspeed"           // pump speed

// shower time
#define TOPIC_SHOWERTIME "showertime"           // for sending shower time results
#define TOPIC_SHOWER_TIMER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT "shower_coldshot" // used to trigger a coldshot from an MQTT command

// MQTT for EXTERNAL SENSORS
#define TOPIC_EXTERNAL_SENSORS "sensors"   // for sending sensor values to MQTT
#define PAYLOAD_EXTERNAL_SENSORS "temp_%d" // for formatting the payload for each external dallas sensor


////////////////////////////////////////////////////////////////////////////////////////////////////
// THESE DEFAULT VALUES CAN ALSO BE SET AND STORED WITHTIN THE APPLICATION (see 'set' command)    //
////////////////////////////////////////////////////////////////////////////////////////////////////

// Set LED pin used for showing the EMS bus connection status. Solid means EMS bus working, flashing is an error
// can be either the onboard LED on the ESP8266 (LED_BULLETIN) or external via an external pull-up LED (e.g. D1 on a bbqkees' board)
// can be enabled and disabled via the 'set led' command and pin set by 'set led_gpio'
#define EMSESP_LED_GPIO LED_BUILTIN

#ifdef LOGICANALYZER
#define EMSESP_DALLAS_GPIO D1
#define EMSESP_DALLAS_PARASITE false
#else
// set this if using an external temperature sensor like a DS18B20
// D5 is the default on a bbqkees board
#define EMSESP_DALLAS_GPIO D5
#define EMSESP_DALLAS_PARASITE false
#endif

// By default the EMS bus will be scanned for known devices based on the product ids in ems_devices.h
// You can override the Thermostat and Boiler types here
#define EMSESP_BOILER_TYPE EMS_ID_NONE
#define EMSESP_THERMOSTAT_TYPE EMS_ID_NONE
