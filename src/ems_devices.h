/*
 * General information about known EMS devices
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 * See ChangeLog.md for History
 * See README.md for Acknowledgments
 *
 */

#pragma once

#include "ems.h"

/*
 * Common Type
 */
#define EMS_TYPE_Version 0x02
#define EMS_TYPE_UBADevices 0x07 // EMS connected devices

/*
 * Boiler Telegram Types...
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

#define EMS_OFFSET_UBAParameterWW_wwtemp 2                  // WW Temperature
#define EMS_OFFSET_UBAParameterWW_wwactivated 1             // WW Activated
#define EMS_OFFSET_UBAParameterWW_wwComfort 9               // WW is in comfort or eco mode
#define EMS_VALUE_UBAParameterWW_wwComfort_Hot 0x00         // the value for hot
#define EMS_VALUE_UBAParameterWW_wwComfort_Eco 0xD8         // the value for eco
#define EMS_VALUE_UBAParameterWW_wwComfort_Intelligent 0xEC // the value for intelligent

#define EMS_OFFSET_UBASetPoints_flowtemp 0 // flow temp

// SM and HP Types
#define EMS_TYPE_SM10Monitor 0x97    // SM10Monitor
#define EMS_TYPE_SM100Monitor 0x0262 // SM100Monitor
#define EMS_TYPE_SM100Status 0x0264  // SM100Status
#define EMS_TYPE_SM100Status2 0x026A // SM100Status2
#define EMS_TYPE_SM100Energy 0x028E  // SM100Energy
#define EMS_TYPE_HPMonitor1 0xE3     // HeatPump Monitor 1
#define EMS_TYPE_HPMonitor2 0xE5     // HeatPump Monitor 2

#define EMS_TYPE_ISM1StatusMessage 0x0003  // Solar Module Junkers ISM1 Status
#define EMS_TYPE_ISM1Set 0x0001            // for setting values of the solar module like max boiler temp
#define EMS_OFFSET_ISM1Set_MaxBoilerTemp 6 // position of max boiler temp  e.g. 50 in the following example: 90 30 FF 06 00 01 50 (CRC=2C)

/*
 * Thermostat Types
 */

// Common for all thermostats
#define EMS_TYPE_RCTime 0x06               // is an automatic thermostat broadcast
#define EMS_TYPE_RCOutdoorTempMessage 0xA3 // is an automatic thermostat broadcast, outdoor external temp

// RC10 specific
#define EMS_TYPE_RC10StatusMessage 0xB1         // is an automatic thermostat broadcast giving us temps
#define EMS_OFFSET_RC10StatusMessage_setpoint 1 // setpoint temp
#define EMS_OFFSET_RC10StatusMessage_curr 2     // current temp

#define EMS_TYPE_RC10Set 0xB0     // for setting values like temp and mode
#define EMS_OFFSET_RC10Set_temp 4 // position of thermostat setpoint temperature

// RC20 specific
#define EMS_TYPE_RC20StatusMessage 0x91         // is an automatic thermostat broadcast giving us temps
#define EMS_OFFSET_RC20StatusMessage_setpoint 1 // setpoint temp
#define EMS_OFFSET_RC20StatusMessage_curr 2     // current temp

#define EMS_TYPE_RC20Set 0xA8      // for setting values like temp and mode
#define EMS_OFFSET_RC20Set_mode 23 // position of thermostat mode
#define EMS_OFFSET_RC20Set_temp 28 // position of thermostat setpoint temperature

// RC30 specific
#define EMS_TYPE_RC30StatusMessage 0x41         // is an automatic thermostat broadcast giving us temps
#define EMS_OFFSET_RC30StatusMessage_setpoint 1 // setpoint temp
#define EMS_OFFSET_RC30StatusMessage_curr 2     // current temp

#define EMS_TYPE_RC30Set 0xA7      // for setting values like temp and mode
#define EMS_OFFSET_RC30Set_mode 23 // position of thermostat mode
#define EMS_OFFSET_RC30Set_temp 28 // position of thermostat setpoint temperature


// RC35 specific
#define EMS_TYPE_RC35StatusMessage_HC1 0x3E // is an automatic thermostat broadcast giving us temps on HC1
#define EMS_TYPE_RC35StatusMessage_HC2 0x48 // is an automatic thermostat broadcast giving us temps on HC2
#define EMS_TYPE_RC35StatusMessage_HC3 0x52 // is an automatic thermostat broadcast giving us temps on HC3
#define EMS_TYPE_RC35StatusMessage_HC4 0x5C // is an automatic thermostat broadcast giving us temps on HC4

#define EMS_OFFSET_RC35StatusMessage_setpoint 2 // desired temp
#define EMS_OFFSET_RC35StatusMessage_curr 3     // current temp
#define EMS_OFFSET_RC35StatusMessage_mode 1     // day mode, also summer on RC3's
#define EMS_OFFSET_RC35StatusMessage_mode1 0    // for holiday mode


