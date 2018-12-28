/**
 * ems.cpp
 * handles all the processing of the EMS messages
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP-Boiler
 */

#include "ems.h"
#include "emsuart.h"

#include <Arduino.h>
#include <CircularBuffer.h> // https://github.com/rlogiacco/CircularBuffer
#include <TimeLib.h>

// Check for ESPurna vs ESPHelper (standalone)
#ifdef USE_CUSTOM_H
#include "debug.h"
extern void debugSend(const char * format, ...);
#define myDebug(...) debugSend(__VA_ARGS__)
#else
#include <ESPHelper.h>
extern ESPHelper myESP;
#define myDebug(x, ...) myESP.printf(x, ##__VA_ARGS__)
#endif

// include custom configuration settings
#include "my_config.h"

// calculates size of an 2d array at compile time
template <typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
    return N;
}

_EMS_Sys_Status EMS_Sys_Status; // EMS Status

CircularBuffer<_EMS_TxTelegram, 20> EMS_TxQueue; // FIFO queue for Tx send buffer

// callbacks per type
void _process_UBAMonitorFast(uint8_t * data, uint8_t length);
void _process_UBAMonitorSlow(uint8_t * data, uint8_t length);
void _process_UBAMonitorWWMessage(uint8_t * data, uint8_t length);
void _process_UBAParameterWW(uint8_t * data, uint8_t length);

// Thermostat

// Common
void _process_Version(uint8_t * data, uint8_t length);
void _process_SetPoints(uint8_t * data, uint8_t length);
void _process_RCTime(uint8_t * data, uint8_t length);
void _process_RCOutdoorTempMessage(uint8_t * data, uint8_t length);

// RC20
void _process_RC20Set(uint8_t * data, uint8_t length);
void _process_RC20StatusMessage(uint8_t * data, uint8_t length);

// RC30
void _process_RC30Set(uint8_t * data, uint8_t length);
void _process_RC30StatusMessage(uint8_t * data, uint8_t length);

// Easy
void _process_EasyStatusMessage(uint8_t * data, uint8_t length);

const _Thermostat_Types Thermostat_Types[] = {

    {EMS_ID_THERMOSTAT_RC20, "RC20  (Nefit Moduline 300)"},
    {EMS_ID_THERMOSTAT_RC30, "RC30  (Nefit Moduline 400)"},
    {EMS_ID_THERMOSTAT_EASY, "TC100 (Nefit Easy/CT100)"}

};
uint8_t _Thermostat_Types_max = ArraySize(Thermostat_Types); // number of defined thermostat types

const _EMS_Types EMS_Types[] = {

    // Command commands
    {EMS_ID_NONE, EMS_TYPE_Version, "Version", _process_Version},

    // Boiler commands
    {EMS_ID_BOILER, EMS_TYPE_UBAMonitorFast, "UBAMonitorFast", _process_UBAMonitorFast},
    {EMS_ID_BOILER, EMS_TYPE_UBAMonitorSlow, "UBAMonitorSlow", _process_UBAMonitorSlow},
    {EMS_ID_BOILER, EMS_TYPE_UBAMonitorWWMessage, "UBAMonitorWWMessage", _process_UBAMonitorWWMessage},
    {EMS_ID_BOILER, EMS_TYPE_UBAParameterWW, "UBAParameterWW", _process_UBAParameterWW},
    {EMS_ID_BOILER, EMS_TYPE_UBATotalUptimeMessage, "UBATotalUptimeMessage", NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAMaintenanceSettingsMessage, "UBAMaintenanceSettingsMessage", NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAParametersMessage, "UBAParametersMessage", NULL},
    {EMS_ID_BOILER, EMS_TYPE_UBAMaintenanceStatusMessage, "UBAMaintenanceStatusMessage", NULL},

    // Thermostat commands
    // common
    {EMS_ID_THERMOSTAT, EMS_TYPE_RCTime, "RCTime", _process_RCTime},
    {EMS_ID_THERMOSTAT, EMS_TYPE_RCOutdoorTempMessage, "RCOutdoorTempMessage", _process_RCOutdoorTempMessage},
    {EMS_ID_THERMOSTAT, EMS_TYPE_UBASetPoints, "UBASetPoints", _process_SetPoints},

    // RC20
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC20Set, "RC20Set", _process_RC20Set},
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC20StatusMessage, "RC20StatusMessage", _process_RC20StatusMessage},

    // RC30
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC30Set, "RC30Set", _process_RC30Set},
    {EMS_ID_THERMOSTAT, EMS_TYPE_RC30StatusMessage, "RC30StatusMessage", _process_RC30StatusMessage},

    // Easy
    {EMS_ID_THERMOSTAT, EMS_TYPE_EasyStatusMessage, "EasyStatusMessage", _process_EasyStatusMessage}

};
uint8_t _EMS_Types_max = ArraySize(EMS_Types); // number of defined types

// reserve space for the data we collect from the Boiler and Thermostat
_EMS_Boiler     EMS_Boiler;
_EMS_Thermostat EMS_Thermostat;

// CRC lookup table with poly 12 for faster checking
const uint8_t ems_crc_table[] =
    {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
     0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4A,
     0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70,
     0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96,
     0x98, 0x9A, 0x9C, 0x9E, 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC,
     0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2,
     0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE, 0x19, 0x1B, 0x1D, 0x1F, 0x11,
     0x13, 0x15, 0x17, 0x09, 0x0B, 0x0D, 0x0F, 0x01, 0x03, 0x05, 0x07, 0x39, 0x3B, 0x3D, 0x3F, 0x31, 0x33, 0x35, 0x37,
     0x29, 0x2B, 0x2D, 0x2F, 0x21, 0x23, 0x25, 0x27, 0x59, 0x5B, 0x5D, 0x5F, 0x51, 0x53, 0x55, 0x57, 0x49, 0x4B, 0x4D,
     0x4F, 0x41, 0x43, 0x45, 0x47, 0x79, 0x7B, 0x7D, 0x7F, 0x71, 0x73, 0x75, 0x77, 0x69, 0x6B, 0x6D, 0x6F, 0x61, 0x63,
     0x65, 0x67, 0x99, 0x9B, 0x9D, 0x9F, 0x91, 0x93, 0x95, 0x97, 0x89, 0x8B, 0x8D, 0x8F, 0x81, 0x83, 0x85, 0x87, 0xB9,
     0xBB, 0xBD, 0xBF, 0xB1, 0xB3, 0xB5, 0xB7, 0xA9, 0xAB, 0xAD, 0xAF, 0xA1, 0xA3, 0xA5, 0xA7, 0xD9, 0xDB, 0xDD, 0xDF,
     0xD1, 0xD3, 0xD5, 0xD7, 0xC9, 0xCB, 0xCD, 0xCF, 0xC1, 0xC3, 0xC5, 0xC7, 0xF9, 0xFB, 0xFD, 0xFF, 0xF1, 0xF3, 0xF5,
     0xF7, 0xE9, 0xEB, 0xED, 0xEF, 0xE1, 0xE3, 0xE5, 0xE7};

