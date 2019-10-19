/*
 * Header file for ems.cpp
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 * See ChangeLog.md for history
 *
 */

#pragma once

#include <Arduino.h>
#include <list> // std::list

// EMS tx_mode types
#define EMS_TXMODE_DEFAULT 1 // Default (was previously known as tx_mode 2 in v1.8.x)
#define EMS_TXMODE_EMSPLUS 2 // EMS+
#define EMS_TXMODE_HT3 3     // Junkers HT3

#define EMS_ID_NONE 0x00 // used as a dest in broadcast messages and empty device IDs

// Fixed EMS IDs
#define EMS_ID_ME 0x0B      // our device, hardcoded as the "Service Key"
#define EMS_ID_BOILER 0x08  // all UBA Boilers have 0x08
#define EMS_ID_SM 0x30      // Solar Module SM10, SM100 and ISM1
#define EMS_ID_HP 0x38      // HeatPump
#define EMS_ID_GATEWAY 0x48 // KM200 Web Gateway

// Product IDs
#define EMS_PRODUCTID_SM10 73   // SM10 solar module
#define EMS_PRODUCTID_SM50 162  // SM50 solar module
#define EMS_PRODUCTID_SM100 163 // SM100 solar module
#define EMS_PRODUCTID_ISM1 101  // Junkers ISM1 solar module

#define EMS_MIN_TELEGRAM_LENGTH 6  // minimal length for a validation telegram, including CRC
#define EMS_MAX_TELEGRAM_LENGTH 32 // max length of a telegram, including CRC, for Rx and Tx.

// default values for null values
#define EMS_VALUE_INT_ON 1             // boolean true
#define EMS_VALUE_INT_OFF 0            // boolean false
#define EMS_VALUE_INT_NOTSET 0xFF      // for 8-bit unsigned ints/bytes
#define EMS_VALUE_SHORT_NOTSET -32768  // for 2-byte signed shorts
#define EMS_VALUE_USHORT_NOTSET 0x8000 // for 2-byte unsigned shorts
#define EMS_VALUE_LONG_NOTSET 0xFFFFFF // for 3-byte longs

// thermostat specific
#define EMS_THERMOSTAT_MAXHC 4     // max number of heating circuits
#define EMS_THERMOSTAT_DEFAULTHC 1 // default heating circuit is 1
#define EMS_THERMOSTAT_WRITE_YES true
#define EMS_THERMOSTAT_WRITE_NO false

// trigger settings to determine if hot tap water or the heating is active
#define EMS_BOILER_BURNPOWER_TAPWATER 100
#define EMS_BOILER_SELFLOWTEMP_HEATING 30 // was 70, changed to 30 for https://github.com/proddy/EMS-ESP/issues/193

// define maximum setable tapwater temperature
#define EMS_BOILER_TAPWATER_TEMPERATURE_MAX 60

#define EMS_TX_TELEGRAM_QUEUE_MAX 50 // max size of Tx FIFO queue. Number of Tx records to send.

//#define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_VERBOSE // turn on for debugging
#define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_NONE

#define EMS_SYS_DEVICEMAP_LENGTH 15 // size of the 0x07 telegram data part which stores all active EMS devices

// define the model types
// which get rendered to html colors in the web interface in file custom.js in function listCustomStats()
#define EMS_MODELTYPE_BOILER 1     // success color
#define EMS_MODELTYPE_THERMOSTAT 2 // info color
#define EMS_MODELTYPE_SM 3         // warning color
#define EMS_MODELTYPE_HP 4         // success color
#define EMS_MODELTYPE_OTHER 5      // no color
#define EMS_MODELTYPE_UNKNOWN 6    // no color
#define EMS_MODELTYPE_MIXING 7

#define EMS_MODELTYPE_UNKNOWN_STRING "unknown?" // model type text to use when discovering an unknown device

/* EMS UART transfer status */
typedef enum {
    EMS_RX_STATUS_IDLE,
    EMS_RX_STATUS_BUSY // Rx package is being received
} _EMS_RX_STATUS;

