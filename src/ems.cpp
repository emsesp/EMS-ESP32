/**
 * ems.cpp
 *
 * Handles all the processing of the EMS messages
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#include "ems.h"
#include "ems_devices.h"
#include "emsuart.h"
#include <Arduino.h>
#include <CircularBuffer.h> // https://github.com/rlogiacco/CircularBuffer
#include <MyESP.h>
#include <list> // std::list

#ifdef TESTS
#include "test_data.h"
uint8_t _TEST_DATA_max = ArraySize(TEST_DATA);
#endif

// myESP for logging to telnet and serial
#define myDebug(...) myESP.myDebug(__VA_ARGS__)
#define myDebug_P(...) myESP.myDebug_P(__VA_ARGS__)

_EMS_Sys_Status EMS_Sys_Status; // EMS Status

CircularBuffer<_EMS_TxTelegram, EMS_TX_TELEGRAM_QUEUE_MAX> EMS_TxQueue; // FIFO queue for Tx send buffer

// for storing all detected EMS devices
std::list<_Generic_Type> Devices;

// macros used in the _process* functions
#define _toByte(i) (EMS_RxTelegram->data[i])
#define _toShort(i) ((EMS_RxTelegram->data[i] << 8) + EMS_RxTelegram->data[i + 1])
#define _toLong(i) ((EMS_RxTelegram->data[i] << 16) + (EMS_RxTelegram->data[i + 1] << 8) + (EMS_RxTelegram->data[i + 2]))
#define _bitRead(i, bit) (((EMS_RxTelegram->data[i]) >> (bit)) & 0x01)

//
// process callbacks per type
//

// generic
void _process_Version(_EMS_RxTelegram * EMS_RxTelegram);

// EMS master/Boiler devices
void _process_UBAMonitorFast(_EMS_RxTelegram * EMS_RxTelegram);
void _process_UBAMonitorSlow(_EMS_RxTelegram * EMS_RxTelegram);
void _process_UBAMonitorWWMessage(_EMS_RxTelegram * EMS_RxTelegram);
void _process_UBAParameterWW(_EMS_RxTelegram * EMS_RxTelegram);
void _process_UBATotalUptimeMessage(_EMS_RxTelegram * EMS_RxTelegram);
void _process_UBAParametersMessage(_EMS_RxTelegram * EMS_RxTelegram);

void _process_SetPoints(_EMS_RxTelegram * EMS_RxTelegram);

// SM10
void _process_SM10Monitor(_EMS_RxTelegram * EMS_RxTelegram);

// SM100
void _process_SM100Monitor(_EMS_RxTelegram * EMS_RxTelegram);
void _process_SM100Status(_EMS_RxTelegram * EMS_RxTelegram);
void _process_SM100Status2(_EMS_RxTelegram * EMS_RxTelegram);
void _process_SM100Energy(_EMS_RxTelegram * EMS_RxTelegram);

// Common for most thermostats
void _process_RCTime(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RCOutdoorTempMessage(_EMS_RxTelegram * EMS_RxTelegram);

// RC10
void _process_RC10Set(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RC10StatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

// RC20
void _process_RC20Set(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RC20StatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

// RC30
void _process_RC30Set(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RC30StatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

// RC35
void _process_RC35Set(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RC35StatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

// Easy type devices like C100
void _process_EasyStatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

// RC1010, RC300, RC310
void _process_RCPLUSStatusMessage(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RCPLUSSetMessage(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RCPLUSStatusHeating(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RCPLUSStatusHeating(_EMS_RxTelegram * EMS_RxTelegram);
void _process_RCPLUSStatusMode(_EMS_RxTelegram * EMS_RxTelegram);

// Junkers FR10
void _process_FR10StatusMessage(_EMS_RxTelegram * EMS_RxTelegram);

/**
 * Recognized EMS types and the functions they call to process the telegrams
 * Format: MODEL ID, TYPE ID, Description, function, emsplus
 */
const _EMS_Type EMS_Types[] = {

    // common
    {EMS_MODEL_ALL, EMS_TYPE_Version, "Version", _process_Version},

    // Boiler commands
    {EMS_MODEL_UBA, EMS_TYPE_UBAMonitorFast, "UBAMonitorFast", _process_UBAMonitorFast},
    {EMS_MODEL_UBA, EMS_TYPE_UBAMonitorSlow, "UBAMonitorSlow", _process_UBAMonitorSlow},
    {EMS_MODEL_UBA, EMS_TYPE_UBAMonitorWWMessage, "UBAMonitorWWMessage", _process_UBAMonitorWWMessage},
    {EMS_MODEL_UBA, EMS_TYPE_UBAParameterWW, "UBAParameterWW", _process_UBAParameterWW},
    {EMS_MODEL_UBA, EMS_TYPE_UBATotalUptimeMessage, "UBATotalUptimeMessage", _process_UBATotalUptimeMessage},
    {EMS_MODEL_UBA, EMS_TYPE_UBAMaintenanceSettingsMessage, "UBAMaintenanceSettingsMessage", NULL},
    {EMS_MODEL_UBA, EMS_TYPE_UBAParametersMessage, "UBAParametersMessage", _process_UBAParametersMessage},
    {EMS_MODEL_UBA, EMS_TYPE_UBASetPoints, "UBASetPoints", _process_SetPoints},

    // Other devices
    {EMS_MODEL_OTHER, EMS_TYPE_SM10Monitor, "SM10Monitor", _process_SM10Monitor},
    {EMS_MODEL_OTHER, EMS_TYPE_SM100Monitor, "SM100Monitor", _process_SM100Monitor},
    {EMS_MODEL_OTHER, EMS_TYPE_SM100Status, "SM100Status", _process_SM100Status},
    {EMS_MODEL_OTHER, EMS_TYPE_SM100Status2, "SM100Status2", _process_SM100Status2},
    {EMS_MODEL_OTHER, EMS_TYPE_SM100Energy, "SM100Energy", _process_SM100Energy},

    // RC10
    {EMS_MODEL_RC10, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_RC10, EMS_TYPE_RC10Set, "RC10Set", _process_RC10Set},
    {EMS_MODEL_RC10, EMS_TYPE_RC10StatusMessage, "RC10StatusMessage", _process_RC10StatusMessage},

    // RC20 and RC20F
    {EMS_MODEL_RC20, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_MODEL_RC20, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_RC20, EMS_TYPE_RC20Set, "RC20Set", _process_RC20Set},
    {EMS_MODEL_RC20, EMS_TYPE_RC20StatusMessage, "RC20StatusMessage", _process_RC20StatusMessage},

    {EMS_MODEL_RC20F, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_MODEL_RC20F, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_RC20F, EMS_TYPE_RC20Set, "RC20Set", _process_RC20Set},
    {EMS_MODEL_RC20F, EMS_TYPE_RC20StatusMessage, "RC20StatusMessage", _process_RC20StatusMessage},

    // RC30
    {EMS_MODEL_RC30, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_MODEL_RC30, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_RC30, EMS_TYPE_RC30Set, "RC30Set", _process_RC30Set},
    {EMS_MODEL_RC30, EMS_TYPE_RC30StatusMessage, "RC30StatusMessage", _process_RC30StatusMessage},

    // RC35
    {EMS_MODEL_RC35, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_MODEL_RC35, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_RC35, EMS_TYPE_RC35Set_HC1, "RC35Set_HC1", _process_RC35Set},
    {EMS_MODEL_RC35, EMS_TYPE_RC35StatusMessage_HC1, "RC35StatusMessage_HC1", _process_RC35StatusMessage},
    {EMS_MODEL_RC35, EMS_TYPE_RC35Set_HC2, "RC35Set_HC2", _process_RC35Set},
    {EMS_MODEL_RC35, EMS_TYPE_RC35StatusMessage_HC2, "RC35StatusMessage_HC2", _process_RC35StatusMessage},

    // ES73
    {EMS_MODEL_ES73, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_MODEL_ES73, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_MODEL_ES73, EMS_TYPE_RC35Set_HC1, "RC35Set", _process_RC35Set},
    {EMS_MODEL_ES73, EMS_TYPE_RC35StatusMessage_HC1, "RC35StatusMessage", _process_RC35StatusMessage},

    // Easy
    {EMS_MODEL_EASY, EMS_TYPE_EasyStatusMessage, "EasyStatusMessage", _process_EasyStatusMessage},
    {EMS_MODEL_BOSCHEASY, EMS_TYPE_EasyStatusMessage, "EasyStatusMessage", _process_EasyStatusMessage},

    // Nefit 1010, RC300, RC310 (EMS Plus)
    {EMS_MODEL_ALL, EMS_TYPE_RCPLUSStatusMessage, "RCPLUSStatusMessage", _process_RCPLUSStatusMessage},
    {EMS_MODEL_ALL, EMS_TYPE_RCPLUSSet, "RCPLUSSetMessage", _process_RCPLUSSetMessage},
    {EMS_MODEL_ALL, EMS_TYPE_RCPLUSStatusHeating, "RCPLUSStatusHeating", _process_RCPLUSStatusHeating},
    {EMS_MODEL_ALL, EMS_TYPE_RCPLUSStatusMode, "RCPLUSStatusMode", _process_RCPLUSStatusMode},

    // Junkers FR10
    {EMS_MODEL_ALL, EMS_TYPE_FR10StatusMessage, "FR10StatusMessage", _process_FR10StatusMessage}


};

// calculate sizes of arrays at compile
uint8_t _EMS_Types_max        = ArraySize(EMS_Types);        // number of defined types
uint8_t _Boiler_Types_max     = ArraySize(Boiler_Types);     // number of boiler models
uint8_t _Other_Types_max      = ArraySize(Other_Types);      // number of other ems devices
uint8_t _Thermostat_Types_max = ArraySize(Thermostat_Types); // number of defined thermostat types

// these structs contain the data we store from the Boiler and Thermostat
_EMS_Boiler     EMS_Boiler;     // for boiler
_EMS_Thermostat EMS_Thermostat; // for thermostat
_EMS_Other      EMS_Other;      // for other known EMS devices

// CRC lookup table with poly 12 for faster checking
const uint8_t ems_crc_table[] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26,
                                 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E,
                                 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76,
                                 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E,
                                 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6,
                                 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
                                 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE, 0x19, 0x1B, 0x1D, 0x1F, 0x11, 0x13, 0x15, 0x17, 0x09, 0x0B, 0x0D, 0x0F,
                                 0x01, 0x03, 0x05, 0x07, 0x39, 0x3B, 0x3D, 0x3F, 0x31, 0x33, 0x35, 0x37, 0x29, 0x2B, 0x2D, 0x2F, 0x21, 0x23, 0x25, 0x27,
                                 0x59, 0x5B, 0x5D, 0x5F, 0x51, 0x53, 0x55, 0x57, 0x49, 0x4B, 0x4D, 0x4F, 0x41, 0x43, 0x45, 0x47, 0x79, 0x7B, 0x7D, 0x7F,
                                 0x71, 0x73, 0x75, 0x77, 0x69, 0x6B, 0x6D, 0x6F, 0x61, 0x63, 0x65, 0x67, 0x99, 0x9B, 0x9D, 0x9F, 0x91, 0x93, 0x95, 0x97,
                                 0x89, 0x8B, 0x8D, 0x8F, 0x81, 0x83, 0x85, 0x87, 0xB9, 0xBB, 0xBD, 0xBF, 0xB1, 0xB3, 0xB5, 0xB7, 0xA9, 0xAB, 0xAD, 0xAF,
                                 0xA1, 0xA3, 0xA5, 0xA7, 0xD9, 0xDB, 0xDD, 0xDF, 0xD1, 0xD3, 0xD5, 0xD7, 0xC9, 0xCB, 0xCD, 0xCF, 0xC1, 0xC3, 0xC5, 0xC7,
                                 0xF9, 0xFB, 0xFD, 0xFF, 0xF1, 0xF3, 0xF5, 0xF7, 0xE9, 0xEB, 0xED, 0xEF, 0xE1, 0xE3, 0xE5, 0xE7};

const uint8_t  TX_WRITE_TIMEOUT_COUNT = 2;     // 3 retries before timeout
const uint32_t EMS_BUS_TIMEOUT        = 15000; // timeout in ms before recognizing the ems bus is offline (15 seconds)
const uint32_t EMS_POLL_TIMEOUT       = 5000;  // timeout in ms before recognizing the ems bus is offline (5 seconds)