#define EMS_TYPE_RC35Set_HC1 0x3D // for setting values like temp and mode (Working mode HC1)
#define EMS_TYPE_RC35Set_HC2 0x47 // for setting values like temp and mode (Working mode HC2)
#define EMS_TYPE_RC35Set_HC3 0x51 // for setting values like temp and mode (Working mode HC3)
#define EMS_TYPE_RC35Set_HC4 0x5B // for setting values like temp and mode (Working mode HC4)

#define EMS_OFFSET_RC35Set_mode 7             // position of thermostat mode
#define EMS_OFFSET_RC35Set_temp_day 2         // position of thermostat setpoint temperature for day time
#define EMS_OFFSET_RC35Set_temp_night 1       // position of thermostat setpoint temperature for night time
#define EMS_OFFSET_RC35Set_temp_holiday 3     // temp during holiday 0x47
#define EMS_OFFSET_RC35Set_heatingtype 0      // floor heating = 3 0x47
#define EMS_OFFSET_RC35Set_circuitcalctemp 14 // calculated circuit temperature 0x48

// Easy specific
#define EMS_TYPE_EasyStatusMessage 0x0A          // reading values on an Easy Thermostat
#define EMS_OFFSET_EasyStatusMessage_setpoint 10 // setpoint temp
#define EMS_OFFSET_EasyStatusMessage_curr 8      // current temp

// RC1010, RC310 and RC300 specific (EMS Plus)
#define EMS_TYPE_RCPLUSStatusMessage_HC1 0x01A5       // is an automatic thermostat broadcast giving us temps for HC1
#define EMS_TYPE_RCPLUSStatusMessage_HC2 0x01A6       // is an automatic thermostat broadcast giving us temps for HC2
#define EMS_TYPE_RCPLUSStatusMessage_HC3 0x01A7       // is an automatic thermostat broadcast giving us temps for HC3
#define EMS_TYPE_RCPLUSStatusMessage_HC4 0x01A8       // is an automatic thermostat broadcast giving us temps for HC4
#define EMS_TYPE_RCPLUSStatusMode 0x1AF               // summer/winter mode
#define EMS_OFFSET_RCPLUSStatusMessage_mode 10        // thermostat mode (auto, manual)
#define EMS_OFFSET_RCPLUSStatusMessage_setpoint 3     // setpoint temp
#define EMS_OFFSET_RCPLUSStatusMessage_curr 0         // current temp
#define EMS_OFFSET_RCPLUSStatusMessage_currsetpoint 6 // target setpoint temp

#define EMS_TYPE_RCPLUSSet 0x01B9               // setpoint temp message and mode
#define EMS_OFFSET_RCPLUSSet_mode 0             // operation mode(Auto=0xFF, Manual=0x00)
#define EMS_OFFSET_RCPLUSSet_temp_comfort3 1    // comfort3 level
#define EMS_OFFSET_RCPLUSSet_temp_comfort2 2    // comfort2 level
#define EMS_OFFSET_RCPLUSSet_temp_comfort1 3    // comfort1 level
#define EMS_OFFSET_RCPLUSSet_temp_eco 4         // eco level
#define EMS_OFFSET_RCPLUSSet_temp_setpoint 8    // temp setpoint, when changing of templevel (in auto) value is reset and set to FF
#define EMS_OFFSET_RCPLUSSet_manual_setpoint 10 // manual setpoint

// Junkers FR10, FW100 (EMS Plus)
#define EMS_TYPE_JunkersStatusMessage 0x6F         // is an automatic thermostat broadcast giving us temps
#define EMS_OFFSET_JunkersStatusMessage_setpoint 2 // setpoint temp
#define EMS_OFFSET_JunkersStatusMessage_curr 4     // current temp

// MM100 (EMS Plus)
#define EMS_TYPE_MMPLUSStatusMessage_HC1 0x01D7     // mixer status HC1
#define EMS_TYPE_MMPLUSStatusMessage_HC2 0x01D8     // mixer status HC2
#define EMS_TYPE_MMPLUSStatusMessage_HC3 0x01D9     // mixer status HC3
#define EMS_TYPE_MMPLUSStatusMessage_HC4 0x01DA     // mixer status HC4
#define EMS_OFFSET_MMPLUSStatusMessage_flow_temp 3  // flow temperature


// Known EMS devices
typedef enum {
    EMS_MODEL_NONE, // unset
    EMS_MODEL_ALL,  // common for all devices

    // heatpump
    EMS_MODEL_HP,

    // solar module
    EMS_MODEL_SM,

    // boiler
    EMS_MODEL_UBA,

    // and the thermostats
    EMS_MODEL_ES73,
    EMS_MODEL_RC10,
    EMS_MODEL_RC20,
    EMS_MODEL_RC20F,
    EMS_MODEL_RC30,
    EMS_MODEL_RC35,
    EMS_MODEL_EASY,
    EMS_MODEL_RC300,
    EMS_MODEL_CW100,
    EMS_MODEL_1010,
    EMS_MODEL_OT,
    EMS_MODEL_FW100,
    EMS_MODEL_FR10,
    EMS_MODEL_FR100,
    EMS_MODEL_FR110,
    EMS_MODEL_FW120,

    // mixing devices
    EMS_MODEL_MM100

} _EMS_MODEL_ID;

