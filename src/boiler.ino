/*
 * EMS-ESP-Boiler
 * Paul Derbyshire - May 2018 - https://github.com/proddy/EMS-ESP-Boiler
 * https://community.home-assistant.io/t/thermostat-and-boiler-controller-for-ems-based-boilers-nefit-buderus-bosch-using-esp/53382
 *
 * See README for Acknowledgments
 */

// local libraries
#include "ems.h"
#include "emsuart.h"
#include "my_config.h"
#include "version.h"

// my libraries
#include <MyESP.h>

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <CRC32.h>       // https://github.com/bakercp/CRC32

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

#define myDebug(...) myESP.myDebug(__VA_ARGS__)
#define myDebug_P(...) myESP.myDebug_P(__VA_ARGS__)

// timers, all values are in seconds
#define PUBLISHVALUES_TIME 120 // every 2 minutes post HA values
Ticker publishValuesTimer;

#define SYSTEMCHECK_TIME 10 // every 10 seconds check if Boiler is online and execute other requests
Ticker systemCheckTimer;

#define REGULARUPDATES_TIME 60 // every minute a call is made to fetch data from EMS devices manually
Ticker regularUpdatesTimer;

#define LEDCHECK_TIME 1 // every second blink heartbeat LED
Ticker ledcheckTimer;

// thermostat scan - for debugging
Ticker scanThermostat;
#define SCANTHERMOSTAT_TIME 1
uint8_t scanThermostat_count = 0;

Ticker showerColdShotStopTimer;

// thermostat
#define TOPIC_THERMOSTAT_DATA "thermostat_data"         // for sending thermostat values
#define TOPIC_THERMOSTAT_CMD_TEMP "thermostat_cmd_temp" // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CMD_MODE "thermostat_cmd_mode" // for received thermostat mode changes
#define THERMOSTAT_CURRTEMP "thermostat_currtemp"       // current temperature
#define THERMOSTAT_SELTEMP "thermostat_seltemp"         // selected temperature
#define THERMOSTAT_MODE "thermostat_mode"               // mode

// boiler
#define TOPIC_BOILER_DATA "boiler_data"                // for sending boiler values
#define TOPIC_BOILER_TAPWATER_ACTIVE "tapwater_active" // if hot tap water is running
#define TOPIC_BOILER_HEATING_ACTIVE "heating_active"   // if heating is on

// shower time
#define TOPIC_SHOWERTIME "showertime"           // for sending shower time results
#define TOPIC_SHOWER_TIMER "shower_timer"       // toggle switch for enabling the shower logic
#define TOPIC_SHOWER_ALERT "shower_alert"       // toggle switch for enabling the shower alarm logic
#define TOPIC_SHOWER_COLDSHOT "shower_coldshot" // used to trigger a coldshot from HA publish
#define SHOWER_ALARM "shower_alarm"             // for notifying HA that shower time has reached its limit

// shower settings for DEBUGGING only
#ifdef SHOWER_TEST
#undef SHOWER_PAUSE_TIME
#undef SHOWER_MIN_DURATION
#undef SHOWER_MAX_DURATION
#undef SHOWER_COLDSHOT_DURATION
#undef SHOWER_OFFSET_TIME
const unsigned long SHOWER_PAUSE_TIME        = 15000; // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION      = 20000; // 20 secs, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION      = 25000; // 25 secs, before trigger a shot of cold water
const unsigned long SHOWER_COLDSHOT_DURATION = 5;     // in seconds! how long for cold water shot
const unsigned long SHOWER_OFFSET_TIME       = 0;     // 0 seconds grace time, to calibrate actual time under the shower
#endif

typedef struct {
    bool shower_timer; // true if we want to report back on shower times
    bool shower_alert; // true if we want the alert of cold water
} _Boiler_Status;

typedef struct {
    bool          showerOn;
    unsigned long timerStart;    // ms
    unsigned long timerPause;    // ms
    unsigned long duration;      // ms
    bool          doingColdShot; // true if we've just sent a jolt of cold water
} _Boiler_Shower;