// init stats and counters and buffers
// uses -255 or 255 for values that haven't been set yet (EMS_VALUE_INT_NOTSET and EMS_VALUE_FLOAT_NOTSET)
void ems_init() {
    // overall status
    EMS_Sys_Status.emsRxPgks        = 0;
    EMS_Sys_Status.emsTxPkgs        = 0;
    EMS_Sys_Status.emxCrcErr        = 0;
    EMS_Sys_Status.emsRxStatus      = EMS_RX_STATUS_IDLE;
    EMS_Sys_Status.emsTxStatus      = EMS_TX_STATUS_IDLE;
    EMS_Sys_Status.emsRefreshed     = false;
    EMS_Sys_Status.emsPollEnabled   = false; // start up with Poll disabled
    EMS_Sys_Status.emsBusConnected  = false;
    EMS_Sys_Status.emsRxTimestamp   = 0;
    EMS_Sys_Status.emsTxCapable     = false;
    EMS_Sys_Status.emsTxDisabled    = false;
    EMS_Sys_Status.emsPollFrequency = 0;
    EMS_Sys_Status.txRetryCount     = 0;
    EMS_Sys_Status.emsTxDelay       = 0;

    // thermostat
    EMS_Thermostat.setpoint_roomTemp = EMS_VALUE_SHORT_NOTSET;
    EMS_Thermostat.curr_roomTemp     = EMS_VALUE_SHORT_NOTSET;
    EMS_Thermostat.hour              = 0;
    EMS_Thermostat.minute            = 0;
    EMS_Thermostat.second            = 0;
    EMS_Thermostat.day               = 0;
    EMS_Thermostat.month             = 0;
    EMS_Thermostat.year              = 0;
    EMS_Thermostat.mode              = 255; // dummy value
    EMS_Thermostat.day_mode          = 255; // dummy value
    EMS_Thermostat.device_id         = EMS_ID_NONE;
    EMS_Thermostat.write_supported   = false;
    EMS_Thermostat.hc                = 1;                    // default heating circuit is 1
    EMS_Thermostat.daytemp           = EMS_VALUE_INT_NOTSET; // 0x47 byte
    EMS_Thermostat.nighttemp         = EMS_VALUE_INT_NOTSET; // 0x47 byte
    EMS_Thermostat.holidaytemp       = EMS_VALUE_INT_NOTSET; // 0x47 byte
    EMS_Thermostat.heatingtype       = EMS_VALUE_INT_NOTSET; // 0x47 byte floor heating = 3
    EMS_Thermostat.circuitcalctemp   = EMS_VALUE_INT_NOTSET; // 0x48 byte 14


    // UBAParameterWW
    EMS_Boiler.wWActivated   = EMS_VALUE_INT_NOTSET; // Warm Water activated
    EMS_Boiler.wWSelTemp     = EMS_VALUE_INT_NOTSET; // Warm Water selected temperature
    EMS_Boiler.wWCircPump    = EMS_VALUE_INT_NOTSET; // Warm Water circulation pump available
    EMS_Boiler.wWDesiredTemp = EMS_VALUE_INT_NOTSET; // Warm Water desired temperature to prevent infection
    EMS_Boiler.wWComfort     = EMS_VALUE_INT_NOTSET;

    // UBAMonitorFast
    EMS_Boiler.selFlowTemp = EMS_VALUE_INT_NOTSET;   // Selected flow temperature
    EMS_Boiler.curFlowTemp = EMS_VALUE_SHORT_NOTSET; // Current flow temperature
    EMS_Boiler.retTemp     = EMS_VALUE_SHORT_NOTSET; // Return temperature
    EMS_Boiler.burnGas     = EMS_VALUE_INT_NOTSET;   // Gas on/off
    EMS_Boiler.fanWork     = EMS_VALUE_INT_NOTSET;   // Fan on/off
    EMS_Boiler.ignWork     = EMS_VALUE_INT_NOTSET;   // Ignition on/off
    EMS_Boiler.heatPmp     = EMS_VALUE_INT_NOTSET;   // Boiler pump on/off
    EMS_Boiler.wWHeat      = EMS_VALUE_INT_NOTSET;   // 3-way valve on WW
    EMS_Boiler.wWCirc      = EMS_VALUE_INT_NOTSET;   // Circulation on/off
    EMS_Boiler.selBurnPow  = EMS_VALUE_INT_NOTSET;   // Burner max power
    EMS_Boiler.curBurnPow  = EMS_VALUE_INT_NOTSET;   // Burner current power
    EMS_Boiler.flameCurr   = EMS_VALUE_SHORT_NOTSET; // Flame current in micro amps
    EMS_Boiler.sysPress    = EMS_VALUE_INT_NOTSET;   // System pressure
    strlcpy(EMS_Boiler.serviceCodeChar, "??", sizeof(EMS_Boiler.serviceCodeChar));
    EMS_Boiler.serviceCode = EMS_VALUE_SHORT_NOTSET;

    // UBAMonitorSlow
    EMS_Boiler.extTemp     = EMS_VALUE_SHORT_NOTSET; // Outside temperature
    EMS_Boiler.boilTemp    = EMS_VALUE_SHORT_NOTSET; // Boiler temperature
    EMS_Boiler.pumpMod     = EMS_VALUE_INT_NOTSET;   // Pump modulation
    EMS_Boiler.burnStarts  = EMS_VALUE_LONG_NOTSET;  // # burner restarts
    EMS_Boiler.burnWorkMin = EMS_VALUE_LONG_NOTSET;  // Total burner operating time
    EMS_Boiler.heatWorkMin = EMS_VALUE_LONG_NOTSET;  // Total heat operating time

    // UBAMonitorWWMessage
    EMS_Boiler.wWCurTmp  = EMS_VALUE_SHORT_NOTSET; // Warm Water current temperature
    EMS_Boiler.wWStarts  = EMS_VALUE_LONG_NOTSET;  // Warm Water # starts
    EMS_Boiler.wWWorkM   = EMS_VALUE_LONG_NOTSET;  // Warm Water # minutes
    EMS_Boiler.wWOneTime = EMS_VALUE_INT_NOTSET;   // Warm Water one time function on/off
    EMS_Boiler.wWCurFlow = EMS_VALUE_INT_NOTSET;

    // UBATotalUptimeMessage
    EMS_Boiler.UBAuptime = EMS_VALUE_LONG_NOTSET; // Total UBA working hours

    // UBAParametersMessage
    EMS_Boiler.heating_temp = EMS_VALUE_INT_NOTSET; // Heating temperature setting on the boiler
    EMS_Boiler.pump_mod_max = EMS_VALUE_INT_NOTSET; // Boiler circuit pump modulation max. power
    EMS_Boiler.pump_mod_min = EMS_VALUE_INT_NOTSET; // Boiler circuit pump modulation min. power

    // Other EMS devices values
    EMS_Other.SMcollectorTemp  = EMS_VALUE_SHORT_NOTSET; // collector temp from SM10/SM100
    EMS_Other.SMbottomTemp     = EMS_VALUE_SHORT_NOTSET; // bottom temp from SM10/SM100
    EMS_Other.SMpumpModulation = EMS_VALUE_INT_NOTSET;   // modulation solar pump SM10/SM100
    EMS_Other.SMpump           = EMS_VALUE_INT_NOTSET;   // pump active
    EMS_Other.SMEnergyLastHour = EMS_VALUE_SHORT_NOTSET;
    EMS_Other.SMEnergyToday    = EMS_VALUE_SHORT_NOTSET;
    EMS_Other.SMEnergyTotal    = EMS_VALUE_SHORT_NOTSET;

    // calculated values
    EMS_Boiler.tapwaterActive = EMS_VALUE_INT_NOTSET; // Hot tap water is on/off
    EMS_Boiler.heatingActive  = EMS_VALUE_INT_NOTSET; // Central heating is on/off

    // set boiler type
    EMS_Boiler.product_id = EMS_ID_NONE;
    strlcpy(EMS_Boiler.version, "?", sizeof(EMS_Boiler.version));

    // set thermostat model
    EMS_Thermostat.model_id   = EMS_MODEL_NONE;
    EMS_Thermostat.product_id = EMS_ID_NONE;
    strlcpy(EMS_Thermostat.version, "?", sizeof(EMS_Thermostat.version));

    // set other types
    EMS_Other.SM = false;

    // default logging is none
    ems_setLogging(EMS_SYS_LOGGING_DEFAULT);
}

// Getters and Setters for parameters
void ems_setPoll(bool b) {
    EMS_Sys_Status.emsPollEnabled = b;
    myDebug_P(PSTR("EMS Bus Poll is set to %s"), EMS_Sys_Status.emsPollEnabled ? "enabled" : "disabled");
}

bool ems_getPoll() {
    return EMS_Sys_Status.emsPollEnabled;
}

void ems_setTxDelay(uint8_t delay) {
    EMS_Sys_Status.emsTxDelay = delay;
}

uint8_t ems_getTxDelay() {
    return EMS_Sys_Status.emsTxDelay;
}

bool ems_getEmsRefreshed() {
    return EMS_Sys_Status.emsRefreshed;
}

void ems_setEmsRefreshed(bool b) {
    EMS_Sys_Status.emsRefreshed = b;
}

void ems_setThermostatHC(uint8_t hc) {
    EMS_Thermostat.hc = hc;
}

bool ems_getBoilerEnabled() {
    return (EMS_Boiler.device_id != EMS_ID_NONE);
}

bool ems_getThermostatEnabled() {
    return (EMS_Thermostat.device_id != EMS_ID_NONE);
}

uint8_t ems_getThermostatModel() {
    return (EMS_Thermostat.model_id);
}

void ems_setTxDisabled(bool b) {
    EMS_Sys_Status.emsTxDisabled = b;
}

uint32_t ems_getPollFrequency() {
    return EMS_Sys_Status.emsPollFrequency;
}

bool ems_getTxCapable() {
    if ((EMS_Sys_Status.emsPollFrequency == 0) || (EMS_Sys_Status.emsPollFrequency > EMS_POLL_TIMEOUT)) {
        EMS_Sys_Status.emsTxCapable = false;
    }
    return EMS_Sys_Status.emsTxCapable;
}

bool ems_getBusConnected() {
    if ((millis() - EMS_Sys_Status.emsRxTimestamp) > EMS_BUS_TIMEOUT) {
        EMS_Sys_Status.emsBusConnected = false;
    }
    return EMS_Sys_Status.emsBusConnected;
}

_EMS_SYS_LOGGING ems_getLogging() {
    return EMS_Sys_Status.emsLogging;
}

void ems_setLogging(_EMS_SYS_LOGGING loglevel) {
    if (loglevel <= EMS_SYS_LOGGING_VERBOSE) {
        EMS_Sys_Status.emsLogging = loglevel;
        if (loglevel == EMS_SYS_LOGGING_NONE) {
            myDebug_P(PSTR("System Logging set to None"));
        } else if (loglevel == EMS_SYS_LOGGING_BASIC) {
            myDebug_P(PSTR("System Logging set to Basic"));
        } else if (loglevel == EMS_SYS_LOGGING_VERBOSE) {
            myDebug_P(PSTR("System Logging set to Verbose"));
        } else if (loglevel == EMS_SYS_LOGGING_THERMOSTAT) {
            myDebug_P(PSTR("System Logging set to Thermostat only"));
        } else if (loglevel == EMS_SYS_LOGGING_RAW) {
            myDebug_P(PSTR("System Logging set to Raw mode"));
        }
    }
}

/**
 * Calculate CRC checksum using lookup table for speed
 * len is length of all the data in bytes (including the header & CRC byte at end)
 */
uint8_t _crcCalculator(uint8_t * data, uint8_t len) {
    uint8_t crc = 0;

    // read data and stop before the CRC
    for (uint8_t i = 0; i < len - 1; i++) {
        crc = ems_crc_table[crc];
        crc ^= data[i];
    }

    return crc;
}

// like itoa but for hex, and quicker
char * _hextoa(uint8_t value, char * buffer) {
    char * p    = buffer;
    byte   nib1 = (value >> 4) & 0x0F;
    byte   nib2 = (value >> 0) & 0x0F;
    *p++        = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    *p++        = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    *p          = '\0'; // null terminate just in case
    return buffer;
}

// for decimals 0 to 99, printed as a string
char * _smallitoa(uint8_t value, char * buffer) {
    buffer[0] = ((value / 10) == 0) ? '0' : (value / 10) + '0';
    buffer[1] = (value % 10) + '0';
    buffer[2] = '\0';
    return buffer;
}

/* for decimals 0 to 999, printed as a string
 * From @nomis
 */
char * _smallitoa3(uint16_t value, char * buffer) {
    buffer[0] = ((value / 100) == 0) ? '0' : (value / 100) + '0';
    buffer[1] = (((value % 100) / 10) == 0) ? '0' : ((value % 100) / 10) + '0';
    buffer[2] = (value % 10) + '0';
    buffer[3] = '\0';
    return buffer;
}

/**
 * Find the pointer to the EMS_Types array for a given type ID
 * or -1 if not found
 */
int _ems_findType(uint8_t type) {
    uint8_t i         = 0;
    bool    typeFound = false;
    // scan through known ID types
    while (i < _EMS_Types_max) {
        if (EMS_Types[i].type == type) {
            typeFound = true; // we have a match
            break;
        }
        i++;
    }

    return (typeFound ? i : -1);
}

