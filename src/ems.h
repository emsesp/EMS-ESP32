/*
 * Header file for ems.cpp
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 * See ChangeLog.md for history
 * See README.md for Acknowledgments
 *
 */

#pragma once

#include <Arduino.h>

// EMS IDs
#define EMS_ID_NONE 0x00      // Fixed - used as a dest in broadcast messages and empty device IDs
#define EMS_PLUS_ID_NONE 0x01 // Fixed - used as a dest in broadcast messages and empty device IDs
#define EMS_ID_ME 0x0B        // Fixed - our device, hardcoded as the "Service Key"
#define EMS_ID_DEFAULT_BOILER 0x08
#define EMS_ID_SM 0x30      // Solar Module SM10 and SM100
#define EMS_ID_GATEWAY 0x48 // KM200 Web Gateway

#define EMS_MIN_TELEGRAM_LENGTH 6 // minimal length for a validation telegram, including CRC

// max length of a telegram, including CRC, for Rx and Tx.
#define EMS_MAX_TELEGRAM_LENGTH 32

// default values
#define EMS_VALUE_INT_ON 1             // boolean true
#define EMS_VALUE_INT_OFF 0            // boolean false
#define EMS_VALUE_INT_NOTSET 0xFF      // for 8-bit ints
#define EMS_VALUE_SHORT_NOTSET 0x8000  // for 2-byte shorts
#define EMS_VALUE_LONG_NOTSET 0xFFFFFF // for 3-byte longs

#define EMS_THERMOSTAT_WRITE_YES true
#define EMS_THERMOSTAT_WRITE_NO false

// trigger settings to determine if hot tap water or the heating is active
#define EMS_BOILER_BURNPOWER_TAPWATER 100
#define EMS_BOILER_SELFLOWTEMP_HEATING 70

// define maximum settable tapwater temperature
#define EMS_BOILER_TAPWATER_TEMPERATURE_MAX 60

#define EMS_TX_TELEGRAM_QUEUE_MAX 100 // max size of Tx FIFO queue

// #define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_VERBOSE
#define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_NONE

/* EMS UART transfer status */
typedef enum {
    EMS_RX_STATUS_IDLE,
    EMS_RX_STATUS_BUSY // Rx package is being received
} _EMS_RX_STATUS;

typedef enum {
    EMS_TX_STATUS_IDLE, // ready
    EMS_TX_STATUS_WAIT  // waiting for response from last Tx
} _EMS_TX_STATUS;

#define EMS_TX_SUCCESS 0x01 // EMS single byte after a Tx Write indicating a success
#define EMS_TX_ERROR 0x04   // EMS single byte after a Tx Write indicating an error

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
    uint16_t         emsRxPgks;        // received
    uint16_t         emsTxPkgs;        // sent
    uint16_t         emxCrcErr;        // CRC errors
    bool             emsPollEnabled;   // flag enable the response to poll messages
    _EMS_SYS_LOGGING emsLogging;       // logging
    bool             emsRefreshed;     // fresh data, needs to be pushed out to MQTT
    bool             emsBusConnected;  // is there an active bus
    uint32_t         emsRxTimestamp;   // timestamp of last EMS message received
    uint32_t         emsPollFrequency; // time between EMS polls
    bool             emsTxCapable;     // able to send via Tx
    bool             emsTxDisabled;    // true to prevent all Tx
    uint8_t          txRetryCount;     // # times the last Tx was re-sent
    bool             emsTxDelay;       // if true, slows down the Tx transmit
} _EMS_Sys_Status;

// The Tx send package
typedef struct {
    _EMS_TX_TELEGRAM_ACTION action; // read, write, validate, init
    uint8_t                 dest;
    uint16_t                type;
    uint8_t                 offset;
    uint8_t                 length;             // full length of complete telegram
    uint8_t                 dataValue;          // value to validate against
    uint16_t                type_validate;      // type to call after a successful Write command
    uint8_t                 comparisonValue;    // value to compare against during a validate
    uint8_t                 comparisonOffset;   // offset of where the byte is we want to compare too later
    uint16_t                comparisonPostRead; // after a successful write call this to read from this type ID
    bool                    forceRefresh;       // should we send to MQTT after a successful Tx?
    uint32_t                timestamp;          // when created
    uint8_t                 data[EMS_MAX_TELEGRAM_LENGTH];
} _EMS_TxTelegram;

