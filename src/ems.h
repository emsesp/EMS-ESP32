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

// EMS bus IDs
#define EMS_BUSID_DEFAULT 0x0B // Default 0x0B (Service Key)

// EMS tx_mode types
#define EMS_TXMODE_DEFAULT 1 // Default (was previously known as tx_mode 2 in v1.8.x)
#define EMS_TXMODE_EMSPLUS 2 // EMS+
#define EMS_TXMODE_HT3 3     // Junkers HT3

#define EMS_ID_NONE 0x00 // used as a dest in broadcast messages and empty device IDs

#define EMS_MIN_TELEGRAM_LENGTH 6  // minimal length for a validation telegram, including CRC
#define EMS_MAX_TELEGRAM_LENGTH 32 // max length of a telegram, including CRC, for Rx and Tx.

// default values for null values
#define EMS_VALUE_BOOL_ON 0x01         // boolean true
#define EMS_VALUE_BOOL_ON2 0xFF        // boolean true, EMS sometimes uses 0xFF for TRUE
#define EMS_VALUE_BOOL_OFF 0x00        // boolean false
#define EMS_VALUE_INT_NOTSET 0xFF      // for 8-bit unsigned ints/bytes
#define EMS_VALUE_SHORT_NOTSET -32000  // was -32768 for 2-byte signed shorts
#define EMS_VALUE_USHORT_NOTSET 32000  // was 0x8000 for 2-byte unsigned shorts
#define EMS_VALUE_LONG_NOTSET 0xFFFFFF // for 3-byte longs
#define EMS_VALUE_BOOL_NOTSET 0xFE     // random number that's not 0, 1 or FF

// thermostat specific
#define EMS_THERMOSTAT_MAXHC 4     // max number of heating circuits
#define EMS_THERMOSTAT_DEFAULTHC 1 // default heating circuit is 1
#define EMS_THERMOSTAT_WRITE_YES true
#define EMS_THERMOSTAT_WRITE_NO false

// mixing specific
#define EMS_MIXING_MAXHC 4  // max number of heating circuits
#define EMS_MIXING_MAXWWC 2 // max number of warm water circuits

// Device Flags
// They are unique to the model type (mixing, solar, thermostat etc)
enum EMS_DEVICE_FLAG_TYPES : uint8_t {
    EMS_DEVICE_FLAG_NONE     = 0,
    EMS_DEVICE_FLAG_MMPLUS   = 20, // mixing EMS+
    EMS_DEVICE_FLAG_MM10     = 21, // mixing MM10, MM50
    EMS_DEVICE_FLAG_SM10     = 10,
    EMS_DEVICE_FLAG_SM100    = 11, // for SM100 and SM200
    EMS_DEVICE_FLAG_EASY     = 1,
    EMS_DEVICE_FLAG_RC10     = 2,
    EMS_DEVICE_FLAG_RC20     = 3,
    EMS_DEVICE_FLAG_RC30     = 4,
    EMS_DEVICE_FLAG_RC30N    = 5, // newer type of RC30 with RC35 circuit
    EMS_DEVICE_FLAG_RC35     = 6,
    EMS_DEVICE_FLAG_RC300    = 7,
    EMS_DEVICE_FLAG_JUNKERS1 = 31,       // use 0x65 for HC
    EMS_DEVICE_FLAG_JUNKERS2 = 32,       // use 0x79 for HC, older models
    EMS_DEVICE_FLAG_JUNKERS  = (1 << 6), // 6th bit set if its junkers HT3
    EMS_DEVICE_FLAG_NO_WRITE = (1 << 7)  // top bit set if thermostat write not supported
};

// trigger settings to determine if hot tap water or the heating is active
#define EMS_BOILER_BURNPOWER_TAPWATER 100
#define EMS_BOILER_SELFLOWTEMP_HEATING 30 // was 70, changed to 30 for https://github.com/proddy/EMS-ESP/issues/193

// define min & maximum setable tapwater temperature
#define EMS_BOILER_TAPWATER_TEMPERATURE_MAX 60
#define EMS_BOILER_TAPWATER_TEMPERATURE_MIN 30

#define EMS_TX_TELEGRAM_QUEUE_MAX 50 // max size of Tx FIFO queue. Number of Tx records to send.

//#define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_VERBOSE // turn on for debugging
#define EMS_SYS_LOGGING_DEFAULT EMS_SYS_LOGGING_NONE

