/*
 * EMS-ESP-Boiler
 * Paul Derbyshire - May 2018 - https://github.com/proddy/EMS-ESP-Boiler
 * https://community.home-assistant.io/t/thermostat-and-boiler-controller-for-ems-based-boilers-nefit-buderus-bosch-using-esp/53382
 *
 * See README for Acknowledgments
 */

// local libraries
#include "ESPHelper.h"
#include "ems.h"
#include "emsuart.h"
#include "my_config.h"
#include "version.h"

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <CRC32.h>       // https://github.com/bakercp/CRC32

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// timers, all values are in seconds
#define PUBLISHVALUES_TIME 120 // every 2 minutes post HA values
Ticker publishValuesTimer;

#define SYSTEMCHECK_TIME 10 // every 10 seconds check if Boiler is online and execute other requests
Ticker systemCheckTimer;

#define REGULARUPDATES_TIME 60 // every minute a call is made
Ticker regularUpdatesTimer;

#define HEARTBEAT_TIME 1 // every second blink heartbeat LED
Ticker heartbeatTimer;

// thermostat scan - for debugging
Ticker scanThermostat;
#define SCANTHERMOSTAT_TIME 4
uint8_t scanThermostat_count;

Ticker showerColdShotStopTimer;

// GPIOs
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
#define TOPIC_BOILER_DATA MQTT_BOILER "boiler_data"                // for sending boiler values
#define TOPIC_BOILER_TAPWATER_ACTIVE MQTT_BOILER "tapwater_active" // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE MQTT_BOILER "heating_active"   // if heating is on

// shower time
#define TOPIC_SHOWERTIME MQTT_BOILER "showertime"           // for sending shower time results
#define TOPIC_SHOWER_ALARM "shower_alarm"                   // for notifying HA that shower time has reached its limit
#define TOPIC_SHOWER_TIMER MQTT_BOILER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT MQTT_BOILER "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT MQTT_BOILER "shower_coldshot" // used to trigger a coldshot from HA publish

// logging - EMS_SYS_LOGGING_VERBOSE, EMS_SYS_LOGGING_NONE, EMS_SYS_LOGGING_BASIC (see ems.h)
#define BOILER_DEFAULT_LOGGING EMS_SYS_LOGGING_NONE

// shower settings for DEBUGGING only
#ifdef SHOWER_TEST
#undef SHOWER_PAUSE_TIME
#undef SHOWER_MIN_DURATION
#undef SHOWER_MAX_DURATION
#undef SHOWER_COLDSHOT_DURATION
#undef SHOWER_OFFSET_TIME
const unsigned long SHOWER_PAUSE_TIME   = 15000;  // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION = 20000;  // 20 secs, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION = 25000;  // 25 secs, before trigger a shot of cold water
const unsigned long SHOWER_COLDSHOT_DURATION = 5; // in seconds! how long for cold water shot
const unsigned long SHOWER_OFFSET_TIME       = 0; // 0 seconds grace time, to calibrate actual time under the shower
#endif

typedef struct {
    bool wifi_connected;
    bool shower_timer; // true if we want to report back on shower times
    bool shower_alert; // true if we want the cold water reminder
} _Boiler_Status;

typedef struct {
    bool          showerOn;
    unsigned long timerStart;    // ms
    unsigned long timerPause;    // ms
    unsigned long duration;      // ms
    bool          doingColdShot; // true if we've just sent a jolt of cold water
} _Boiler_Shower;

// ESPHelper
netInfo   homeNet = {.mqttHost = MQTT_IP,
                   .mqttUser = MQTT_USER,
                   .mqttPass = MQTT_PASS,
                   .mqttPort = 1883, // this is the default, change if using another port
                   .ssid     = WIFI_SSID,
                   .pass     = WIFI_PASSWORD};
ESPHelper myESP(&homeNet);

