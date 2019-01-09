/*
 * General information about known EMS devices
 *
 */

#pragma once

#include "ems.h"

/* 
 * Boiler...
 */
#define EMS_TYPE_UBAMonitorFast 0x18              // is an automatic monitor broadcast
#define EMS_TYPE_UBAMonitorSlow 0x19              // is an automatic monitor broadcast
#define EMS_TYPE_UBAMonitorWWMessage 0x34         // is an automatic monitor broadcast
#define EMS_TYPE_UBAMaintenanceStatusMessage 0x1C // is an automatic monitor broadcast
#define EMS_TYPE_UBAParameterWW 0x33
#define EMS_TYPE_UBATotalUptimeMessage 0x14
#define EMS_TYPE_UBAMaintenanceSettingsMessage 0x15
#define EMS_TYPE_UBAParametersMessage 0x16
#define EMS_TYPE_UBASetPoints 0x1A
#define EMS_TYPE_UBAFunctionTest 0x1D

#define EMS_OFFSET_UBAParameterWW_wwtemp 2              // WW Temperature
#define EMS_OFFSET_UBAParameterWW_wwactivated 1         // WW Activated
#define EMS_OFFSET_UBAParameterWW_wwComfort 9           // WW is in comfort or eco mode
#define EMS_VALUE_UBAParameterWW_wwComfort_Comfort 0x00 // the value for comfort
#define EMS_VALUE_UBAParameterWW_wwComfort_Eco 0xD8     // the value for eco

/* 
 * Thermostat...
 */

// Common for all thermostats
#define EMS_TYPE_RCTime 0x06               // is an automatic thermostat broadcast
#define EMS_TYPE_RCOutdoorTempMessage 0xA3 // is an automatic thermostat broadcast, outdoor external temp

// RC20 specific
#define EMS_TYPE_RC20StatusMessage 0x91       // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC20Set 0xA8                 // for setting values like temp and mode
#define EMS_OFFSET_RC20Set_mode 23            // position of thermostat mode
#define EMS_OFFSET_RC20Set_temp 28            // position of thermostat setpoint temperature
#define EMS_TYPE_RC20StatusMessage_setpoint 1 // setpoint temp
#define EMS_TYPE_RC20StatusMessage_curr 2     // current temp

// RC30 specific
#define EMS_TYPE_RC30StatusMessage 0x41       // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC30Set 0xA7                 // for setting values like temp and mode
#define EMS_OFFSET_RC30Set_mode 23            // position of thermostat mode
#define EMS_OFFSET_RC30Set_temp 28            // position of thermostat setpoint temperature
#define EMS_TYPE_RC30StatusMessage_setpoint 1 // setpoint temp
#define EMS_TYPE_RC30StatusMessage_curr 2     // current temp

// RC35 specific
#define EMS_TYPE_RC35StatusMessage 0x3E       // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC35StatusMessage_setpoint 2 // desired temp
#define EMS_TYPE_RC35StatusMessage_curr 3     // current temp
#define EMS_TYPE_RC35Set 0x3D                 // for setting values like temp and mode (Working mode HC1)
#define EMS_OFFSET_RC35Set_mode 7             // position of thermostat mode
#define EMS_OFFSET_RC35Set_temp_day 2         // position of thermostat setpoint temperature for day time
#define EMS_OFFSET_RC35Set_temp_night 1       // position of thermostat setpoint temperature for night time

// Easy specific
#define EMS_TYPE_EasyStatusMessage 0x0A        // reading values on an Easy Thermostat
#define EMS_TYPE_EasyStatusMessage_setpoint 10 // setpoint temp
#define EMS_TYPE_EasyStatusMessage_curr 8      // current temp

// Known EMS types
typedef enum {
    EMS_MODEL_NONE,
    EMS_MODEL_ALL, // common for all devices

    // service key
    EMS_MODEL_SERVICEKEY, // this is us

    // main buderus boiler type devices
    EMS_MODEL_BK15,
    EMS_MODEL_UBA,
    EMS_MODEL_BC10,
    EMS_MODEL_BC25,
    EMS_MODEL_MM10,
    EMS_MODEL_WM10,
    EMS_MODEL_RFM20,

    // thermostats
    EMS_MODEL_ES73,
    EMS_MODEL_RC20,
    EMS_MODEL_RC20F,
    EMS_MODEL_RC30,
    EMS_MODEL_RC35,
    EMS_MODEL_EASY

} _EMS_MODEL_ID;

// EMS types for known Buderus devices. This list will be extended when new devices are recognized.
// format is MODEL_ID, PRODUCT ID, TYPE_ID, DESCRIPTION
const _Model_Type Model_Types[] = {

    // me
    {EMS_MODEL_SERVICEKEY, 999, 0x0B, "Service Key"},

    // various boilers and buderus type devices
    {EMS_MODEL_UBA, 123, 0x08, "MC10/UBA3 Boiler"},
    {EMS_MODEL_BK15, 64, 0x08, "Sieger BK15 Boiler"},
    {EMS_MODEL_BC10, 190, 0x09, "BC10 Base Controller"},
    {EMS_MODEL_BC25, 125, 0x09, "BC25 Base Controller"},
    {EMS_MODEL_RFM20, 68, 0x09, "RFM20 RC20F Receiver"},
    {EMS_MODEL_MM10, 251, 0x21, "MM10 Mixer Module"},  // warning, fake product id!
    {EMS_MODEL_WM10, 250, 0x11, "WM10 Switch Module"}, // warning, fake product id!

    // controllers and thermostats
    {EMS_MODEL_ES73, 76, 0x10, "Sieger ES73"},
    {EMS_MODEL_RC20, 77, 0x17, "RC20 (e.g. Nefit Moduline 300)"},
    {EMS_MODEL_RC20F, 93, 0x18, "RC20F"},
    {EMS_MODEL_RC30, 78, 0x10, "RC30 (e.g. Nefit Moduline 400)"},
    {EMS_MODEL_RC35, 86, 0x10, "RC35 (or compatible"},
    {EMS_MODEL_EASY, 202, 0x18, "TC100 (e.g. Nefit Easy or CT100)"}

};
/* 
 * Known thermostat types and their abilities
 */
const _Thermostat_Type Thermostat_Types[] = {

    {EMS_MODEL_RC20, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC20F, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC30, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC35, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_EASY, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_ES73, EMS_THERMOSTAT_READ_YES, EMS_THERMOSTAT_WRITE_YES}

};