#define EMS_SYS_DEVICEMAP_LENGTH 15 // size of the 0x07 telegram data part which stores all active EMS devices

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
    EMS_SYS_LOGGING_NONE,         // no messages
    EMS_SYS_LOGGING_RAW,          // raw data mode
    EMS_SYS_LOGGING_WATCH,        // watch a specific type ID
    EMS_SYS_LOGGING_BASIC,        // only basic read/write messages
    EMS_SYS_LOGGING_THERMOSTAT,   // only telegrams sent from thermostat
    EMS_SYS_LOGGING_SOLARMODULE,  // only telegrams sent from solar module
    EMS_SYS_LOGGING_MIXINGMODULE, // only telegrams sent from mixing module
    EMS_SYS_LOGGING_VERBOSE,      // everything
    EMS_SYS_LOGGING_JABBER,       // lots of debug output...
    EMS_SYS_LOGGING_DEVICE        // watch the device ID
} _EMS_SYS_LOGGING;

// status/counters since last power on
typedef struct {
    _EMS_RX_STATUS   emsRxStatus;
    _EMS_TX_STATUS   emsTxStatus;
    uint16_t         emsRxPgks;           // # successfull received
    uint16_t         emsTxPkgs;           // # successfull sent
    uint16_t         emxCrcErr;           // CRC errors
    bool             emsPollEnabled;      // flag enable the response to poll messages
    _EMS_SYS_LOGGING emsLogging;          // logging
    uint16_t         emsLogging_ID;       // the type or device ID to watch
    uint8_t          emsRefreshedFlags;   // fresh data, needs to be pushed out to MQTT
    bool             emsBusConnected;     // is there an active bus
    uint32_t         emsRxTimestamp;      // timestamp of last EMS message received
    uint32_t         emsPollFrequency;    // time between EMS polls
    bool             emsTxCapable;        // able to send via Tx
    bool             emsTxDisabled;       // true to prevent all Tx
    uint8_t          txRetryCount;        // # times the last Tx was re-sent
    uint8_t          emsIDMask;           // Buderus: 0x00, Junkers: 0x80
    uint8_t          emsPollAck[1];       // acknowledge buffer for Poll
    uint8_t          emsTxMode;           // Tx mode 1, 2 or 3
    uint8_t          emsbusid;            // EMS bus ID, default 0x0B for Service Key
    uint8_t          emsMasterThermostat; // product ID for the default thermostat to use
} _EMS_Sys_Status;

// The Rx receive package
typedef struct {
    unsigned long timestamp;    // timestamp from millis()
    uint8_t *     telegram;     // the full data package
    uint8_t       data_length;  // length in bytes of the data
    uint8_t       length;       // full length of the complete telegram
    uint8_t       src;          // source ID
    uint8_t       dest;         // destination ID
    uint16_t      type;         // type ID as a 2-byte to support EMS+
    uint8_t       offset;       // offset
    uint8_t *     data;         // pointer to where telegram data starts
    bool          emsplus;      // true if ems+/ems 2.0
    uint8_t       emsplus_type; // FF, F7 or F9
} _EMS_RxTelegram;

// The Tx send package
typedef struct {
    _EMS_TX_TELEGRAM_ACTION action; // read, write, validate, init
    uint8_t                 dest;
    uint16_t                type;
    uint8_t                 offset;
    uint8_t                 length;             // full length of complete telegram, including CRC
    bool                    emsplus;            // true if ems+/ems 2.0
    uint8_t                 dataValue;          // value to validate against
    uint16_t                type_validate;      // type to call after a successful Write command
    uint8_t                 comparisonValue;    // value to compare against during a validate command
    uint8_t                 comparisonOffset;   // offset of where the byte is we want to compare too during validation
    uint16_t                comparisonPostRead; // after a successful write, do a read from this type ID
    unsigned long           timestamp;          // when created
    uint8_t                 data[EMS_MAX_TELEGRAM_LENGTH];
} _EMS_TxTelegram;

// default empty Tx, must match struct
const _EMS_TxTelegram EMS_TX_TELEGRAM_NEW = {
    EMS_TX_TELEGRAM_INIT, // action
    EMS_ID_NONE,          // dest
    EMS_ID_NONE,          // type
    0,                    // offset
    0,                    // length
    false,                // emsplus (no)
    0,                    // dataValue
    EMS_ID_NONE,          // type_validate
    0,                    // comparisonValue
    0,                    // comparisonOffset
    EMS_ID_NONE,          // comparisonPostRead
    0,                    // timestamp
    {0x00}                // data
};