const uint8_t RX_READ_TIMEOUT_COUNT = 3; // 3 retries before timeout

uint8_t emsRxRetryCount; // used for retries when sending failed

// init stats and counters and buffers
// uses -255 or 255 for values that haven't been set yet (EMS_VALUE_INT_NOTSET and EMS_VALUE_FLOAT_NOTSET)
void ems_init() {
    // overall status
    EMS_Sys_Status.emsRxPgks            = 0;
    EMS_Sys_Status.emsTxPkgs            = 0;
    EMS_Sys_Status.emxCrcErr            = 0;
    EMS_Sys_Status.emsRxStatus          = EMS_RX_IDLE;
    EMS_Sys_Status.emsTxStatus          = EMS_TX_IDLE;
    EMS_Sys_Status.emsRefreshed         = false;
    EMS_Sys_Status.emsPollEnabled       = false; // start up with Poll disabled
    EMS_Sys_Status.emsTxEnabled         = true;  // start up with Tx enabled
    EMS_Sys_Status.emsThermostatEnabled = true;  // there is a RCxx thermostat active as default
    EMS_Sys_Status.emsBoilerEnabled     = false; // boiler is not connected yet

    EMS_Sys_Status.emsLogging = EMS_SYS_LOGGING_NONE; // Verbose logging is off

    // thermostat
    EMS_Thermostat.type   = EMS_ID_THERMOSTAT; // type, see my_config.h
    EMS_Thermostat.hour   = 0;
    EMS_Thermostat.minute = 0;
    EMS_Thermostat.second = 0;
    EMS_Thermostat.day    = 0;
    EMS_Thermostat.month  = 0;
    EMS_Thermostat.year   = 0;
    EMS_Thermostat.mode   = 255; // dummy value

    // UBAParameterWW
    EMS_Boiler.wWActivated   = EMS_VALUE_INT_NOTSET; // Warm Water activated
    EMS_Boiler.wWSelTemp     = EMS_VALUE_INT_NOTSET; // Warm Water selected temperature
    EMS_Boiler.wWCircPump    = EMS_VALUE_INT_NOTSET; // Warm Water circulation pump available
    EMS_Boiler.wWDesiredTemp = EMS_VALUE_INT_NOTSET; // Warm Water desired temperature to prevent infection
    EMS_Boiler.wWComfort     = EMS_VALUE_INT_NOTSET;

    // UBAMonitorFast
    EMS_Boiler.selFlowTemp = EMS_VALUE_INT_NOTSET;   // Selected flow temperature
    EMS_Boiler.curFlowTemp = EMS_VALUE_FLOAT_NOTSET; // Current flow temperature
    EMS_Boiler.retTemp     = EMS_VALUE_FLOAT_NOTSET; // Return temperature
    EMS_Boiler.burnGas     = EMS_VALUE_INT_NOTSET;   // Gas on/off
    EMS_Boiler.fanWork     = EMS_VALUE_INT_NOTSET;   // Fan on/off
    EMS_Boiler.ignWork     = EMS_VALUE_INT_NOTSET;   // Ignition on/off
    EMS_Boiler.heatPmp     = EMS_VALUE_INT_NOTSET;   // Boiler pump on/off
    EMS_Boiler.wWHeat      = EMS_VALUE_INT_NOTSET;   // 3-way valve on WW
    EMS_Boiler.wWCirc      = EMS_VALUE_INT_NOTSET;   // Circulation on/off
    EMS_Boiler.selBurnPow  = EMS_VALUE_INT_NOTSET;   // Burner max power
    EMS_Boiler.curBurnPow  = EMS_VALUE_INT_NOTSET;   // Burner current power
    EMS_Boiler.flameCurr   = EMS_VALUE_FLOAT_NOTSET; // Flame current in micro amps
    EMS_Boiler.sysPress    = EMS_VALUE_FLOAT_NOTSET; // System pressure
    EMS_Boiler.serviceCodeChar1 = EMS_VALUE_INT_NOTSET; //
    EMS_Boiler.serviceCodeChar2 = EMS_VALUE_INT_NOTSET; //

    // UBAMonitorSlow
    EMS_Boiler.extTemp     = EMS_VALUE_FLOAT_NOTSET; // Outside temperature
    EMS_Boiler.boilTemp    = EMS_VALUE_FLOAT_NOTSET; // Boiler temperature
    EMS_Boiler.pumpMod     = EMS_VALUE_INT_NOTSET;   // Pump modulation
    EMS_Boiler.burnStarts  = EMS_VALUE_INT_NOTSET;   // # burner restarts
    EMS_Boiler.burnWorkMin = EMS_VALUE_INT_NOTSET;   // Total burner operating time
    EMS_Boiler.heatWorkMin = EMS_VALUE_INT_NOTSET;   // Total heat operating time

    // UBAMonitorWWMessage
    EMS_Boiler.wWCurTmp  = EMS_VALUE_FLOAT_NOTSET; // Warm Water current temperature:
    EMS_Boiler.wWStarts  = EMS_VALUE_INT_NOTSET;   // Warm Water # starts
    EMS_Boiler.wWWorkM   = EMS_VALUE_INT_NOTSET;   // Warm Water # minutes
    EMS_Boiler.wWOneTime = EMS_VALUE_INT_NOTSET;   // Warm Water one time function on/off
    EMS_Boiler.wWCurFlow = EMS_VALUE_INT_NOTSET;

    EMS_Boiler.tapwaterActive = EMS_VALUE_INT_NOTSET; // Hot tap water is on/off
    EMS_Boiler.heatingActive  = EMS_VALUE_INT_NOTSET; // Central heating is on/off
}

// Getters and Setters for parameters
void ems_setPoll(bool b) {
    EMS_Sys_Status.emsPollEnabled = b;
    myDebug("EMS Bus Poll is set to %s\n", EMS_Sys_Status.emsPollEnabled ? "enabled" : "disabled");
}

bool ems_getPoll() {
    return EMS_Sys_Status.emsPollEnabled;
}

/**
 * ! Getters and Setters for parameters
 */