command_t PROGMEM project_cmds[] = {

    {"l [n]", "set logging (0=none, 1=raw, 2=basic, 3=thermostat only, 4=verbose)"},
    {"s", "show statistics"},
    {"h", "list supported EMS telegram type IDs"},
    {"M", "publish to MQTT"},
    {"Q", "print Tx Queue"},
    {"P", "toggle EMS Poll response on/off"},
    {"X", "toggle EMS Tx transmission on/off"},
    {"S", "toggle Shower timer on/off"},
    {"A", "toggle shower Alert on/off"},
    {"r [s]", "send raw telegram to EMS (s=XX XX XX...)"},
    {"b [xx]", "send boiler read request (xx=telegram type ID in hex)"},
    {"t [xx]", "send thermostat read request (xx=telegram type ID in hex)"},
    {"w [nn]", "set boiler warm water temperature (min 30)"},
    {"a [n]", "set boiler warm tap water (0=off, 1=on)"},
    {"T [xx]", "set thermostat temperature"},
    {"m [n]", "set thermostat mode (1=manual, 2=auto)"}
    //{"U [c]", "do a thermostat scan on all ids (c=start id) for debugging only"}

};

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
int           boilerStatus     = false;
bool          startMQTTsent    = false;

uint8_t last_boilerActive = 0xFF; // for remembering last setting of the tap water or heating on/off

// toggle for heartbeat LED
bool heartbeatEnabled;

// logging messages with fixed strings (newline done automatically)
void myDebugLog(const char * s) {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("%s\n", s);
    }
}

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision = 1) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret = a;
    // check for 0x8000 (sensor missing)
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

// takes an int value at prints it to debug log
void _renderIntfractionalValue(const char * prefix, const char * postfix, uint8_t value, uint8_t decimals) {
    myDebug("  %s: ", prefix);
    char s[20];
    myDebug("%s.", _int_to_char(s, value/(decimals*10)));
    myDebug("%s", _int_to_char(s, value%(decimals*10)));
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
    myDebug("  System logging is set to ");
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug("Basic");
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug("Verbose");
    } else if (sysLog == EMS_SYS_LOGGING_THERMOSTAT) {
        myDebug("Thermostat only");
    } else {
        myDebug("None");
    }

    myDebug("\n  # EMS type handlers: %d\n", ems_getEmsTypesCount());

    myDebug("  Thermostat is %s, Poll is %s, Tx is %s, Shower Timer is %s, Shower Alert is %s\n",
            (ems_getThermostatEnabled() ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsPollEnabled) ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsTxEnabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_timer) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_alert) ? "enabled" : "disabled"));

    myDebug("  EMS Bus Stats: Connected=%s, # Rx telegrams=%d, # Tx telegrams=%d, # Crc Errors=%d\n",
            (ems_getBoilerEnabled() ? "yes" : "no"),
            EMS_Sys_Status.emsRxPgks,
            EMS_Sys_Status.emsTxPkgs,
            EMS_Sys_Status.emxCrcErr);

    myDebug("\n%sBoiler stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);

    // active stats
    myDebug("  Hot tap water is %s\n", (EMS_Boiler.tapwaterActive ? "running" : "off"));
    myDebug("  Central Heating is %s\n", (EMS_Boiler.heatingActive ? "active" : "off"));

    // UBAParameterWW
    _renderBoolValue("Warm Water activated", EMS_Boiler.wWActivated);
    _renderBoolValue("Warm Water circulation pump available", EMS_Boiler.wWCircPump);
    myDebug("  Warm Water is set to %s\n", (EMS_Boiler.wWComfort ? "Comfort" : "ECO"));
    _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
    _renderIntValue("Warm Water desired temperature", "C", EMS_Boiler.wWDesiredTemp);

    // UBAMonitorWWMessage
    _renderFloatValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
    _renderIntfractionalValue("Warm Water current tapwater flow", "l/min", EMS_Boiler.wWCurFlow, 1);
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
    myDebug("  Current System Service Code: %c%c \n", EMS_Boiler.serviceCodeChar1, EMS_Boiler.serviceCodeChar2);

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
    if (ems_getThermostatEnabled()) {
        myDebug("\n%sThermostat stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Thermostat type: ");
        ems_printThermostatType();
        myDebug("\n  Thermostat time is ");
        if (EMS_ID_THERMOSTAT != EMS_ID_THERMOSTAT_EASY) {
            myDebug("%02d:%02d:%02d %d/%d/%d\n",
                    EMS_Thermostat.hour,
                    EMS_Thermostat.minute,
                    EMS_Thermostat.second,
                    EMS_Thermostat.day,
                    EMS_Thermostat.month,
                    EMS_Thermostat.year + 2000);
        } else {
            myDebug("<not supported>\n");
        }

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
            myDebug("?\n");
            // myDebug("? (value is %d)\n", EMS_Thermostat.mode);
        }
    }

    // show the Shower Info
    if (Boiler_Status.shower_timer) {
        myDebug("\n%s Shower stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Shower Timer is %s\n", (Boiler_Shower.showerOn ? "active" : "off"));
    }

    myDebug("\n");
}