/**
 * debug print a telegram to telnet/serial including the CRC
 */
void _debugPrintTelegram(const char * prefix, _EMS_RxTelegram * EMS_RxTelegram, const char * color, bool raw = false) {
    char      output_str[200] = {0};
    char      buffer[16]      = {0};
    uint8_t * data            = EMS_RxTelegram->telegram;
    uint8_t   data_len        = EMS_RxTelegram->data_length; // length of data block
    uint8_t   length          = EMS_RxTelegram->length;      // includes CRC

    strlcpy(output_str, "(", sizeof(output_str));
    strlcat(output_str, COLOR_CYAN, sizeof(output_str));
    strlcat(output_str, _smallitoa((uint8_t)((EMS_RxTelegram->timestamp / 3600000) % 24), buffer), sizeof(output_str));
    strlcat(output_str, ":", sizeof(output_str));
    strlcat(output_str, _smallitoa((uint8_t)((EMS_RxTelegram->timestamp / 60000) % 60), buffer), sizeof(output_str));
    strlcat(output_str, ":", sizeof(output_str));
    strlcat(output_str, _smallitoa((uint8_t)((EMS_RxTelegram->timestamp / 1000) % 60), buffer), sizeof(output_str));
    strlcat(output_str, ".", sizeof(output_str));
    strlcat(output_str, _smallitoa3(EMS_RxTelegram->timestamp % 1000, buffer), sizeof(output_str));
    strlcat(output_str, COLOR_RESET, sizeof(output_str));
    strlcat(output_str, ") ", sizeof(output_str));

    strlcat(output_str, color, sizeof(output_str));
    strlcat(output_str, prefix, sizeof(output_str));

    if (!raw) {
        strlcat(output_str, " telegram: ", sizeof(output_str));
    }

    for (int i = 0; i < (length - 1); i++) {
        strlcat(output_str, _hextoa(data[i], buffer), sizeof(output_str));
        strlcat(output_str, " ", sizeof(output_str)); // add space
    }

    if (raw) {
        strlcat(output_str, _hextoa(data[length - 1], buffer), sizeof(output_str));
    } else {
        strlcat(output_str, "(CRC=", sizeof(output_str));
        strlcat(output_str, _hextoa(data[length - 1], buffer), sizeof(output_str));
        strlcat(output_str, ")", sizeof(output_str));

        // print number of data bytes only if its a valid telegram
        if (data_len) {
            strlcat(output_str, " #data=", sizeof(output_str));
            strlcat(output_str, itoa(data_len, buffer, 10), sizeof(output_str));
        }
    }

    strlcat(output_str, COLOR_RESET, sizeof(output_str));

    myDebug(output_str);
}

/**
 * send the contents of the Tx buffer to the UART
 * we take telegram from the queue and send it, but don't remove it until later when its confirmed successful
 */
void _ems_sendTelegram() {
    // check if we have something in the queue to send
    if (EMS_TxQueue.isEmpty()) {
        return;
    }

    // get the first in the queue, which is at the head
    // we don't remove from the queue yet
    _EMS_TxTelegram EMS_TxTelegram = EMS_TxQueue.first();

    // if there is no destination, also delete it from the queue
    if (EMS_TxTelegram.dest == EMS_ID_NONE) {
        EMS_TxQueue.shift(); // remove from queue
        return;
    }

    // if we're in raw mode just fire and forget
    // e.g. send 0B 88 02 00 20
    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_RAW) {
        _EMS_RxTelegram EMS_RxTelegram; // create new Rx object

        EMS_TxTelegram.data[EMS_TxTelegram.length - 1] = _crcCalculator(EMS_TxTelegram.data, EMS_TxTelegram.length); // add the CRC
        EMS_RxTelegram.length                          = EMS_TxTelegram.length;                                      // full length of telegram
        EMS_RxTelegram.telegram                        = EMS_TxTelegram.data;
        EMS_RxTelegram.timestamp                       = millis();               // now
        _debugPrintTelegram("Sending raw: ", &EMS_RxTelegram, COLOR_CYAN, true); // always show
        emsuart_tx_buffer(EMS_TxTelegram.data, EMS_TxTelegram.length);           // send the telegram to the UART Tx
        EMS_TxQueue.shift();                                                     // remove from queue
        return;
    }

    // create header
    EMS_TxTelegram.data[0] = EMS_ID_ME; // src
    // dest
    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
        EMS_TxTelegram.data[1] = EMS_TxTelegram.dest;
    } else {
        // for a READ or VALIDATE
        EMS_TxTelegram.data[1] = EMS_TxTelegram.dest | 0x80; // read has 8th bit set
    }
    EMS_TxTelegram.data[2] = EMS_TxTelegram.type;   // type
    EMS_TxTelegram.data[3] = EMS_TxTelegram.offset; // offset

    // see if it has data, add the single data value byte
    // otherwise leave it alone and assume the data has been pre-populated
    if (EMS_TxTelegram.length == EMS_MIN_TELEGRAM_LENGTH) {
        // for reading this is #bytes we want to read (the size)
        // for writing its the value we want to write
        EMS_TxTelegram.data[4] = EMS_TxTelegram.dataValue;
    }
    // finally calculate CRC and add it to the end
    uint8_t crc                                    = _crcCalculator(EMS_TxTelegram.data, EMS_TxTelegram.length);
    EMS_TxTelegram.data[EMS_TxTelegram.length - 1] = crc;

    // print debug info
    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
        char s[64] = {0};
        if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
            snprintf(s, sizeof(s), "Sending write of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
            snprintf(s, sizeof(s), "Sending read of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
            snprintf(s, sizeof(s), "Sending validate of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        }

        _EMS_RxTelegram EMS_RxTelegram;
        EMS_RxTelegram.length    = EMS_TxTelegram.length; // complete length of telegram
        EMS_RxTelegram.telegram  = EMS_TxTelegram.data;
        EMS_RxTelegram.timestamp = millis(); // now
        _debugPrintTelegram(s, &EMS_RxTelegram, COLOR_CYAN);
    }

    // send the telegram to the UART Tx
    emsuart_tx_buffer(EMS_TxTelegram.data, EMS_TxTelegram.length);

    EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_WAIT;
}


/**
 * Takes the last write command and turns into a validate request
 * placing it on the queue
 */
void _createValidate() {
    if (EMS_TxQueue.isEmpty()) {
        return;
    }

    // release the Tx lock
    EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_IDLE;

    // get the first in the queue, which is at the head
    _EMS_TxTelegram EMS_TxTelegram = EMS_TxQueue.first();

    // safety check: only do a validate after a write and when we have a type to validate
    if ((EMS_TxTelegram.action != EMS_TX_TELEGRAM_WRITE) || (EMS_TxTelegram.type_validate == EMS_ID_NONE)) {
        EMS_TxQueue.shift(); // remove from queue
        return;
    }

    // create a new Telegram copying from the last write
    _EMS_TxTelegram new_EMS_TxTelegram;
    new_EMS_TxTelegram.action = EMS_TX_TELEGRAM_VALIDATE;

    // copy old Write record
    new_EMS_TxTelegram.type_validate      = EMS_TxTelegram.type_validate;
    new_EMS_TxTelegram.dest               = EMS_TxTelegram.dest;
    new_EMS_TxTelegram.type               = EMS_TxTelegram.type;
    new_EMS_TxTelegram.comparisonValue    = EMS_TxTelegram.comparisonValue;
    new_EMS_TxTelegram.comparisonPostRead = EMS_TxTelegram.comparisonPostRead;
    new_EMS_TxTelegram.comparisonOffset   = EMS_TxTelegram.comparisonOffset;

    // this is what is different
    new_EMS_TxTelegram.offset    = EMS_TxTelegram.comparisonOffset; // location of byte to fetch
    new_EMS_TxTelegram.dataValue = 1;                               // fetch single byte
    new_EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;         // is always 6 bytes long (including CRC at end)

    // remove old telegram from queue and add this new read one
    EMS_TxQueue.shift();                     // remove from queue
    EMS_TxQueue.unshift(new_EMS_TxTelegram); // add back to queue making it first to be picked up next (FIFO)
}

/**
 * Entry point triggered by an interrupt in emsuart.cpp
 * length is size of all the telegram bytes including the CRC, excluding the BRK at the end
 * Read commands are asynchronous as they're handled by the interrupt
 * When a telegram is processed we forcefully erase it from the stack to prevent overflow
 */
void ems_parseTelegram(uint8_t * telegram, uint8_t length) {
    if ((length != 0) && (telegram[0] != 0x00)) {
        _ems_readTelegram(telegram, length);
    }

    // clear the Rx buffer just be safe and prevent duplicates
    memset(telegram, 0, EMS_MAXBUFFERSIZE);
}

/**
 * the main logic that parses the telegram message
 * When we receive a Poll Request we need to send any Tx packages quickly within a 200ms window
 * length is total number of bytes of the telegram including the CRC byte at the end (if it exists)
 */
void _ems_readTelegram(uint8_t * telegram, uint8_t length) {
    // create the Rx package
    static _EMS_RxTelegram EMS_RxTelegram;
    static uint32_t        _last_emsPollFrequency = 0;
    EMS_RxTelegram.telegram                       = telegram;
    EMS_RxTelegram.timestamp                      = millis();
    EMS_RxTelegram.length                         = length;

    // check if we just received a single byte
    // it could well be a Poll request from the boiler for us, which will have a value of 0x8B (0x0B | 0x80)
    // or either a return code like 0x01 or 0x04 from the last Write command
    if (length == 1) {
        uint32_t timenow_microsecs      = micros();
        EMS_Sys_Status.emsPollFrequency = (timenow_microsecs - _last_emsPollFrequency);
        _last_emsPollFrequency          = timenow_microsecs;

        // check first for a Poll for us
        uint8_t value = telegram[0]; // 1st byte of data package

        if ((value & 0x7F) == EMS_ID_ME) {
            EMS_Sys_Status.emsTxCapable = true;

            // do we have something to send thats waiting in the Tx queue?
            // if so send it if the Queue is not in a wait state
            if ((!EMS_TxQueue.isEmpty()) && (EMS_Sys_Status.emsTxStatus == EMS_TX_STATUS_IDLE)) {
                _ems_sendTelegram(); // perform the read/write command immediately
            } else {
                // nothing to send so just send a poll acknowledgement back
                if (EMS_Sys_Status.emsPollEnabled) {
                    emsuart_tx_poll();
                }
            }
        } else if (EMS_Sys_Status.emsTxStatus == EMS_TX_STATUS_WAIT) {
            // this may be a single byte 01 (success) or 04 (error) from a recent write command?
            if (value == EMS_TX_SUCCESS) {
                EMS_Sys_Status.emsTxPkgs++;
                // got a success 01. Send a validate to check the value of the last write
                emsuart_tx_poll(); // send a poll to free the EMS bus
                _createValidate(); // create a validate Tx request (if needed)
            } else if (value == EMS_TX_ERROR) {
                // last write failed (04), delete it from queue and dont bother to retry
                if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
                    myDebug_P(PSTR("** Write command failed from host"));
                }
                emsuart_tx_poll(); // send a poll to free the EMS bus
                _removeTxQueue();  // remove from queue
            }
        }

        return; // all done here
    }

    // ignore anything that doesn't resemble a proper telegram package
    // minimal is 5 bytes, excluding CRC at the end
    if (length <= 4) {
        //_debugPrintTelegram("Noisy data:", &EMS_RxTelegram COLOR_RED);
        return;
    }

    // fill in the rest of the telegram
    EMS_RxTelegram.src    = telegram[0] & 0x7F; // removing 8th bit as we deal with both reads and writes here
    EMS_RxTelegram.dest   = telegram[1] & 0x7F; // remove 8th bit (don't care if read or write)
    EMS_RxTelegram.offset = telegram[3];        // offset is always 4th byte

    // determing if its normal ems or ems plus
    if (telegram[2] >= 0xF0) {
        // its EMS plus / EMS 2.0
        EMS_RxTelegram.emsplus = true;

        if (telegram[2] == 0xFF) {
            EMS_RxTelegram.type = (telegram[4] << 8) + telegram[5]; // is a long in bytes 5 & 6
            EMS_RxTelegram.data = telegram + 6;

            if (length <= 7) {
                EMS_RxTelegram.data_length = 0; // special broadcast on ems+ have no data values
            } else {
                EMS_RxTelegram.data_length = length - 7; // remove 6 byte header plus CRC
            }
        } else {
            // its F9 or F7
            uint8_t shift       = (telegram[4] != 0xFF); // true (1) if byte 4 is not 0xFF, then telegram is 1 byte longer
            EMS_RxTelegram.type = (telegram[5 + shift] << 8) + telegram[6 + shift];
            EMS_RxTelegram.data = telegram + 6 + shift; // there is a special byte after the typeID which we ignore for now
            if (length <= (9 + shift)) {
                EMS_RxTelegram.data_length = 0; // special broadcast on ems+ have no data values
            } else {
                EMS_RxTelegram.data_length = length - (9 + shift);
            }
        }
    } else {
        // Normal EMS 1.0
        EMS_RxTelegram.emsplus     = false;
        EMS_RxTelegram.type        = telegram[2]; // 3rd byte
        EMS_RxTelegram.data        = telegram + 4;
        EMS_RxTelegram.data_length = length - 5; // remove 4 bytes header plus CRC
    }

    // Assume at this point we have something that vaguely resembles a telegram in the format [src] [dest] [type] [offset] [data] [crc]
    // validate the CRC, if it's bad ignore it
    if (telegram[length - 1] != _crcCalculator(telegram, length)) {
        EMS_Sys_Status.emxCrcErr++;
        if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
            _debugPrintTelegram("Corrupt telegram: ", &EMS_RxTelegram, COLOR_RED, true);
        }
        return;
    }

    // if we are in raw logging mode then just print out the telegram as it is
    // but still continue to process it
    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_RAW) {
        _debugPrintTelegram("", &EMS_RxTelegram, COLOR_WHITE, true);
    }

    // here we know its a valid incoming telegram of at least 6 bytes
    // we use this to see if we always have a connection to the boiler, in case of drop outs
    EMS_Sys_Status.emsRxTimestamp  = EMS_RxTelegram.timestamp; // timestamp of last read
    EMS_Sys_Status.emsBusConnected = true;

    // now lets process it and see what to do next
    _processType(&EMS_RxTelegram);
}