command_t PROGMEM project_cmds[] = {

    {"l [n]", "set logging (0=none, 1=raw, 2=basic, 3=thermostat only, 4=verbose)"},
    {"s", "show statistics"},
    {"D", "scan EMS connected Devices"},
    {"h", "list supported EMS telegram type IDs"},
    {"M", "publish to MQTT"},
    {"Q", "print Tx Queue"},
    {"U [n]", "do a deep scan of all thermostat message types, starting at n"},
    {"P", "toggle EMS Poll response on/off"},
    {"X", "toggle EMS Tx transmission on/off"},
    {"S", "toggle Shower timer on/off"},
    {"A", "toggle shower Alert on/off"},
    {"r [s]", "send raw telegram in hex to EMS (s=XX XX XX...)"},
    {"b [xx]", "send boiler read request (xx=telegram type ID in hex)"},
    {"t [xx]", "send thermostat read request (xx=telegram type ID in hex)"},
    {"w [n]", "set boiler warm water temperature (min 30)"},
    {"a [n]", "set boiler warm tap water (0=off, 1=on)"},
    {"T [n]", "set thermostat temperature"},
    {"m [n]", "set thermostat mode (0=low/night, 1=manual/day, 2=auto)"}

};

// store for overall system status
_Boiler_Status Boiler_Status;
_Boiler_Shower Boiler_Shower;

// CRC checks
uint32_t previousBoilerPublishCRC     = 0;
uint32_t previousThermostatPublishCRC = 0;

// Times
const unsigned long POLL_TIMEOUT_ERR = 10000; // if no signal from boiler for last 10 seconds, assume its offline
const unsigned long TX_HOLD_LED_TIME = 2000;  // how long to hold the Tx LED because its so quick

unsigned long timestamp; // for internal timings, via millis()

uint8_t last_boilerActive = 0xFF; // for remembering last setting of the tap water or heating on/off

// logging messages with fixed strings
void myDebugLog(const char * s) {
    if (ems_getLogging() >= EMS_SYS_LOGGING_BASIC) {
        myDebug(s);
    }
}

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision = 1) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret = a;
    // check for 0x8000 (sensor missing)
    if (f == EMS_VALUE_FLOAT_NOTSET) {
        strlcpy(ret, "?", sizeof(ret));
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
        strlcpy(s, "on", sizeof(s));
    } else if (value == EMS_VALUE_INT_OFF) {
        strlcpy(s, "off", sizeof(s));
    } else {
        strlcpy(s, "?", sizeof(s));
    }
    return s;
}

// convert int (single byte) to text value
char * _int_to_char(char * s, uint8_t value) {
    if (value == EMS_VALUE_INT_NOTSET) {
        strlcpy(s, "?", sizeof(s));
    } else {
        itoa(value, s, 10);
    }
    return s;
}

// takes a float value at prints it to debug log
void _renderFloatValue(const char * prefix, const char * postfix, float value) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));
    strlcat(buffer, _float_to_char(s, value), sizeof(buffer));

    if (postfix != NULL) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }
    myDebug(buffer);
}

// takes an int (single byte) value at prints it to debug log
void _renderIntValue(const char * prefix, const char * postfix, uint8_t value) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));
    strlcat(buffer, _int_to_char(s, value), sizeof(buffer));

    if (postfix != NULL) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }
    myDebug(buffer);
}

// takes an int value, converts to a fraction
void _renderIntfractionalValue(const char * prefix, const char * postfix, uint8_t value, uint8_t decimals) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));
    strlcat(buffer, _int_to_char(s, value / (decimals * 10)), sizeof(buffer));
    strlcat(buffer, ".", sizeof(buffer));
    strlcat(buffer, _int_to_char(s, value % (decimals * 10)), sizeof(buffer));

    if (postfix != NULL) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }
    myDebug(buffer);
}

