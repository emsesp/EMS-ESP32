/*
 * Header file for EMS.cpp
 *
 */

#pragma once

#include "emsuart.h"
#include "my_config.h" // include custom configuration settings
#include <Arduino.h>
#include <CircularBuffer.h> // https://github.com/rlogiacco/CircularBuffer
#include <MyESP.h>

// EMS IDs
#define EMS_ID_NONE 0x00 // Fixed - used as a dest in broadcast messages and empty type IDs
#define EMS_ID_ME 0x0B   // Fixed - our device, hardcoded as "Service Key"

//#define EMS_ID_THERMOSTAT 0xFF // Fixed - to recognize a Thermostat
//#define EMS_ID_BOILER 0x08     // Fixed - also known as MC10.

#define EMS_MIN_TELEGRAM_LENGTH 6 // minimal length for a validation telegram, including CRC

// max length of a telegram, including CRC, for Rx and Tx.
// This can differs per firmware version and typically 32 is the max
#define EMS_MAX_TELEGRAM_LENGTH 99

// define here the EMS telegram types you need

// Common for all EMS devices
#define EMS_TYPE_Version 0x02 // version of the UBA controller (boiler)

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
#define EMS_TYPE_RC20StatusMessage 0x91 // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC20Set 0xA8           // for setting values like temp and mode
#define EMS_OFFSET_RC20Set_mode 23      // position of thermostat mode
#define EMS_OFFSET_RC20Set_temp 28      // position of thermostat setpoint temperature

// RC30 specific
#define EMS_TYPE_RC30StatusMessage 0x41 // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC30Set 0xA7           // for setting values like temp and mode
#define EMS_OFFSET_RC30Set_mode 23      // position of thermostat mode
#define EMS_OFFSET_RC30Set_temp 28      // position of thermostat setpoint temperature

// RC35 specific - not implemented yet
#define EMS_TYPE_RC35StatusMessage 0x3E // is an automatic thermostat broadcast giving us temps
#define EMS_TYPE_RC35Set 0x3D           // for setting values like temp and mode
#define EMS_OFFSET_RC35Set_mode 7       // position of thermostat mode
#define EMS_OFFSET_RC35Set_temp 2       // position of thermostat setpoint temperature

// Easy specific
#define EMS_TYPE_EasyStatusMessage 0x0A // reading values on an Easy Thermostat

// default values
#define EMS_VALUE_INT_ON 1             // boolean true
#define EMS_VALUE_INT_OFF 0            // boolean false
#define EMS_VALUE_INT_NOTSET 0xFF      // for 8-bit ints
#define EMS_VALUE_LONG_NOTSET 0xFFFFFF // for 3-byte longs
#define EMS_VALUE_FLOAT_NOTSET -255    // float unset

/* EMS UART transfer status */
typedef enum {
    EMS_RX_IDLE,
    EMS_RX_ACTIVE // Rx package is being sent
} _EMS_RX_STATUS;

typedef enum {
    EMS_TX_IDLE,
    EMS_TX_ACTIVE, // Tx package being sent, no break sent
    EMS_TX_SUCCESS,
    EMS_TX_ERROR
} _EMS_TX_STATUS;

typedef enum {
    EMS_TX_TELEGRAM_INIT,     // just initialized
    EMS_TX_TELEGRAM_READ,     // doing a read request
    EMS_TX_TELEGRAM_WRITE,    // doing a write request
    EMS_TX_TELEGRAM_VALIDATE, // do a read but only to validate the last write
    EMS_TX_TELEGRAM_RAW       // sending in raw mode
} _EMS_TX_TELEGRAM_ACTION;

/* EMS logging */
typedef enum {
    EMS_SYS_LOGGING_NONE,       // no messages
    EMS_SYS_LOGGING_RAW,        // raw data mode
    EMS_SYS_LOGGING_BASIC,      // only basic read/write messages
    EMS_SYS_LOGGING_THERMOSTAT, // only telegrams sent from thermostat
    EMS_SYS_LOGGING_VERBOSE     // everything
} _EMS_SYS_LOGGING;

// status/counters since last power on
typedef struct {
    _EMS_RX_STATUS   emsRxStatus;
    _EMS_TX_STATUS   emsTxStatus;
    uint16_t         emsRxPgks;            // received
    uint16_t         emsTxPkgs;            // sent
    uint16_t         emxCrcErr;            // CRC errors
    bool             emsPollEnabled;       // flag enable the response to poll messages
    bool             emsTxEnabled;         // flag if we're allowing sending of Tx packages
    bool             emsThermostatEnabled; // if there is a RCxx thermostat active
    bool             emsBoilerEnabled;     // is the boiler online
    _EMS_SYS_LOGGING emsLogging;           // logging
    bool             emsRefreshed;         // fresh data, needs to be pushed out to MQTT
    bool             emsBusConnected;      // is there an active bus
} _EMS_Sys_Status;