void ems_setTxEnabled(bool b) {
    EMS_Sys_Status.emsTxEnabled = b;
    myDebug("EMS Bus Tx is set to %s\n", EMS_Sys_Status.emsTxEnabled ? "enabled" : "disabled");
}

bool ems_getTxEnabled() {
    return EMS_Sys_Status.emsTxEnabled;
}

bool ems_getBoilerEnabled() {
    return EMS_Sys_Status.emsBoilerEnabled;
}

bool ems_getEmsRefreshed() {
    return EMS_Sys_Status.emsRefreshed;
}

void ems_setEmsRefreshed(bool b) {
    EMS_Sys_Status.emsRefreshed = b;
}

bool ems_getThermostatEnabled() {
    return EMS_Sys_Status.emsThermostatEnabled;
}

void ems_setThermostatEnabled(bool b) {
    EMS_Sys_Status.emsThermostatEnabled = b;
    myDebug("Thermostat is set to %s\n", EMS_Sys_Status.emsThermostatEnabled ? "enabled" : "disabled");
}

_EMS_SYS_LOGGING ems_getLogging() {
    return EMS_Sys_Status.emsLogging;
}

uint8_t ems_getEmsTypesCount() {
    return _EMS_Types_max;
}

uint8_t ems_getThermostatTypesCount() {
    return _Thermostat_Types_max;
}

void ems_setLogging(_EMS_SYS_LOGGING loglevel) {
    if (loglevel <= EMS_SYS_LOGGING_VERBOSE) {
        EMS_Sys_Status.emsLogging = loglevel;
        myDebug("System Logging is set to ");
        if (loglevel == EMS_SYS_LOGGING_NONE) {
            myDebug("None\n");
        } else if (loglevel == EMS_SYS_LOGGING_BASIC) {
            myDebug("Basic\n");
        } else if (loglevel == EMS_SYS_LOGGING_VERBOSE) {
            myDebug("Verbose\n");
        } else if (loglevel == EMS_SYS_LOGGING_THERMOSTAT) {
            myDebug("Thermostat only\n");
        } else if (loglevel == EMS_SYS_LOGGING_RAW) {
            myDebug("Raw mode\n");
        }
    }
}

/**
 * Calculate CRC checksum using lookup table for speed
 * len is length of data in bytes (including the CRC byte at end)
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

/** 
 * function to turn a telegram int (2 bytes) to a float. The source is *10
 * negative values are stored as 1-compliment (https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c)
 */
float _toFloat(uint8_t i, uint8_t * data) {
    // if the MSB is set, it's a negative number or an error
    if ((data[i] & 0x80) == 0x80) {
        // check if its an invalid number
        // 0x8000 is used when sensor is missing
        if ((data[i] == 0x80) && (data[i + 1] == 0)) {
            return (float)EMS_VALUE_FLOAT_NOTSET; // return -1 to indicate that is unknown
        }
        // its definitely a negative number
        // assume its 1-compliment, otherwise we need add 1 to the total for 2-compliment
        int16_t x = (data[i] << 8) + data[i + 1];
        return ((float)(x)) / 10;
    } else {
        // positive number
        return ((float)(((data[i] << 8) + data[i + 1]))) / 10;
    }
}

// function to turn a telegram long (3 bytes) to a long int
uint16_t _toLong(uint8_t i, uint8_t * data) {
    return (((data[i]) << 16) + ((data[i + 1]) << 8) + (data[i + 2]));
}

/**
 * Find the pointer to the EMS_Types array for a given type ID
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
 * debug print a telegram to telnet console
 * len is length in bytes including the CRC
 */
void _debugPrintTelegram(const char * prefix, uint8_t * data, uint8_t len, const char * color) {
    if (EMS_Sys_Status.emsLogging <= EMS_SYS_LOGGING_BASIC)
        return;

    myDebug("%s%s telegram: ", color, prefix);
    for (int i = 0; i < len - 1; i++) {
        myDebug("%02X ", data[i]);
    }
    myDebug("(CRC=%02X", data[len - 1]);

    // print number of data bytes only if its a valid telegram
    if (len > 5) {
        myDebug(", #data=%d", (len - 5));
    }
    myDebug(")%s\n", COLOR_RESET);
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

    // if we're in raw mode just fire and forget
    if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_RAW) {
        EMS_TxTelegram.data[EMS_TxTelegram.length - 1] =
            _crcCalculator(EMS_TxTelegram.data, EMS_TxTelegram.length);                             // add the CRC
        _debugPrintTelegram("Sending raw", EMS_TxTelegram.data, EMS_TxTelegram.length, COLOR_CYAN); // always show
        emsuart_tx_buffer(EMS_TxTelegram.data, EMS_TxTelegram.length); // send the telegram to the UART Tx
        EMS_Sys_Status.emsTxStatus = EMS_TX_IDLE;                      // finished sending
        EMS_TxQueue.shift();                                           // remove from queue
        return;
    }

    // if Tx is disabled, don't do anything and ignore the request
    if (!EMS_Sys_Status.emsTxEnabled) {
        myDebug("Tx is disabled. Ignoring %s request to 0x%02X.\n",
                ((EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) ? "write" : "read"),
                EMS_TxTelegram.dest & 0x7F);
        EMS_Sys_Status.emsTxStatus = EMS_TX_IDLE; // finished sending
        EMS_TxQueue.shift();                      // remove from queue
        return;
    }

    // if this telegram has already been processed then skip it
    // leave on queue until its processed later on
    if (EMS_TxTelegram.hasSent) {
        // myDebug("Already sent!");
        return;
    }

    EMS_Sys_Status.emsTxStatus = EMS_TX_ACTIVE;

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
    // finally calculate CRC and add it
    EMS_TxTelegram.data[EMS_TxTelegram.length - 1] = _crcCalculator(EMS_TxTelegram.data, EMS_TxTelegram.length);

    // print debug info
    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
        char s[64];
        if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
            sprintf(s, "Sending write of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
            sprintf(s, "Sending read of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
            sprintf(s, "Sending validate of type 0x%02X to 0x%02X:", EMS_TxTelegram.type, EMS_TxTelegram.dest & 0x7F);
        }

        _debugPrintTelegram(s, EMS_TxTelegram.data, EMS_TxTelegram.length, COLOR_CYAN);
    }

    // send the telegram to the UART Tx
    emsuart_tx_buffer(EMS_TxTelegram.data, EMS_TxTelegram.length);

    EMS_Sys_Status.emsTxPkgs++;

    // dirty hack. we really shouldn't be changing values in the buffer directly.
    EMS_TxTelegram.hasSent = true;

    // if it was a write command, check if we need to do a new read to validate the results
    // we do this by turning the last write into a read
    if ((EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) && (EMS_TxTelegram.type_validate != EMS_ID_NONE)) {
        // create a new Telegram copying from the last write
        _EMS_TxTelegram new_EMS_TxTelegram;

        // copy details
        new_EMS_TxTelegram.type_validate   = EMS_TxTelegram.type_validate;
        new_EMS_TxTelegram.dest            = EMS_TxTelegram.dest;
        new_EMS_TxTelegram.type            = EMS_TxTelegram.type;
        new_EMS_TxTelegram.action          = EMS_TX_TELEGRAM_VALIDATE;
        new_EMS_TxTelegram.offset          = EMS_TxTelegram.comparisonOffset; // location of byte to fetch
        new_EMS_TxTelegram.dataValue       = 1;                               // fetch single byte
        new_EMS_TxTelegram.length          = EMS_MIN_TELEGRAM_LENGTH; // is always 6 bytes long (including CRC at end)
        new_EMS_TxTelegram.comparisonValue = EMS_TxTelegram.comparisonValue;
        new_EMS_TxTelegram.comparisonPostRead = EMS_TxTelegram.comparisonPostRead;
        new_EMS_TxTelegram.comparisonOffset   = EMS_TxTelegram.comparisonOffset;
        new_EMS_TxTelegram.hasSent            = false;

        // remove old telegram from queue and add this new read one
        EMS_TxQueue.shift();                     // remove from queue
        EMS_TxQueue.unshift(new_EMS_TxTelegram); // add back to queue making it next in line
    }

    EMS_Sys_Status.emsTxStatus = EMS_TX_IDLE;
}