typedef enum {
    EMS_TX_STATUS_OK,
    EMS_TX_STATUS_IDLE, // ready
    EMS_TX_STATUS_WAIT, // waiting for response from last Tx
    EMS_TX_WTD_TIMEOUT, // watchdog timeout during send
    EMS_TX_BRK_DETECT,  // incoming BRK during Tx
    EMS_TX_REV_DETECT   // waiting to detect reverse bit
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
    EMS_SYS_LOGGING_NONE,        // no messages
    EMS_SYS_LOGGING_RAW,         // raw data mode
    EMS_SYS_LOGGING_BASIC,       // only basic read/write messages
    EMS_SYS_LOGGING_THERMOSTAT,  // only telegrams sent from thermostat
    EMS_SYS_LOGGING_SOLARMODULE, // only telegrams sent from thermostat
    EMS_SYS_LOGGING_VERBOSE,     // everything
    EMS_SYS_LOGGING_JABBER       // lots of debug output...
} _EMS_SYS_LOGGING;

// status/counters since last power on
typedef struct {
    _EMS_RX_STATUS   emsRxStatus;
    _EMS_TX_STATUS   emsTxStatus;
    uint16_t         emsRxPgks;                              // # successfull received
    uint16_t         emsTxPkgs;                              // # successfull sent
    uint16_t         emxCrcErr;                              // CRC errors
    bool             emsPollEnabled;                         // flag enable the response to poll messages
    _EMS_SYS_LOGGING emsLogging;                             // logging
    bool             emsRefreshed;                           // fresh data, needs to be pushed out to MQTT
    bool             emsBusConnected;                        // is there an active bus
    uint32_t         emsRxTimestamp;                         // timestamp of last EMS message received
    uint32_t         emsPollFrequency;                       // time between EMS polls
    bool             emsTxCapable;                           // able to send via Tx
    bool             emsTxDisabled;                          // true to prevent all Tx
    uint8_t          txRetryCount;                           // # times the last Tx was re-sent
    uint8_t          emsIDMask;                              // Buderus: 0x00, Junkers: 0x80
    uint8_t          emsPollAck[1];                          // acknowledge buffer for Poll
    uint8_t          emsTxMode;                              // Tx mode 1, 2 or 3
    char             emsDeviceMap[EMS_SYS_DEVICEMAP_LENGTH]; // contents of 0x07 telegram with bitmasks for all active EMS devices
} _EMS_Sys_Status;

// The Tx send package
typedef struct {
    _EMS_TX_TELEGRAM_ACTION action; // read, write, validate, init
    uint8_t                 dest;
    uint16_t                type;
    uint8_t                 offset;
    uint8_t                 length;             // full length of complete telegram, including CRC
    uint8_t                 dataValue;          // value to validate against
    uint16_t                type_validate;      // type to call after a successful Write command
    uint8_t                 comparisonValue;    // value to compare against during a validate command
    uint8_t                 comparisonOffset;   // offset of where the byte is we want to compare too during validation
    uint16_t                comparisonPostRead; // after a successful write, do a read from this type ID
    bool                    forceRefresh;       // should we send to MQTT after a successful Tx?
    uint32_t                timestamp;          // when created
    uint8_t                 data[EMS_MAX_TELEGRAM_LENGTH];
} _EMS_TxTelegram;

// The Rx receive package
typedef struct {
    uint32_t  timestamp;    // timestamp from millis()
    uint8_t * telegram;     // the full data package
    uint8_t   data_length;  // length in bytes of the data
    uint8_t   length;       // full length of the complete telegram
    uint8_t   src;          // source ID
    uint8_t   dest;         // destination ID
    uint16_t  type;         // type ID as a double byte to support EMS+
    uint8_t   offset;       // offset
    uint8_t * data;         // pointer to where telegram data starts
    bool      emsplus;      // true if ems+/ems 2.0
    uint8_t   emsplus_type; // FF, F7 or F9
} _EMS_RxTelegram;

// default empty Tx, must match struct
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

// where defintions are stored
typedef struct {
    uint8_t product_id;
    char    model_string[70];
} _Boiler_Device;

typedef struct {
    uint8_t product_id;
    char    model_string[50];
} _SolarModule_Device;

typedef struct {
    uint8_t product_id;
    uint8_t device_id;
    char    model_string[50];
} _Other_Device;