// takes a long value at prints it to debug log
void _renderLongValue(const char * prefix, const char * postfix, uint32_t value) {
    char buffer[200] = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    if (value == EMS_VALUE_LONG_NOTSET) {
        strlcat(buffer, "?", sizeof(buffer));
    } else {
        char s[20] = {0};
        strlcat(buffer, ltoa(value, s, 10), sizeof(buffer));
    }

    if (postfix != NULL) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a bool value at prints it to debug log
void _renderBoolValue(const char * prefix, uint8_t value) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _bool_to_char(s, value), sizeof(buffer));

    myDebug(buffer);
}

// Show command - display stats on an 's' command
void showInfo() {
    // General stats from EMS bus

    myDebug("%sEMS-ESP-Boiler system stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug("  System logging set to Basic");
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug("  System logging set to Verbose");
    } else if (sysLog == EMS_SYS_LOGGING_THERMOSTAT) {
        myDebug("  System logging set to Thermostat only");
    } else {
        myDebug("  System logging set to None");
    }

    myDebug("  # EMS type handlers: %d", ems_getEmsTypesCount());

    myDebug("  Thermostat is %s, Boiler is %s, Poll is %s, Tx is %s, Shower Timer is %s, Shower Alert is %s",
            (ems_getThermostatEnabled() ? "enabled" : "disabled"),
            (ems_getBoilerEnabled() ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsPollEnabled) ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsTxEnabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_timer) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_alert) ? "enabled" : "disabled"));

    myDebug("  EMS Bus Stats: Connected=%s, # Rx telegrams=%d, # Tx telegrams=%d, # Crc Errors=%d",
            (ems_getBusConnected() ? "yes" : "no"),
            EMS_Sys_Status.emsRxPgks,
            EMS_Sys_Status.emsTxPkgs,
            EMS_Sys_Status.emxCrcErr);

    myDebug("");

    myDebug("%sBoiler stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);

    // version details
    char buffer_type[64];
    myDebug("  Boiler type: %s", ems_getBoilerType(buffer_type));

    // active stats
    myDebug("  Hot tap water is %s", (EMS_Boiler.tapwaterActive ? "running" : "off"));
    myDebug("  Central Heating is %s", (EMS_Boiler.heatingActive ? "active" : "off"));

    // UBAParameterWW
    _renderBoolValue("Warm Water activated", EMS_Boiler.wWActivated);
    _renderBoolValue("Warm Water circulation pump available", EMS_Boiler.wWCircPump);
    myDebug("  Warm Water is set to %s", (EMS_Boiler.wWComfort ? "Comfort" : "ECO"));
    _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
    _renderIntValue("Warm Water desired temperature", "C", EMS_Boiler.wWDesiredTemp);

    // UBAMonitorWWMessage
    _renderFloatValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
    _renderIntfractionalValue("Warm Water current tap water flow", "l/min", EMS_Boiler.wWCurFlow, 1);
    _renderLongValue("Warm Water # starts", "times", EMS_Boiler.wWStarts);
    if (EMS_Boiler.wWWorkM != EMS_VALUE_LONG_NOTSET) {
        myDebug("  Warm Water active time: %d days %d hours %d minutes",
                EMS_Boiler.wWWorkM / 1440,
                (EMS_Boiler.wWWorkM % 1440) / 60,
                EMS_Boiler.wWWorkM % 60);
    }
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
    myDebug("  Current System Service Code: %s", EMS_Boiler.serviceCodeChar);

    // UBAMonitorSlow
    _renderFloatValue("Outside temperature", "C", EMS_Boiler.extTemp);
    _renderFloatValue("Boiler temperature", "C", EMS_Boiler.boilTemp);
    _renderIntValue("Pump modulation", "%", EMS_Boiler.pumpMod);
    _renderLongValue("Burner # restarts", "times", EMS_Boiler.burnStarts);
    if (EMS_Boiler.burnWorkMin != EMS_VALUE_LONG_NOTSET) {
        myDebug("  Total burner operating time: %d days %d hours %d minutes",
                EMS_Boiler.burnWorkMin / 1440,
                (EMS_Boiler.burnWorkMin % 1440) / 60,
                EMS_Boiler.burnWorkMin % 60);
    }
    if (EMS_Boiler.heatWorkMin != EMS_VALUE_LONG_NOTSET) {
        myDebug("  Total heat operating time: %d days %d hours %d minutes",
                EMS_Boiler.heatWorkMin / 1440,
                (EMS_Boiler.heatWorkMin % 1440) / 60,
                EMS_Boiler.heatWorkMin % 60);
    }
    if (EMS_Boiler.UBAuptime != EMS_VALUE_LONG_NOTSET) {
        myDebug("  Total UBA working time: %d days %d hours %d minutes",
                EMS_Boiler.UBAuptime / 1440,
                (EMS_Boiler.UBAuptime % 1440) / 60,
                EMS_Boiler.UBAuptime % 60);
    }

    myDebug(""); // newline

    // Thermostat stats
    if (ems_getThermostatEnabled()) {
        myDebug("%sThermostat stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Thermostat type: %s", ems_getThermostatType(buffer_type));
        _renderFloatValue("Setpoint room temperature", "C", EMS_Thermostat.setpoint_roomTemp);
        _renderFloatValue("Current room temperature", "C", EMS_Thermostat.curr_roomTemp);
        if (ems_getThermostatModel() != EMS_MODEL_EASY) {
            myDebug("  Thermostat time is %02d:%02d:%02d %d/%d/%d",
                    EMS_Thermostat.hour,
                    EMS_Thermostat.minute,
                    EMS_Thermostat.second,
                    EMS_Thermostat.day,
                    EMS_Thermostat.month,
                    EMS_Thermostat.year + 2000);

            if (EMS_Thermostat.mode == 0) {
                myDebug("  Mode is set to low");
            } else if (EMS_Thermostat.mode == 1) {
                myDebug("  Mode is set to manual");
            } else if (EMS_Thermostat.mode == 2) {
                myDebug("  Mode is set to auto");
            } else {
                myDebug("  Mode is set to ?");
            }
        }
    }

    myDebug(""); // newline

    // show the Shower Info
    if (Boiler_Status.shower_timer) {
        myDebug("%s Shower stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Shower Timer is %s", (Boiler_Shower.showerOn ? "active" : "off"));
    }
}

