/*
 * my_config.h
 *
 * All configurations and customization's go here
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#pragma once

#include "ems.h"

// TOPICS with _CMD_ are used for receiving commands from an MQTT Broker
// EMS-ESP will subscribe to these topics
#define TOPIC_GENERIC_CMD "generic_cmd" // for receiving generic system commands via MQTT

// MQTT for thermostat
// these topics can be suffixed with a Heating Circuit number, e.g. thermostat_cmd_temp1 and thermostat_data1
#define TOPIC_THERMOSTAT_DATA "thermostat_data" // for sending thermostat values to MQTT

#define TOPIC_THERMOSTAT_CMD "thermostat_cmd"              // for receiving thermostat commands via MQTT
#define TOPIC_THERMOSTAT_CMD_TEMP_HA "thermostat_cmd_temp" // temp changes via MQTT, for HA climate component
#define TOPIC_THERMOSTAT_CMD_MODE_HA "thermostat_cmd_mode" // mode changes via MQTT, for HA climate component
#define TOPIC_THERMOSTAT_CMD_TEMP "temp"                   // temp changes via MQTT
#define TOPIC_THERMOSTAT_CMD_MODE "mode"                   // mode changes via MQTT
#define TOPIC_THERMOSTAT_CMD_DAYTEMP "daytemp"             // day temp (RC35 specific)
#define TOPIC_THERMOSTAT_CMD_NIGHTTEMP "nighttemp"         // night temp (RC35 specific)
#define TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP "holidayttemp"    // holiday temp (RC35 specific)

#define THERMOSTAT_CURRTEMP "currtemp"               // current temperature
#define THERMOSTAT_SELTEMP "seltemp"                 // selected temperature
#define THERMOSTAT_HC "hc"                           // which heating circuit number
#define THERMOSTAT_MODE "mode"                       // mode
#define THERMOSTAT_DAYTEMP "daytemp"                 // RC35 specific
#define THERMOSTAT_NIGHTTEMP "nighttemp"             // RC35 specific
#define THERMOSTAT_HOLIDAYTEMP "holidayttemp"        // RC35 specific
#define THERMOSTAT_HEATINGTYPE "heatingtype"         // RC35 specific (3=floorheating)
#define THERMOSTAT_CIRCUITCALCTEMP "circuitcalctemp" // RC35 specific

// mixing module
#define MIXING_HC "hc"   // which heating circuit number
#define MIXING_WWC "wwc" // which warm water circuit number

// MQTT for boiler
#define TOPIC_BOILER_DATA "boiler_data"                // for sending boiler values to MQTT
#define TOPIC_BOILER_TAPWATER_ACTIVE "tapwater_active" // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE "heating_active"   // if heating is on

#define TOPIC_BOILER_CMD "boiler_cmd"                             // for receiving boiler commands via MQTT
#define TOPIC_BOILER_CMD_WWACTIVATED "boiler_cmd_wwactivated"     // change water on/off
#define TOPIC_BOILER_CMD_WWONETIME "boiler_cmd_wwonetime"         // warm warter one time loading
#define TOPIC_BOILER_CMD_WWCIRCULATION "boiler_cmd_wwcirculation" // start warm warter circulation
#define TOPIC_BOILER_CMD_WWTEMP "boiler_cmd_wwtemp"               // wwtemp changes via MQTT
#define TOPIC_BOILER_CMD_COMFORT "comfort"                        // ww comfort setting via MQTT
#define TOPIC_BOILER_CMD_FLOWTEMP "flowtemp"                      // flowtemp value via MQTT

// MQTT for mixing device
#define TOPIC_MIXING_DATA "mixing_data" // for sending mixing device values to MQTT

// MQTT for SM10/SM100/SM200 Solar Module
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
#define TOPIC_SHOWER_DATA "shower_data"  // for sending shower time results
#define TOPIC_SHOWER_TIMER "timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT "alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT "coldshot" // used to trigger a coldshot from an MQTT command
#define TOPIC_SHOWER_DURATION "duration" // duration of the last shower

// MQTT for External Sensors
#define TOPIC_EXTERNAL_SENSORS "sensors"   // for sending sensor values to MQTT
#define PAYLOAD_EXTERNAL_SENSORS "temp_%d" // for formatting the payload for each external dallas sensor