// flags for triggering changes when EMS data is received
typedef enum : uint8_t {
    EMS_DEVICE_UPDATE_FLAG_NONE       = 0,
    EMS_DEVICE_UPDATE_FLAG_BOILER     = (1 << 0),
    EMS_DEVICE_UPDATE_FLAG_THERMOSTAT = (1 << 1),
    EMS_DEVICE_UPDATE_FLAG_MIXING     = (1 << 2),
    EMS_DEVICE_UPDATE_FLAG_SOLAR      = (1 << 3),
    EMS_DEVICE_UPDATE_FLAG_HEATPUMP   = (1 << 4)
} _EMS_DEVICE_UPDATE_FLAG;

typedef enum : uint8_t {
    EMS_DEVICE_TYPE_NONE = 0,
    EMS_DEVICE_TYPE_SERVICEKEY,
    EMS_DEVICE_TYPE_BOILER,
    EMS_DEVICE_TYPE_THERMOSTAT,
    EMS_DEVICE_TYPE_MIXING,
    EMS_DEVICE_TYPE_SOLAR,
    EMS_DEVICE_TYPE_HEATPUMP,
    EMS_DEVICE_TYPE_GATEWAY,
    EMS_DEVICE_TYPE_SWITCH,
    EMS_DEVICE_TYPE_CONTROLLER,
    EMS_DEVICE_TYPE_CONNECT,
    EMS_DEVICE_TYPE_UNKNOWN
} _EMS_DEVICE_TYPE;

// to store mapping of device_ids to their string name
typedef struct {
    _EMS_DEVICE_TYPE device_type;
    char             device_type_string[30];
} _EMS_Device_Types;

// mapping for EMS_Devices_Type
const _EMS_Device_Types EMS_Devices_Types[] = {

    {EMS_DEVICE_TYPE_UNKNOWN, EMS_MODELTYPE_UNKNOWN_STRING}, // the first, at index 0 is reserved for "unknown"
    {EMS_DEVICE_TYPE_NONE, "All"},
    {EMS_DEVICE_TYPE_SERVICEKEY, "Me"},
    {EMS_DEVICE_TYPE_BOILER, "Boiler"},
    {EMS_DEVICE_TYPE_THERMOSTAT, "Thermostat"},
    {EMS_DEVICE_TYPE_MIXING, "Mixing Module"},
    {EMS_DEVICE_TYPE_SOLAR, "Solar Module"},
    {EMS_DEVICE_TYPE_HEATPUMP, "Heat Pump"},
    {EMS_DEVICE_TYPE_GATEWAY, "Gateway"},
    {EMS_DEVICE_TYPE_SWITCH, "Switching Module"},
    {EMS_DEVICE_TYPE_CONTROLLER, "Controller"},
    {EMS_DEVICE_TYPE_CONNECT, "Connect Module"}

};

// to store all known EMS devices to date
typedef struct {
    uint8_t          product_id;
    _EMS_DEVICE_TYPE type;
    char             device_desc[100];
    uint8_t          flags;
} _EMS_Device;

// for storing all recognised EMS devices
typedef struct {
    _EMS_DEVICE_TYPE device_type;   // type (see above)
    uint8_t          product_id;    // product id
    uint8_t          device_id;     // device_id
    const char *     device_desc_p; // pointer to description string in EMS_Devices table
    char             version[10];   // the version number XX.XX
    bool             known;         // is this a known device?
} _Detected_Device;

/*
 * Telegram package defintions
 */
typedef struct {
    // settings
    uint8_t      device_id; // this is typically always 0x08
    uint8_t      device_flags;
    const char * device_desc_p;
    uint8_t      product_id;
    char         version[10];

    uint8_t brand; // 0=unknown, 1=bosch, 2=junkers, 3=buderus, 4=nefit, 5=sieger, 11=worcester

    // UBAParameterWW
    uint8_t wWActivated;     // Warm Water activated
    uint8_t wWSelTemp;       // Warm Water selected temperature
    uint8_t wWCircPump;      // Warm Water circulation pump Available
    uint8_t wWCircPumpMode;  // Warm Water circulation pump mode (1 = 1x3min, ..., 6=6x3min, 7 continuous)
    uint8_t wWCircPumpType;  // Warm Water circulation pump type (0 = charge pump, 0xff = 3-way pump)
    uint8_t wWDesinfectTemp; // Warm Water desinfection temperature
    uint8_t wWComfort;       // Warm water comfort or ECO mode

    // UBAMonitorFast
    uint8_t  selFlowTemp;        // Selected flow temperature
    uint16_t curFlowTemp;        // Current flow temperature
    uint16_t wwStorageTemp1;     // warm water storage temp 1
    uint16_t wwStorageTemp2;     // warm water storage temp 2
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
    uint16_t exhaustTemp; // Exhaust temperature
    uint8_t  pumpMod;     // Pump modulation
    uint32_t burnStarts;  // # burner starts
    uint32_t burnWorkMin; // Total burner operating time
    uint32_t heatWorkMin; // Total heat operating time
    uint16_t switchTemp;  // Switch temperature

    // UBAMonitorWWMessage
    uint16_t wWCurTmp;        // Warm Water current temperature
    uint32_t wWStarts;        // Warm Water # starts
    uint32_t wWWorkM;         // Warm Water # minutes
    uint8_t  wWOneTime;       // Warm Water one time function on/off
    uint8_t  wWDesinfecting;  // Warm Water desinfection on/off
    uint8_t  wWReadiness;     // Warm Water readiness on/off
    uint8_t  wWRecharging;    // Warm Water recharge on/off
    uint8_t  wWTemperatureOK; // Warm Water temperature ok on/off
    uint8_t  wWCurFlow;       // Warm Water current flow in l/min

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
    uint8_t      device_id; // the device ID of the Heat Pump (e.g. 0x30)
    uint8_t      device_flags;
    const char * device_desc_p;
    uint8_t      product_id;
    char         version[10];
    uint8_t      HPModulation; // heatpump modulation in %
    uint8_t      HPSpeed;      // speed 0-100 %
} _EMS_HeatPump;