// The Rx receive package
typedef struct {
    uint32_t  timestamp;   // timestamp from millis()
    uint8_t * telegram;    // the full data package
    uint8_t   data_length; // length in bytes of the data
    uint8_t   length;      // full length of the complete telegram
    uint8_t   src;         // source ID
    uint8_t   dest;        // destination ID
    uint16_t  type;        // type ID as a double byte to support EMS+
    uint8_t   offset;      // offset
    uint8_t * data;        // pointer to where telegram data starts
    bool      emsplus;     // true if ems+/ems 2.0
} _EMS_RxTelegram;

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

typedef struct {
    uint8_t model_id;
    uint8_t product_id;
    uint8_t device_id;
    char    model_string[50];
} _Boiler_Type;

typedef struct {
    uint8_t model_id;
    uint8_t product_id;
    uint8_t device_id;
    char    model_string[50];
} _Other_Type;

// Definition for thermostat devices
typedef struct {
    uint8_t model_id;
    uint8_t product_id;
    uint8_t device_id;
    char    model_string[50];
    bool    write_supported;
} _Thermostat_Type;

typedef struct {
    uint8_t product_id;
    uint8_t device_id;
    char    version[10];
    char    model_string[50];
} _Generic_Type;

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
    uint8_t  selFlowTemp;        // Selected flow temperature
    int16_t  curFlowTemp;        // Current flow temperature
    int16_t  retTemp;            // Return temperature
    uint8_t  burnGas;            // Gas on/off
    uint8_t  fanWork;            // Fan on/off
    uint8_t  ignWork;            // Ignition on/off
    uint8_t  heatPmp;            // Circulating pump on/off
    uint8_t  wWHeat;             // 3-way valve on WW
    uint8_t  wWCirc;             // Circulation on/off
    uint8_t  selBurnPow;         // Burner max power
    uint8_t  curBurnPow;         // Burner current power
    uint16_t flameCurr;          // Flame current in micro amps
    uint8_t  sysPress;           // System pressure
    char     serviceCodeChar[3]; // 2 character status/service code
    uint16_t serviceCode;        // error/service code

    // UBAMonitorSlow
    int16_t  extTemp;     // Outside temperature
    int16_t  boilTemp;    // Boiler temperature
    uint8_t  pumpMod;     // Pump modulation
    uint32_t burnStarts;  // # burner starts
    uint32_t burnWorkMin; // Total burner operating time
    uint32_t heatWorkMin; // Total heat operating time

    // UBAMonitorWWMessage
    int16_t  wWCurTmp;  // Warm Water current temperature:
    uint32_t wWStarts;  // Warm Water # starts
    uint32_t wWWorkM;   // Warm Water # minutes
    uint8_t  wWOneTime; // Warm Water one time function on/off
    uint8_t  wWCurFlow; // Warm Water current flow in l/min

    // UBATotalUptimeMessage
    uint32_t UBAuptime; // Total UBA working hours

    // UBAParametersMessage
    uint8_t heating_temp; // Heating temperature setting on the boiler
    uint8_t pump_mod_max; // Boiler circuit pump modulation max. power
    uint8_t pump_mod_min; // Boiler circuit pump modulation min. power

    // calculated values
    uint8_t tapwaterActive; // Hot tap water is on/off
    uint8_t heatingActive;  // Central heating is on/off

    // settings
    char    version[10];
    uint8_t device_id; // this is typically always 0x08
    uint8_t product_id;
} _EMS_Boiler;

/*
 * Telegram package defintions for Other EMS devices
 */