// send values to HA via MQTT
// a json object is created for the boiler and one for the thermostat
// CRC check is done to see if there are changes in the values since the last send to avoid too much wifi traffic
void publishValues(bool force) {
    char                            s[20] = {0}; // for formatting strings
    StaticJsonBuffer<MQTT_MAX_SIZE> jsonBuffer;
    char                            data[MQTT_MAX_SIZE] = {0};
    JsonObject &                    rootBoiler          = jsonBuffer.createObject();
    size_t                          rlen;
    CRC32                           crc;
    uint32_t                        fchecksum;

    rootBoiler["wWSelTemp"]   = _int_to_char(s, EMS_Boiler.wWSelTemp);
    rootBoiler["selFlowTemp"] = _float_to_char(s, EMS_Boiler.selFlowTemp);
    rootBoiler["outdoorTemp"] = _float_to_char(s, EMS_Boiler.extTemp);
    rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);
    rootBoiler["wWComfort"]   = EMS_Boiler.wWComfort ? "Comfort" : "ECO";
    rootBoiler["wWCurTmp"]    = _float_to_char(s, EMS_Boiler.wWCurTmp);
    snprintf(s, sizeof(s), "%i.%i", EMS_Boiler.wWCurFlow / 10, EMS_Boiler.wWCurFlow % 10);
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
    rootBoiler["ServiceCode"] = EMS_Boiler.serviceCodeChar;

    rlen = rootBoiler.measureLength();
    rootBoiler.printTo(data, rlen + 1); // form the json string

    // calculate hash and send values if something has changed, to save unnecessary wifi traffic
    for (size_t i = 0; i < rlen - 1; i++) {
        crc.update(data[i]);
    }
    fchecksum = crc.finalize();
    if ((previousBoilerPublishCRC != fchecksum) || force) {
        previousBoilerPublishCRC = fchecksum;
        myDebugLog("Publishing boiler data via MQTT");

        // send values via MQTT
        myESP.mqttPublish(TOPIC_BOILER_DATA, data);
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    if ((last_boilerActive != ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive)) || force) {
        myDebugLog("Publishing hot water and heating state via MQTT");
        myESP.mqttPublish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive == 1 ? "1" : "0");
        myESP.mqttPublish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive == 1 ? "1" : "0");

        last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
    }

    // handle the thermostat values separately
    if (ems_getThermostatEnabled()) {
        // only send thermostat values if we actually have them
        if (((int)EMS_Thermostat.curr_roomTemp == (int)0) || ((int)EMS_Thermostat.setpoint_roomTemp == (int)0))
            return;

        // build json object
        JsonObject & rootThermostat         = jsonBuffer.createObject();
        rootThermostat[THERMOSTAT_CURRTEMP] = _float_to_char(s, EMS_Thermostat.curr_roomTemp);
        rootThermostat[THERMOSTAT_SELTEMP]  = _float_to_char(s, EMS_Thermostat.setpoint_roomTemp);

        // RC20 has different mode settings
        if (ems_getThermostatModel() == EMS_MODEL_RC20) {
            if (EMS_Thermostat.mode == 0) {
                rootThermostat[THERMOSTAT_MODE] = "low";
            } else if (EMS_Thermostat.mode == 1) {
                rootThermostat[THERMOSTAT_MODE] = "manual";
            } else {
                rootThermostat[THERMOSTAT_MODE] = "auto";
            }
        } else {
            if (EMS_Thermostat.mode == 0) {
                rootThermostat[THERMOSTAT_MODE] = "night";
            } else if (EMS_Thermostat.mode == 1) {
                rootThermostat[THERMOSTAT_MODE] = "day";
            } else {
                rootThermostat[THERMOSTAT_MODE] = "auto";
            }
        }

        data[0] = '\0'; // reset data for next package
        rlen    = rootThermostat.measureLength();
        rootThermostat.printTo(data, rlen + 1); // form the json string

        // calculate new CRC
        crc.reset();
        for (size_t i = 0; i < rlen - 1; i++) {
            crc.update(data[i]);
        }
        uint32_t checksum = crc.finalize();
        if ((previousThermostatPublishCRC != checksum) || force) {
            previousThermostatPublishCRC = checksum;
            myDebugLog("Publishing thermostat data via MQTT");

            // send values via MQTT
            myESP.mqttPublish(TOPIC_THERMOSTAT_DATA, data);
        }
    }
}