// The Tx send package
typedef struct {
    _EMS_TX_TELEGRAM_ACTION action; // read or write
    uint8_t                 dest;
    uint8_t                 type;
    uint8_t                 offset;
    uint8_t                 length;
    uint8_t                 dataValue;          // value to validate against
    uint8_t                 type_validate;      // type to call after a successful Write command
    uint8_t                 comparisonValue;    // value to compare against during a validate
    uint8_t                 comparisonOffset;   // offset of where the byte is we want to compare too later
    uint8_t                 comparisonPostRead; // after a successful write call this to read
    bool                    forceRefresh;       // should we send to MQTT after a successful Tx?
    unsigned long           timestamp;          // when created
    uint8_t                 data[EMS_MAX_TELEGRAM_LENGTH];
} _EMS_TxTelegram;

#define EMS_TX_TELEGRAM_QUEUE_MAX 20 // max size of Tx FIFO queue

// default empty Tx
const _EMS_TxTelegram EMS_TX_TELEGRAM_NEW = {
    EMS_TX_TELEGRAM_INIT, // action
    EMS_ID_NONE,          // dest
    EMS_ID_NONE,          // type
    0,                    // offset
    0,                    // length
    0,                    // data value
    EMS_ID_NONE,          // type_validate
    0,                    // comparisonValue
    0,                    // comparisonOffset
    EMS_ID_NONE,          // comparisonPostRead
    false,                // forceRefresh
    0,                    // timestamp
    {0x00}                // data
};

// Known Buderus non-Thermostat types
typedef enum {
    EMS_MODEL_NONE,
    EMS_MODEL_ALL, // common for all devices

    // service key
    EMS_MODEL_SERVICEKEY, // this is us

    // main buderus boiler type devices
    EMS_MODEL_BK15,
    EMS_MODEL_UBA,
    EMS_MODEL_BC10,
    EMS_MODEL_MM10,
    EMS_MODEL_WM10,

    // thermostats
    EMS_MODEL_ES73,
    EMS_MODEL_RC20,
    EMS_MODEL_RC30,
    EMS_MODEL_RC35,
    EMS_MODEL_EASY

} _EMS_MODEL_ID;

typedef struct {
    _EMS_MODEL_ID model_id;
    uint8_t       product_id;
    uint8_t       type_id;
    char          model_string[50];
} _Model_Type;

/*
 * Telegram package defintions
 */
typedef struct {           // UBAParameterWW
    uint8_t wWActivated;   // Warm Water activated
    uint8_t wWSelTemp;     // Warm Water selected temperature
    uint8_t wWCircPump;    // Warm Water circulation pump Available
    uint8_t wWDesiredTemp; // Warm Water desired temperature
    uint8_t wWComfort;     // Warm water comfort or ECO mode

    // UBAMonitorFast
    uint8_t selFlowTemp;      // Selected flow temperature
    float   curFlowTemp;      // Current flow temperature
    float   retTemp;          // Return temperature
    uint8_t burnGas;          // Gas on/off
    uint8_t fanWork;          // Fan on/off
    uint8_t ignWork;          // Ignition on/off
    uint8_t heatPmp;          // Circulating pump on/off
    uint8_t wWHeat;           // 3-way valve on WW
    uint8_t wWCirc;           // Circulation on/off
    uint8_t selBurnPow;       // Burner max power
    uint8_t curBurnPow;       // Burner current power
    float   flameCurr;        // Flame current in micro amps
    float   sysPress;         // System pressure
    uint8_t serviceCodeChar1; // First  Character in status/service code
    uint8_t serviceCodeChar2; // Second Character in status/service code

    // UBAMonitorSlow
    float    extTemp;     // Outside temperature
    float    boilTemp;    // Boiler temperature
    uint8_t  pumpMod;     // Pump modulation
    uint32_t burnStarts;  // # burner restarts
    uint32_t burnWorkMin; // Total burner operating time
    uint32_t heatWorkMin; // Total heat operating time

    // UBAMonitorWWMessage
    float    wWCurTmp;  // Warm Water current temperature:
    uint32_t wWStarts;  // Warm Water # starts
    uint32_t wWWorkM;   // Warm Water # minutes
    uint8_t  wWOneTime; // Warm Water one time function on/off
    uint8_t  wWCurFlow; // Warm Water current flow in l/min

    // UBATotalUptimeMessage
    uint32_t UBAuptime; // Total UBA working hours

    // calculated values
    uint8_t tapwaterActive; // Hot tap water is on/off
    uint8_t heatingActive;  // Central heating is on/off

    // settings
    char          version[10];
    uint8_t       type_id;
    _EMS_MODEL_ID model_id;
} _EMS_Boiler;