/**
 * print the telegram
 */
void _printMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    // header info
    uint8_t  src  = EMS_RxTelegram->src;
    uint8_t  dest = EMS_RxTelegram->dest;
    uint16_t type = EMS_RxTelegram->type;

    char output_str[200] = {0};
    char buffer[16]      = {0};
    char color_s[20]     = {0};

    // source
    if (src == EMS_Boiler.device_id) {
        strlcpy(output_str, "Boiler", sizeof(output_str));
    } else if (src == EMS_Thermostat.device_id) {
        strlcpy(output_str, "Thermostat", sizeof(output_str));
    } else if (src == EMS_ID_SM) {
        strlcpy(output_str, "SM", sizeof(output_str));
    } else if (src == EMS_ID_GATEWAY) {
        strlcpy(output_str, "Gateway", sizeof(output_str));
    } else {
        strlcpy(output_str, "0x", sizeof(output_str));
        strlcat(output_str, _hextoa(src, buffer), sizeof(output_str));
    }

    strlcat(output_str, " -> ", sizeof(output_str));

    // destination
    if (dest == EMS_ID_ME) {
        strlcat(output_str, "me", sizeof(output_str));
        strlcpy(color_s, COLOR_YELLOW, sizeof(color_s));
    } else if (dest == EMS_ID_NONE) {
        strlcat(output_str, "all", sizeof(output_str));
        strlcpy(color_s, COLOR_GREEN, sizeof(color_s));
    } else if (dest == EMS_Boiler.device_id) {
        strlcat(output_str, "Boiler", sizeof(output_str));
        strlcpy(color_s, COLOR_MAGENTA, sizeof(color_s));
    } else if (dest == EMS_ID_SM) {
        strlcat(output_str, "SM", sizeof(output_str));
        strlcpy(color_s, COLOR_MAGENTA, sizeof(color_s));
    } else if (dest == EMS_ID_GATEWAY) {
        strlcat(output_str, "Gateway", sizeof(output_str));
        strlcpy(color_s, COLOR_MAGENTA, sizeof(color_s));
    } else if (dest == EMS_Thermostat.device_id) {
        strlcat(output_str, "Thermostat", sizeof(output_str));
        strlcpy(color_s, COLOR_MAGENTA, sizeof(color_s));
    } else {
        strlcat(output_str, "0x", sizeof(output_str));
        strlcat(output_str, _hextoa(dest, buffer), sizeof(output_str));
        strlcpy(color_s, COLOR_MAGENTA, sizeof(color_s));
    }

    // type
    strlcat(output_str, ", type 0x", sizeof(output_str));

    if (EMS_RxTelegram->emsplus) {
        strlcat(output_str, _hextoa(type >> 8, buffer), sizeof(output_str));
        strlcat(output_str, _hextoa(type & 0xFF, buffer), sizeof(output_str));
    } else {
        strlcat(output_str, _hextoa(type, buffer), sizeof(output_str));
    }

    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_THERMOSTAT) {
        // only print ones to/from thermostat if logging is set to thermostat only
        if ((src == EMS_Thermostat.device_id) || (dest == EMS_Thermostat.device_id)) {
            _debugPrintTelegram(output_str, EMS_RxTelegram, color_s);
        }
    } else {
        // always print
        _debugPrintTelegram(output_str, EMS_RxTelegram, color_s);
    }
}

/**
 * print detailed telegram
 * and then call its callback if there is one defined
 */
void _ems_processTelegram(_EMS_RxTelegram * EMS_RxTelegram) {
    // print out the telegram for verbose mode
    if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_THERMOSTAT) {
        _printMessage(EMS_RxTelegram);
    }

    // ignore telegrams that don't have any data
    if (EMS_RxTelegram->data_length == 0) {
        return;
    }

    // header
    uint8_t  dest = EMS_RxTelegram->dest;
    uint16_t type = EMS_RxTelegram->type;

    // see if we recognize the type first by scanning our known EMS types list
    bool    typeFound = false;
    uint8_t i         = 0;

    while (i < _EMS_Types_max) {
        if (EMS_Types[i].type == type) {
            // is it a broadcast or something sent to us?
            // we don't really care where it is from
            if ((dest == EMS_ID_NONE) || (dest == EMS_ID_ME)) {
                typeFound = true;
                break;
            }

            /*
            if ((EMS_Types[i].model_id == EMS_MODEL_ALL) || ((src == EMS_Boiler.device_id) || (src == EMS_Thermostat.device_id) || (src == EMS_ID_SM))) {
                typeFound = true;
                break;
            }
            */
        }
        i++;
    }

    // if it's a common type (across ems devices) or something specifically for us process it.
    // dest will be EMS_ID_NONE and offset 0x00 for a broadcast message
    if (typeFound) {
        if ((EMS_Types[i].processType_cb) != (void *)NULL) {
            // print non-verbose message
            if ((EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_BASIC) || (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE)) {
                myDebug_P(PSTR("<--- %s(0x%02X)"), EMS_Types[i].typeString, type);
            }
            // call callback function to process the telegram, only if there is data
            if (EMS_RxTelegram->emsplus) {
                // if EMS+ always proces it
                (void)EMS_Types[i].processType_cb(EMS_RxTelegram);
            } else {
                // only if the offset is 0 as we want to handle full telegrams and not partial
                if (EMS_RxTelegram->offset == EMS_ID_NONE) {
                    (void)EMS_Types[i].processType_cb(EMS_RxTelegram);
                }
            }
        }
    }

    EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_IDLE;
}

/**
 * Remove current Tx telegram from queue and release lock on Tx
 */
void _removeTxQueue() {
    if (!EMS_TxQueue.isEmpty()) {
        EMS_TxQueue.shift(); // remove item from top of the queue
    }
    EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_IDLE;
}

/**
 * deciphers the telegram packet, which has already been checked for valid CRC and has a complete header (min of 5 bytes)
 * length is only data bytes, excluding the BRK
 * We only remove from the Tx queue if the read or write was successful
 */
void _processType(_EMS_RxTelegram * EMS_RxTelegram) {
    uint8_t * telegram = EMS_RxTelegram->telegram;

    // if its an echo of ourselves from the master UBA, ignore
    if (EMS_RxTelegram->src == EMS_ID_ME) {
        // _debugPrintTelegram("echo:", EMS_RxTelegram, COLOR_WHITE);
        return;
    }

    // if its a broadcast and we didn't just send anything, process it and exit
    if (EMS_Sys_Status.emsTxStatus == EMS_TX_STATUS_IDLE) {
        _ems_processTelegram(EMS_RxTelegram);
        return;
    }

    // release the lock on the TxQueue
    EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_IDLE;

    // at this point we can assume Txstatus is EMS_TX_STATUS_WAIT so we just sent a read/write/validate
    // for READ, WRITE or VALIDATE the dest (telegram[1]) is always us, so check for this
    // and if not we probably didn't get any response so remove the last Tx from the queue and process the telegram anyway
    if ((telegram[1] & 0x7F) != EMS_ID_ME) {
        _removeTxQueue();
        _ems_processTelegram(EMS_RxTelegram);
        return;
    }

    // first double check we actually have something in the queue
    if (EMS_TxQueue.isEmpty()) {
        _ems_processTelegram(EMS_RxTelegram);
        return;
    }

    // get the Tx telegram we just sent
    _EMS_TxTelegram EMS_TxTelegram = EMS_TxQueue.first();

    // check action
    // if READ, match the current inbound telegram to what we sent
    // if WRITE, should not happen
    // if VALIDATE, check the contents
    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
        if ((EMS_RxTelegram->src == EMS_TxTelegram.dest) && (EMS_RxTelegram->type == EMS_TxTelegram.type)) {
            // all checks out, read was successful, remove tx from queue and continue to process telegram
            _removeTxQueue();
            EMS_Sys_Status.emsRxPgks++;                       // increment counter
            ems_setEmsRefreshed(EMS_TxTelegram.forceRefresh); // does mqtt need refreshing?
        } else {
            // read not OK, we didn't get back a telegram we expected
            // leave on queue and try again, but continue to process what we received as it may be important
            EMS_Sys_Status.txRetryCount++;
            // if retried too many times, give up and remove it
            if (EMS_Sys_Status.txRetryCount >= TX_WRITE_TIMEOUT_COUNT) {
                if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                    myDebug_P(PSTR("Read failed. Giving up, removing from queue"));
                }
                _removeTxQueue();
            } else {
                if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                    myDebug_P(PSTR("...Retrying read. Attempt %d/%d..."), EMS_Sys_Status.txRetryCount, TX_WRITE_TIMEOUT_COUNT);
                }
            }
        }
        _ems_processTelegram(EMS_RxTelegram); // process it always
    }

    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
        // should not get here, since this is handled earlier receiving a 01 or 04
        myDebug_P(PSTR("** Error ! Write - should not be here"));
    }

    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
        // this is a read telegram which we use to validate the last write
        uint8_t * data         = telegram + 4; // data block starts at position 5
        uint8_t   dataReceived = data[0];      // only a single byte is returned after a read
        if (EMS_TxTelegram.comparisonValue == dataReceived) {
            // validate was successful, the write changed the value
            _removeTxQueue(); // now we can remove the Tx validate command the queue
            if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                myDebug_P(PSTR("Write to 0x%02X was successful"), EMS_TxTelegram.dest);
            }
            // follow up with the post read command
            ems_doReadCommand(EMS_TxTelegram.comparisonPostRead, EMS_TxTelegram.dest, true);
        } else {
            // write failed
            if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                myDebug_P(PSTR("Last write failed. Compared set value 0x%02X with received value 0x%02X"), EMS_TxTelegram.comparisonValue, dataReceived);
            }
            if (++EMS_Sys_Status.txRetryCount > TX_WRITE_TIMEOUT_COUNT) {
                if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                    myDebug_P(PSTR("Write failed. Giving up, removing from queue"));
                }
                _removeTxQueue();
            } else {
                // retry, turn the validate back into a write and try again
                if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                    myDebug_P(PSTR("...Retrying write. Attempt %d/%d..."), EMS_Sys_Status.txRetryCount, TX_WRITE_TIMEOUT_COUNT);
                }
                EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
                EMS_TxTelegram.dataValue = EMS_TxTelegram.comparisonValue;  // restore old value
                EMS_TxTelegram.offset    = EMS_TxTelegram.comparisonOffset; // restore old value
                EMS_TxQueue.shift();                                        // remove validate from queue
                EMS_TxQueue.unshift(EMS_TxTelegram);                        // add back to queue making it next in line
            }
        }
    }

    emsuart_tx_poll(); // send Acknowledgement back to free the EMS bus since we have the telegram
}


/**
 * Check if hot tap water or heating is active
 * using a quick hack for checking the heating. Selected Flow Temp >= 70
 */
void _checkActive() {
    // hot tap water, using flow to check instead of the burner power
    if (EMS_Boiler.wWCurFlow != EMS_VALUE_INT_NOTSET && EMS_Boiler.burnGas != EMS_VALUE_INT_NOTSET) {
        EMS_Boiler.tapwaterActive = ((EMS_Boiler.wWCurFlow != 0) && (EMS_Boiler.burnGas == EMS_VALUE_INT_ON));
    }

    // heating
    if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET && EMS_Boiler.burnGas != EMS_VALUE_INT_NOTSET) {
        EMS_Boiler.heatingActive = ((EMS_Boiler.selFlowTemp >= EMS_BOILER_SELFLOWTEMP_HEATING) && (EMS_Boiler.burnGas == EMS_VALUE_INT_ON));
    }
}