// sets the shower timer on/off
void set_showerTimer() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower timer is %s", Boiler_Status.shower_timer ? "enabled" : "disabled");
    }
}

// sets the shower alert on/off
void set_showerAlert() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower alert is %s", Boiler_Status.shower_alert ? "enabled" : "disabled");
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
            publishValues(true);
            break;
        case 'h': // show type handlers
            ems_printAllTypes();
            break;
        case 'S': // toggle Shower timer support
            Boiler_Status.shower_timer = !Boiler_Status.shower_timer;
            myESP.mqttPublish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
            break;
        case 'A': // toggle Shower alert
            Boiler_Status.shower_alert = !Boiler_Status.shower_alert;
            myESP.mqttPublish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");
            break;
        case 'Q': // print Tx Queue
            ems_printTxQueue();
            break;
        case 'D': // Auto detect EMS devices
            ems_scanDevices();
            break;
        default:
            myDebug("Unknown command. Use ? for help.");
            break;
        }
        return;
    }

    // for commands with parameters, assume command is just a single letter followed by a space
    switch (cmd[0]) {
    case 'T': // set thermostat temp
        ems_setThermostatTemp(strtof(&cmd[2], 0));
        break;
    case 'm': // set thermostat mode
        ems_setThermostatMode(cmd[2] - '0');
        break;
    case 'w': // set warm water temp
        ems_setWarmWaterTemp((uint8_t)strtol(&cmd[2], 0, 10));
        break;
    case 'l': // logging
        ems_setLogging((_EMS_SYS_LOGGING)(cmd[2] - '0'));
        break;
    case 'a': // set ww activate on or off
        if ((cmd[2] - '0') == 1)
            ems_setWarmTapWaterActivated(true);
        else if ((cmd[2] - '0') == 0)
            ems_setWarmTapWaterActivated(false);
        break;
    case 'b': // boiler read command
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16), EMS_Boiler.type_id);
        break;
    case 't': // thermostat command
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16), EMS_Thermostat.type_id);
        break;
    case 'r': // send raw data
        ems_sendRawTelegram(&cmd[2]);
        break;
    case 'x': // experimental, not displayed!
        myDebug("Calling experimental...");
        ems_setLogging(EMS_SYS_LOGGING_VERBOSE);
        ems_setExperimental((uint8_t)strtol(&cmd[2], 0, 16)); // takes HEX param
        break;
    case 'U': // thermostat scan
        ems_setLogging(EMS_SYS_LOGGING_THERMOSTAT);
        publishValuesTimer.detach();
        systemCheckTimer.detach();
        regularUpdatesTimer.detach();
        scanThermostat_count = (uint8_t)strtol(&cmd[2], 0, 16);
        myDebug("Starting a deep message scan on thermometer");
        scanThermostat.attach(SCANTHERMOSTAT_TIME, do_scanThermostat);
        break;
    default:
        myDebug("Unknown command. Use ? for help.");
        break;
    }
    return;
}