// SM Solar Module - SM10Monitor/SM100Monitor
typedef struct {
    bool    SM;               // set true if there is a SM available
    int16_t SMcollectorTemp;  // collector temp
    int16_t SMbottomTemp;     // bottom temp
    uint8_t SMpumpModulation; // modulation solar pump
    uint8_t SMpump;           // pump active
    int16_t SMEnergyLastHour;
    int16_t SMEnergyToday;
    int16_t SMEnergyTotal;
} _EMS_Other;

// Thermostat data
typedef struct {
    uint8_t device_id; // the device ID of the thermostat
    uint8_t model_id;  // thermostat model
    uint8_t product_id;
    bool    write_supported;
    uint8_t hc; // heating circuit 1 or 2
    char    version[10];
    int16_t setpoint_roomTemp; // current set temp
    int16_t curr_roomTemp;     // current room temp
    uint8_t mode;              // 0=low, 1=manual, 2=auto
    bool    day_mode;          // 0=night, 1=day
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t daytemp;
    uint8_t nighttemp;
    uint8_t holidaytemp;
    uint8_t heatingtype;
    uint8_t circuitcalctemp;
} _EMS_Thermostat;

// call back function signature for processing telegram types
typedef void (*EMS_processType_cb)(_EMS_RxTelegram * EMS_RxTelegram);

// Definition for each EMS type, including the relative callback function
typedef struct {
    uint8_t            model_id;
    uint16_t           type; // long to support EMS+ types
    const char         typeString[50];
    EMS_processType_cb processType_cb;
} _EMS_Type;

// function definitions
extern void ems_parseTelegram(uint8_t * telegram, uint8_t len);
void        ems_init();
void        ems_doReadCommand(uint16_t type, uint8_t dest, bool forceRefresh = false);
void        ems_sendRawTelegram(char * telegram);

void ems_setThermostatTemp(float temperature, uint8_t temptype = 0);
void ems_setThermostatMode(uint8_t mode);
void ems_setThermostatHC(uint8_t hc);
void ems_setWarmWaterTemp(uint8_t temperature);
void ems_setFlowTemp(uint8_t temperature);
void ems_setWarmWaterActivated(bool activated);
void ems_setWarmTapWaterActivated(bool activated);
void ems_setPoll(bool b);
void ems_setTxDelay(bool b);
void ems_setLogging(_EMS_SYS_LOGGING loglevel);
void ems_setEmsRefreshed(bool b);
void ems_setWarmWaterModeComfort(uint8_t comfort);
bool ems_checkEMSBUSAlive();
void ems_setModels();
void ems_setTxDisabled(bool b);

void             ems_getThermostatValues();
void             ems_getBoilerValues();
void             ems_getOtherValues();
bool             ems_getPoll();
bool             ems_getTxEnabled();
bool             ems_getThermostatEnabled();
bool             ems_getBoilerEnabled();
bool             ems_getTxDelay();
bool             ems_getBusConnected();
_EMS_SYS_LOGGING ems_getLogging();
bool             ems_getEmsRefreshed();
uint8_t          ems_getThermostatModel();
void             ems_discoverModels();
bool             ems_getTxCapable();
uint32_t         ems_getPollFrequency();

void   ems_scanDevices();
void   ems_printAllDevices();
void   ems_printDevices();
char * ems_getThermostatDescription(char * buffer);
void   ems_printTxQueue();
char * ems_getBoilerDescription(char * buffer);
void   ems_testTelegram(uint8_t test_num);
void   ems_startupTelegrams();

// private functions
uint8_t _crcCalculator(uint8_t * data, uint8_t len);
void    _processType(_EMS_RxTelegram * EMS_RxTelegram);
void    _debugPrintPackage(const char * prefix, _EMS_RxTelegram * EMS_RxTelegram, const char * color);
void    _ems_clearTxData();
int     _ems_findBoilerModel(uint8_t model_id);
bool    _ems_setModel(uint8_t model_id);
void    _removeTxQueue();
void    _ems_readTelegram(uint8_t * telegram, uint8_t length);

// global so can referenced in other classes
extern _EMS_Sys_Status EMS_Sys_Status;
extern _EMS_Boiler     EMS_Boiler;
extern _EMS_Thermostat EMS_Thermostat;
extern _EMS_Other      EMS_Other;
