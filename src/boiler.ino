/*
 * EMS-ESP-Boiler
 * Paul Derbyshire - May 2018 - https://github.com/proddy/EMS-ESP-Boiler
 * https://community.home-assistant.io/t/thermostat-and-boiler-controller-for-ems-based-boilers-nefit-buderus-bosch-using-esp/53382
 *
 * See ReadMe.md for Acknowledgments
 */

// local libraries
#include "ESPHelper.h"
#include "ems.h"
#include "emsuart.h"

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <CRC32.h>       // https://github.com/bakercp/CRC32

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// these are set as -D build flags during compilation
//#define WIFI_SSID "<my_ssid>"
//#define WIFI_PASSWORD "<my_password>"
//#define MQTT_IP "<broker_ip>"
//#define MQTT_USER "<broker_username>"
//#define MQTT_PASS "<broker_password>"

// timers, all values are in seconds
#define PUBLISHVALUES_TIME 300 // every 5 mins post HA values
#define SYSTEMCHECK_TIME 10    // every 10 seconds check if Boiler is online and execute other requests
#define REGULARUPDATES_TIME 60 // every minute a call is made, so for our 2 calls theres a write cmd every 30seconds
#define HEARTBEAT_TIME 1       // every second blink heartbeat LED
Ticker  publishValuesTimer;
Ticker  systemCheckTimer;
Ticker  regularUpdatesTimer;
Ticker  heartbeatTimer;
Ticker  showerColdShotStopTimer;
uint8_t regularUpdatesCount = 0;
#define MAX_MANUAL_CALLS 2 // number of ems reads we do during the fetch cycle (in regularUpdates)

// GPIOs
#define LED_RX D1                 // GPIO5
#define LED_TX D2                 // GPIO4
#define LED_ERR D3                // GPIO0
#define LED_HEARTBEAT LED_BUILTIN // onboard LED

// hostname is also used as the MQTT topic identifier (home/<hostname>)
#define HOSTNAME "boiler"

// app specific - do not change
#define MQTT_BOILER MQTT_BASE HOSTNAME "/"
#define TOPIC_START MQTT_BOILER MQTT_TOPIC_START

// thermostat
#define TOPIC_THERMOSTAT_DATA MQTT_BOILER "thermostat_data"         // for sending thermostat values
#define TOPIC_THERMOSTAT_CMD_TEMP MQTT_BOILER "thermostat_cmd_temp" // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CMD_MODE MQTT_BOILER "thermostat_cmd_mode" // for received thermostat mode changes
#define TOPIC_THERMOSTAT_CURRTEMP "thermostat_currtemp"             // current temperature
#define TOPIC_THERMOSTAT_SELTEMP "thermostat_seltemp"               // selected temperature
#define TOPIC_THERMOSTAT_MODE "thermostat_mode"                     // mode

// boiler
#define TOPIC_BOILER_DATA MQTT_BOILER "boiler_data"                              // for sending boiler values
#define TOPIC_BOILER_WARM_WATER_SELECTED_TEMPERATURE MQTT_BOILER "boiler_wwtemp" // warm water selected temp

// shower time
#define TOPIC_SHOWERTIME MQTT_BOILER "showertime"           // for sending shower time results
#define TOPIC_SHOWER_ALARM "shower_alarm"                   // for notifying HA that shower time has reached its limit
#define TOPIC_SHOWER_TIMER MQTT_BOILER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT MQTT_BOILER "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT MQTT_BOILER "shower_coldshot" // used to trigger a coldshot from HA publish

// default values
// thermostat support, shower timing and shower alert all enabled (1) (disabled = 0)
#define BOILER_THERMOSTAT_ENABLED 1
#define BOILER_SHOWER_TIMER 1
#define BOILER_SHOWER_ALERT 0

// logging - EMS_SYS_LOGGING_VERBOSE, EMS_SYS_LOGGING_NONE, EMS_SYS_LOGGING_BASIC (see ems.h)
#define BOILER_DEFAULT_LOGGING EMS_SYS_LOGGING_NONE

// shower settings
#ifndef SHOWER_TEST
const unsigned long SHOWER_PAUSE_TIME   = 15000;  // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION = 180000; // 3 minutes, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION = 420000; // 7 minutes, before trigger a shot of cold water
const unsigned long SHOWER_COLDSHOT_DURATION = 5; // 5 seconds for cold water - note, must be in seconds
const unsigned long SHOWER_OFFSET_TIME       = 8000; // 8 seconds grace time, to calibrate actual time under the shower
#else
// for DEBUGGING only
const unsigned long SHOWER_PAUSE_TIME   = 15000;  // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION = 20000;  // 20 secs, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION = 25000;  // 25 secs, before trigger a shot of cold water
const unsigned long SHOWER_COLDSHOT_DURATION = 5; // in seconds! how long for cold water shot
const unsigned long SHOWER_OFFSET_TIME       = 0; // 0 seconds grace time, to calibrate actual time under the shower
#endif