// MQTT Callback to handle incoming/outgoing changes
void MQTTcallback(unsigned int type, const char * topic, const char * message) {
    // we're connected. lets subscribe to some topics
    if (type == MQTT_CONNECT_EVENT) {
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_TEMP);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_MODE);
        myESP.mqttSubscribe(TOPIC_SHOWER_TIMER);
        myESP.mqttSubscribe(TOPIC_SHOWER_ALERT);
        myESP.mqttSubscribe(TOPIC_SHOWER_COLDSHOT);

        // publish to HA the status of the Shower parameters
        myESP.mqttPublish(TOPIC_SHOWER_TIMER, Boiler_Status.shower_timer ? "1" : "0");
        myESP.mqttPublish(TOPIC_SHOWER_ALERT, Boiler_Status.shower_alert ? "1" : "0");
    }

    if (type == MQTT_MESSAGE_EVENT) {
        // thermostat temp changes
        if (strcmp(topic, TOPIC_THERMOSTAT_CMD_TEMP) == 0) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug("MQTT topic: thermostat temperature value %s", _float_to_char(s, f));
            ems_setThermostatTemp(f);
            publishValues(true); // publish back so HA is immediately updated
        }

        // thermostat mode changes
        if (strcmp(topic, TOPIC_THERMOSTAT_CMD_MODE) == 0) {
            myDebug("MQTT topic: thermostat mode value %s", message);
            if (strcmp((char *)message, "auto") == 0) {
                ems_setThermostatMode(2);
            } else if (strcmp((char *)message, "manual") == 0) {
                ems_setThermostatMode(1);
            }
        }

        // shower timer
        if (strcmp(topic, TOPIC_SHOWER_TIMER) == 0) {
            if (message[0] == '1') {
                Boiler_Status.shower_timer = true;
            } else if (message[0] == '0') {
                Boiler_Status.shower_timer = false;
            }
            set_showerTimer();
        }

        // shower alert
        if (strcmp(topic, TOPIC_SHOWER_ALERT) == 0) {
            if (message[0] == '1') {
                Boiler_Status.shower_alert = true;
            } else if (message[0] == '0') {
                Boiler_Status.shower_alert = false;
            }
            set_showerAlert();
        }

        // shower cold shot
        if (strcmp(topic, TOPIC_SHOWER_COLDSHOT) == 0) {
            _showerColdShotStart();
        }
    }
}