/**
 * the main logic that parses the telegram message
 * length is only data bytes, excluding the BRK
 * Read commands are asynchronous as they're handled by the interrupt
 * When we receive a Poll Request we need to send any Tx packages quickly
 */
void ems_parseTelegram(uint8_t * telegram, uint8_t length) {
    // check if we just received a single byte
    // it could well be a Poll request from the boiler which has an ID 0x8B (0x0B | 0x80 to set 8th bit)
    // or either a return code like 0x01 or 0x04 from the last Write command issued
    if (length == 1) {
        uint8_t value = telegram[0]; // 1st byte of data package

        // check first for a Poll for us
        if (value == (EMS_ID_ME | 0x80)) {
            // set the timestamp of the last poll, we use this to see if we have a connection to the boiler
            EMS_Sys_Status.emsBoilerEnabled = true;

            // do we have something to send thats waiting in the Tx queue? if so send it
            if (!EMS_TxQueue.isEmpty()) {
                _ems_sendTelegram(); // perform the read/write command
            } else {
                // nothing to send so just send a poll acknowledgement back
                if (EMS_Sys_Status.emsPollEnabled) {
                    emsaurt_tx_poll();
                }
            }
        } else if ((value == EMS_TX_ERROR) || (value == EMS_TX_SUCCESS)) {
            // if its a success (01) or failure (04), then see if its from one of our last writes
            // a response from UBA after a write should be within a specific time period < 100ms
            // TODO what we should really do here is just cancel the write operation
            if (!EMS_TxQueue.isEmpty()) {
                _EMS_TxTelegram EMS_TxTelegram = EMS_TxQueue.first(); // get current Tx package we last sent
                if ((EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) && (value == EMS_TX_ERROR)) {
                    if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE) {
                        myDebug("** Error: last write failed. removing write op from queue!\n");
                    }
                    EMS_TxQueue.shift(); // write failed so remove from queue. pretty sloppy.
                }
                emsaurt_tx_poll(); // send a poll to free the EMS bus
            }
        }
        return; // all done here, quit. if we haven't processes anything its a poll but not for us
    }

    // ignore anything that doesn't resemble a proper telegram package
    // minimal is 5 bytes, excluding CRC at the end
    /*
    if (length < EMS_MIN_TELEGRAM_LENGTH - 1) {
        _debugPrintTelegram("Noisy data:", telegram, length, COLOR_RED);
        return;
    }
    */

    // Assume at this point we have something that vaguely resembles a telegram
    // see if we got a telegram as [src] [dest] [type] [offset] [data] [crc]
    // so is at least 6 bytes long and the CRC checks out (which is last byte)
    uint8_t crc = _crcCalculator(telegram, length);
    if (telegram[length - 1] != crc) {
        EMS_Sys_Status.emxCrcErr++;
        _debugPrintTelegram("Corrupt telegram:", telegram, length, COLOR_RED);
    } else {
        // if we in raw mode then just output the telegram
        if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_RAW) {
            for (int i = 0; i < length; i++) {
                myDebug("%02X ", telegram[i]);
            }
            myDebug("\n");
        }

        // here we know its a valid incoming telegram of at least 6 bytes
        // lets process it and see what to do next
        _processType(telegram, length);
    }
}

/**
 * print detailed telegram
 * and then call its callback if there is one defined
 */