const uint8_t SHOWER_BURNPOWER_MIN = 80;
typedef struct {
    bool wifi_connected;
    bool boiler_online;
    bool thermostat_enabled;
    bool shower_timer; // true if we want to report back on shower times
    bool shower_alert; // true if we want the cold water reminder
} _Boiler_Status;

typedef struct {
    bool          showerOn;
    bool          hotWaterOn;
    unsigned long timerStart;    // ms
    unsigned long timerPause;    // ms
    unsigned long duration;      // ms
    bool          doingColdShot; // true if we've just sent a jolt of cold water
} _Boiler_Shower;

// ESPHelper
netInfo homeNet = {.mqttHost = MQTT_IP,
                   .mqttUser = MQTT_USER,
                   .mqttPass = MQTT_PASS,
                   .mqttPort = 1883, // this is the default, change if using another port
                   .ssid     = WIFI_SSID,
                   .pass     = WIFI_PASSWORD};

ESPHelper myESP(&homeNet);

command_t PROGMEM project_cmds[] = {{"s", "show statistics"},
                                    {"h", "list supported EMS telegram type ids"},
                                    {"P", "publish all stat to MQTT"},
                                    {"v", "[n] set logging (0=none, 1=basic, 2=verbose)"},
                                    {"p", "poll ems response on/off (default is off)"},
                                    {"T", "thermostat monitoring on/off"},
                                    {"S", "shower timer on/off"},
                                    {"A", "shower alert on/off"},
                                    {"r", "[n] send EMS request (n=telegram type id. Use 'h' for list)"},
                                    {"t", "[n] set thermostat temperature to n"},
                                    {"m", "[n] set thermostat mode (1=manual, 2=auto)"},
                                    {"w", "[n] set boiler warm water temperature to n (min 30)"},
                                    {"a", "[n] boiler warm water on (n=1) or off (n=0)"},
                                    {"x", "[n] experimental (warning: for debugging only!)"}};

// calculates size of an 2d array at compile time
template <typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
    return N;
}

// store for overall system status
_Boiler_Status Boiler_Status;
_Boiler_Shower Boiler_Shower;

// Debugger to telnet
#define myDebug(x, ...) myESP.printf(x, ##__VA_ARGS__);

// CRC checks
uint32_t previousBoilerPublishCRC     = 0;
uint32_t previousThermostatPublishCRC = 0;

// Times
const unsigned long POLL_TIMEOUT_ERR = 10000; // if no signal from boiler for last 10 seconds, assume its offline
const unsigned long TX_HOLD_LED_TIME = 2000;  // how long to hold the Tx LED because its so quick

unsigned long timestamp; // for internal timings, via millis()
static int    connectionStatus = NO_CONNECTION;
bool          startMQTTsent    = false;

// toggle for heartbeat LED
bool heartbeatEnabled;

// logging messages with fixed strings (newline done automatically)
void myDebugLog(const char * s) {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("%s\n", s);
    }
#ifdef DEBUG
    myESP.logger(LOG_HA, s);
#endif
}

// convert float to char
//char * _float_to_char(char * a, float f, uint8_t precision = 1);
char * _float_to_char(char * a, float f, uint8_t precision = 1) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret = a;
    // check for 0x8000 (sensor missing), which has a -1 value
    if (f == EMS_VALUE_FLOAT_NOTSET) {
        strcpy(ret, "?");
    } else {
        long whole = (long)f;
        itoa(whole, a, 10);
        while (*a != '\0')
            a++;
        *a++         = '.';
        long decimal = abs((long)((f - whole) * p[precision]));
        itoa(decimal, a, 10);
    }
    return ret;
}

// convert bool to text
char * _bool_to_char(char * s, uint8_t value) {
    if (value == EMS_VALUE_INT_ON) {
        strcpy(s, "on");
    } else if (value == EMS_VALUE_INT_OFF) {
        strcpy(s, "off");
    } else {
        strcpy(s, "?");
    }
    return s;
}

// convert int to text value
char * _int_to_char(char * s, uint8_t value) {
    if (value == EMS_VALUE_INT_NOTSET) {
        strcpy(s, "?");
    } else {
        itoa(value, s, 10);
    }
    return s;
}

// takes a float value at prints it to debug log
void _renderFloatValue(const char * prefix, const char * postfix, float value) {
    myDebug("  %s: ", prefix);
    char s[20];
    myDebug("%s", _float_to_char(s, value));

    if (postfix != NULL) {
        myDebug(" %s", postfix);
    }

    myDebug("\n");
}

// takes an int value at prints it to debug log
void _renderIntValue(const char * prefix, const char * postfix, uint8_t value) {
    myDebug("  %s: ", prefix);
    char s[20];
    myDebug("%s", _int_to_char(s, value));

    if (postfix != NULL) {
        myDebug(" %s", postfix);
    }

    myDebug("\n");
}