// Init callback, which is used to set functions and call methods after a wifi connection has been established
void WIFICallback() {
// when finally we're all set up, we can fire up the uart (this will enable the UART interrupts)
#ifdef DEBUG_SUPPORT
    myDebug("Warning, in DEBUG mode. EMS bus has been disabled. See -DDEBUG_SUPPORT build option.");
#else
    // Important! This is where we enable the UART service to scan the incoming serial Tx/Rx bus signals
    // This is done after we have a WiFi signal to avoid any resource conflicts
    emsuart_init();
    myDebug("[UART] Opened Rx/Tx connection");
#endif

    // now that we're connected, check to see if we boiler and thermostat set
    // otherwise this will initiate a self scan
    ems_setModels();
}

// Initialize the boiler settings
void initShower() {
    // default showr settings
    Boiler_Status.shower_timer  = BOILER_SHOWER_TIMER;
    Boiler_Status.shower_alert  = BOILER_SHOWER_ALERT;
    Boiler_Shower.timerStart    = 0;
    Boiler_Shower.timerPause    = 0;
    Boiler_Shower.duration      = 0;
    Boiler_Shower.doingColdShot = false;
}

// call PublishValues without forcing, so using CRC to see if we really need to publish
void do_publishValues() {
    publishValues(false);
}

// callback to light up the LED, called via Ticker every second
void do_ledcheck() {
#ifndef NO_LED
    if (ems_getBusConnected()) {
        digitalWrite(BOILER_LED, (BOILER_LED == LED_BUILTIN) ? LOW : HIGH); // light on. For onboard LED high=off
    } else {
        int state = digitalRead(BOILER_LED);
        digitalWrite(BOILER_LED, !state);
    }
    //WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + (state ? 4 : 8), (1 << BOILER_LED)); // 4 is on, 8 is off
#endif
}

// Thermostat scan
void do_scanThermostat() {
    myDebug("> Scanning thermostat message type #0x%02X..", scanThermostat_count);
    ems_doReadCommand(scanThermostat_count, EMS_Thermostat.type_id);
    scanThermostat_count++;
}