/**
 * UBAParameterWW - type 0x33 - warm water parameters
 * received only after requested (not broadcasted)
 */
void _process_UBAParameterWW(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.wWActivated   = (_toByte(1) == 0xFF); // 0xFF means on
    EMS_Boiler.wWSelTemp     = _toByte(2);
    EMS_Boiler.wWCircPump    = (_toByte(6) == 0xFF); // 0xFF means on
    EMS_Boiler.wWDesiredTemp = _toByte(8);
    EMS_Boiler.wWComfort     = _toByte(EMS_OFFSET_UBAParameterWW_wwComfort);

    EMS_Sys_Status.emsRefreshed = true; // when we receieve this, lets force an MQTT publish
}

/**
 * UBATotalUptimeMessage - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void _process_UBATotalUptimeMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.UBAuptime        = _toLong(0);
    EMS_Sys_Status.emsRefreshed = true; // when we receieve this, lets force an MQTT publish
}

/**
 * UBAParametersMessage - type 0x16
 */
void _process_UBAParametersMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.heating_temp = _toByte(1);
    EMS_Boiler.pump_mod_max = _toByte(9);
    EMS_Boiler.pump_mod_min = _toByte(10);
}

/**
 * UBAMonitorWWMessage - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void _process_UBAMonitorWWMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.wWCurTmp  = _toShort(1);
    EMS_Boiler.wWStarts  = _toLong(13);
    EMS_Boiler.wWWorkM   = _toLong(10);
    EMS_Boiler.wWOneTime = _bitRead(5, 1);
    EMS_Boiler.wWCurFlow = _toByte(9);
}

/**
 * UBAMonitorFast - type 0x18 - central heating monitor part 1 (25 bytes long)
 * received every 10 seconds
 */
void _process_UBAMonitorFast(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.selFlowTemp = _toByte(0);
    EMS_Boiler.curFlowTemp = _toShort(1);
    EMS_Boiler.retTemp     = _toShort(13);

    EMS_Boiler.burnGas = _bitRead(7, 0);
    EMS_Boiler.fanWork = _bitRead(7, 2);
    EMS_Boiler.ignWork = _bitRead(7, 3);
    EMS_Boiler.heatPmp = _bitRead(7, 5);
    EMS_Boiler.wWHeat  = _bitRead(7, 6);
    EMS_Boiler.wWCirc  = _bitRead(7, 7);

    EMS_Boiler.curBurnPow = _toByte(4);
    EMS_Boiler.selBurnPow = _toByte(3); // burn power max setting

    EMS_Boiler.flameCurr = _toShort(15);

    // read the service code / installation status as appears on the display
    EMS_Boiler.serviceCodeChar[0] = char(_toByte(18)); // ascii character 1
    EMS_Boiler.serviceCodeChar[1] = char(_toByte(19)); // ascii character 2
    EMS_Boiler.serviceCodeChar[2] = '\0';              // null terminate string

    // read error code
    EMS_Boiler.serviceCode = _toShort(20);

    // system pressure. FF means missing
    EMS_Boiler.sysPress = _toByte(17); // this is *10

    // at this point do a quick check to see if the hot water or heating is active
    _checkActive();
}

/**
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 */
void _process_UBAMonitorSlow(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Boiler.extTemp     = _toShort(0); // 0x8000 if not available
    EMS_Boiler.boilTemp    = _toShort(2); // 0x8000 if not available
    EMS_Boiler.pumpMod     = _toByte(9);
    EMS_Boiler.burnStarts  = _toLong(10);
    EMS_Boiler.burnWorkMin = _toLong(13);
    EMS_Boiler.heatWorkMin = _toLong(19);
}

/**
 * type 0xB1 - data from the RC10 thermostat (0x17)
 * For reading the temp values only
 * received every 60 seconds
 * e.g. 17 0B 91 00 80 1E 00 CB 27 00 00 00 00 05 01 00 CB 00 (CRC=47), #data=14
 */
void _process_RC10StatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_RC10StatusMessage_setpoint); // is * 2
    EMS_Thermostat.curr_roomTemp     = _toByte(EMS_OFFSET_RC10StatusMessage_curr);     // is * 10

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
 * For reading the temp values only
 * received every 60 seconds
 */
void _process_RC20StatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_RC20StatusMessage_setpoint); // is * 2
    EMS_Thermostat.curr_roomTemp     = _toShort(EMS_OFFSET_RC20StatusMessage_curr);    // is * 10

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0x41 - data from the RC30 thermostat(0x10) - 14 bytes long
 * For reading the temp values only * received every 60 seconds 
*/
void _process_RC30StatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_RC30StatusMessage_setpoint); // is * 2
    EMS_Thermostat.curr_roomTemp     = _toShort(EMS_OFFSET_RC30StatusMessage_curr);    // note, its 2 bytes here

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0x3E and 0x48 - data from the RC35 thermostat (0x10) - 16 bytes
 * For reading the temp values only
 * received every 60 seconds
 */
void _process_RC35StatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_RC35StatusMessage_setpoint); // is * 2

    // check if temp sensor is unavailable
    if (EMS_RxTelegram->data[3] == 0x7D) {
        EMS_Thermostat.curr_roomTemp = EMS_VALUE_SHORT_NOTSET;
    } else {
        EMS_Thermostat.curr_roomTemp = _toShort(EMS_OFFSET_RC35StatusMessage_curr);
    }
    EMS_Thermostat.day_mode = _bitRead(EMS_OFFSET_RC35Get_mode_day, 1); // get day mode flag

    EMS_Thermostat.circuitcalctemp = _toByte(EMS_OFFSET_RC35Set_circuitcalctemp); // 0x48 calculated temperature

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0x0A - data from the Nefit Easy/TC100 thermostat (0x18) - 31 bytes long
 * The Easy has a digital precision of its floats to 2 decimal places, so values must be divided by 100
 */
void _process_EasyStatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.curr_roomTemp     = _toShort(EMS_OFFSET_EasyStatusMessage_curr);     // is *100
    EMS_Thermostat.setpoint_roomTemp = _toShort(EMS_OFFSET_EasyStatusMessage_setpoint); // is *100

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0x01A5 - data from the Nefit RC1010 thermostat (0x18) and RC300/310s on 0x10
 * EMS+ messages may come in with different offsets so handle them here
 */
void _process_RCPLUSStatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    if (EMS_RxTelegram->offset == 0) {
        // the whole telegram
        // e.g. Thermostat -> all, telegram: 10 00 FF 00 01 A5 00 D7 21 00 00 00 00 30 01 84 01 01 03 01 84 01 F1 00 00 11 01 00 08 63 00
        //                                   10 00 FF 00 01 A5 80 00 01 30 28 00 30 28 01 54 03 03 01 01 54 02 A8 00 00 11 01 03 FF FF 00
        EMS_Thermostat.curr_roomTemp     = _toShort(EMS_OFFSET_RCPLUSStatusMessage_curr);    // value is * 10
        EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_RCPLUSStatusMessage_setpoint); // value is * 2

        EMS_Thermostat.day_mode = _bitRead(EMS_OFFSET_RCPLUSGet_mode_day, 1); // get day mode flag

        // room night setpoint is _toByte(2) (value is *2)
        // boiler set temp is _toByte(4) (value is *2)
        // day night is byte(8), 0x01 for night, 0x00 for day
    }

    // actual set point
    // e.g. Thermostat -> all, telegram: 10 00 FF 07 01 A5 32
    if (EMS_RxTelegram->offset == 7) {
        // to add...
    }

    // next set point
    // e.g. Thermostat -> all, telegram: 18 00 FF 06 01 A5 22
    if (EMS_RxTelegram->offset == 6) {
        // to add...
    }
}

/**
 * type 0x01B9 - heating data from the Nefit RC1010 thermostat (0x18) and RC300/310s on 0x10
 */
void _process_RCPLUSStatusHeating(_EMS_RxTelegram * EMS_RxTelegram) {
    // see wiki
    // operation mode, comfort levels 1,2,3, eco level
}

/**
 * type 0x01AF - summer/winter mode from the Nefit RC1010 thermostat (0x18) and RC300/310s on 0x10
 */
void _process_RCPLUSStatusMode(_EMS_RxTelegram * EMS_RxTelegram) {
    // _toByte(0); // 0x00=OFF 0x01=Automatic 0x02=Forced
}

/**
 * FR10 Junkers - type x6F01
 */
void _process_FR10StatusMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    if (EMS_RxTelegram->data_length == 6) {
        // e.g. 90 00 FF 00 00 6F 03 01 00 BE 00 BF
        EMS_Thermostat.curr_roomTemp     = _toByte(EMS_OFFSET_FR10StatusMessage_curr);     // value is * 10
        EMS_Thermostat.setpoint_roomTemp = _toByte(EMS_OFFSET_FR10StatusMessage_setpoint); // value is * 10, which is different from other EMS+ devices
    }
}

/**
 * to complete....
 */
void _process_RCPLUSSetMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    // to complete
}

/**
 * type 0xB0 - for reading the mode from the RC10 thermostat (0x17)
 * received only after requested
 */
void _process_RC10Set(_EMS_RxTelegram * EMS_RxTelegram) {
    // mode not implemented yet
}

/**
 * type 0xA8 - for reading the mode from the RC20 thermostat (0x17)
 * received only after requested
 */
void _process_RC20Set(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.mode = _toByte(EMS_OFFSET_RC20Set_mode);
}

/**
 * type 0xA7 - for reading the mode from the RC30 thermostat (0x10)
 * received only after requested
 */
void _process_RC30Set(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.mode = _toByte(EMS_OFFSET_RC30Set_mode);
}

/**
 * type 0x3D and 0x47 - for reading the mode from the RC35 thermostat (0x10)
 * Working Mode Heating Circuit 1 & 2 (HC1, HC2)
 * received only after requested
 */