// takes a bool value at prints it to debug log
void _renderBoolValue(const char * prefix, uint8_t value) {
    myDebug("  %s: ", prefix);
    char s[20];
    myDebug("%s\n", _bool_to_char(s, value));
}

// Show command - display stats on an 's' command
void showInfo() {
    // General stats from EMS bus

    myDebug("%sEMS-ESP-Boiler system stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
    myDebug("  System Logging is set to ");
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug("Basic");
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug("Verbose");
    } else {
        myDebug("None");
    }

    myDebug("\n  # EMS type handlers: %d\n", ems_getEmsTypesCount());

    myDebug("  Thermostat is %s, Poll is %s, Shower timer is %s, Shower alert is %s\n",
            ((Boiler_Status.thermostat_enabled) ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsPollEnabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_timer) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_alert) ? "enabled" : "disabled"));

    myDebug("  EMS Bus Stats: Connected=%s, # Rx telegrams=%d, # Tx telegrams=%d, # Crc Errors=%d, ",
            (Boiler_Status.boiler_online ? "yes" : "no"),
            EMS_Sys_Status.emsRxPgks,
            EMS_Sys_Status.emsTxPkgs,
            EMS_Sys_Status.emxCrcErr);

    myDebug("Rx Status=");
    switch (EMS_Sys_Status.emsRxStatus) {
    case EMS_RX_IDLE:
        myDebug("idle");
        break;
    case EMS_RX_ACTIVE:
        myDebug("active");
        break;
    }

    myDebug(", Tx Status=");
    switch (EMS_Sys_Status.emsTxStatus) {
    case EMS_TX_IDLE:
        myDebug("idle");
        break;
    case EMS_TX_PENDING:
        myDebug("pending");
        break;
    case EMS_TX_ACTIVE:
        myDebug("active");
        break;
    }

    myDebug(", Last Tx Action=");
    switch (EMS_TxTelegram.action) {
    case EMS_TX_READ:
        myDebug("read");
        break;
    case EMS_TX_WRITE:
        myDebug("write");
        break;
    case EMS_TX_VALIDATE:
        myDebug("validate");
        break;
    case EMS_TX_NONE:
        myDebug("none");
        break;
    }

    myDebug("\n\n%sBoiler stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);

    // UBAParameterWW
    _renderBoolValue("Warm Water activated", EMS_Boiler.wWActivated);
    _renderBoolValue("Warm Water circulation pump available", EMS_Boiler.wWCircPump);
    _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
    _renderIntValue("Warm Water desired temperature", "C", EMS_Boiler.wWDesiredTemp);

    // UBAMonitorWWMessage
    _renderFloatValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
    _renderIntValue("Warm Water # starts", "times", EMS_Boiler.wWStarts);
    myDebug("  Warm Water active time: %d days %d hours %d minutes\n",
            EMS_Boiler.wWWorkM / 1440,
            (EMS_Boiler.wWWorkM % 1440) / 60,
            EMS_Boiler.wWWorkM % 60);
    _renderBoolValue("Warm Water 3-way valve", EMS_Boiler.wWHeat);

    // UBAMonitorFast
    _renderIntValue("Selected flow temperature", "C", EMS_Boiler.selFlowTemp);
    _renderFloatValue("Current flow temperature", "C", EMS_Boiler.curFlowTemp);
    _renderFloatValue("Return temperature", "C", EMS_Boiler.retTemp);
    _renderBoolValue("Gas", EMS_Boiler.burnGas);
    _renderBoolValue("Boiler pump", EMS_Boiler.heatPmp);
    _renderBoolValue("Fan", EMS_Boiler.fanWork);
    _renderBoolValue("Ignition", EMS_Boiler.ignWork);
    _renderBoolValue("Circulation pump", EMS_Boiler.wWCirc);
    _renderIntValue("Burner selected max power", "%", EMS_Boiler.selBurnPow);
    _renderIntValue("Burner current power", "%", EMS_Boiler.curBurnPow);
    _renderFloatValue("Flame current", "uA", EMS_Boiler.flameCurr);
    _renderFloatValue("System pressure", "bar", EMS_Boiler.sysPress);

    // UBAMonitorSlow
    _renderFloatValue("Outside temperature", "C", EMS_Boiler.extTemp);
    _renderFloatValue("Boiler temperature", "C", EMS_Boiler.boilTemp);
    _renderIntValue("Pump modulation", "%", EMS_Boiler.pumpMod);
    _renderIntValue("Burner # restarts", "times", EMS_Boiler.burnStarts);
    myDebug("  Total burner operating time: %d days %d hours %d minutes\n",
            EMS_Boiler.burnWorkMin / 1440,
            (EMS_Boiler.burnWorkMin % 1440) / 60,
            EMS_Boiler.burnWorkMin % 60);
    myDebug("  Total heat operating time: %d days %d hours %d minutes\n",
            EMS_Boiler.heatWorkMin / 1440,
            (EMS_Boiler.heatWorkMin % 1440) / 60,
            EMS_Boiler.heatWorkMin % 60);

    // Thermostat stats
    if (Boiler_Status.thermostat_enabled) {
        myDebug("\n%sThermostat stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Thermostat time is %02d:%02d:%02d %d/%d/%d\n",
                EMS_Thermostat.hour,
                EMS_Thermostat.minute,
                EMS_Thermostat.second,
                EMS_Thermostat.day,
                EMS_Thermostat.month,
                EMS_Thermostat.year + 2000);

        _renderFloatValue("Setpoint room temperature", "C", EMS_Thermostat.setpoint_roomTemp);
        _renderFloatValue("Current room temperature", "C", EMS_Thermostat.curr_roomTemp);
        myDebug("  Mode is set to ");
        if (EMS_Thermostat.mode == 0) {
            myDebug("low\n");
        } else if (EMS_Thermostat.mode == 1) {
            myDebug("manual\n");
        } else if (EMS_Thermostat.mode == 2) {
            myDebug("auto\n");
        } else {
            myDebug("<%d>\n", EMS_Thermostat.mode);
        }
    }

    // show the Shower Info
    if (Boiler_Status.shower_timer) {
        myDebug("\n%sShower stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Hot water is %s\n", (Boiler_Shower.hotWaterOn ? "running" : "stopped"));
        myDebug("  Shower is %s\n", (Boiler_Shower.showerOn ? "on" : "off"));
    }

    myDebug("\n");
}