// do a system health check every now and then to see if we all connections
void do_systemCheck() {
    if (!ems_getBusConnected()) {
        myDebug("Error! Unable to connect to EMS bus. Please make sure you're not in DEBUG_SUPPORT mode. Retrying in %d seconds...",
                SYSTEMCHECK_TIME);
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
void do_regularUpdates() {
    myDebugLog("Calling scheduled data refresh from EMS devices..");
    ems_getThermostatValues(); // get Thermostat temps (if supported)
    ems_getBoilerValues();
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    myDebugLog("[Shower] doing a shot of cold water");
    ems_setWarmTapWaterActivated(false);
    Boiler_Shower.doingColdShot = true;
    // start the timer for n seconds which will reset the water back to hot
    showerColdShotStopTimer.attach(SHOWER_COLDSHOT_DURATION, _showerColdShotStop);
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (Boiler_Shower.doingColdShot) {
        myDebugLog("[Shower] finished shot of cold. hot water back on");
        ems_setWarmTapWaterActivated(true);
        Boiler_Shower.doingColdShot = false;
        showerColdShotStopTimer.detach(); // disable the timer
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
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!Boiler_Shower.showerOn && (timestamp - Boiler_Shower.timerStart) > SHOWER_MIN_DURATION) {
                    Boiler_Shower.showerOn = true;
                    myDebugLog("[Shower] hot water still running, starting shower timer");
                }
                // check if the shower has been on too long
                else if ((((timestamp - Boiler_Shower.timerStart) > SHOWER_MAX_DURATION) && !Boiler_Shower.doingColdShot)
                         && Boiler_Status.shower_alert) {
                    myESP.sendHACommand(SHOWER_ALARM);
                    myDebugLog("[Shower] exceeded max shower time");
                    _showerColdShotStart();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if ((Boiler_Shower.timerStart != 0) && (Boiler_Shower.timerPause == 0)) {
                Boiler_Shower.timerPause = timestamp;
            }

            // if shower has been off for longer than the wait time
            if ((Boiler_Shower.timerPause != 0) && ((timestamp - Boiler_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
                // it is over the wait period, so assume that the shower has finished and calculate the total time and publish
                // because its unsigned long, can't have negative so check if length is less than OFFSET_TIME
                if ((Boiler_Shower.timerPause - Boiler_Shower.timerStart) > SHOWER_OFFSET_TIME) {
                    Boiler_Shower.duration = (Boiler_Shower.timerPause - Boiler_Shower.timerStart - SHOWER_OFFSET_TIME);
                    if (Boiler_Shower.duration > SHOWER_MIN_DURATION) {
                        char s[50]      = {0};
                        char buffer[16] = {0};
                        strlcpy(s, itoa((uint8_t)((Boiler_Shower.duration / (1000 * 60)) % 60), buffer, 10), sizeof(s));
                        strlcat(s, " minutes and ", sizeof(s));
                        strlcat(s, itoa((uint8_t)((Boiler_Shower.duration / 1000) % 60), buffer, 10), sizeof(s));
                        strlcat(s, " seconds", sizeof(s));
                        if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                            myDebug("[Shower] finished with duration %s", s);
                        }
                        myESP.mqttPublish(TOPIC_SHOWERTIME, s); // publish to HA
                    }
                }

                // reset everything
                Boiler_Shower.timerStart = 0;
                Boiler_Shower.timerPause = 0;
                Boiler_Shower.showerOn   = false;
                _showerColdShotStop(); // turn hot water back on in case its off
            }
        }
    }
}

//
// SETUP
//
void setup() {
#ifndef NO_LED
    // set pin for LED
    pinMode(BOILER_LED, OUTPUT);
    digitalWrite(BOILER_LED, (BOILER_LED == LED_BUILTIN) ? HIGH : LOW); // light on. For onboard high=off
    ledcheckTimer.attach(LEDCHECK_TIME, do_ledcheck);                   // blink heartbeat LED
#endif

    // Timers using Ticker library
    publishValuesTimer.attach(PUBLISHVALUES_TIME, do_publishValues);    // post HA values
    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck);          // check if Boiler is online
    regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates); // regular reads from the EMS

    // set up myESP for Wifi, MQTT, MDNS and Telnet
    // with callbacks
    myESP.setup(APP_HOSTNAME, APP_NAME, APP_VERSION, WIFI_SSID, WIFI_PASSWORD, MQTT_IP, MQTT_USER, MQTT_PASS);
    myESP.consoleSetCallBackProjectCmds(project_cmds, ArraySize(project_cmds), myDebugCallback); // set up Telnet commands
    myESP.setWIFICallback(WIFICallback);
    myESP.setMQTTCallback(MQTTcallback);

    // init Shower specific parameters
    initShower();

    ems_setLogging(BOILER_DEFAULT_LOGGING); // set default logging. see my_config.h

    // init the EMS bus
    // call ems.cpp's init function to set all the internal params
    ems_init(MY_BOILER_MODELID, MY_THERMOSTAT_MODELID);
}

//
// Main loop
//
void loop() {
    timestamp = millis();

    // the main loop
    myESP.loop();

    // publish the values to MQTT, regardless if the values haven't changed
    // we don't want to publish when doing a deep scan of the thermostat
    if (ems_getEmsRefreshed() && (scanThermostat_count == 0)) {
        publishValues(true);
        ems_setEmsRefreshed(false);
    }

    // do shower logic if it is enabled
    if (Boiler_Status.shower_timer) {
        showerCheck();
    }
}