typedef struct {
    uint8_t product_id;
    char    model_string[50];
} _HeatPump_Device;

typedef struct {
    uint8_t model_id;
    uint8_t product_id;
    uint8_t device_id;
    char    model_string[50];
    bool    write_supported;
} _Thermostat_Device;

typedef struct {
    uint8_t product_id;
    char    model_string[50];
} _Mixing_Device;

// for consolidating all types
typedef struct {
    uint8_t model_type; // 1=boiler, 2=thermostat, 3=sm, 4=other, 5=unknown
    uint8_t product_id;
    uint8_t device_id;
    char    version[10];
    char    model_string[50];
} _Generic_Device;


/*
 * Telegram package defintions
 */
typedef struct {
    // settings
    uint8_t device_id; // this is typically always 0x08
    uint8_t product_id;
    char    version[10];

    // UBAParameterWW
    uint8_t wWActivated;   // Warm Water activated
    uint8_t wWSelTemp;     // Warm Water selected temperature
    uint8_t wWCircPump;    // Warm Water circulation pump Available
    uint8_t wWDesiredTemp; // Warm Water desired temperature
    uint8_t wWComfort;     // Warm water comfort or ECO mode

    // UBAMonitorFast
    uint8_t  selFlowTemp;        // Selected flow temperature
    uint16_t curFlowTemp;        // Current flow temperature
    uint16_t retTemp;            // Return temperature
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
    uint16_t boilTemp;    // Boiler temperature
    uint8_t  pumpMod;     // Pump modulation
    uint32_t burnStarts;  // # burner starts
    uint32_t burnWorkMin; // Total burner operating time
    uint32_t heatWorkMin; // Total heat operating time
    uint16_t switchTemp;  // Switch temperature

    // UBAMonitorWWMessage
    uint16_t wWCurTmp;  // Warm Water current temperature
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

} _EMS_Boiler;

/*
 * Telegram package defintions for Other EMS devices
 */
typedef struct {
    uint8_t device_id; // the device ID of the Heat Pump (e.g. 0x30)
    uint8_t model_id;  // Solar Module / Heat Pump model
    uint8_t product_id;
    char    version[10];

    uint8_t HPModulation; // heatpump modulation in %
    uint8_t HPSpeed;      // speed 0-100 %
} _EMS_HeatPump;

typedef struct {
    uint8_t device_id;
    uint8_t model_id;
    uint8_t product_id;
    char    version[10];
} _EMS_Other;

typedef struct {
    uint8_t device_id;
    uint8_t model_id;
    uint8_t product_id;
    char    version[10];
    uint8_t hc;     // heating circuit 1, 2, 3 or 4
    bool    active; // true if there is data for this HC

    uint16_t flowTemp;
    uint8_t  pumpMod;
    uint8_t  valveStatus;
} _EMS_Mixing_HC;

// Mixer data
typedef struct {
    bool           detected;
    _EMS_Mixing_HC hc[EMS_THERMOSTAT_MAXHC]; // array for the 4 heating circuits
} _EMS_Mixing;

// SM Solar Module - SM10/SM100/ISM1
typedef struct {
    uint8_t device_id; // the device ID of the Solar Module
    uint8_t model_id;  // Solar Module
    uint8_t product_id;
    char    version[10];

    int16_t  collectorTemp;          // collector temp
    int16_t  bottomTemp;             // bottom temp
    uint8_t  pumpModulation;         // modulation solar pump
    uint8_t  pump;                   // pump active
    int16_t  setpoint_maxBottomTemp; // setpoint for maximum collector temp
    uint16_t EnergyLastHour;
    uint16_t EnergyToday;
    uint16_t EnergyTotal;
    uint32_t pumpWorkMin; // Total solar pump operating time
} _EMS_SolarModule;

// heating circuit
typedef struct {
    uint8_t hc;                // heating circuit 1,2, 3 or 4
    bool    active;            // true if there is data for this HC
    int16_t setpoint_roomTemp; // current set temp
    int16_t curr_roomTemp;     // current room temp
    uint8_t mode;              // 0=low, 1=manual, 2=auto (or night, day on RC35s)
    uint8_t day_mode;          // 0=night, 1=day
    uint8_t summer_mode;
    uint8_t holiday_mode;
    uint8_t daytemp;
    uint8_t nighttemp;
    uint8_t holidaytemp;
    uint8_t heatingtype;     // type of heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
    uint8_t circuitcalctemp; // calculated setpoint flow temperature
} _EMS_Thermostat_HC;