// send values to HA via MQTT
void publishValues(bool force) {
    char s[20]; // for formatting strings

    // Boiler values as one JSON object
    StaticJsonBuffer<512> jsonBuffer;
    char                  data[512];
    JsonObject &          rootBoiler = jsonBuffer.createObject();

    rootBoiler["wWSelTemp"]   = _int_to_char(s, EMS_Boiler.wWSelTemp);
    rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);
    rootBoiler["wWCurTmp"]    = _float_to_char(s, EMS_Boiler.wWCurTmp);
    rootBoiler["wWHeat"]      = _bool_to_char(s, EMS_Boiler.wWHeat);
    rootBoiler["curFlowTemp"] = _float_to_char(s, EMS_Boiler.curFlowTemp);
    rootBoiler["retTemp"]     = _float_to_char(s, EMS_Boiler.retTemp);
    rootBoiler["burnGas"]     = _bool_to_char(s, EMS_Boiler.burnGas);
    rootBoiler["heatPmp"]     = _bool_to_char(s, EMS_Boiler.heatPmp);
    rootBoiler["fanWork"]     = _bool_to_char(s, EMS_Boiler.fanWork);
    rootBoiler["ignWork"]     = _bool_to_char(s, EMS_Boiler.ignWork);
    rootBoiler["wWCirc"]      = _bool_to_char(s, EMS_Boiler.wWCirc);
    rootBoiler["selBurnPow"]  = _int_to_char(s, EMS_Boiler.selBurnPow);
    rootBoiler["curBurnPow"]  = _int_to_char(s, EMS_Boiler.curBurnPow);
    rootBoiler["sysPress"]    = _float_to_char(s, EMS_Boiler.sysPress);
    rootBoiler["boilTemp"]    = _float_to_char(s, EMS_Boiler.boilTemp);
    rootBoiler["pumpMod"]     = _int_to_char(s, EMS_Boiler.pumpMod);

    size_t len = rootBoiler.measureLength();
    rootBoiler.printTo(data, len + 1); // form the json string

    // calculate hash and send values if something has changed, to save unnecessary wifi traffic
    CRC32 crc;
    for (size_t i = 0; i < len - 1; i++) {
        crc.update(data[i]);
    }
    uint32_t checksum = crc.finalize();
    //myDebug("HASH=%d %08x, len=%d, s=%s\n", checksum, checksum, len, data);

    if ((previousBoilerPublishCRC != checksum) || force) {
        previousBoilerPublishCRC = checksum;
        if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
            myDebug("Publishing boiler data via MQTT\n");
        }

        // send values via MQTT
        myESP.publish(TOPIC_BOILER_DATA, data);
    }

    // handle the thermostat values separately
    if (EMS_Sys_Status.emsThermostatEnabled) {
        // only send thermostat values if we actually have them
        if (((int)EMS_Thermostat.curr_roomTemp == (int)0) || ((int)EMS_Thermostat.setpoint_roomTemp == (int)0))
            return;

        // build json object
        JsonObject & rootThermostat               = jsonBuffer.createObject();
        rootThermostat[TOPIC_THERMOSTAT_CURRTEMP] = _float_to_char(s, EMS_Thermostat.curr_roomTemp);
        rootThermostat[TOPIC_THERMOSTAT_SELTEMP]  = _float_to_char(s, EMS_Thermostat.setpoint_roomTemp);

        // send mode 0=low, 1=manual, 2=auto
        if (EMS_Thermostat.mode == 0) {
            rootThermostat[TOPIC_THERMOSTAT_MODE] = "low";
        } else if (EMS_Thermostat.mode == 1) {
            rootThermostat[TOPIC_THERMOSTAT_MODE] = "manual";
        } else {
            rootThermostat[TOPIC_THERMOSTAT_MODE] = "auto";
        }

        size_t len = rootThermostat.measureLength();
        rootThermostat.printTo(data, len + 1); // form the json string

        // calculate new CRC
        crc.reset();
        for (size_t i = 0; i < len - 1; i++) {
            crc.update(data[i]);
        }
        uint32_t checksum = crc.finalize();

        if ((previousThermostatPublishCRC != checksum) || force) {
            previousThermostatPublishCRC = checksum;
            if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                myDebug("Publishing thermostat data via MQTT\n");
            }

            // send values via MQTT
            myESP.publish(TOPIC_THERMOSTAT_DATA, data);
        }
    }
}