// Mixing Module per HC
typedef struct {
    uint8_t  hc;     // heating circuit 1, 2, 3 or 4
    bool     active; // true if there is data for this HC
    uint16_t flowTemp;
    uint8_t  pumpMod;
    uint8_t  valveStatus;
    uint8_t  flowSetTemp;
} _EMS_MixingModule_HC;

// Mixing Module per WWC
typedef struct {
    uint8_t  wwc;    // warm water circuit 1, 2
    bool     active; // true if there is data for this WWC
    uint16_t flowTemp;
    uint8_t  pumpMod;
    uint8_t  tempStatus;
} _EMS_MixingModule_WWC;

// Mixing data
typedef struct {
    uint8_t               device_id;
    uint8_t               device_flags;
    const char *          device_desc_p;
    uint8_t               product_id;
    char                  version[10];
    _EMS_MixingModule_HC  hc[EMS_MIXING_MAXHC];   // array for the 4 heating circuits
    _EMS_MixingModule_WWC wwc[EMS_MIXING_MAXWWC]; // array for the 2 ww circuits
} _EMS_MixingModule;

// Solar Module - SM10/SM100/SM200/ISM1
typedef struct {
    uint8_t      device_id;    // the device ID of the Solar Module
    uint8_t      device_flags; // Solar Module flags
    const char * device_desc_p;
    uint8_t      product_id;
    char         version[10];
    int16_t      collectorTemp;          // collector temp (TS1)
    int16_t      bottomTemp;             // bottom temp (TS2)
    int16_t      bottomTemp2;            // bottom temp cylinder 2 (TS5)
    uint8_t      pumpModulation;         // modulation solar pump
    uint8_t      pump;                   // pump active
    uint8_t      valveStatus;            // valve status (VS2)
    int16_t      setpoint_maxBottomTemp; // setpoint for maximum collector temp
    uint16_t     EnergyLastHour;
    uint16_t     EnergyToday;
    uint16_t     EnergyTotal;
    uint32_t     pumpWorkMin; // Total solar pump operating time
} _EMS_SolarModule;

// heating circuit
typedef struct {
    uint8_t hc;                // heating circuit 1,2, 3 or 4
    bool    active;            // true if there is data for this HC
    int16_t setpoint_roomTemp; // current set temp
    int16_t curr_roomTemp;     // current room temp
    uint8_t mode;              // 0=low, 1=manual, 2=auto (or night, day on RC35s)
    uint8_t mode_type;         // 0=night/eco, 1=day/comfort
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
    uint8_t            device_id;    // the device ID of the thermostat
    uint8_t            device_flags; // thermostat model flags
    const char *       device_desc_p;
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
    _EMS_DEVICE_UPDATE_FLAG device_flag;
    uint16_t                type;
    const char              typeString[30];
    EMS_processType_cb      processType_cb;
} _EMS_Type;

typedef enum : uint8_t {
    THERMOSTAT_TEMP_MODE_AUTO = 0,
    THERMOSTAT_TEMP_MODE_NIGHT,
    THERMOSTAT_TEMP_MODE_DAY,
    THERMOSTAT_TEMP_MODE_HOLIDAY,
    THERMOSTAT_TEMP_MODE_NOFROST,
    THERMOSTAT_TEMP_MODE_ECO, // 'sparen'
    THERMOSTAT_TEMP_MODE_HEAT // 'heizen'
} _THERMOSTAT_TEMP_MODE;