void _ems_processTelegram(uint8_t * telegram, uint8_t length) {
    // header
    uint8_t   src  = telegram[0] & 0x7F;
    uint8_t   dest = telegram[1] & 0x7F; // remove 8th bit to handle both reads and writes
    uint8_t   type = telegram[2];
    uint8_t * data = telegram + 4; // data block starts at position 5

    // print detailed telegram data
    if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_THERMOSTAT) {
        char color_s[20];
        char src_s[20];
        char dest_s[20];
        char s[100];

        // set source string
        if (src == EMS_ID_BOILER) {
            strcpy(src_s, "Boiler -> ");
        } else if (src == EMS_ID_THERMOSTAT) {
            strcpy(src_s, "Thermostat -> ");
        } else {
            sprintf(src_s, "0x%02X -> ", src);
        }

        // set destination string
        if (dest == EMS_ID_ME) {
            strcpy(dest_s, "me");
            strcpy(color_s, COLOR_YELLOW);
        } else if (dest == EMS_ID_NONE) {
            // it's probably just a broadcast
            strcpy(dest_s, "all");
            strcpy(color_s, COLOR_GREEN);
        } else if (dest == EMS_ID_BOILER) {
            strcpy(dest_s, "Boiler");
            strcpy(color_s, COLOR_MAGENTA);
        } else if (dest == EMS_ID_THERMOSTAT) {
            strcpy(dest_s, "Thermostat");
            strcpy(color_s, COLOR_MAGENTA);
        } else {
            sprintf(dest_s, "0x%02X", dest);
            strcpy(color_s, COLOR_MAGENTA);
        }

        sprintf(s, "%s%s, type 0x%02X", src_s, dest_s, type);
        // and print telegram

        if (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_THERMOSTAT) {
            // only print ones to/from thermostat if logging is set to thermostat only
            if ((src == EMS_ID_THERMOSTAT) || (dest == EMS_ID_THERMOSTAT)) {
                _debugPrintTelegram(s, telegram, length, color_s);
            }
        } else {
            // allways print
            _debugPrintTelegram(s, telegram, length, color_s);
        }
    }

    // try and match it against known types and call the call handler function
    // only process telegrams broadcasting to everyone or sent to us specifically
    if ((dest == EMS_ID_ME) || (dest == EMS_ID_NONE)) {
        int  i         = 0;
        bool typeFound = false;
        while (i < _EMS_Types_max) {
            if (((EMS_Types[i].src == src) || (EMS_Types[i].src == EMS_ID_NONE)) && (EMS_Types[i].type == type)) {
                // we have a match
                typeFound = true;
                // call callback to fetch the values from the telegram
                // data block is sent, which starts with the 5th byte of the telegram
                // return value tells us if we need to force send values back to MQTT
                // the length is the #bytes of the data (excluding the header and CRC)
                if ((EMS_Types[i].processType_cb) != (void *)NULL) {
                    // print non-verbose message
                    if ((EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_BASIC)
                        || (EMS_Sys_Status.emsLogging == EMS_SYS_LOGGING_VERBOSE)) {
                        myDebug("<--- %s(0x%02X) received\n", EMS_Types[i].typeString, type);
                    }
                    (void)EMS_Types[i].processType_cb(data, length - 5);
                }
                break;
            }
            i++;
        }
    }
}


/**
 * deciphers the telegram packet
 * length is only data bytes, excluding the BRK
 * We only remove from the Tx queue if the read or write was successful
 */
void _processType(uint8_t * telegram, uint8_t length) {
    // header
    uint8_t   src  = telegram[0] & 0x7F; // removing 8th bit as we deal with both reads and writes
    uint8_t   dest = telegram[1] & 0x7F; // remove 8th bit to handle both reads and writes
    uint8_t   type = telegram[2];
    uint8_t * data = telegram + 4; // data block starts at position 5

    // if its an echo of ourselves from the master, ignore
    if (src == EMS_ID_ME) {
        // _debugPrintTelegram("Telegram echo:", telegram, length, COLOR_BLUE);
        return;
    }

    // did we request this telegram? If so it would be a read or a validate telegram still on the Tx queue
    // with the same type
    // if its a validate check the value, or if its a read, update the Read counter
    // then we can safely removed the read/validate from the queue
    if ((dest == EMS_ID_ME) && (!EMS_TxQueue.isEmpty())) {
        _EMS_TxTelegram EMS_TxTelegram = EMS_TxQueue.first(); // get current Tx package we last sent

        // do the types match? If so we were expecting this response back to us
        if (EMS_TxTelegram.type == type) {
            emsaurt_tx_poll(); // send Acknowledgement back to free the EMS bus

            // if last action was a read, where just happy that we got a response back
            if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
                EMS_Sys_Status.emsRxPgks++;             // increment rx counter
                emsRxRetryCount = 0;                    // reset retry count
                _ems_processTelegram(telegram, length); // and process it
                if (EMS_TxTelegram.forceRefresh) {
                    ems_setEmsRefreshed(true); // set the MQTT refresh flag to force sending to MQTT
                }
                EMS_TxQueue.shift(); // remove read from queue
            } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
                // this read was for a validate. Do a compare on the 1 byte result from the last write
                uint8_t dataReceived = data[0]; // only a single byte is returned after a read
                if (EMS_TxTelegram.comparisonValue == dataReceived) {
                    // there is a match, so write must have been successful
                    EMS_TxQueue.shift(); // remove validate from queue
                    if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                        myDebug("Write to 0x%02X successful.\n", EMS_TxTelegram.dest);
                    }
                    ems_doReadCommand(EMS_TxTelegram.comparisonPostRead,
                                      EMS_TxTelegram.dest,
                                      true); // get values and force a refresh to MQTT
                } else {
                    // write failed.
                    if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                        myDebug("Last write failed. Compared set value 0x%02X with received value 0x%02X. ",
                                EMS_TxTelegram.comparisonValue,
                                dataReceived);
                    }
                    if (emsRxRetryCount++ >= RX_READ_TIMEOUT_COUNT) {
                        // give up
                        if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                            myDebug("Giving up!\n");
                        }
                        EMS_TxQueue.shift(); // remove from queue
                    } else {
                        // retry, turn the validate back into a write and try again
                        if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
                            myDebug("Retrying attempt %d...\n", emsRxRetryCount);
                        }
                        EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
                        EMS_TxTelegram.dataValue = EMS_TxTelegram.comparisonValue;  // restore old value
                        EMS_TxTelegram.offset    = EMS_TxTelegram.comparisonOffset; // restore old value
                        EMS_TxQueue.shift();                                        // remove validate from queue
                        EMS_TxQueue.unshift(EMS_TxTelegram); // add back to queue making it next in line
                    }
                }
            }
        }
        // telegram was for us, but seems we didn't ask for it
    } else {
        // we didn't request it
        _ems_processTelegram(telegram, length); // and process and print it
    }
}

/**
 * Check if hot tap water or heating is active
 * using a quick hack:
 *   heating is on if Selected Flow Temp >= 70 (in my case)
 *   tap water is on if Selected Flow Temp = 0 and Selected Burner Power >= 115
 */
bool _checkActive() {
    // hot tap water
    EMS_Boiler.tapwaterActive =
        ((EMS_Boiler.wWCurFlow != 0) //this is easier
         && (EMS_Boiler.burnGas == EMS_VALUE_INT_ON));

    // heating
    EMS_Boiler.heatingActive =
        ((EMS_Boiler.selFlowTemp >= EMS_BOILER_SELFLOWTEMP_HEATING) && (EMS_Boiler.burnGas == EMS_VALUE_INT_ON));
}

/**
 * UBAParameterWW - type 0x33 - warm water parameters
 * received only after requested (not broadcasted)
 */
void _process_UBAParameterWW(uint8_t * data, uint8_t length) {
    EMS_Boiler.wWActivated   = (data[1] == 0xFF); // 0xFF means on
    EMS_Boiler.wWSelTemp     = data[2];
    EMS_Boiler.wWCircPump    = (data[6] == 0xFF); // 0xFF means on
    EMS_Boiler.wWDesiredTemp = data[8];
    EMS_Boiler.wWComfort     = (data[EMS_OFFSET_UBAParameterWW_wwComfort] == 0x00);

    // when we receieve this, lets force an MQTT publish
    EMS_Sys_Status.emsRefreshed = true;
}