// sets the shower timer on/off
void set_showerTimer() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower timer is %s\n", Boiler_Status.shower_timer ? "enabled" : "disabled");
    }
}

// sets the shower alert on/off
void set_showerAlert() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower alert is %s\n", Boiler_Status.shower_alert ? "enabled" : "disabled");
    }
}

// extra commands options for telnet debug window
void myDebugCallback() {
    char * cmd = myESP.consoleGetLastCommand();
    bool   b;

    switch (cmd[0]) {
    case 's':
        showInfo();
        break;
    case 'p':
        b = !ems_getPoll();
        ems_setPoll(b);
        break;
    case 'P':
        myESP.logger(LOG_HA, "Force publish values");
        publishValues(true);
        break;
    case 'r': // read command for Boiler or Thermostat
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16));
        break;
    case 't': // set thermostat temp
        ems_setThermostatTemp(strtof(&cmd[2], 0));
        break;
    case 'h': // show type handlers
        ems_printAllTypes();
        break;
    case 'm': // set thermostat mode
        if ((cmd[2] - '0') == 1)
            ems_setThermostatMode(1);
        else if ((cmd[2] - '0') == 2)
            ems_setThermostatMode(2);
        break;
    case 'w': // set warm water temp
        ems_setWarmWaterTemp((uint8_t)strtol(&cmd[2], 0, 10));
        break;
    case 'v': // verbose
        ems_setLogging((_EMS_SYS_LOGGING)(cmd[2] - '0'));
        updateHeartbeat();
        break;
    case 'a': // set ww activate on or off
        if ((cmd[2] - '0') == 1)
            ems_setWarmWaterActivated(true);
        else if ((cmd[2] - '0') == 0)
            ems_setWarmWaterActivated(false);
        break;
    case 'x':                                                 // experimental code for debugging - use with caution!
        ems_setExperimental((uint8_t)strtol(&cmd[2], 0, 16)); // takes HEX param
        break;
    case 'T': // toggle Thermostat
        b = !ems_getThermostatEnabled();
        ems_setThermostatEnabled(b);
        Boiler_Status.thermostat_enabled = b;
        break;
    case 'S': // toggle Shower timer support
        Boiler_Status.shower_timer = !Boiler_Status.shower_timer;
        myESP.publish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
        break;
    case 'A': // toggle Shower alert
        Boiler_Status.shower_alert = !Boiler_Status.shower_alert;
        myESP.publish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");
        break;
    default:
        myDebug("Unknown command '%c'. Use ? for help.\n", cmd[0]);
        break;
    }
}