typedef enum {
    EMS_THERMOSTAT_MODE_UNKNOWN,
    EMS_THERMOSTAT_MODE_OFF,
    EMS_THERMOSTAT_MODE_MANUAL,
    EMS_THERMOSTAT_MODE_AUTO,
    EMS_THERMOSTAT_MODE_NIGHT,
    EMS_THERMOSTAT_MODE_DAY,
    EMS_THERMOSTAT_MODE_ECO,
    EMS_THERMOSTAT_MODE_COMFORT
} _EMS_THERMOSTAT_MODE;

// function definitions
void    ems_dumpBuffer(const char * prefix, uint8_t * telegram, uint8_t length);
void    ems_parseTelegram(uint8_t * telegram, uint8_t len);
void    ems_init();
void    ems_doReadCommand(uint16_t type, uint8_t dest);
void    ems_sendRawTelegram(char * telegram);
void    ems_printDevices();
uint8_t ems_printDevices_s(char * buffer, uint16_t len);
void    ems_printTxQueue();
void    ems_testTelegram(uint8_t test_num);
void    ems_startupTelegrams();
bool    ems_checkEMSBUSAlive();
void    ems_setThermostatTemp(float temperature, uint8_t hc, _THERMOSTAT_TEMP_MODE temptype = THERMOSTAT_TEMP_MODE_AUTO);
void    ems_setThermostatMode(uint8_t mode, uint8_t hc);
void    ems_setWarmWaterTemp(uint8_t temperature);
void    ems_setFlowTemp(uint8_t temperature);
void    ems_setWarmWaterActivated(bool activated);
void    ems_setWarmWaterOnetime(bool activated);
void    ems_setWarmWaterCirculation(bool activated);
void    ems_setWarmTapWaterActivated(bool activated);
void    ems_setPoll(bool b);
void    ems_setLogging(_EMS_SYS_LOGGING loglevel, uint16_t type_id);
void    ems_setLogging(_EMS_SYS_LOGGING loglevel, bool quiet = false);
void    ems_setWarmWaterModeComfort(uint8_t comfort);
void    ems_setModels();
void    ems_setTxDisabled(bool b);
void    ems_setTxMode(uint8_t mode);
void    ems_setEMSbusid(uint8_t id);
void    ems_setMasterThermostat(uint8_t product_id);
char *  ems_getDeviceDescription(_EMS_DEVICE_TYPE device_type, char * buffer, bool name_only = false);
bool    ems_getDeviceTypeDescription(uint8_t device_id, char * buffer);
char *  ems_getDeviceTypeName(_EMS_DEVICE_TYPE device_type, char * buffer);

void ems_getThermostatValues();
void ems_getBoilerValues();
void ems_getSolarModuleValues();
void ems_getMixingModuleValues();

bool ems_getPoll();
bool ems_getTxEnabled();

bool ems_getThermostatEnabled();
bool ems_getMixingModuleEnabled();
bool ems_getBoilerEnabled();
bool ems_getSolarModuleEnabled();
bool ems_getHeatPumpEnabled();

bool             ems_getBusConnected();
_EMS_SYS_LOGGING ems_getLogging();
uint8_t          ems_getThermostatFlags();
uint8_t          ems_getSolarModuleFlags();
void             ems_discoverModels();
bool             ems_getTxCapable();
uint32_t         ems_getPollFrequency();
bool             ems_getTxDisabled();
void             ems_Device_add_flags(unsigned int flags);
bool             ems_Device_has_flags(unsigned int flags);
void             ems_Device_remove_flags(unsigned int flags);
bool             ems_isHT3();
void             ems_scanDevices();

// private functions
uint8_t _crcCalculator(uint8_t * data, uint8_t len);
void    _processType(_EMS_RxTelegram * EMS_RxTelegram);
void    _debugPrintPackage(const char * prefix, _EMS_RxTelegram * EMS_RxTelegram, const char * color);
void    _ems_clearTxData();
void    _removeTxQueue();
int8_t  _getHeatingCircuit(_EMS_RxTelegram * EMS_RxTelegram);
void    _printMessage(_EMS_RxTelegram * EMS_RxTelegram, const int show_type = -1);

// global so can referenced in other classes
extern _EMS_Sys_Status   EMS_Sys_Status;
extern _EMS_Boiler       EMS_Boiler;
extern _EMS_Thermostat   EMS_Thermostat;
extern _EMS_SolarModule  EMS_SolarModule;
extern _EMS_HeatPump     EMS_HeatPump;
extern _EMS_MixingModule EMS_MixingModule;

extern std::list<_Detected_Device> Devices;