// Thermostat data
typedef struct {
    uint8_t            device_id; // the device ID of the thermostat
    uint8_t            model_id;  // thermostat model
    uint8_t            product_id;
    char               version[10];
    char               datetime[25]; // HH:MM:SS DD/MM/YYYY
    bool               write_supported;
    _EMS_Thermostat_HC hc[EMS_THERMOSTAT_MAXHC]; // array for the 4 heating circuits
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
extern void ems_dumpBuffer(const char * prefix, uint8_t * telegram, uint8_t length);
extern void ems_parseTelegram(uint8_t * telegram, uint8_t len);
void        ems_init();
void        ems_doReadCommand(uint16_t type, uint8_t dest, bool forceRefresh = false);
void        ems_sendRawTelegram(char * telegram);
void        ems_scanDevices();
void        ems_printAllDevices();
void        ems_printDevices();
uint8_t     ems_printDevices_s(char * buffer, uint16_t len);
void        ems_printTxQueue();
void        ems_testTelegram(uint8_t test_num);
void        ems_startupTelegrams();
bool        ems_checkEMSBUSAlive();
void        ems_clearDeviceList();

void ems_setThermostatTemp(float temperature, uint8_t hc, uint8_t temptype = 0);
void ems_setThermostatMode(uint8_t mode, uint8_t hc);
void ems_setWarmWaterTemp(uint8_t temperature);
void ems_setFlowTemp(uint8_t temperature);
void ems_setWarmWaterActivated(bool activated);
void ems_setWarmTapWaterActivated(bool activated);
void ems_setPoll(bool b);
void ems_setLogging(_EMS_SYS_LOGGING loglevel, bool silent = false);
void ems_setEmsRefreshed(bool b);
void ems_setWarmWaterModeComfort(uint8_t comfort);
void ems_setModels();
void ems_setTxDisabled(bool b);
void ems_setTxMode(uint8_t mode);

uint8_t _getHeatingCircuit(_EMS_RxTelegram * EMS_RxTelegram);

char *           ems_getThermostatDescription(char * buffer, bool name_only = false);
char *           ems_getBoilerDescription(char * buffer, bool name_only = false);
char *           ems_getSolarModuleDescription(char * buffer, bool name_only = false);
char *           ems_getHeatPumpDescription(char * buffer, bool name_only = false);
void             ems_getThermostatValues();
void             ems_getBoilerValues();
void             ems_getSolarModuleValues();
bool             ems_getPoll();
bool             ems_getTxEnabled();
bool             ems_getThermostatEnabled();
bool             ems_getMixingDeviceEnabled();
bool             ems_getBoilerEnabled();
bool             ems_getSolarModuleEnabled();
bool             ems_getHeatPumpEnabled();
bool             ems_getBusConnected();
_EMS_SYS_LOGGING ems_getLogging();
bool             ems_getEmsRefreshed();
uint8_t          ems_getThermostatModel();
uint8_t          ems_getSolarModuleModel();
void             ems_discoverModels();
bool             ems_getTxCapable();
uint32_t         ems_getPollFrequency();
bool             ems_getTxDisabled();

// private functions
uint8_t _crcCalculator(uint8_t * data, uint8_t len);
void    _processType(_EMS_RxTelegram * EMS_RxTelegram);
void    _debugPrintPackage(const char * prefix, _EMS_RxTelegram * EMS_RxTelegram, const char * color);
void    _ems_clearTxData();
void    _removeTxQueue();

// global so can referenced in other classes
extern _EMS_Sys_Status  EMS_Sys_Status;
extern _EMS_Boiler      EMS_Boiler;
extern _EMS_Thermostat  EMS_Thermostat;
extern _EMS_SolarModule EMS_SolarModule;
extern _EMS_HeatPump    EMS_HeatPump;
extern _EMS_Other       EMS_Other;
extern _EMS_Mixing      EMS_Mixing;

extern std::list<_Generic_Device> Devices;