// MQTT Callback to handle incoming/outgoing changes
void MQTTcallback(char * topic, byte * payload, uint8_t length) {
    // check if start is received, if so return boottime - defined in ESPHelper.h
    if (strcmp(topic, TOPIC_START) == 0) {
        payload[length] = '\0'; // add null terminator
        //myDebug("MQTT topic boottime: %s\n", payload);
        myESP.setBoottime((char *)payload);
        return;
    }

    // thermostat temp changes
    if (strcmp(topic, TOPIC_THERMOSTAT_CMD_TEMP) == 0) {
        float f = strtof((char *)payload, 0);
        char  s[10];
        myDebug("MQTT topic: thermostat temp value %s\n", _float_to_char(s, f));
        ems_setThermostatTemp(f);
        // publish back so HA is immediately updated
        publishValues(true);
        return;
    }

    // thermostat mode changes
    if (strcmp(topic, TOPIC_THERMOSTAT_CMD_MODE) == 0) {
        payload[length] = '\0'; // add null terminator
        myDebug("MQTT topic: thermostat mode value %s\n", payload);
        if (strcmp((char *)payload, "auto") == 0) {
            ems_setThermostatMode(2);
        } else if (strcmp((char *)payload, "manual") == 0) {
            ems_setThermostatMode(1);
        }
        return;
    }

    // boiler_warm_water_selected_temperature
    if (strcmp(topic, TOPIC_BOILER_WARM_WATER_SELECTED_TEMPERATURE) == 0) {
        uint8_t i = strtol((char *)payload, 0, 10);
        myDebug("MQTT topic: boiler_warm_water_selected_temperature value %d\n", i);
#ifndef NO_TX
        ems_setWarmWaterTemp(i);
#endif
        // publish back so HA is immediately updated
        publishValues(true);
        return;
    }

    // shower timer
    if (strcmp(topic, TOPIC_SHOWER_TIMER) == 0) {
        if (payload[0] == '1') {
            Boiler_Status.shower_timer = true;
        } else if (payload[0] == '0') {
            Boiler_Status.shower_timer = false;
        }
        set_showerTimer();
        return;
    }

    // shower alert
    if (strcmp(topic, TOPIC_SHOWER_ALERT) == 0) {
        if (payload[0] == '1') {
            Boiler_Status.shower_alert = true;
        } else if (payload[0] == '0') {
            Boiler_Status.shower_alert = false;
        }
        set_showerAlert();
        return;
    }

    // shower cold shot
    if (strcmp(topic, TOPIC_SHOWER_COLDSHOT) == 0) {
        if (Boiler_Status.shower_alert) {
            _showerColdShotStart();
        }
        return;
    }

    // if HA is booted, restart device too
    if (strcmp(topic, MQTT_HA) == 0) {
        payload[length] = '\0'; // add null terminator
        if (strcmp((char *)payload, "start") == 0) {
            myDebug("HA rebooted - restarting device\n");
            myESP.resetESP();
        }
    }
}

// WifiCallback, called when a WiFi connect has successfully been established
void WIFIcallback() {
    Boiler_Status.wifi_connected = true;

#ifdef USE_LED
    // turn off the LEDs since we've finished the boot loading
    digitalWrite(LED_RX, LOW);
    digitalWrite(LED_TX, LOW);
    digitalWrite(LED_ERR, LOW);
    digitalWrite(LED_HEARTBEAT, HIGH);
#endif

    // when finally we're all set up, we can fire up the uart (this will enable the UART interrupts)
    emsuart_init();
}

// Sets the LED heartbeat depending on the logging setting
void updateHeartbeat() {
    _EMS_SYS_LOGGING logSetting = ems_getLogging();
    if (logSetting == EMS_SYS_LOGGING_VERBOSE) {
        heartbeatEnabled = true;
    } else {
        heartbeatEnabled = false;
#ifdef USE_LED
        // ...and turn off LED
        digitalWrite(LED_HEARTBEAT, HIGH);
#endif
    }
}

// Initialize the boiler settings
void _initBoiler() {
    // default settings
    Boiler_Status.shower_timer       = BOILER_SHOWER_TIMER;
    Boiler_Status.shower_alert       = BOILER_SHOWER_ALERT;
    Boiler_Status.thermostat_enabled = BOILER_THERMOSTAT_ENABLED;
    ems_setThermostatEnabled(Boiler_Status.thermostat_enabled);

    // init boiler
    Boiler_Status.wifi_connected = false;
    Boiler_Status.boiler_online  = false;

    // init shower
    Boiler_Shower.timerStart    = 0;
    Boiler_Shower.timerPause    = 0;
    Boiler_Shower.duration      = 0;
    Boiler_Shower.doingColdShot = false;

    // heartbeat only if verbose logging
    ems_setLogging(BOILER_DEFAULT_LOGGING);
    updateHeartbeat();
}

// call PublishValues without forcing, so using CRC to see if we really need to publish
void do_publishValues() {
    publishValues(false);
}

//
// SETUP
// Note: we don't init the UART here as we should wait until everything is loaded first. It's done in loop()
//
void setup() {
#ifdef USE_LED
    // set pin for LEDs - start up with all lit up while we sort stuff out
    pinMode(LED_RX, OUTPUT);
    pinMode(LED_TX, OUTPUT);
    pinMode(LED_ERR, OUTPUT);
    pinMode(LED_HEARTBEAT, OUTPUT);
    digitalWrite(LED_RX, HIGH);
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_ERR, HIGH);
    digitalWrite(LED_HEARTBEAT, LOW);                 // onboard LED is on
    heartbeatTimer.attach(HEARTBEAT_TIME, heartbeat); // blink heartbeat LED
#endif

    // Timers using Ticker library
    publishValuesTimer.attach(PUBLISHVALUES_TIME, do_publishValues); // post HA values
    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck);       // check if Boiler is online