// send values to HA via MQTT
// a json object is created for the boiler and one for the thermostat
// CRC check is done to see if there are changes in the values since the last send to avoid too much wifi traffic
void publishValues(bool force) {
    char s[20]; // for formatting strings

    // Boiler values as one JSON object
    StaticJsonBuffer<512> jsonBuffer;
    char                  data[512];
    JsonObject &          rootBoiler = jsonBuffer.createObject();

    rootBoiler["wWSelTemp"]   = _int_to_char(s, EMS_Boiler.wWSelTemp);
    rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);
    sprintf(s, "%s",  (EMS_Boiler.wWComfort ? "Comfort" : "ECO"));
    rootBoiler["wWComfort"]   = s;
    rootBoiler["wWCurTmp"]    = _float_to_char(s, EMS_Boiler.wWCurTmp);
    
    sprintf(s, "%i.%i", EMS_Boiler.wWCurFlow/10, EMS_Boiler.wWCurFlow%10);
    rootBoiler["wWCurFlow"]   = s;
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
    sprintf(s, "%c%c", EMS_Boiler.serviceCodeChar1, EMS_Boiler.serviceCodeChar2);
    rootBoiler["ServiceCode"] = s;

    size_t len = rootBoiler.measureLength();
    rootBoiler.printTo(data, len + 1); // form the json string

    // calculate hash and send values if something has changed, to save unnecessary wifi traffic
    CRC32 crc;
    for (size_t i = 0; i < len - 1; i++) {
        crc.update(data[i]);
    }
    uint32_t checksum = crc.finalize();
    if ((previousBoilerPublishCRC != checksum) || force) {
        previousBoilerPublishCRC = checksum;
        if (ems_getLogging() >= EMS_SYS_LOGGING_BASIC) {
            myDebug("Publishing boiler data via MQTT\n");
        }

        // send values via MQTT
        myESP.publish(TOPIC_BOILER_DATA, data);
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    if ((last_boilerActive != ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive)) || force) {
        if (ems_getLogging() >= EMS_SYS_LOGGING_BASIC) {
            myDebug("Publishing hot water and heating state via MQTT\n");
        }
        myESP.publish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive == 1 ? "1" : "0");
        myESP.publish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive == 1 ? "1" : "0");

        last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
    }

    // handle the thermostat values separately
    if (ems_getThermostatEnabled()) {
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
            if (ems_getLogging() >= EMS_SYS_LOGGING_BASIC) {
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
    char *  cmd = myESP.consoleGetLastCommand();
    uint8_t len = strlen(cmd);
    bool    b;

    // look for single letter commands
    if (len == 1) {
        switch (cmd[0]) {
        case 's':
            showInfo();
            break;
        case 'P': // toggle Poll
            b = !ems_getPoll();
            ems_setPoll(b);
            break;
        case 'X': // toggle Tx
            b = !ems_getTxEnabled();
            ems_setTxEnabled(b);
            break;
        case 'M':
            //myESP.logger(LOG_HA, "Force publish values");
            publishValues(true);
            break;
        case 'h': // show type handlers
            ems_printAllTypes();
            break;
        case 'S': // toggle Shower timer support
            Boiler_Status.shower_timer = !Boiler_Status.shower_timer;
            myESP.publish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
            break;
        case 'A': // toggle Shower alert
            Boiler_Status.shower_alert = !Boiler_Status.shower_alert;
            myESP.publish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");
            break;
        case 'Q': //print Tx Queue
            ems_printTxQueue();
            break;
        default:
            myDebug("Unknown command. Use ? for help.\n");
            break;
        }
        return;
    }

    // for commands with parameters, assume command is just one letter
    switch (cmd[0]) {
    case 'T': // set thermostat temp
        ems_setThermostatTemp(strtof(&cmd[2], 0));
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
    case 'l': // logging
        ems_setLogging((_EMS_SYS_LOGGING)(cmd[2] - '0'));
        updateHeartbeat();
        break;
    case 'a': // set ww activate on or off
        if ((cmd[2] - '0') == 1)
            ems_setWarmTapWaterActivated(true);
        else if ((cmd[2] - '0') == 0)
            ems_setWarmTapWaterActivated(false);
        break;
    case 'b': // boiler read command
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16), EMS_ID_BOILER);
        break;
    case 't': // thermostat command
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16), EMS_ID_THERMOSTAT);
        break;
    case 'r': // send raw data
        ems_sendRawTelegram(&cmd[2]);
        break;
    case 'x': // experimental, not displayed!
        myDebug("Calling experimental...\n");
        ems_setLogging(EMS_SYS_LOGGING_VERBOSE);
        ems_setExperimental((uint8_t)strtol(&cmd[2], 0, 16)); // takes HEX param
        break;
    case 'U': // thermostat scan
        myDebug("Doing a type ID scan on thermostat...\n");
        ems_setLogging(EMS_SYS_LOGGING_THERMOSTAT);
        publishValuesTimer.detach();
        systemCheckTimer.detach();
        regularUpdatesTimer.detach();
        scanThermostat_count = (uint8_t)strtol(&cmd[2], 0, 16);
        scanThermostat.attach(SCANTHERMOSTAT_TIME, do_scanThermostat);
        break;
    default:
        myDebug("Unknown command. Use ? for help.\n");
        break;
    }
    return;
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
        _showerColdShotStart();
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