// Definition for thermostat type
typedef struct {
    _EMS_MODEL_ID model_id;
    bool          read_supported;
    bool          write_supported;
} _Thermostat_Type;

#define EMS_THERMOSTAT_READ_YES true
#define EMS_THERMOSTAT_READ_NO false
#define EMS_THERMOSTAT_WRITE_YES true
#define EMS_THERMOSTAT_WRITE_NO false

// Thermostat data
typedef struct {
    uint8_t       type_id;  // the type ID of the thermostat
    _EMS_MODEL_ID model_id; // which Thermostat type
    bool          read_supported;
    bool          write_supported;
    char          version[10];
    float         setpoint_roomTemp; // current set temp
    float         curr_roomTemp;     // current room temp
    uint8_t       mode;              // 0=low, 1=manual, 2=auto
    uint8_t       hour;
    uint8_t       minute;
    uint8_t       second;
    uint8_t       day;
    uint8_t       month;
    uint8_t       year;
} _EMS_Thermostat;

// call back function signature
typedef void (*EMS_processType_cb)(uint8_t * data, uint8_t length);

// Definition for each EMS type, including the relative callback function
typedef struct {
    _EMS_MODEL_ID      model_id;
    uint8_t            type;
    const char         typeString[50];
    EMS_processType_cb processType_cb;
} _EMS_Type;

// ANSI Colors
#define COLOR_RESET "\x1B[0m"
#define COLOR_BLACK "\x1B[0;30m"
#define COLOR_RED "\x1B[0;31m"
#define COLOR_GREEN "\x1B[0;32m"
#define COLOR_YELLOW "\x1B[0;33m"
#define COLOR_BLUE "\x1B[0;34m"
#define COLOR_MAGENTA "\x1B[0;35m"
#define COLOR_CYAN "\x1B[0;36m"
#define COLOR_WHITE "\x1B[0;37m"
#define COLOR_BOLD_ON "\x1B[1m"
#define COLOR_BOLD_OFF "\x1B[21m"

// function definitions
extern void ems_parseTelegram(uint8_t * telegram, uint8_t len);
void        ems_init();
void        ems_doReadCommand(uint8_t type, uint8_t dest, bool forceRefresh = false);
void        ems_sendRawTelegram(char * telegram);

void ems_setThermostatTemp(float temp);
void ems_setThermostatMode(uint8_t mode);
void ems_setWarmWaterTemp(uint8_t temperature);
void ems_setWarmWaterActivated(bool activated);
void ems_setWarmWaterModeComfort(bool comfort);
void ems_setWarmTapWaterActivated(bool activated);
void ems_setExperimental(uint8_t value);
void ems_setPoll(bool b);
void ems_setTxEnabled(bool b);
void ems_setThermostatEnabled(bool b);
void ems_setBoilerEnabled(bool b);
void ems_setLogging(_EMS_SYS_LOGGING loglevel);
void ems_setEmsRefreshed(bool b);
void ems_setBusConnected(bool b);
void ems_setWarmWaterModeComfort(bool comfort);

void             ems_getThermostatValues();
void             ems_getBoilerValues();
bool             ems_getPoll();
bool             ems_getTxEnabled();
bool             ems_getThermostatEnabled();
bool             ems_getBoilerEnabled();
bool             ems_getBusConnected();
_EMS_SYS_LOGGING ems_getLogging();
uint8_t          ems_getEmsTypesCount();
bool             ems_getEmsRefreshed();
void             ems_getVersions();
_EMS_MODEL_ID    ems_getThermostatModel();

void   ems_printAllTypes();
char * ems_getThermostatType(char * buffer);
void   ems_printTxQueue();
char * ems_getBoilerType(char * buffer);

// private functions
uint8_t _crcCalculator(uint8_t * data, uint8_t len);
void    _processType(uint8_t * telegram, uint8_t length);
void    _debugPrintPackage(const char * prefix, uint8_t * data, uint8_t len, const char * color);
void    _ems_clearTxData();
int     _ems_findModel(_EMS_MODEL_ID model_id);
char *  _ems_buildModelString(char * buffer, uint8_t size, _EMS_MODEL_ID model_id);

// global so can referenced in other classes
extern _EMS_Sys_Status EMS_Sys_Status;
extern _EMS_Boiler     EMS_Boiler;
extern _EMS_Thermostat EMS_Thermostat;