#ifndef NO_TX
    regularUpdatesTimer.attach((REGULARUPDATES_TIME / MAX_MANUAL_CALLS), regularUpdates); // regular reads from the EMS
#endif

    // set up WiFi
    myESP.setWifiCallback(WIFIcallback);

    // set up MQTT
    myESP.setMQTTCallback(MQTTcallback);
    myESP.addSubscription(MQTT_HA);
    myESP.addSubscription(TOPIC_START);
    myESP.addSubscription(TOPIC_THERMOSTAT_CMD_TEMP);
    myESP.addSubscription(TOPIC_THERMOSTAT_CMD_MODE);
    myESP.addSubscription(TOPIC_SHOWER_TIMER);
    myESP.addSubscription(TOPIC_SHOWER_ALERT);
    myESP.addSubscription(TOPIC_BOILER_WARM_WATER_SELECTED_TEMPERATURE);
    myESP.addSubscription(TOPIC_SHOWER_COLDSHOT);

    myESP.consoleSetCallBackProjectCmds(project_cmds, ArraySize(project_cmds), myDebugCallback); // set up Telnet commands
    myESP.begin(HOSTNAME); // start wifi and mqtt services

    // init ems stats
    ems_init();

    // init Boiler specific params
    _initBoiler();
}

// flash LEDs
// Using a faster way to write to pins as digitalWrite does a lot of overhead like pin checking & disabling interrupts
void showLEDs() {
    // ERR LED
    if (!Boiler_Status.boiler_online) {
        WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + 4, (1 << LED_ERR)); // turn on
        EMS_Sys_Status.emsRxStatus = EMS_RX_IDLE;
        EMS_Sys_Status.emsTxStatus = EMS_TX_IDLE;
    } else {
        WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + 8, (1 << LED_ERR)); // turn off
    }

    // Rx LED
    WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + ((EMS_Sys_Status.emsRxStatus == EMS_RX_IDLE) ? 8 : 4), (1 << LED_RX));

    // Tx LED
    // because sends are quick, if we did a recent send show the LED for a short while
    uint64_t t = (timestamp - EMS_Sys_Status.emsLastTx);
    WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + ((t < TX_HOLD_LED_TIME) ? 4 : 8), (1 << LED_TX));
}

// heartbeat callback to light up the LED, called via Ticker
void heartbeat() {
    if (heartbeatEnabled) {
#ifdef USE_LED
        int state = digitalRead(LED_HEARTBEAT);
        digitalWrite(LED_HEARTBEAT, !state);
#endif
    }
}