// Init callback, which is used to set functions and call methods when telnet has started
void InitCallback() {
    ems_setLogging(BOILER_DEFAULT_LOGGING); // turn off logging as default startup
}

// WifiCallback, called when a WiFi connect has successfully been established
void WIFIcallback() {
    Boiler_Status.wifi_connected = true;

#ifdef USE_LED
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
        digitalWrite(LED_HEARTBEAT, HIGH); // ...and turn off LED
#endif
    }
}

// Initialize the boiler settings
void initBoiler() {
    // default settings
    Boiler_Status.shower_timer = BOILER_SHOWER_TIMER;
    Boiler_Status.shower_alert = BOILER_SHOWER_ALERT;
    ems_setThermostatEnabled(BOILER_THERMOSTAT_ENABLED);

    // init boiler
    Boiler_Status.wifi_connected = false;

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
    pinMode(LED_HEARTBEAT, OUTPUT);
    digitalWrite(LED_HEARTBEAT, LOW);                 // onboard LED is on
    heartbeatTimer.attach(HEARTBEAT_TIME, heartbeat); // blink heartbeat LED
#endif

    // Timers using Ticker library
    publishValuesTimer.attach(PUBLISHVALUES_TIME, do_publishValues); // post HA values
    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck);       // check if Boiler is online
    regularUpdatesTimer.attach(REGULARUPDATES_TIME, regularUpdates); // regular reads from the EMS

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
    myESP.addSubscription(TOPIC_BOILER_TAPWATER_ACTIVE);
    myESP.addSubscription(TOPIC_BOILER_HEATING_ACTIVE);
    myESP.addSubscription(TOPIC_SHOWER_COLDSHOT);

    myESP.setInitCallback(InitCallback);

    myESP.consoleSetCallBackProjectCmds(project_cmds, ArraySize(project_cmds), myDebugCallback); // set up Telnet commands
    myESP.begin(HOSTNAME, APP_NAME, APP_VERSION); // start wifi and mqtt services

    // init ems statisitcs
    ems_init();

    // init Boiler specific parameters
    initBoiler();
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

// Thermostat scan
void do_scanThermostat() {
    //myDebug("Scanning %d..\n", scanThermostat_count);
    ems_doReadCommand(scanThermostat_count, EMS_ID_THERMOSTAT);
    scanThermostat_count++;
}

// do a healthcheck every now and then to see if we connections
void do_systemCheck() {
    // first do a system check to see if there is still a connection to the EMS
    if (!ems_getBoilerEnabled()) {
        myDebug("Error! Unable to connect to EMS bus. Please check connections. Retry in %d seconds...\n",
                SYSTEMCHECK_TIME);
    }
}