/**
 * UBAMonitorWWMessage - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void _process_UBAMonitorWWMessage(uint8_t * data, uint8_t length) {
    EMS_Boiler.wWCurTmp  = _toFloat(1, data);
    EMS_Boiler.wWStarts  = _toLong(13, data);
    EMS_Boiler.wWWorkM   = _toLong(10, data);
    EMS_Boiler.wWOneTime = bitRead(data[5], 1);
    EMS_Boiler.wWCurFlow = data[9];
}

/**
 * UBAMonitorFast - type 0x18 - central heating monitor part 1 (25 bytes long)
 * received every 10 seconds
 */
void _process_UBAMonitorFast(uint8_t * data, uint8_t length) {
    EMS_Boiler.selFlowTemp = data[0];
    EMS_Boiler.curFlowTemp = _toFloat(1, data);
    EMS_Boiler.retTemp     = _toFloat(13, data);

    uint8_t v          = data[7];
    EMS_Boiler.burnGas = bitRead(v, 0);
    EMS_Boiler.fanWork = bitRead(v, 2);
    EMS_Boiler.ignWork = bitRead(v, 3);
    EMS_Boiler.heatPmp = bitRead(v, 5);
    EMS_Boiler.wWHeat  = bitRead(v, 6);
    EMS_Boiler.wWCirc  = bitRead(v, 7);

    EMS_Boiler.curBurnPow = data[4];
    EMS_Boiler.selBurnPow = data[3]; // burn power max setting

    EMS_Boiler.flameCurr = _toFloat(15, data);

    //read the service code / installation status as appears on the display
    EMS_Boiler.serviceCodeChar1 = data[18];  //ascii character 1
    EMS_Boiler.serviceCodeChar2 = data[19];  //ascii character 2
    
    if (data[17] == 0xFF) { // missing value for system pressure
        EMS_Boiler.sysPress = 0;
    } else {
        EMS_Boiler.sysPress = (((float)data[17]) / (float)10);
    }

    // at this point do a quick check to see if the hot water or heating is active
    (void)_checkActive();
}

/**
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 */
void _process_UBAMonitorSlow(uint8_t * data, uint8_t length) {
    EMS_Boiler.extTemp     = _toFloat(0, data); // 0x8000 if not available
    EMS_Boiler.boilTemp    = _toFloat(2, data); // 0x8000 if not available
    EMS_Boiler.pumpMod     = data[9];
    EMS_Boiler.burnStarts  = _toLong(10, data);
    EMS_Boiler.burnWorkMin = _toLong(13, data);
    EMS_Boiler.heatWorkMin = _toLong(19, data);
}

/**
 * RC20StatusMessage - type 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
 * For reading the temp values only
 * received every 60 seconds
 */
void _process_RC20StatusMessage(uint8_t * data, uint8_t length) {
    EMS_Thermostat.setpoint_roomTemp = ((float)data[1]) / (float)2;
    EMS_Thermostat.curr_roomTemp     = _toFloat(2, data);

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back to Home Assistant via MQTT
}

/**
 * RC30StatusMessage - type 0x41 - data from the RC30 thermostat (0x10) - 14 bytes long
 * For reading the temp values only
 * received every 60 seconds
 */
void _process_RC30StatusMessage(uint8_t * data, uint8_t length) {
    EMS_Thermostat.setpoint_roomTemp = ((float)data[1]) / (float)2;
    EMS_Thermostat.curr_roomTemp     = _toFloat(2, data);

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back to Home Assistant via MQTT
}

/**
 * EasyStatusMessage - type 0x0A - data from the Nefit Easy/TC100 thermostat (0x18) - 31 bytes long
 * The Easy has a digital precision of its floats to 2 decimal places, so values is divided by 100
 */
void _process_EasyStatusMessage(uint8_t * data, uint8_t length) {
    EMS_Thermostat.curr_roomTemp     = ((float)(((data[8] << 8) + data[9]))) / 100;
    EMS_Thermostat.setpoint_roomTemp = ((float)(((data[10] << 8) + data[11]))) / 100;

    EMS_Sys_Status.emsRefreshed = true; // triggers a send the values back to Home Assistant via MQTT
}

/**
 * RC20Temperature - type 0xA8 - for reading the mode from the RC20 thermostat (0x17)
 * received only after requested
 */
void _process_RC20Set(uint8_t * data, uint8_t length) {
    EMS_Thermostat.mode = data[EMS_OFFSET_RC20Set_mode];
}

/**
 * RC30Temperature - type 0xA7 - for reading the mode from the RC30 thermostat (0x10)
 * received only after requested
 */
void _process_RC30Set(uint8_t * data, uint8_t length) {
    EMS_Thermostat.mode = data[EMS_OFFSET_RC30Set_mode];
}

/**
 * RCOutdoorTempMessage - type 0xA3 - for external temp settings from the the RC* thermostats
 */
void _process_RCOutdoorTempMessage(uint8_t * data, uint8_t length) {
    // add support here if you're reading external sensors
}

/**
 * Version - type 0x02 - get the firmware version and type of a EMS device (Boiler, Thermostat etc)
 * When a thermostat is connecting it will send out 0x02 messages too, which we'll ignore
 * We don't bother storing these values anywhere, just print them for now
 *  Moduline 300, Type 77. Version 3.03
 *  Moduline 400, Type 78, Version 3.03
 *  Nefit Easy = Type 202. Version 2.19 
 *  Nefit Trendline HRC30 = Type 123. Version 6.1
 */
void _process_Version(uint8_t * data, uint8_t length) {
    // ignore short messages that we can't interpret
    if (length >= 3) {
        uint8_t type  = data[0];
        uint8_t major = data[1];
        uint8_t minor = data[2];
        if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_BASIC) {
            myDebug("Product ID %d. Version %02d.%02d\n", type, major, minor);
        }
    }
}

/**
 * UBASetPoint 0x1A, for RC20
 */
void _process_SetPoints(uint8_t * data, uint8_t length) {
    if (EMS_Sys_Status.emsLogging >= EMS_SYS_LOGGING_THERMOSTAT) {
        if (length != 0) {
            uint8_t setpoint = data[0];
            uint8_t hk_power = data[1];
            uint8_t ww_power = data[2];
            myDebug(" SetPoint=%d, hk_power=%d ww_power=%d\n", setpoint, hk_power, ww_power);
        }
        myDebug("\n");
    }
}