// do a healthcheck every now and then to see if we connections
void do_systemCheck() {
    // first do a system check to see if there is still a connection to the EMS
    Boiler_Status.boiler_online = ((timestamp - EMS_Sys_Status.emsLastPoll) < POLL_TIMEOUT_ERR);
    if (!Boiler_Status.boiler_online) {
        myDebug("Error! Unable to connect to EMS bus. Please check connections. Retry in 10 seconds...\n");
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
// number of calls is defined in MAX_MANUAL_CALLS
// it's done as a cycle to prevent collisions, since we can only do 1 read command at a time
void regularUpdates() {
    uint8_t cycle = (regularUpdatesCount++ % MAX_MANUAL_CALLS);

    // only do calls if the EMS is connected and alive
    if (Boiler_Status.boiler_online) {
        if ((cycle == 0) && Boiler_Status.thermostat_enabled) {
            // force get the thermostat mode which is not broadcasted
            // important! this is only configured for the RC20. Look up the correct telegram type for other thermostat versions
            if (EMS_ID_THERMOSTAT == 0x17) { // RC20 is type 0x17
                ems_doReadCommand(EMS_TYPE_RC20Temperature);
            }
        } else if (cycle == 1) {
            ems_doReadCommand(EMS_TYPE_UBAParameterWW); // get Warm Water values
        }
    }
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    myDebugLog("Shower: doing a shot of cold");
    ems_setWarmWaterActivated(false);
    Boiler_Shower.doingColdShot = true;
    // start the timer for n seconds which will reset the water back to hot
    showerColdShotStopTimer.attach(SHOWER_COLDSHOT_DURATION, _showerColdShotStop);
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (Boiler_Shower.doingColdShot) {
        myDebugLog("Shower: finished shot of cold. hot water back on");
        ems_setWarmWaterActivated(true);
        Boiler_Shower.doingColdShot = false;
        showerColdShotStopTimer.detach();
    }
}

//
// Main loop
//
void loop() {
    connectionStatus = myESP.loop();
    timestamp        = millis();

    // update the Rx Tx and ERR LEDs
#ifdef USE_LED
    showLEDs();
#endif

    // do not continue unless we have a wifi connection
    if (connectionStatus < WIFI_ONLY) {
        return;
    }

    // if first time connected to MQTT, send welcome start message
    // which will send all the state values from HA back to the clock via MQTT and return the boottime
    if ((!startMQTTsent) && (connectionStatus == FULL_CONNECTION)) {
        myESP.sendStart();
        startMQTTsent = true;

        // publish to HA the status of the Shower parameters
        myESP.publish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
        myESP.publish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");

#ifndef NO_TX
        if (Boiler_Status.boiler_online) {
            // now that we're connected lets get some data from the EMS
            ems_doReadCommand(EMS_TYPE_UBAParameterWW);
            ems_setWarmWaterActivated(true); // make sure warm water if activated, in case it got stuck with the shower alert
        } else {
            myDebugLog("Boot: can't connect to EMS.");
        }
#endif
    }

    // if we received new data and flagged for pushing, do it
    if (EMS_Sys_Status.emsRefreshed) {
        EMS_Sys_Status.emsRefreshed = false;
        publishValues(true);
    }

    /*
     * Shower Logic
     */
    if (Boiler_Status.shower_timer) {
        // if already in cold mode, ignore all this logic until we're out of the cold blast
        if (!Boiler_Shower.doingColdShot) {
            // these values come from UBAMonitorFast - type 0x18) which is broadcasted every second so our timings are accurate enough
            // and no need to fetch the values from the boiler
            Boiler_Shower.hotWaterOn =
                ((EMS_Boiler.selBurnPow >= SHOWER_BURNPOWER_MIN) && (EMS_Boiler.selFlowTemp == 0) && EMS_Boiler.burnGas);

            // is the hot water running?
            if (Boiler_Shower.hotWaterOn) {
                // if heater was previously off, start the timer
                if (Boiler_Shower.timerStart == 0) {
                    // hot water just started...
                    Boiler_Shower.timerStart    = timestamp;
                    Boiler_Shower.timerPause    = 0; // remove any last pauses
                    Boiler_Shower.doingColdShot = false;
                    Boiler_Shower.duration      = 0;
                    Boiler_Shower.showerOn      = false;
                    myDebugLog("Shower: hot water on...");
                } else {
                    // hot water has been  on for a while
                    // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                    if (!Boiler_Shower.showerOn && (timestamp - Boiler_Shower.timerStart) > SHOWER_MIN_DURATION) {
                        Boiler_Shower.showerOn = true;
                        myDebugLog("Shower: hot water still running, starting shower timer");
                    }
                    // check if the shower has been on too long
                    else if ((((timestamp - Boiler_Shower.timerStart) > SHOWER_MAX_DURATION)
                              && !Boiler_Shower.doingColdShot)
                             && Boiler_Status.shower_alert) {
                        myESP.sendHACommand(TOPIC_SHOWER_ALARM);
                        myDebugLog("Shower: exceeded max shower time");
                        _showerColdShotStart();
                    }
                }
            } else { // hot water is off
                // if it just turned off, record the time as it could be a short pause
                if ((Boiler_Shower.timerStart != 0) && (Boiler_Shower.timerPause == 0)) {
                    Boiler_Shower.timerPause = timestamp;
                    myDebugLog("Shower: hot water turned off");
                }

                // if shower has been off for longer than the wait time
                if ((Boiler_Shower.timerPause != 0) && ((timestamp - Boiler_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
                    /*
                    sprintf(s,
                            "Shower: duration %d offset %d",
                            (Boiler_Shower.timerPause - Boiler_Shower.timerStart),
                            SHOWER_OFFSET_TIME);
                    myDebugLog("s");
                    */

                    // it is over the wait period, so assume that the shower has finished and calculate the total time and publish
                    // because its unsigned long, can't have negative so check if length is less than OFFSET_TIME
                    if ((Boiler_Shower.timerPause - Boiler_Shower.timerStart) > SHOWER_OFFSET_TIME) {
                        Boiler_Shower.duration =
                            (Boiler_Shower.timerPause - Boiler_Shower.timerStart - SHOWER_OFFSET_TIME);
                        if (Boiler_Shower.duration > SHOWER_MIN_DURATION) {
                            char s[50];
                            sprintf(s,
                                    "%d minutes and %d seconds",
                                    (uint8_t)((Boiler_Shower.duration / (1000 * 60)) % 60),
                                    (uint8_t)((Boiler_Shower.duration / 1000) % 60));

                            if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                                myDebug("Shower: finished with duration %s\n", s);
                            }
                            myESP.publish(TOPIC_SHOWERTIME, s); // publish to HA
                        }
                    }

                    // reset everything
                    myDebugLog("Shower: resetting timers");
                    Boiler_Shower.timerStart = 0;
                    Boiler_Shower.timerPause = 0;
                    Boiler_Shower.showerOn   = false;
                    _showerColdShotStop(); // turn hot water back on in case its off
                }
            }
        }
    }

    // yield to prevent watchdog from timing out
    yield();
}