// EMS types for known boilers. This list will be extended when new devices are recognized.
// The device_id is always 0x08
// format is PRODUCT ID, DESCRIPTION
const _Boiler_Device Boiler_Devices[] = {

    {72, "MC10 Module"},
    {123, "Buderus GBx72/Nefit Trendline/Junkers Cerapur"},
    {115, "Nefit Topline Compact/Buderus GB162"},
    {203, "Buderus Logamax U122/Junkers Cerapur"},
    {208, "Buderus Logamax plus/GB192"},
    {64, "Sieger BK15/Nefit Smartline/Buderus GB152"},
    {EMS_PRODUCTID_HEATRONIC, "Bosch Condens 2500/Junkers Heatronic 3"},
    {122, "Nefit Proline"},
    {170, "Buderus Logano GB212"},
    {172, "Nefit Enviline"}

};

/*
 * Known Solar Module types, device type 0x30
 * format is PRODUCT ID, DESCRIPTION
 */
const _SolarModule_Device SolarModule_Devices[] = {

    {EMS_PRODUCTID_SM10, "SM10 Solar Module"},
    {EMS_PRODUCTID_SM100, "SM100 Solar Module"},
    {EMS_PRODUCTID_ISM1, "Junkers ISM1 Solar Module"},
    {EMS_PRODUCTID_SM50, "SM50 Solar Module"}

};

const _Mixing_Device Mixing_Devices[] = {
    {160, 0x20, "MM100 Mixing Module"},
    {160, 0x21, "MM100 Mixing Module"},
};

// Other EMS devices which are not considered boilers, thermostats or solar modules
// format is PRODUCT ID, DEVICE ID, DESCRIPTION
const _Other_Device Other_Devices[] = {

    {71, 0x11, "WM10 Switch Module"},

    {69, 0x21, "MM10 Mixer Module"},
    {159, 0x21, "MM50 Mixing Module"},

    {68, 0x09, "BC10/RFM20 Receiver"},
    {190, 0x09, "BC10 Base Controller"},
    {114, 0x09, "BC10 Base Controller"},
    {125, 0x09, "BC25 Base Controller"},
    {169, 0x09, "BC40 Base Controller"},
    {152, 0x09, "Junkers Controller"},

    {205, 0x02, "Nefit Moduline Easy Connect"},
    {206, 0x02, "Bosch Easy Connect"},
    {171, 0x02, "EMS-OT OpenTherm converter"},

    {189, EMS_ID_GATEWAY, "Web Gateway KM200"}

};

// heatpump, device ID 0x38
// format is PRODUCT ID, DEVICE ID, DESCRIPTION
const _HeatPump_Device HeatPump_Devices[] = {{252, "HeatPump Module"}};

/*
 * Known thermostat types and their capabilities
 * format is MODEL_ID, PRODUCT ID, DEVICE ID, DESCRIPTION
 */
const _Thermostat_Device Thermostat_Devices[] = {

    // Easy devices - not currently supporting write operations
    {EMS_MODEL_EASY, 202, 0x18, "Logamatic TC100/Nefit Moduline Easy", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_EASY, 203, 0x18, "Bosch EasyControl CT200", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_CW100, 157, 0x18, "Bosch CW100", EMS_THERMOSTAT_WRITE_NO},

    // Buderus/Nefit
    {EMS_MODEL_RC10, 79, 0x17, "RC10/Moduline 100", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC20, 77, 0x17, "RC20/Moduline 300", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC20F, 93, 0x18, "RC20F", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC30, 78, 0x10, "RC30/Moduline 400", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC35, 86, 0x10, "RC35", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_RC300, 158, 0x10, "RC300/RC310/Moduline 3000", EMS_THERMOSTAT_WRITE_YES},
    {EMS_MODEL_1010, 165, 0x18, "RC100/Moduline 1010", EMS_THERMOSTAT_WRITE_NO},

    // Sieger
    {EMS_MODEL_ES73, 76, 0x10, "Sieger ES73", EMS_THERMOSTAT_WRITE_YES},

    // Junkers
    {EMS_MODEL_FW100, 105, 0x10, "Junkers FW100", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_FR10, 111, 0x18, "Junkers FR10", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_FR100, 105, 0x18, "Junkers FR100", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_FR110, 108, 0x18, "Junkers FR110", EMS_THERMOSTAT_WRITE_NO},
    {EMS_MODEL_FW120, 192, 0x10, "Junkers FW120", EMS_THERMOSTAT_WRITE_NO}

};