/**
 * process_RCTime - type 0x06 - date and time from a thermostat - 14 bytes long
 * common for all thermostats
 */
void _process_RCTime(uint8_t * data, uint8_t length) {
    if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_EASY) {
        return; // not supported
    }

    EMS_Thermostat.hour   = data[2];
    EMS_Thermostat.minute = data[4];
    EMS_Thermostat.second = data[5];
    EMS_Thermostat.day    = data[3];
    EMS_Thermostat.month  = data[1];
    EMS_Thermostat.year   = data[0];

    // we can optional set the time based on the thermostat's time if we want.
    /*
    setTime(EMS_Thermostat.hour,
            EMS_Thermostat.minute,
            EMS_Thermostat.second,
            EMS_Thermostat.day,
            EMS_Thermostat.month,
            EMS_Thermostat.year + 2000);
    */
}

/**
 * Print the Tx queue - for debugging
 */
void ems_printTxQueue() {
    _EMS_TxTelegram EMS_TxTelegram;
    char            sType[20];

    myDebug("Tx queue (%d/%d)\n", EMS_TxQueue.size(), EMS_TxQueue.capacity);

    for (byte i = 0; i < EMS_TxQueue.size(); i++) {
        EMS_TxTelegram = EMS_TxQueue[i]; // retrieves the i-th element from the buffer without removing it

        // get action
        if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_WRITE) {
            strcpy(sType, "write");
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_READ) {
            strcpy(sType, "read");
        } else if (EMS_TxTelegram.action == EMS_TX_TELEGRAM_VALIDATE) {
            strcpy(sType, "validate");
        } else {
            strcpy(sType, "?");
        }

        myDebug(" [%d] action=%s dest=0x%02x type=0x%02x offset=%d length=%d dataValue=%d "
                "comparisonValue=%d hasSent=%d, type_validate=0x%02x comparisonPostRead=0x%02x\n",
                i,
                sType,
                EMS_TxTelegram.dest & 0x7F,
                EMS_TxTelegram.type,
                EMS_TxTelegram.offset,
                EMS_TxTelegram.length,
                EMS_TxTelegram.dataValue,
                EMS_TxTelegram.comparisonValue,
                EMS_TxTelegram.hasSent,
                EMS_TxTelegram.type_validate,
                EMS_TxTelegram.comparisonPostRead);
    }
}

/**
 * Generic function to return temperature settings from the thermostat
 * Supports RC20, RC30 and Easy
 */
void ems_getThermostatValues() {
    if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_RC20) {
        ems_doReadCommand(EMS_TYPE_RC20StatusMessage, EMS_ID_THERMOSTAT); // to get the setpoint temp
        ems_doReadCommand(EMS_TYPE_RC20Set, EMS_ID_THERMOSTAT);           // to get the mode
    } else if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_RC30) {
        ems_doReadCommand(EMS_TYPE_RC30StatusMessage, EMS_ID_THERMOSTAT); // to get the setpoint temp
        ems_doReadCommand(EMS_TYPE_RC30Set, EMS_ID_THERMOSTAT);           // to get the mode
    } else if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_EASY) {
        ems_doReadCommand(EMS_TYPE_EasyStatusMessage, EMS_ID_THERMOSTAT);
    }
}

/**
 *  print out current thermostat type
 */
void ems_printThermostatType() {
    int  i         = 0;
    bool typeFound = false;
    while (i < _Thermostat_Types_max) {
        if (Thermostat_Types[i].id == EMS_ID_THERMOSTAT) {
            typeFound = true; // we have a match
            break;
        }
        i++;
    }
    if (typeFound) {
        myDebug("%s [ID 0x%02X]", Thermostat_Types[i].typeString, Thermostat_Types[i].id);
    } else {
        myDebug("Unknown? [ID 0x%02X]", Thermostat_Types[i].id);
    }
}

/**
 * Print out all handled types
 */
void ems_printAllTypes() {
    myDebug("These %d telegram type IDs are recognized:\n", _EMS_Types_max);
    uint8_t i;
    char    s[20];

    for (i = 0; i < _EMS_Types_max; i++) {
        if (EMS_Types[i].src == EMS_ID_THERMOSTAT) {
            strcpy(s, "Thermostat");
        } else if (EMS_Types[i].src == EMS_ID_BOILER) {
            strcpy(s, "Boiler");
        } else {
            strcpy(s, "Common");
        }
        myDebug(" %s:\ttype ID %02X (%s)\n", s, EMS_Types[i].type, EMS_Types[i].typeString);
    }

    myDebug("\nThese %d telegram Thermostats are natively supported:\n", _Thermostat_Types_max);

    for (i = 0; i < _Thermostat_Types_max; i++) {
        myDebug(" %s  [ID 0x%02X]\n", Thermostat_Types[i].typeString, Thermostat_Types[i].id);
    }
}

/**
 * Send a command to UART Tx to Read from another device
 * Read commands when sent must respond by the destination (target) immediately (or within 10ms)
 */