void _process_RC35Set(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Thermostat.mode = _toByte(EMS_OFFSET_RC35Set_mode);

    EMS_Thermostat.daytemp     = _toByte(EMS_OFFSET_RC35Set_temp_day);     // is * 2
    EMS_Thermostat.nighttemp   = _toByte(EMS_OFFSET_RC35Set_temp_night);   // is * 2
    EMS_Thermostat.holidaytemp = _toByte(EMS_OFFSET_RC35Set_temp_holiday); // is * 2
    EMS_Thermostat.heatingtype = _toByte(EMS_OFFSET_RC35Set_heatingtype);  // byte 0 bit floor heating = 3 0x47

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * type 0xA3 - for external temp settings from the the RC* thermostats
 */
void _process_RCOutdoorTempMessage(_EMS_RxTelegram * EMS_RxTelegram) {
    // add support here if you're reading external sensors
}

/*
 * SM10Monitor - type 0x97
 */
void _process_SM10Monitor(_EMS_RxTelegram * EMS_RxTelegram) {
    EMS_Other.SMcollectorTemp  = _toShort(2);    // collector temp from SM10, is *10
    EMS_Other.SMbottomTemp     = _toShort(5);    // bottom temp from SM10, is *10
    EMS_Other.SMpumpModulation = _toByte(4);     // modulation solar pump
    EMS_Other.SMpump           = _bitRead(7, 1); // active if bit 1 is set

    EMS_Other.SM                = true;
    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/*
 * SM100Monitor - type 0x0262 EMS+
 * e.g, 30 00 FF 00 02 62 01 AC
 *      30 00 FF 18 02 62 80 00
 *      30 00 FF 00 02 62 01 A1 - for bottom temps
 */
void _process_SM100Monitor(_EMS_RxTelegram * EMS_RxTelegram) {
    // only process the complete telegram, not partial
    if (EMS_RxTelegram->offset != 0) {
        return;
    }

    EMS_Other.SMcollectorTemp = _toShort(0); // collector temp from SM100, is *10

    if (EMS_RxTelegram->data_length > 2) {
        EMS_Other.SMbottomTemp = _toShort(2); // bottom temp from SM100, is *10
    }

    EMS_Other.SM                = true;
    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/*
 * SM100Status - type 0x0264 EMS+ for pump modulation
 * e.g. 30 00 FF 09 02 64 64 = 100%
 *      30 00 FF 09 02 64 1E = 30%
 */
void _process_SM100Status(_EMS_RxTelegram * EMS_RxTelegram) {
    // check for complete telegram
    if (EMS_RxTelegram->offset == 0) {
        EMS_Other.SMpumpModulation = _toByte(9); // modulation solar pump
    } else if (EMS_RxTelegram->offset == 0x09) {
        // or short telegram with a single byte with offset 09
        EMS_Other.SMpumpModulation = _toByte(0); // modulation solar pump
    }

    EMS_Other.SM                = true;
    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/*
 * SM100Status2 - type 0x026A EMS+ for pump on/off at offset 0x0A
 */
void _process_SM100Status2(_EMS_RxTelegram * EMS_RxTelegram) {
    // check for complete telegram
    if (EMS_RxTelegram->offset == 0) {
        EMS_Other.SMpump = _bitRead(10, 2); // 03=off 04=on at offset 10 which is byte 10
    } else if (EMS_RxTelegram->offset == 0x0A) {
        // or short telegram with a single byte with offset 0A
        EMS_Other.SMpump = _bitRead(0, 2); // 03=off 04=on
    }

    EMS_Other.SM                = true;
    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/*
 * SM100Energy - type 0x028E EMS+ for energy readings
 */
void _process_SM100Energy(_EMS_RxTelegram * EMS_RxTelegram) {
    // e.g. 30 00 FF 00 02 8E 00 00 00 00 00 00 06 C5 00 00 76 35

    EMS_Other.SMEnergyLastHour = _toShort(2);  // last hour / 10 in Wh
    EMS_Other.SMEnergyToday    = _toShort(6);  // todays in Wh
    EMS_Other.SMEnergyTotal    = _toShort(10); // total / 10 in kWh

    EMS_Other.SM                = true;
    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back via MQTT
}

/**
 * UBASetPoint 0x1A
 */
void _process_SetPoints(_EMS_RxTelegram * EMS_RxTelegram) {
    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
        if (EMS_RxTelegram->data_length != 0) {
            uint8_t setpoint = EMS_RxTelegram->data[0]; // flow temp
            //uint8_t ww_power = data[2]; // power in %

            /* use this logic if the value is *2
            char s[5];
            char s2[5];
            strlcpy(s, itoa(setpoint >> 1, s2, 10), 5);
            strlcat(s, ".", sizeof(s));
            strlcat(s, ((setpoint & 0x01) ? "5" : "0"), 5);
            myDebug_P(PSTR(" Boiler flow temp %s C, Warm Water power %d %"), s, ww_power);
            */

            myDebug_P(PSTR(" Boiler flow temperature is %d C"), setpoint);
        }
    }
}

/**
 * process_RCTime - type 0x06 - date and time from a thermostat - 14 bytes long
 * common for all thermostats
 */
void _process_RCTime(_EMS_RxTelegram * EMS_RxTelegram) {
    if ((EMS_Thermostat.model_id == EMS_MODEL_EASY) || (EMS_Thermostat.model_id == EMS_MODEL_BOSCHEASY)) {
        return; // not supported
    }

    EMS_Thermostat.hour   = _toByte(2);
    EMS_Thermostat.minute = _toByte(4);
    EMS_Thermostat.second = _toByte(5);
    EMS_Thermostat.day    = _toByte(3);
    EMS_Thermostat.month  = _toByte(1);
    EMS_Thermostat.year   = _toByte(0);
}

/*
 * add an EMS device to our list of detected devices
 */
void _addDevice(uint8_t product_id, uint8_t device_id, char * version, const char * model_string) {
    _Generic_Type device;

    // if its a duplicate don't add
    bool found = false;
    for (std::list<_Generic_Type>::iterator it = Devices.begin(); it != Devices.end(); it++) {
        if (((it)->product_id == product_id) && ((it)->device_id == device_id)) {
            found = true;
        }
    }

    if (!found) {
        device.product_id = product_id;
        device.device_id  = device_id;
        strlcpy(device.version, version, sizeof(device.version));
        strlcpy(device.model_string, model_string, sizeof(device.model_string));
        Devices.push_back(device);
    }
}

/**
 * type 0x02 - get the firmware version and type of an EMS device
 * look up known devices via the product id and setup if not already set
 */
void _process_Version(_EMS_RxTelegram * EMS_RxTelegram) {
    // ignore short messages that we can't interpret
    if (EMS_RxTelegram->data_length < 3) {
        return;
    }

    uint8_t product_id  = _toByte(0);
    char    version[10] = {0};
    snprintf(version, sizeof(version), "%02d.%02d", _toByte(1), _toByte(2));

    // see if its a known boiler
    int  i         = 0;
    bool typeFound = false;
    while (i < _Boiler_Types_max) {
        if (Boiler_Types[i].product_id == product_id) {
            typeFound = true; // we have a matching product id. i is the index.
            break;
        }
        i++;
    }

    if (typeFound) {
        // its a boiler
        myDebug_P(PSTR("Boiler found. Model %s (DeviceID:0x%02X ProductID:%d Version:%s)"),
                  Boiler_Types[i].model_string,
                  Boiler_Types[i].device_id,
                  product_id,
                  version);

        // add to list
        _addDevice(product_id, Boiler_Types[i].device_id, version, Boiler_Types[i].model_string);

        // if its a boiler set it, unless it already has been set by checking for a productID
        // it will take the first one found in the list
        if (((EMS_Boiler.device_id == EMS_ID_NONE) || (EMS_Boiler.device_id == Boiler_Types[i].device_id)) && EMS_Boiler.product_id == EMS_ID_NONE) {
            myDebug_P(PSTR("* Setting Boiler to model %s (DeviceID:0x%02X ProductID:%d Version:%s)"),
                      Boiler_Types[i].model_string,
                      Boiler_Types[i].device_id,
                      product_id,
                      version);

            EMS_Boiler.device_id  = Boiler_Types[i].device_id;
            EMS_Boiler.product_id = Boiler_Types[i].product_id;
            strlcpy(EMS_Boiler.version, version, sizeof(EMS_Boiler.version));

            myESP.fs_saveConfig(); // save config to SPIFFS

            ems_getBoilerValues(); // get Boiler values that we would usually have to wait for
        }
        return;
    }

    // its not a boiler, maybe its a known thermostat?
    i = 0;
    while (i < _Thermostat_Types_max) {
        if (Thermostat_Types[i].product_id == product_id) {
            typeFound = true; // we have a matching product id. i is the index.
            break;
        }
        i++;
    }

    if (typeFound) {
        // its a known thermostat
        if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
            myDebug_P(PSTR("Thermostat found. Model %s (DeviceID:0x%02X ProductID:%d Version:%s)"),
                      Thermostat_Types[i].model_string,
                      Thermostat_Types[i].device_id,
                      product_id,
                      version);
        }

        // add to list
        _addDevice(product_id, Boiler_Types[i].device_id, version, Thermostat_Types[i].model_string);

        // if we don't have a thermostat set, use this one
        if (((EMS_Thermostat.device_id == EMS_ID_NONE) || (EMS_Thermostat.model_id == EMS_MODEL_NONE)
             || (EMS_Thermostat.device_id == Thermostat_Types[i].device_id))
            && EMS_Thermostat.product_id == EMS_ID_NONE) {
            myDebug_P(PSTR("* Setting Thermostat model to %s (DeviceID:0x%02X ProductID:%d Version:%s)"),
                      Thermostat_Types[i].model_string,
                      Thermostat_Types[i].device_id,
                      product_id,
                      version);

            EMS_Thermostat.model_id        = Thermostat_Types[i].model_id;
            EMS_Thermostat.device_id       = Thermostat_Types[i].device_id;
            EMS_Thermostat.write_supported = Thermostat_Types[i].write_supported;
            EMS_Thermostat.product_id      = product_id;
            strlcpy(EMS_Thermostat.version, version, sizeof(EMS_Thermostat.version));

            myESP.fs_saveConfig(); // save config to SPIFFS

            // get Thermostat values (if supported)
            ems_getThermostatValues();
        }
        return;
    }

    // finally look for the other EMS devices
    i = 0;
    while (i < _Other_Types_max) {
        if (Other_Types[i].product_id == product_id) {
            typeFound = true; // we have a matching product id. i is the index.
            break;
        }
        i++;
    }

    if (typeFound) {
        myDebug_P(PSTR("Device found. Model %s with DeviceID 0x%02X, ProductID %d, Version %s"),
                  Other_Types[i].model_string,
                  Other_Types[i].device_id,
                  product_id,
                  version);

        // add to list
        _addDevice(product_id, Other_Types[i].device_id, version, Other_Types[i].model_string);

        // see if this is a Solar Module SM10
        if (Other_Types[i].device_id == EMS_ID_SM) {
            EMS_Other.SM = true; // we have detected a SM10
            myDebug_P(PSTR("SM10 Solar Module support enabled."));
        }

        // fetch other values
        ems_getOtherValues();
        return;

    } else {
        myDebug_P(PSTR("Unrecognized device found. DeviceID 0x%02X, ProductID %d, Version %s"), EMS_RxTelegram->src, product_id, version);

        // add to list
        _addDevice(product_id, EMS_RxTelegram->src, version, "unknown?");
    }
}

/*
 * Figure out the boiler and thermostat types
 */
void ems_discoverModels() {
    myDebug_P(PSTR("Starting auto discover of EMS devices..."));

    // boiler
    ems_doReadCommand(EMS_TYPE_Version, EMS_Boiler.device_id); // get version details of boiler

    // solar module
    ems_doReadCommand(EMS_TYPE_Version, EMS_ID_SM); // check if there is Solar Module available

    // thermostat
    // if it hasn't been set, auto discover it
    if (EMS_Thermostat.device_id == EMS_ID_NONE) {
        ems_scanDevices(); // auto-discover it
    } else {
        // set the model as hardcoded (see my_devices.h) and fetch the version and product id
        ems_doReadCommand(EMS_TYPE_Version, EMS_Thermostat.device_id);
    }
}

/**
 * Print the Tx queue - for debugging
 */
void ems_printTxQueue() {
    _EMS_TxTelegram EMS_TxTelegram;
    char            sType[20] = {0};

    if (EMS_TxQueue.size() == 0) {
        myDebug_P(PSTR("Tx queue is empty"));
        return;
    }

    myDebug_P(PSTR("Tx queue (%d/%d)"), EMS_TxQueue.size(), EMS_TxQueue.capacity);

    for (byte i = 0; i < EMS_TxQueue.size(); i++) {
        EMS_TxTelegram = EMS_TxQueue[i]; // retrieves the i-th element from the buffer without removing it

        // get action
        if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
            strlcpy(sType, "write", sizeof(sType));
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
            strlcpy(sType, "read", sizeof(sType));
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
            strlcpy(sType, "validate", sizeof(sType));
        } else {
            strlcpy(sType, "?", sizeof(sType));
        }

        char     addedTime[15] = {0};
        uint32_t upt           = EMS_TxTelegram.timestamp;
        snprintf(addedTime,
                 sizeof(addedTime),
                 "(%02d:%02d:%02d)",
                 (uint8_t)((upt / (1000 * 60 * 60)) % 24),
                 (uint8_t)((upt / (1000 * 60)) % 60),
                 (uint8_t)((upt / 1000) % 60));

        myDebug_P(PSTR(" [%d] action=%s dest=0x%02x type=0x%02x offset=%d length=%d dataValue=%d "
                       "comparisonValue=%d type_validate=0x%02x comparisonPostRead=0x%02x @ %s"),
                  i + 1,
                  sType,
                  EMS_TxTelegram.dest & 0x7F,
                  EMS_TxTelegram.type,
                  EMS_TxTelegram.offset,
                  EMS_TxTelegram.length,
                  EMS_TxTelegram.dataValue,
                  EMS_TxTelegram.comparisonValue,
                  EMS_TxTelegram.type_validate,
                  EMS_TxTelegram.comparisonPostRead,
                  addedTime);
    }
}

/**
 * Generic function to return various settings from the thermostat
 */
void ems_getThermostatValues() {
    if (!ems_getThermostatEnabled()) {
        return;
    }

    uint8_t model_id = EMS_Thermostat.model_id;
    uint8_t type     = EMS_Thermostat.device_id;
    uint8_t hc       = EMS_Thermostat.hc;

    if (model_id == EMS_MODEL_RC20) {
        ems_doReadCommand(EMS_TYPE_RC20StatusMessage, type); // to get the setpoint temp
        ems_doReadCommand(EMS_TYPE_RC20Set, type);           // to get the mode
    } else if (model_id == EMS_MODEL_RC30) {
        ems_doReadCommand(EMS_TYPE_RC30StatusMessage, type); // to get the setpoint temp
        ems_doReadCommand(EMS_TYPE_RC30Set, type);           // to get the mode
    } else if ((model_id == EMS_MODEL_RC35) || (model_id == EMS_MODEL_ES73)) {
        if (hc == 1) {
            ems_doReadCommand(EMS_TYPE_RC35StatusMessage_HC1, type); // to get the setpoint temp
            ems_doReadCommand(EMS_TYPE_RC35Set_HC1, type);           // to get the mode
        } else if (hc == 2) {
            ems_doReadCommand(EMS_TYPE_RC35StatusMessage_HC2, type); // to get the setpoint temp
            ems_doReadCommand(EMS_TYPE_RC35Set_HC2, type);           // to get the mode
        }
    } else if ((model_id == EMS_MODEL_EASY) || (model_id == EMS_MODEL_BOSCHEASY)) {
        ems_doReadCommand(EMS_TYPE_EasyStatusMessage, type);
    }

    ems_doReadCommand(EMS_TYPE_RCTime, type); // get Thermostat time
}

/**
 * Generic function to return various settings from the thermostat
 */
void ems_getBoilerValues() {
    ems_doReadCommand(EMS_TYPE_UBAMonitorFast, EMS_Boiler.device_id);        // get boiler stats, instead of waiting 10secs for the broadcast
    ems_doReadCommand(EMS_TYPE_UBAMonitorSlow, EMS_Boiler.device_id);        // get more boiler stats, instead of waiting 60secs for the broadcast
    ems_doReadCommand(EMS_TYPE_UBAParameterWW, EMS_Boiler.device_id);        // get Warm Water values
    ems_doReadCommand(EMS_TYPE_UBAParametersMessage, EMS_Boiler.device_id);  // get MC10 boiler values
    ems_doReadCommand(EMS_TYPE_UBATotalUptimeMessage, EMS_Boiler.device_id); // get uptime from boiler
}

/*
 * Get other values from EMS devices
 */
void ems_getOtherValues() {
    if (EMS_Other.SM) {
        ems_doReadCommand(EMS_TYPE_SM10Monitor, EMS_ID_SM); // fetch all from SM10Monitor, e.g. 0B B0 97 00 16
    }
}

/**
 *  returns current thermostat type as a string
 */
char * ems_getThermostatDescription(char * buffer) {
    uint8_t size = 128;
    if (!ems_getThermostatEnabled()) {
        strlcpy(buffer, "<not enabled>", size);
    } else {
        int  i      = 0;
        bool found  = false;
        char tmp[6] = {0};

        // scan through known ID types
        while (i < _Thermostat_Types_max) {
            if (Thermostat_Types[i].product_id == EMS_Thermostat.product_id) {
                found = true; // we have a match
                break;
            }
            i++;
        }

        if (found) {
            strlcpy(buffer, Thermostat_Types[i].model_string, size);
        } else {
            strlcpy(buffer, "DeviceID: 0x", size);
            strlcat(buffer, _hextoa(EMS_Thermostat.device_id, tmp), size);
        }

        strlcat(buffer, " (ProductID:", size);
        if (EMS_Thermostat.product_id == EMS_ID_NONE) {
            strlcat(buffer, "?", size);
        } else {
            strlcat(buffer, itoa(EMS_Thermostat.product_id, tmp, 10), size);
        }
        strlcat(buffer, " Version:", size);
        strlcat(buffer, EMS_Thermostat.version, size);
        strlcat(buffer, ")", size);
    }

    return buffer;
}

/**
 *  returns current boiler type as a string
 */
char * ems_getBoilerDescription(char * buffer) {
    uint8_t size = 128;
    if (!ems_getBoilerEnabled()) {
        strlcpy(buffer, "<not enabled>", size);
    } else {
        int  i      = 0;
        bool found  = false;
        char tmp[6] = {0};

        // scan through known ID types
        while (i < _Boiler_Types_max) {
            if (Boiler_Types[i].product_id == EMS_Boiler.product_id) {
                found = true; // we have a match
                break;
            }
            i++;
        }
        if (found) {
            strlcpy(buffer, Boiler_Types[i].model_string, size);
        } else {
            strlcpy(buffer, "DeviceID: 0x", size);
            strlcat(buffer, _hextoa(EMS_Boiler.device_id, tmp), size);
        }

        strlcat(buffer, " (ProductID:", size);
        if (EMS_Boiler.product_id == EMS_ID_NONE) {
            strlcat(buffer, "?", size);
        } else {
            strlcat(buffer, itoa(EMS_Boiler.product_id, tmp, 10), size);
        }
        strlcat(buffer, " Version:", size);
        strlcat(buffer, EMS_Boiler.version, size);
        strlcat(buffer, ")", size);
    }

    return buffer;
}

/**
 * Find the versions of our connected devices
 */
void ems_scanDevices() {
    myDebug_P(PSTR("Started scan on EMS bus for known devices"));

    std::list<uint8_t> Device_Ids; // create a new list

    // copy over boilers
    for (_Boiler_Type bt : Boiler_Types) {
        Device_Ids.push_back(bt.device_id);
    }

    // copy over thermostats
    for (_Thermostat_Type tt : Thermostat_Types) {
        Device_Ids.push_back(tt.device_id);
    }

    // copy over others
    for (_Other_Type ot : Other_Types) {
        Device_Ids.push_back(ot.device_id);
    }

    // remove duplicates and reserved IDs (like our own device)
    Device_Ids.sort();
    Device_Ids.unique();
    Device_Ids.remove(EMS_MODEL_NONE);

    // send the read command with Version command
    for (uint8_t device_id : Device_Ids) {
        ems_doReadCommand(EMS_TYPE_Version, device_id);
    }
}

/**
 * Print out all handled types
 */
void ems_printAllDevices() {
    uint8_t i;

    myDebug_P(PSTR("\nThese %d devices are supported as boiler units:"), _Boiler_Types_max);
    for (i = 0; i < _Boiler_Types_max; i++) {
        myDebug_P(PSTR(" %s%s%s (DeviceID:0x%02X ProductID:%d)"),
                  COLOR_BOLD_ON,
                  Boiler_Types[i].model_string,
                  COLOR_BOLD_OFF,
                  Boiler_Types[i].device_id,
                  Boiler_Types[i].product_id);
    }

    myDebug_P(PSTR("\nThese %d devices are supported as other known EMS devices:"), _Other_Types_max);
    for (i = 0; i < _Other_Types_max; i++) {
        myDebug_P(PSTR(" %s%s%s (DeviceID:0x%02X ProductID:%d)"),
                  COLOR_BOLD_ON,
                  Other_Types[i].model_string,
                  COLOR_BOLD_OFF,
                  Other_Types[i].device_id,
                  Other_Types[i].product_id);
    }

    myDebug_P(PSTR("\nThe following telegram type IDs are supported:"));
    for (i = 0; i < _EMS_Types_max; i++) {
        if ((EMS_Types[i].model_id == EMS_MODEL_ALL) || (EMS_Types[i].model_id == EMS_MODEL_UBA)) {
            myDebug_P(PSTR(" type %02X (%s)"), EMS_Types[i].type, EMS_Types[i].typeString);
        }
    }

    myDebug_P(PSTR("\nThese %d thermostat devices are supported:"), _Thermostat_Types_max);
    for (i = 0; i < _Thermostat_Types_max; i++) {
        myDebug_P(PSTR(" %s%s%s (DeviceID:0x%02X ProductID:%d) can write:%c"),
                  COLOR_BOLD_ON,
                  Thermostat_Types[i].model_string,
                  COLOR_BOLD_OFF,
                  Thermostat_Types[i].device_id,
                  Thermostat_Types[i].product_id,
                  (Thermostat_Types[i].write_supported) ? 'y' : 'n');
    }

    // print out known devices
    ems_printDevices();

    myDebug_P(PSTR("")); // newline
}

/**
 * print out contents of the device list that was captured
 */
void ems_printDevices() {
    if (Devices.size() != 0) {
        myDebug_P(PSTR("\nThese %d EMS devices were detected:"), Devices.size());
        for (std::list<_Generic_Type>::iterator it = Devices.begin(); it != Devices.end(); it++) {
            myDebug_P(PSTR(" %s%s%s (DeviceID:0x%02X ProductID:%d Version:%s)"),
                      COLOR_BOLD_ON,
                      (it)->model_string,
                      COLOR_BOLD_OFF,
                      (it)->device_id,
                      (it)->product_id,
                      (it)->version);
        }

        myDebug_P(PSTR("\nNote: if any devices are marked as 'unknown?' please report this as a GitHub issue so the EMS devices list can be "
                       "updated.\n"));
    }
}

/**
 * Send a command to UART Tx to Read from another device
 * Read commands when sent must respond by the destination (target) immediately (or within 10ms)
 */
void ems_doReadCommand(uint16_t type, uint8_t dest, bool forceRefresh) {
    // if not a valid type of boiler is not accessible then quits
    if ((type == EMS_ID_NONE) || (dest == EMS_ID_NONE)) {
        return;
    }

    // if we're preventing all outbound traffic, quit
    if (EMS_Sys_Status.emsTxDisabled) {
        myDebug_P(PSTR("in Listen Mode. All Tx is disabled."));
        return;
    }

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    // see if its a known type
    int i = _ems_findType(type);

    if ((ems_getLogging() == EMS_SYS_LOGGING_BASIC) || (ems_getLogging() == EMS_SYS_LOGGING_VERBOSE)) {
        if (i == -1) {
            myDebug_P(PSTR("Requesting type (0x%02X) from dest 0x%02X"), type, dest);
        } else {
            myDebug_P(PSTR("Requesting type %s(0x%02X) from dest 0x%02X"), EMS_Types[i].typeString, type, dest);
        }
    }
    EMS_TxTelegram.action             = EMS_TX_TELEGRAM_READ;    // read command
    EMS_TxTelegram.dest               = dest;                    // set 8th bit to indicate a read
    EMS_TxTelegram.offset             = 0;                       // 0 for all data
    EMS_TxTelegram.length             = EMS_MIN_TELEGRAM_LENGTH; // is always 6 bytes long (including CRC at end)
    EMS_TxTelegram.type               = type;
    EMS_TxTelegram.dataValue          = EMS_MAX_TELEGRAM_LENGTH; // for a read this is the # bytes we want back
    EMS_TxTelegram.type_validate      = EMS_ID_NONE;
    EMS_TxTelegram.comparisonValue    = 0;
    EMS_TxTelegram.comparisonOffset   = 0;
    EMS_TxTelegram.comparisonPostRead = EMS_ID_NONE;
    EMS_TxTelegram.forceRefresh       = forceRefresh; // should we send to MQTT after a successful read?

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Send a raw telegram to the bus
 * telegram is a string of hex values
 */
void ems_sendRawTelegram(char * telegram) {
    uint8_t count = 0;
    char *  p;
    char    value[10] = {0};

    if (EMS_Sys_Status.emsTxDisabled) {
        return; // user has disabled all Tx
    }

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    // get first value, which should be the src
    if ((p = strtok(telegram, " ,"))) { // delimiter
        strlcpy(value, p, sizeof(value));
        EMS_TxTelegram.data[0] = (uint8_t)strtol(value, 0, 16);
    }
    // and interate until end
    while (p != 0) {
        if ((p = strtok(NULL, " ,"))) {
            strlcpy(value, p, sizeof(value));
            uint8_t val                  = (uint8_t)strtol(value, 0, 16);
            EMS_TxTelegram.data[++count] = val;
            if (count == 1) {
                EMS_TxTelegram.dest = val;
            } else if (count == 2) {
                EMS_TxTelegram.type = val;
            } else if (count == 3) {
                EMS_TxTelegram.offset = val;
            }
        }
    }

    if (count == 0) {
        return; // nothing to send
    }

    // calculate length including header and CRC
    EMS_TxTelegram.length        = count + 2;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;
    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_RAW;

    // add to Tx queue. Assume it's not full.
    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the temperature of the thermostat
 * temptype 0 = normal, 1=night temp, 2=day temp, 3=holiday temp
 */
void ems_setThermostatTemp(float temperature, uint8_t temptype) {
    if (!ems_getThermostatEnabled()) {
        return;
    }

    if (!EMS_Thermostat.write_supported) {
        myDebug_P(PSTR("Write not supported for this model Thermostat"));
        return;
    }

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    uint8_t model_id = EMS_Thermostat.model_id;
    uint8_t type     = EMS_Thermostat.device_id;
    uint8_t hc       = EMS_Thermostat.hc; // heating circuit

    EMS_TxTelegram.action = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest   = type;

    myDebug_P(PSTR("Setting new thermostat temperature"));

    // when doing a comparison  to validate the new temperature we call a different type

    if (model_id == EMS_MODEL_RC20) {
        EMS_TxTelegram.type               = EMS_TYPE_RC20Set;
        EMS_TxTelegram.offset             = EMS_OFFSET_RC20Set_temp;
        EMS_TxTelegram.comparisonPostRead = EMS_TYPE_RC20StatusMessage;
    } else if (model_id == EMS_MODEL_RC10) {
        EMS_TxTelegram.type               = EMS_TYPE_RC10Set;
        EMS_TxTelegram.offset             = EMS_OFFSET_RC10Set_temp;
        EMS_TxTelegram.comparisonPostRead = EMS_TYPE_RC10StatusMessage;
    } else if (model_id == EMS_MODEL_RC30) {
        EMS_TxTelegram.type               = EMS_TYPE_RC30Set;
        EMS_TxTelegram.offset             = EMS_OFFSET_RC30Set_temp;
        EMS_TxTelegram.comparisonPostRead = EMS_TYPE_RC30StatusMessage;
    } else if ((model_id == EMS_MODEL_RC35) || (model_id == EMS_MODEL_ES73)) {
        switch (temptype) {
        case 1: // change the night temp
            EMS_TxTelegram.offset = EMS_OFFSET_RC35Set_temp_night;
            break;
        case 2: //  change the day temp
            EMS_TxTelegram.offset = EMS_OFFSET_RC35Set_temp_day;
            break;
        case 3:                        // change the holiday temp
            EMS_TxTelegram.offset = 3; //holiday on RC35
            break;
        default:
        case 0: // automatic selection, if no type is defined, we use the standard code
            if (EMS_Thermostat.day_mode == 0) {
                EMS_TxTelegram.offset = EMS_OFFSET_RC35Set_temp_night;
            } else if (EMS_Thermostat.day_mode == 1) {
                EMS_TxTelegram.offset = EMS_OFFSET_RC35Set_temp_day;
            }
            break;
        }

        if (hc == 1) {
            // heating circuit 1
            EMS_TxTelegram.type               = EMS_TYPE_RC35Set_HC1;
            EMS_TxTelegram.comparisonPostRead = EMS_TYPE_RC35StatusMessage_HC1;
        } else {
            // heating circuit 2
            EMS_TxTelegram.type               = EMS_TYPE_RC35Set_HC2;
            EMS_TxTelegram.comparisonPostRead = EMS_TYPE_RC35StatusMessage_HC2;
        }
    }

    EMS_TxTelegram.length           = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.dataValue        = (uint8_t)((float)temperature * (float)2); // value
    EMS_TxTelegram.type_validate    = EMS_TxTelegram.type;
    EMS_TxTelegram.comparisonOffset = EMS_TxTelegram.offset;
    EMS_TxTelegram.comparisonValue  = EMS_TxTelegram.dataValue;

    EMS_TxTelegram.forceRefresh = false; // send to MQTT is done automatically in EMS_TYPE_RC*StatusMessage
    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the thermostat working mode (0=low/night, 1=manual/day, 2=auto/clock)
 * 0xA8 on a RC20 and 0xA7 on RC30
 */
void ems_setThermostatMode(uint8_t mode) {
    if (!ems_getThermostatEnabled()) {
        return;
    }

    if (!EMS_Thermostat.write_supported) {
        myDebug_P(PSTR("Write not supported for this model Thermostat"));
        return;
    }

    uint8_t model_id = EMS_Thermostat.model_id;
    uint8_t type     = EMS_Thermostat.device_id;
    uint8_t hc       = EMS_Thermostat.hc;

    myDebug_P(PSTR("Setting thermostat mode to %d"), mode);

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest      = type;
    EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.dataValue = mode;

    // handle different thermostat types
    if (model_id == EMS_MODEL_RC20) {
        EMS_TxTelegram.type   = EMS_TYPE_RC20Set;
        EMS_TxTelegram.offset = EMS_OFFSET_RC20Set_mode;
    } else if (model_id == EMS_MODEL_RC30) {
        EMS_TxTelegram.type   = EMS_TYPE_RC30Set;
        EMS_TxTelegram.offset = EMS_OFFSET_RC30Set_mode;
    } else if ((model_id == EMS_MODEL_RC35) || (model_id == EMS_MODEL_ES73)) {
        EMS_TxTelegram.type   = (hc == 2) ? EMS_TYPE_RC35Set_HC2 : EMS_TYPE_RC35Set_HC1;
        EMS_TxTelegram.offset = EMS_OFFSET_RC35Set_mode;
    }

    EMS_TxTelegram.type_validate      = EMS_TxTelegram.type; // callback to EMS_TYPE_RC30Temperature to fetch temps
    EMS_TxTelegram.comparisonOffset   = EMS_TxTelegram.offset;
    EMS_TxTelegram.comparisonValue    = EMS_TxTelegram.dataValue;
    EMS_TxTelegram.comparisonPostRead = EMS_TxTelegram.type;
    EMS_TxTelegram.forceRefresh       = false; // send to MQTT is done automatically in 0xA8 process

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the warm water temperature 0x33
 */
void ems_setWarmWaterTemp(uint8_t temperature) {
    // check for invalid temp values
    if ((temperature < 30) || (temperature > EMS_BOILER_TAPWATER_TEMPERATURE_MAX)) {
        return;
    }

    myDebug_P(PSTR("Setting boiler warm water temperature to %d C"), temperature);

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest      = EMS_Boiler.device_id;
    EMS_TxTelegram.type      = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset    = EMS_OFFSET_UBAParameterWW_wwtemp;
    EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.dataValue = temperature; // value to compare against. must be a single int

    EMS_TxTelegram.type_validate      = EMS_TYPE_UBAParameterWW; // validate
    EMS_TxTelegram.comparisonOffset   = EMS_OFFSET_UBAParameterWW_wwtemp;
    EMS_TxTelegram.comparisonValue    = temperature;
    EMS_TxTelegram.comparisonPostRead = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.forceRefresh       = false; // no need to send since this is done by 0x33 process

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the boiler flow temp
 */
void ems_setFlowTemp(uint8_t temperature) {
    myDebug_P(PSTR("Setting boiler flow temperature to %d C"), temperature);

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest      = EMS_Boiler.device_id;
    EMS_TxTelegram.type      = EMS_TYPE_UBASetPoints;
    EMS_TxTelegram.offset    = EMS_OFFSET_UBASetPoints_flowtemp;
    EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.dataValue = temperature; // value to compare against. must be a single int

    EMS_TxTelegram.type_validate      = EMS_TYPE_UBASetPoints; // validate
    EMS_TxTelegram.comparisonOffset   = EMS_OFFSET_UBASetPoints_flowtemp;
    EMS_TxTelegram.comparisonValue    = temperature;
    EMS_TxTelegram.comparisonPostRead = EMS_TYPE_UBASetPoints;
    EMS_TxTelegram.forceRefresh       = false;

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the warm water mode to comfort to Eco/Comfort
 * 1 = Hot, 2 = Eco, 3 = Intelligent
 */
void ems_setWarmWaterModeComfort(uint8_t comfort) {
    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    if (comfort == 1) {
        myDebug_P(PSTR("Setting boiler warm water comfort mode to Hot"));
        EMS_TxTelegram.dataValue = EMS_VALUE_UBAParameterWW_wwComfort_Hot;
    } else if (comfort == 2) {
        myDebug_P(PSTR("Setting boiler warm water comfort mode to Eco"));
        EMS_TxTelegram.dataValue = EMS_VALUE_UBAParameterWW_wwComfort_Eco;
    } else if (comfort == 3) {
        myDebug_P(PSTR("Setting boiler warm water comfort mode to Intelligent"));
        EMS_TxTelegram.dataValue = EMS_VALUE_UBAParameterWW_wwComfort_Intelligent;
    } else {
        return; // invalid comfort value
    }

    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest          = EMS_Boiler.device_id;
    EMS_TxTelegram.type          = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset        = EMS_OFFSET_UBAParameterWW_wwComfort;
    EMS_TxTelegram.length        = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.type_validate = EMS_ID_NONE; // don't validate

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Activate / De-activate the Warm Water 0x33
 * true = on, false = off
 */
void ems_setWarmWaterActivated(bool activated) {
    myDebug_P(PSTR("Setting boiler warm water %s"), activated ? "on" : "off");

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest          = EMS_Boiler.device_id;
    EMS_TxTelegram.type          = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset        = EMS_OFFSET_UBAParameterWW_wwactivated;
    EMS_TxTelegram.length        = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;               // don't validate
    EMS_TxTelegram.dataValue     = (activated ? 0xFF : 0x00); // 0xFF is on, 0x00 is off

    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Activate / De-activate the Warm Tap Water
 * true = on, false = off
 * Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
 */
void ems_setWarmTapWaterActivated(bool activated) {
    myDebug_P(PSTR("Setting boiler warm tap water %s"), activated ? "on" : "off");

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx
    EMS_TxTelegram.timestamp       = millis();            // set timestamp
    EMS_Sys_Status.txRetryCount    = 0;                   // reset retry counter

    // clear Tx to make sure all data is set to 0x00
    for (int i = 0; (i < EMS_MAX_TELEGRAM_LENGTH); i++) {
        EMS_TxTelegram.data[i] = 0x00;
    }

    EMS_TxTelegram.action = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest   = EMS_Boiler.device_id;
    EMS_TxTelegram.type   = EMS_TYPE_UBAFunctionTest;
    EMS_TxTelegram.offset = 0;
    EMS_TxTelegram.length = 22; // 17 bytes of data including header and CRC

    EMS_TxTelegram.type_validate      = EMS_TxTelegram.type;
    EMS_TxTelegram.comparisonOffset   = 0;                   // 1st byte
    EMS_TxTelegram.comparisonValue    = (activated ? 0 : 1); // value is 1 if in Test mode (not activated)
    EMS_TxTelegram.comparisonPostRead = EMS_TxTelegram.type;
    EMS_TxTelegram.forceRefresh       = true; // send new value to MQTT after successful write

    // create header
    EMS_TxTelegram.data[0] = EMS_ID_ME;             // src
    EMS_TxTelegram.data[1] = EMS_TxTelegram.dest;   // dest
    EMS_TxTelegram.data[2] = EMS_TxTelegram.type;   // type
    EMS_TxTelegram.data[3] = EMS_TxTelegram.offset; // offset

    // we use the special test mode 0x1D for this. Setting the first data to 5A puts the system into test mode and
    // a setting of 0x00 puts it back into normal operarting mode
    // when in test mode we're able to mess around with the core 3-way valve settings
    if (!activated) {
        // on
        EMS_TxTelegram.data[4] = 0x5A; // test mode on
        EMS_TxTelegram.data[5] = 0x00; // burner output 0%
        EMS_TxTelegram.data[7] = 0x64; // boiler pump capacity 100%
        EMS_TxTelegram.data[8] = 0xFF; // 3-way valve hot water only
    }

    EMS_TxQueue.push(EMS_TxTelegram); // add to queue
}

/**
 * Start up sequence for UBA Master, hopefully to initialize a handshake
 * Still experimental
 */
void ems_startupTelegrams() {
    if ((EMS_Sys_Status.emsTxDisabled) || (!EMS_Sys_Status.emsBusConnected)) {
        myDebug_P(PSTR("Unable to send startup sequence when in listen mode or the bus is disabled"));
    }

    myDebug_P(PSTR("Sending startup sequence..."));
    char s[20] = {0};

    // Write type 0x1D to get out of function test mode
    snprintf(s, sizeof(s), "%02X %02X 1D 00 00", EMS_ID_ME, EMS_Boiler.device_id);
    ems_sendRawTelegram(s);

    // Read type 0x01
    snprintf(s, sizeof(s), "%02X %02X 01 00 1B", EMS_ID_ME, EMS_Boiler.device_id | 0x80);
    ems_sendRawTelegram(s);
}

/**
 * Test parsing of telgrams by injecting fake telegrams and simulating the response
 */
void ems_testTelegram(uint8_t test_num) {
#ifdef TESTS
    if ((test_num == 0) || (test_num > _TEST_DATA_max)) {
        myDebug_P(PSTR("Invalid test. Pick between 1 and %d"), _TEST_DATA_max);
        return;
    }

    // stop all Tx
    if (!EMS_TxQueue.isEmpty()) {
        EMS_TxQueue.clear();
        EMS_Sys_Status.emsTxStatus = EMS_TX_STATUS_IDLE;
    }

    static uint8_t * telegram = (uint8_t *)malloc(EMS_MAX_TELEGRAM_LENGTH); // warning, memory is not free'd so use only for debugging

    char telegram_string[200];
    strlcpy(telegram_string, TEST_DATA[test_num - 1], sizeof(telegram_string));

    uint8_t length = 0;
    char *  p;
    char    value[10] = {0};

    // get first value, which should be the src
    if ((p = strtok(telegram_string, " ,"))) {
        strlcpy(value, p, sizeof(value));
        telegram[0] = (uint8_t)strtol(value, 0, 16);
    }

    // and interate until end
    while (p != 0) {
        if ((p = strtok(NULL, " ,"))) {
            strlcpy(value, p, sizeof(value));
            uint8_t val        = (uint8_t)strtol(value, 0, 16);
            telegram[++length] = val;
        }
    }

    length++;                                                // this is the total amount of bytes
    telegram[length] = _crcCalculator(telegram, length + 1); // add the CRC

    myDebug_P(PSTR("[TEST %d] Injecting telegram %s"), test_num, TEST_DATA[test_num - 1]);

    // go an parse it
    _ems_readTelegram(telegram, length + 1); // include CRC in length
#else
    myDebug_P(PSTR("Firmware not compiled with test data set"));
#endif
}