// EMS telegrams to send after startup
void firstTimeFetch() {
    ems_doReadCommand(EMS_TYPE_UBAMonitorFast, EMS_ID_BOILER); // get boiler stats which usually comes every 10 sec
    ems_doReadCommand(EMS_TYPE_UBAMonitorSlow, EMS_ID_BOILER); // get boiler stats which usually comes every 60 sec
    ems_doReadCommand(EMS_TYPE_UBAParameterWW, EMS_ID_BOILER); // get Warm Water values

    if (ems_getThermostatEnabled()) {
        ems_getThermostatValues();                             // get Thermostat temps (if supported)
        ems_doReadCommand(EMS_TYPE_RCTime, EMS_ID_THERMOSTAT); // get Thermostat time
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
void regularUpdates() {
    ems_doReadCommand(EMS_TYPE_UBAParameterWW, EMS_ID_BOILER); // get Warm Water values

    if (ems_getThermostatEnabled()) {
        ems_getThermostatValues(); // get Thermostat temps (if supported)
    }
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    myDebugLog("Shower: doing a shot of cold");
    ems_setWarmTapWaterActivated(false);
    Boiler_Shower.doingColdShot = true;
    // start the timer for n seconds which will reset the water back to hot
    showerColdShotStopTimer.attach(SHOWER_COLDSHOT_DURATION, _showerColdShotStop);
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (Boiler_Shower.doingColdShot) {
        myDebugLog("Shower: finished shot of cold. hot water back on");
        ems_setWarmTapWaterActivated(true);
        Boiler_Shower.doingColdShot = false;
        showerColdShotStopTimer.detach();
    }
}

/* 
 *  Shower Logic
 */
void showerCheck() {
    // if already in cold mode, ignore all this logic until we're out of the cold blast
    if (!Boiler_Shower.doingColdShot) {
        // is the hot water running?
        if (EMS_Boiler.tapwaterActive) {
            // if heater was previously off, start the timer
            if (Boiler_Shower.timerStart == 0) {
                // hot water just started...
                Boiler_Shower.timerStart    = timestamp;
                Boiler_Shower.timerPause    = 0; // remove any last pauses
                Boiler_Shower.doingColdShot = false;
                Boiler_Shower.duration      = 0;
                Boiler_Shower.showerOn      = false;
#ifdef SHOWER_TEST
                myDebugLog("Shower: hot water on...");
#endif
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!Boiler_Shower.showerOn && (timestamp - Boiler_Shower.timerStart) > SHOWER_MIN_DURATION) {
                    Boiler_Shower.showerOn = true;
#ifdef SHOWER_TEST

                    myDebugLog("Shower: hot water still running, starting shower timer");
#endif
                }
                // check if the shower has been on too long
                else if ((((timestamp - Boiler_Shower.timerStart) > SHOWER_MAX_DURATION) && !Boiler_Shower.doingColdShot)
                         && Boiler_Status.shower_alert) {
                    myESP.sendHACommand(TOPIC_SHOWER_ALARM);
#ifdef SHOWER_TEST
                    myDebugLog("Shower: exceeded max shower time");
#endif
                    _showerColdShotStart();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if ((Boiler_Shower.timerStart != 0) && (Boiler_Shower.timerPause == 0)) {
                Boiler_Shower.timerPause = timestamp;
#ifdef SHOWER_TEST
                myDebugLog("Shower: hot water turned off");
#endif
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
                    Boiler_Shower.duration = (Boiler_Shower.timerPause - Boiler_Shower.timerStart - SHOWER_OFFSET_TIME);
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

#ifdef SHOWER_TEST
                // reset everything
                myDebugLog("Shower: resetting timers");
#endif
                Boiler_Shower.timerStart = 0;
                Boiler_Shower.timerPause = 0;
                Boiler_Shower.showerOn   = false;
                _showerColdShotStop(); // turn hot water back on in case its off
            }
        }
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

    // if this is the first time we've connected to MQTT, send a welcome start message
    // which will send all the state values from HA back to the clock via MQTT and return the boottime
    if ((!startMQTTsent) && (connectionStatus == FULL_CONNECTION)) {
        myESP.sendStart();
        startMQTTsent = true;

        // publish to HA the status of the Shower parameters
        myESP.publish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
        myESP.publish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");
    }

    // if the EMS bus has just connected, send a request to fetch some initial values
    if (ems_getBoilerEnabled() && boilerStatus == false) {
        boilerStatus = true;
        firstTimeFetch();
    }

    // publish the values to MQTT, regardless if the values haven't changed
    if (ems_getEmsRefreshed()) {
        publishValues(true);
        ems_setEmsRefreshed(false);
    }

    // do shower logic if its enabled
    if (Boiler_Status.shower_timer) {
        showerCheck();
    }

    yield(); // yield to prevent watchdog from timing out
}