void ems_doReadCommand(uint8_t type, uint8_t dest, bool forceRefresh) {
    // if not a valid type of boiler is not accessible then quit
    if ((type == EMS_ID_NONE) || (!EMS_Sys_Status.emsBoilerEnabled)) {
        return;
    }

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    // see if its a known type
    int i = _ems_findType(type);

    if ((ems_getLogging() == EMS_SYS_LOGGING_BASIC) || (ems_getLogging() == EMS_SYS_LOGGING_VERBOSE)) {
        if (i == -1) {
            myDebug("Requesting type (0x%02X) from dest 0x%02X\n", type, dest);
        } else {
            myDebug("Requesting type %s(0x%02X) from dest 0x%02X\n", EMS_Types[i].typeString, type, dest);
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
 */
void ems_sendRawTelegram(char * telegram) {
    uint8_t count = 0;
    char *  p, value[10];

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    // get first value, which should be the src
    if (p = strtok(telegram, " ,")) { // delimiter
        strcpy(value, p);
        EMS_TxTelegram.data[0] = (uint8_t)strtol(value, 0, 16);
    }
    // and interate until end
    while (p != 0) {
        if (p = strtok(NULL, " ,")) {
            strcpy(value, p);
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

    // calculate length including header and CRC
    EMS_TxTelegram.length        = count + 2;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;
    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_RAW;

    // add to Tx queue. Assume it's not full.
    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the temperature of the thermostat
 */
void ems_setThermostatTemp(float temperature) {
    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest   = EMS_ID_THERMOSTAT;

    myDebug("Setting new thermostat temperature\n");

    if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_RC20) {
        EMS_TxTelegram.type      = EMS_TYPE_RC20Set;
        EMS_TxTelegram.offset    = EMS_OFFSET_RC20Set_temp;
        EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
        EMS_TxTelegram.dataValue = (uint8_t)((float)temperature * (float)2); // value

        EMS_TxTelegram.type_validate    = EMS_TxTelegram.type;
        EMS_TxTelegram.comparisonOffset = EMS_TxTelegram.offset;
        EMS_TxTelegram.comparisonValue  = EMS_TxTelegram.dataValue;
        EMS_TxTelegram.comparisonPostRead =
            EMS_TYPE_RC20StatusMessage; // call a different type to refresh temperature value

    } else if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_RC30) {
        EMS_TxTelegram.type      = EMS_TYPE_RC30Set;
        EMS_TxTelegram.offset    = EMS_OFFSET_RC30Set_temp;
        EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
        EMS_TxTelegram.dataValue = (uint8_t)((float)temperature * (float)2); // value

        EMS_TxTelegram.type_validate    = EMS_TxTelegram.type;
        EMS_TxTelegram.comparisonOffset = EMS_TxTelegram.offset;
        EMS_TxTelegram.comparisonValue  = EMS_TxTelegram.dataValue;
        EMS_TxTelegram.comparisonPostRead =
            EMS_TYPE_RC30StatusMessage; // call a different type to refresh temperature value
    } else if (EMS_Thermostat.type == EMS_ID_THERMOSTAT_EASY) {
        myDebug("Setting new thermostat temperature on an Easy (not working yet!)\n");
        return;
    }

    EMS_TxTelegram.forceRefresh = false; // send to MQTT is done automatically in EMS_TYPE_RC30StatusMessage
    EMS_TxQueue.push(EMS_TxTelegram);
}

/**
 * Set the thermostat working mode (0=low, 1=manual, 2=auto/clock)
 * 0xA8 on a RC20 and 0xA7 on RC30
 */
void ems_setThermostatMode(uint8_t mode) {
    if (EMS_ID_THERMOSTAT == EMS_ID_THERMOSTAT_EASY) {
        // doesn't support Easy yet
        return;
    }

    myDebug("Setting thermostat mode to %d\n", mode);

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest      = EMS_ID_THERMOSTAT;
    EMS_TxTelegram.length    = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.dataValue = mode;

    // handle different thermostat types
    if (EMS_ID_THERMOSTAT == EMS_ID_THERMOSTAT_RC20) {
        EMS_TxTelegram.type   = EMS_TYPE_RC20Set;
        EMS_TxTelegram.offset = EMS_OFFSET_RC20Set_mode;
    } else if (EMS_ID_THERMOSTAT == EMS_ID_THERMOSTAT_RC30) {
        EMS_TxTelegram.type   = EMS_TYPE_RC30Set;
        EMS_TxTelegram.offset = EMS_OFFSET_RC30Set_mode;
    } else {
        myDebug("Error! not supported\n");
        return;
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

    myDebug("Setting boiler warm water temperature to %d C\n", temperature);

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action    = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest      = EMS_ID_BOILER;
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
 * Activate / De-activate the Warm Water 0x33
 * true = on, false = off
 */
void ems_setWarmWaterActivated(bool activated) {
    myDebug("Setting boiler warm water %s\n", activated ? "on" : "off");

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest          = EMS_ID_BOILER;
    EMS_TxTelegram.type          = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset        = EMS_OFFSET_UBAParameterWW_wwactivated;
    EMS_TxTelegram.length        = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;               // don't validate
    EMS_TxTelegram.dataValue     = (activated ? 0xFF : 0x00); // 0xFF is on, 0x00 is off
    EMS_TxQueue.push(EMS_TxTelegram);
}

void ems_setWarmWaterModeComfort(bool comfort) {
    myDebug("Setting boiler warm water to comfort mode %s\n", comfort ? "Comfort" : "Eco");

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest          = EMS_ID_BOILER;
    EMS_TxTelegram.type          = EMS_TYPE_UBAParameterWW;
    EMS_TxTelegram.offset        = EMS_OFFSET_UBAParameterWW_wwComfort;
    EMS_TxTelegram.length        = EMS_MIN_TELEGRAM_LENGTH;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;               // don't validate
    EMS_TxTelegram.dataValue     = (comfort ? EMS_VALUE_UBAParameterWW_wwComfort_Comfort : EMS_VALUE_UBAParameterWW_wwComfort_Eco); // 0x00 is on, 0xD8 is off
    EMS_TxQueue.push(EMS_TxTelegram);
}
/**
 * Activate / De-activate the Warm Tap Water
 * true = on, false = off
 * Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
 */
void ems_setWarmTapWaterActivated(bool activated) {
    myDebug("Setting boiler warm tap water %s\n", activated ? "on" : "off");

    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    // clear Tx to make sure all data is set to 0x00
    for (int i = 0; (i < EMS_TX_MAXBUFFERSIZE); i++) {
        EMS_TxTelegram.data[i] = 0x00;
    }

    EMS_TxTelegram.action = EMS_TX_TELEGRAM_WRITE;
    EMS_TxTelegram.dest   = EMS_ID_BOILER;
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
 * experimental code for debugging - not in production
 */
void ems_setExperimental(uint8_t value) {
    /*
    _EMS_TxTelegram EMS_TxTelegram = EMS_TX_TELEGRAM_NEW; // create new Tx

    EMS_TxTelegram.action        = EMS_TX_TELEGRAM_READ; // read command
    EMS_TxTelegram.dest          = EMS_ID_THERMOSTAT;    // set 8th bit to indicate a read
    EMS_TxTelegram.offset        = 0;                    // 0 for all data
    EMS_TxTelegram.length        = 8;
    EMS_TxTelegram.type          = 0xF0;
    EMS_TxTelegram.type_validate = EMS_ID_NONE;

    // EMS Plus test
    // Sending read to 0x18: telegram: 0B 98 F0 00 01 B9 63 DB (len 8)
    EMS_TxTelegram.data[0] = EMS_ID_ME;             // src
    EMS_TxTelegram.data[1] = EMS_TxTelegram.dest;   // dest
    EMS_TxTelegram.data[2] = 0xF0;                  // marker
    EMS_TxTelegram.data[3] = EMS_TxTelegram.offset; // offset
    EMS_TxTelegram.data[4] = 0x01;                  // hi byte
    EMS_TxTelegram.data[5] = 0xB9;                  // low byte

    EMS_TxTelegram.data[6] = 99; // max length

    EMS_TxQueue.push(EMS_TxTelegram);
    */
}
