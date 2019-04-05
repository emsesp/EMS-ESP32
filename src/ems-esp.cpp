/*
 * EMS-ESP
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 * See ChangeLog.md for history
 * See README.md for Acknowledgments
 */

// local libraries
#include "ds18.h"
#include "ems.h"
#include "ems_devices.h"
#include "emsuart.h"
#include "my_config.h"
#include "version.h"

// Dallas external temp sensors
DS18 ds18;

// shared libraries
#include <MyESP.h>

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <CRC32.h>       // https://github.com/bakercp/CRC32

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

#define myDebug(...) myESP.myDebug(__VA_ARGS__)
#define myDebug_P(...) myESP.myDebug_P(__VA_ARGS__)

// set to value >0 if the ESP is overheating or there are timing issues. Recommend a value of 1.
#define EMSESP_DELAY 1 // initially set to 0 for no delay

// timers, all values are in seconds
#define DEFAULT_PUBLISHWAIT 120 // every 2 minutes publish MQTT values, including Dallas sensors
Ticker publishValuesTimer;
Ticker publishSensorValuesTimer;

#define SYSTEMCHECK_TIME 20 // every 20 seconds check if Boiler is online
Ticker systemCheckTimer;

#define REGULARUPDATES_TIME 60 // every minute a call is made to fetch data from EMS devices manually
Ticker regularUpdatesTimer;

#define LEDCHECK_TIME 500 // every 1/2 second blink the heartbeat LED
Ticker ledcheckTimer;

// thermostat scan - for debugging
Ticker scanThermostat;
#define SCANTHERMOSTAT_TIME 1
uint8_t scanThermostat_count = 0;

Ticker showerColdShotStopTimer;

// if using the shower timer, change these settings
#define SHOWER_PAUSE_TIME 15000     // in ms. 15 seconds, max time if water is switched off & on during a shower
#define SHOWER_MIN_DURATION 120000  // in ms. 2 minutes, before recognizing its a shower
#define SHOWER_OFFSET_TIME 5000     // in ms. 5 seconds grace time, to calibrate actual time under the shower
#define SHOWER_COLDSHOT_DURATION 10 // in seconds. 10 seconds for cold water before turning back hot water
#define SHOWER_MAX_DURATION 420000  // in ms. 7 minutes, before trigger a shot of cold water

typedef struct {
    uint32_t timestamp;      // for internal timings, via millis()
    uint8_t  dallas_sensors; // count of dallas sensors

    // custom params
    bool     shower_timer; // true if we want to report back on shower times
    bool     shower_alert; // true if we want the alert of cold water
    bool     led;          // LED on/off
    bool     silent_mode;  // stop automatic Tx on/off
    uint16_t publish_wait; // frequency of MQTT publish in seconds
    uint8_t  led_gpio;
    uint8_t  dallas_gpio;
    uint8_t  dallas_parasite;
} _EMSESP_Status;

typedef struct {
    bool     showerOn;
    uint32_t timerStart;    // ms
    uint32_t timerPause;    // ms
    uint32_t duration;      // ms
    bool     doingColdShot; // true if we've just sent a jolt of cold water
} _EMSESP_Shower;

command_t PROGMEM project_cmds[] = {

    {true, "led <on | off>", "toggle status LED on/off"},
    {true, "led_gpio <gpio>", "set the LED pin. Default is the onboard LED (D1=5)"},
    {true, "dallas_gpio <gpio>", "set the pin for external Dallas temperature sensors (D5=14)"},
    {true, "dallas_parasite <on | off>", "set to on if powering Dallas via parasite"},
    {true, "thermostat_type <type ID>", "set the thermostat type id (e.g. 10 for 0x10)"},
    {true, "boiler_type <type ID>", "set the boiler type id (e.g. 8 for 0x08)"},
    {true, "silent_mode <on | off>", "when on all automatic Tx is disabled"},
    {true, "shower_timer <on | off>", "notify via MQTT all shower durations"},
    {true, "shower_alert <on | off>", "send a warning of cold water after shower time is exceeded"},
    {true, "publish_wait <seconds>", "set frequency for publishing to MQTT"},

    {false, "info", "show data captured on the EMS bus"},
    {false, "log <n | b | t | r | v>", "set logging mode to none, basic, thermostat only, raw or verbose"},
    {false, "publish", "publish all values to MQTT"},
    {false, "refresh", "fetch values from the EMS devices"},
    {false, "types", "list supported EMS telegram type IDs"},
    {false, "queue", "show current Tx queue"},
    {false, "autodetect", "detect EMS devices and attempt to automatically set boiler and thermostat types"},
    {false, "shower <timer | alert>", "toggle either timer or alert on/off"},
    {false, "send XX ...", "send raw telegram data as hex to EMS bus"},
    {false, "thermostat read <type ID>", "send read request to the thermostat"},
    {false, "thermostat temp <degrees>", "set current thermostat temperature"},
    {false, "thermostat mode <mode>", "set mode (0=low/night, 1=manual/day, 2=auto)"},
    {false, "thermostat scan <type ID>", "do a read on all type IDs"},
    {false, "boiler read <type ID>", "send read request to boiler"},
    {false, "boiler wwtemp <degrees>", "set boiler warm water temperature"},
    {false, "boiler tapwater <on | off>", "set boiler warm tap water on/off"},
    {false, "boiler comfort <hot | eco | intelligent>", "set boiler warm water comfort setting"}

};

// store for overall system status
_EMSESP_Status EMSESP_Status;
_EMSESP_Shower EMSESP_Shower;

// logging messages with fixed strings
void myDebugLog(const char * s) {
    if (ems_getLogging() >= EMS_SYS_LOGGING_BASIC) {
        myDebug(s);
    }
}

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision = 2) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret = a;
    // check for 0x8000 (sensor missing)
    if (f == EMS_VALUE_SHORT_NOTSET) {
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

// convert short (two bytes) to text value
// negative values are assumed stored as 1-compliment (https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c)
char * _short_to_char(char * s, int16_t value, uint8_t decimals = 1) {
    // remove errors on invalid values
    if (abs(value) >= EMS_VALUE_SHORT_NOTSET) {
        strlcpy(s, "?", sizeof(s));
        return (s);
    }

    if (decimals == 0) {
        itoa(value, s, 10);
        return (s);
    }

    // floating point
    char s2[5] = {0};
    // check for negative values
    if (value < 0) {
        strlcpy(s, "-", 2);
        value = abs(value);
    }
    strlcpy(s, itoa(value / (decimals * 10), s2, 10), 5);
    strlcat(s, ".", sizeof(s));
    strlcat(s, itoa(value % (decimals * 10), s2, 10), 5);

    return s;
}

// takes a short value (2 bytes), converts to a fraction
// most values stored a s short are either *10 or *100
void _renderShortValue(const char * prefix, const char * postfix, int16_t value, uint8_t decimals = 1) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _short_to_char(s, value, decimals), sizeof(buffer));

    if (postfix != NULL) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// convert int (single byte) to text value
char * _int_to_char(char * s, uint8_t value, uint8_t div = 1) {
    if (value == EMS_VALUE_INT_NOTSET) {
        strlcpy(s, "?", sizeof(s));
        return (s);
    }

    char s2[5] = {0};

    switch (div) {
    case 1:
        itoa(value, s, 10);
        break;

    case 2:
        strlcpy(s, itoa(value >> 1, s2, 10), 5);
        strlcat(s, ".", sizeof(s));
        strlcat(s, ((value & 0x01) ? "5" : "0"), 5);
        break;

    case 10:
        strlcpy(s, itoa(value / 10, s2, 10), 5);
        strlcat(s, ".", sizeof(s));
        strlcat(s, itoa(value % 10, s2, 10), 5);
        break;

    default:
        itoa(value, s, 10);
        break;
    }

    return s;
}

// takes an int value (1 byte), converts to a fraction
void _renderIntValue(const char * prefix, const char * postfix, uint8_t value, uint8_t div = 1) {
    char buffer[200] = {0};
    char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _int_to_char(s, value, div), sizeof(buffer));

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

    char buffer_type[128] = {0};

    myDebug("%sEMS-ESP system stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
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

    myDebug("  LED is %s, Silent mode is %s", EMSESP_Status.led ? "on" : "off", EMSESP_Status.silent_mode ? "on" : "off");
    if (EMSESP_Status.dallas_sensors > 0) {
        myDebug("  %d external temperature sensor%s found", EMSESP_Status.dallas_sensors, (EMSESP_Status.dallas_sensors == 1) ? "" : "s");
    }

    myDebug("  Thermostat is %s, Boiler is %s, Shower Timer is %s, Shower Alert is %s",
            (ems_getThermostatEnabled() ? "enabled" : "disabled"),
            (ems_getBoilerEnabled() ? "enabled" : "disabled"),
            ((EMSESP_Status.shower_timer) ? "enabled" : "disabled"),
            ((EMSESP_Status.shower_alert) ? "enabled" : "disabled"));

    myDebug("\n%sEMS Bus stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);

    if (ems_getBusConnected()) {
        myDebug("  Bus is connected");

        myDebug("  Rx: Poll=%d ms, # Rx telegrams read=%d, # Crc Errors=%d",
                ems_getPollFrequency(),
                EMS_Sys_Status.emsRxPgks,
                EMS_Sys_Status.emxCrcErr);

        if (ems_getTxCapable()) {
            myDebug("  Tx: available, # Tx telegrams sent=%d", EMS_Sys_Status.emsTxPkgs);
        } else {
            myDebug("  Tx: no signal");
        }
    } else {
        myDebug("  No connection can be made to the EMS bus");
    }

    myDebug("");
    myDebug("%sBoiler stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);

    // version details
    myDebug("  Boiler type: %s", ems_getBoilerDescription(buffer_type));

    // active stats
    if (ems_getBusConnected()) {
        if (EMS_Boiler.tapwaterActive != EMS_VALUE_INT_NOTSET) {
            myDebug("  Hot tap water: %s", EMS_Boiler.tapwaterActive ? "running" : "off");
        }

        if (EMS_Boiler.heatingActive != EMS_VALUE_INT_NOTSET) {
            myDebug("  Central heating: %s", EMS_Boiler.heatingActive ? "active" : "off");
        }
    }

    // UBAParameterWW
    _renderBoolValue("Warm Water activated", EMS_Boiler.wWActivated);
    _renderBoolValue("Warm Water circulation pump available", EMS_Boiler.wWCircPump);
    if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
        myDebug("  Warm Water comfort setting: Hot");
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
        myDebug("  Warm Water comfort setting: Eco");
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
        myDebug("  Warm Water comfort setting: Intelligent");
    }

    _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
    _renderIntValue("Warm Water desired temperature", "C", EMS_Boiler.wWDesiredTemp);

    // UBAMonitorWWMessage
    _renderShortValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
    _renderIntValue("Warm Water current tap water flow", "l/min", EMS_Boiler.wWCurFlow, 10);
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
    _renderShortValue("Current flow temperature", "C", EMS_Boiler.curFlowTemp);
    _renderShortValue("Return temperature", "C", EMS_Boiler.retTemp);
    _renderBoolValue("Gas", EMS_Boiler.burnGas);
    _renderBoolValue("Boiler pump", EMS_Boiler.heatPmp);
    _renderBoolValue("Fan", EMS_Boiler.fanWork);
    _renderBoolValue("Ignition", EMS_Boiler.ignWork);
    _renderBoolValue("Circulation pump", EMS_Boiler.wWCirc);
    _renderIntValue("Burner selected max power", "%", EMS_Boiler.selBurnPow);
    _renderIntValue("Burner current power", "%", EMS_Boiler.curBurnPow);
    _renderShortValue("Flame current", "uA", EMS_Boiler.flameCurr);
    _renderIntValue("System pressure", "bar", EMS_Boiler.sysPress, 10);
    if (EMS_Boiler.serviceCode == EMS_VALUE_SHORT_NOTSET) {
        myDebug("  System service code: %s", EMS_Boiler.serviceCodeChar);
    } else {
        myDebug("  System service code: %s (%d)", EMS_Boiler.serviceCodeChar, EMS_Boiler.serviceCode);
    }

    // UBAParametersMessage
    _renderIntValue("Heating temperature setting on the boiler", "C", EMS_Boiler.heating_temp);
    _renderIntValue("Boiler circuit pump modulation max power", "%", EMS_Boiler.pump_mod_max);
    _renderIntValue("Boiler circuit pump modulation min power", "%", EMS_Boiler.pump_mod_min);

    // UBAMonitorSlow
    if (EMS_Boiler.extTemp != EMS_VALUE_SHORT_NOTSET) {
        _renderShortValue("Outside temperature", "C", EMS_Boiler.extTemp);
    }
    _renderShortValue("Boiler temperature", "C", EMS_Boiler.boilTemp);
    _renderIntValue("Pump modulation", "%", EMS_Boiler.pumpMod);
    _renderLongValue("Burner # starts", "times", EMS_Boiler.burnStarts);
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

    // For SM10 Solar Module
    if (EMS_Other.SM10) {
        myDebug(""); // newline
        myDebug("%sSolar Module stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        _renderShortValue("  Collector temperature", "C", EMS_Other.SM10collectorTemp);
        _renderShortValue("  Bottom temperature", "C", EMS_Other.SM10bottomTemp);
        _renderIntValue("  Pump modulation", "%", EMS_Other.SM10pumpModulation);
        _renderBoolValue("  Pump active", EMS_Other.SM10pump);
    }

    // Thermostat stats
    if (ems_getThermostatEnabled()) {
        myDebug(""); // newline
        myDebug("%sThermostat stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Thermostat type: %s", ems_getThermostatDescription(buffer_type));
        if ((ems_getThermostatModel() == EMS_MODEL_EASY) || (ems_getThermostatModel() == EMS_MODEL_BOSCHEASY)) {
            // for easy temps are * 100
            // also we don't have the time or mode
            _renderShortValue("Set room temperature", "C", EMS_Thermostat.setpoint_roomTemp, 10);
            _renderShortValue("Current room temperature", "C", EMS_Thermostat.curr_roomTemp, 10);
        } else {
            // because we store in 2 bytes short, when converting to a single byte we'll loose the negative value if its unset
            if ((EMS_Thermostat.setpoint_roomTemp <= 0) || (EMS_Thermostat.curr_roomTemp <= 0)) {
                EMS_Thermostat.setpoint_roomTemp = EMS_VALUE_INT_NOTSET;
                EMS_Thermostat.curr_roomTemp     = EMS_VALUE_INT_NOTSET;
            }
            _renderIntValue("Setpoint room temperature", "C", EMS_Thermostat.setpoint_roomTemp, 2); // convert to a single byte * 2
            _renderIntValue("Current room temperature", "C", EMS_Thermostat.curr_roomTemp, 10);     // is *10

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

    // Dallas
    if (EMSESP_Status.dallas_sensors != 0) {
        myDebug(""); // newline
        char buffer[128] = {0};
        char valuestr[8] = {0}; // for formatting temp
        myDebug("%sExternal temperature sensors:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        for (uint8_t i = 0; i < EMSESP_Status.dallas_sensors; i++) {
            myDebug("  Sensor #%d %s: %s C", i + 1, ds18.getDeviceString(buffer, i), _float_to_char(valuestr, ds18.getValue(i)));
        }
    }

    // show the Shower Info
    if (EMSESP_Status.shower_timer) {
        myDebug(""); // newline
        myDebug("%sShower stats:%s", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Shower is %s", (EMSESP_Shower.showerOn ? "running" : "off"));
    }

    myDebug(""); // newline
}

// send all dallas sensor values as a JSON package to MQTT
void publishSensorValues() {
    StaticJsonDocument<200> doc;
    JsonObject              sensors = doc.to<JsonObject>();

    bool hasdata     = false;
    char label[8]    = {0};
    char valuestr[8] = {0}; // for formatting temp

    // see if the sensor values have changed, if so send
    for (uint8_t i = 0; i < EMSESP_Status.dallas_sensors; i++) {
        double sensorValue = ds18.getValue(i);
        if (sensorValue != DS18_DISCONNECTED && sensorValue != DS18_CRC_ERROR) {
            sprintf(label, PAYLOAD_EXTERNAL_SENSORS, (i + 1));
            sensors[label] = _float_to_char(valuestr, sensorValue);
            hasdata        = true;
        }
    }

    if (hasdata) {
        char data[200] = {0};
        serializeJson(doc, data, sizeof(data));
        myESP.mqttPublish(TOPIC_EXTERNAL_SENSORS, data);
    }
}

// send values via MQTT
// a json object is created for the boiler and one for the thermostat
// CRC check is done to see if there are changes in the values since the last send to avoid too much wifi traffic
void publishValues(bool force) {
    char                              s[20] = {0}; // for formatting strings
    StaticJsonDocument<MQTT_MAX_SIZE> doc;
    char                              data[MQTT_MAX_SIZE] = {0};
    CRC32                             crc;
    uint32_t                          fchecksum;

    static uint8_t  last_boilerActive            = 0xFF; // for remembering last setting of the tap water or heating on/off
    static uint32_t previousBoilerPublishCRC     = 0;    // CRC check for boiler values
    static uint32_t previousThermostatPublishCRC = 0;    // CRC check for thermostat values
    static uint32_t previousOtherPublishCRC      = 0;    // CRC check for other values (e.g. SM10)

    JsonObject rootBoiler = doc.to<JsonObject>();

    rootBoiler["wWSelTemp"]   = _int_to_char(s, EMS_Boiler.wWSelTemp);
    rootBoiler["selFlowTemp"] = _int_to_char(s, EMS_Boiler.selFlowTemp);
    rootBoiler["outdoorTemp"] = _short_to_char(s, EMS_Boiler.extTemp);
    rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);

    if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
        rootBoiler["wWComfort"] = "Hot";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
        rootBoiler["wWComfort"] = "Eco";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
        rootBoiler["wWComfort"] = "Intelligent";
    }

    rootBoiler["wWCurTmp"]          = _short_to_char(s, EMS_Boiler.wWCurTmp);
    rootBoiler["wWCurFlow"]         = _int_to_char(s, EMS_Boiler.wWCurFlow, 10);
    rootBoiler["wWHeat"]            = _bool_to_char(s, EMS_Boiler.wWHeat);
    rootBoiler["curFlowTemp"]       = _short_to_char(s, EMS_Boiler.curFlowTemp);
    rootBoiler["retTemp"]           = _short_to_char(s, EMS_Boiler.retTemp);
    rootBoiler["burnGas"]           = _bool_to_char(s, EMS_Boiler.burnGas);
    rootBoiler["heatPmp"]           = _bool_to_char(s, EMS_Boiler.heatPmp);
    rootBoiler["fanWork"]           = _bool_to_char(s, EMS_Boiler.fanWork);
    rootBoiler["ignWork"]           = _bool_to_char(s, EMS_Boiler.ignWork);
    rootBoiler["wWCirc"]            = _bool_to_char(s, EMS_Boiler.wWCirc);
    rootBoiler["selBurnPow"]        = _int_to_char(s, EMS_Boiler.selBurnPow);
    rootBoiler["curBurnPow"]        = _int_to_char(s, EMS_Boiler.curBurnPow);
    rootBoiler["sysPress"]          = _int_to_char(s, EMS_Boiler.sysPress, 10);
    rootBoiler["boilTemp"]          = _short_to_char(s, EMS_Boiler.boilTemp);
    rootBoiler["pumpMod"]           = _int_to_char(s, EMS_Boiler.pumpMod);
    rootBoiler["ServiceCode"]       = EMS_Boiler.serviceCodeChar;
    rootBoiler["ServiceCodeNumber"] = EMS_Boiler.serviceCode;

    serializeJson(doc, data, sizeof(data));

    // calculate hash and send values if something has changed, to save unnecessary wifi traffic
    for (size_t i = 0; i < measureJson(doc) - 1; i++) {
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
        myDebugLog("Publishing hot water and heating states via MQTT");
        myESP.mqttPublish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive == 1 ? "1" : "0");
        myESP.mqttPublish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive == 1 ? "1" : "0");

        last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
    }

    // handle the thermostat values separately
    if (ems_getThermostatEnabled()) {
        // only send thermostat values if we actually have them
        if ((EMS_Thermostat.curr_roomTemp <= 0) || (EMS_Thermostat.setpoint_roomTemp <= 0))
            return;

        // build new json object
        doc.clear();
        JsonObject rootThermostat = doc.to<JsonObject>();

        if ((ems_getThermostatModel() == EMS_MODEL_EASY) || (ems_getThermostatModel() == EMS_MODEL_BOSCHEASY)) {
            rootThermostat[THERMOSTAT_SELTEMP]  = _short_to_char(s, EMS_Thermostat.setpoint_roomTemp, 10);
            rootThermostat[THERMOSTAT_CURRTEMP] = _short_to_char(s, EMS_Thermostat.curr_roomTemp, 10);
        } else {
            rootThermostat[THERMOSTAT_SELTEMP]  = _int_to_char(s, EMS_Thermostat.setpoint_roomTemp, 2);
            rootThermostat[THERMOSTAT_CURRTEMP] = _int_to_char(s, EMS_Thermostat.curr_roomTemp, 10);
        }

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
        serializeJson(doc, data, sizeof(data));

        // calculate new CRC
        crc.reset();
        for (size_t i = 0; i < measureJson(doc) - 1; i++) {
            crc.update(data[i]);
        }
        fchecksum = crc.finalize();
        if ((previousThermostatPublishCRC != fchecksum) || force) {
            previousThermostatPublishCRC = fchecksum;
            myDebugLog("Publishing thermostat data via MQTT");

            // send values via MQTT
            myESP.mqttPublish(TOPIC_THERMOSTAT_DATA, data);
        }
    }

    // handle the other values separately
    // For SM10 Solar Module
    if (EMS_Other.SM10) {
        // build new json object
        doc.clear();
        JsonObject rootSM10 = doc.to<JsonObject>();

        rootSM10[SM10_COLLECTORTEMP]  = _short_to_char(s, EMS_Other.SM10collectorTemp);
        rootSM10[SM10_BOTTOMTEMP]     = _short_to_char(s, EMS_Other.SM10bottomTemp);
        rootSM10[SM10_PUMPMODULATION] = _int_to_char(s, EMS_Other.SM10pumpModulation);
        rootSM10[SM10_PUMP]           = _bool_to_char(s, EMS_Other.SM10pump);

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));

        // calculate new CRC
        crc.reset();
        for (size_t i = 0; i < measureJson(doc) - 1; i++) {
            crc.update(data[i]);
        }
        fchecksum = crc.finalize();
        if ((previousOtherPublishCRC != fchecksum) || force) {
            previousOtherPublishCRC = fchecksum;
            myDebugLog("Publishing SM10 data via MQTT");

            // send values via MQTT
            myESP.mqttPublish(TOPIC_SM10_DATA, data);
        }
    }
}

// sets the shower timer on/off
void set_showerTimer() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower timer has been set to %s", EMSESP_Status.shower_timer ? "enabled" : "disabled");
    }
}

// sets the shower alert on/off
void set_showerAlert() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug("Shower alert has been set to %s", EMSESP_Status.shower_alert ? "enabled" : "disabled");
    }
}

// used to read the next string from an input buffer and convert to an 8 bit int
uint8_t _readIntNumber() {
    char * numTextPtr = strtok(NULL, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atoi(numTextPtr);
}

// used to read the next string from an input buffer and convert to a double
float _readFloatNumber() {
    char * numTextPtr = strtok(NULL, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atof(numTextPtr);
}

// used to read the next string from an input buffer as a hex value and convert to an 8 bit int
uint8_t _readHexNumber() {
    char * numTextPtr = strtok(NULL, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return (uint8_t)strtol(numTextPtr, 0, 16);
}

// used to read the next string from an input buffer
char * _readWord() {
    char * word = strtok(NULL, ", \n");
    return word;
}

// publish external dallas sensor temperature values to MQTT
void do_publishSensorValues() {
    if (EMSESP_Status.dallas_sensors != 0) {
        publishSensorValues();
    }
}

// call PublishValues without forcing, so using CRC to see if we really need to publish
void do_publishValues() {
    // don't publish if we're not connected to the EMS bus
    if ((ems_getBusConnected()) && (!myESP.getUseSerial()) && myESP.isMQTTConnected()) {
        publishValues(false);
    }
}

// callback to light up the LED, called via Ticker every second
// fast way is to use WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + (state ? 4 : 8), (1 << EMSESP_Status.led_gpio)); // 4 is on, 8 is off
void do_ledcheck() {
    if (EMSESP_Status.led) {
        if (ems_getBusConnected()) {
            digitalWrite(EMSESP_Status.led_gpio, (EMSESP_Status.led_gpio == LED_BUILTIN) ? LOW : HIGH); // light on. For onboard LED high=off
        } else {
            int state = digitalRead(EMSESP_Status.led_gpio);
            digitalWrite(EMSESP_Status.led_gpio, !state);
        }
    }
}

// Thermostat scan
void do_scanThermostat() {
    if ((ems_getBusConnected()) && (!myESP.getUseSerial())) {
        myDebug("> Scanning thermostat message type #0x%02X...", scanThermostat_count);
        ems_doReadCommand(scanThermostat_count, EMS_Thermostat.type_id);
        scanThermostat_count++;
    }
}

// do a system health check every now and then to see if we all connections
void do_systemCheck() {
    if ((!ems_getBusConnected()) && (!myESP.getUseSerial())) {
        myDebug("Error! Unable to read the EMS bus. Retrying in %d seconds...", SYSTEMCHECK_TIME);
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
// only if we have a EMS connection
void do_regularUpdates() {
    if ((ems_getBusConnected()) && (!myESP.getUseSerial())) {
        myDebugLog("Requesting scheduled EMS device data");
        ems_getThermostatValues();
        ems_getBoilerValues();
        ems_getOtherValues();
    }
}

// initiate a force scan by sending type read requests from 0 to FF to the thermostat
// used to analyze responses for debugging
void startThermostatScan(uint8_t start) {
    ems_setLogging(EMS_SYS_LOGGING_THERMOSTAT);
    publishValuesTimer.detach();
    systemCheckTimer.detach();
    regularUpdatesTimer.detach();
    scanThermostat_count = start;
    myDebug("Starting a deep message scan on thermostat");
    scanThermostat.attach(SCANTHERMOSTAT_TIME, do_scanThermostat);
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (EMSESP_Shower.doingColdShot) {
        myDebugLog("[Shower] finished shot of cold. hot water back on");
        ems_setWarmTapWaterActivated(true);
        EMSESP_Shower.doingColdShot = false;
        showerColdShotStopTimer.detach(); // disable the timer
    }
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    if (EMSESP_Status.shower_alert) {
        myDebugLog("[Shower] doing a shot of cold water");
        ems_setWarmTapWaterActivated(false);
        EMSESP_Shower.doingColdShot = true;
        // start the timer for n seconds which will reset the water back to hot
        showerColdShotStopTimer.attach(SHOWER_COLDSHOT_DURATION, _showerColdShotStop);
    }
}

// callback for loading/saving settings to the file system (SPIFFS)
bool FSCallback(MYESP_FSACTION action, const JsonObject json) {
    if (action == MYESP_FSACTION_LOAD) {
        bool recreate_config = true;

        // led
        EMSESP_Status.led = json["led"];

        // led_gpio
        if (!(EMSESP_Status.led_gpio = json["led_gpio"])) {
            EMSESP_Status.led_gpio = EMSESP_LED_GPIO; // default value
        }

        // dallas_gpio
        if (!(EMSESP_Status.dallas_gpio = json["dallas_gpio"])) {
            EMSESP_Status.dallas_gpio = EMSESP_DALLAS_GPIO; // default value
        }

        // dallas_parasite
        if (!(EMSESP_Status.dallas_parasite = json["dallas_parasite"])) {
            EMSESP_Status.dallas_parasite = EMSESP_DALLAS_PARASITE; // default value
        }

        // thermostat_type
        if (!(EMS_Thermostat.type_id = json["thermostat_type"])) {
            EMS_Thermostat.type_id = EMSESP_THERMOSTAT_TYPE; // set default
        }

        // boiler_type
        if (!(EMS_Boiler.type_id = json["boiler_type"])) {
            EMS_Boiler.type_id = EMSESP_BOILER_TYPE; // set default
        }

        // silent mode
        EMSESP_Status.silent_mode = json["silent_mode"];
        ems_setTxDisabled(EMSESP_Status.silent_mode);

        // shower_timer
        EMSESP_Status.shower_timer = json["shower_timer"];

        // shower_alert
        EMSESP_Status.shower_alert = json["shower_alert"];

        // publish_wait
        if (!(EMSESP_Status.publish_wait = json["publish_wait"])) {
            EMSESP_Status.publish_wait = DEFAULT_PUBLISHWAIT; // default value
        }

        return recreate_config; // return false if some settings are missing and we need to rebuild the file
    }

    if (action == MYESP_FSACTION_SAVE) {
        json["led"]             = EMSESP_Status.led;
        json["led_gpio"]        = EMSESP_Status.led_gpio;
        json["dallas_gpio"]     = EMSESP_Status.dallas_gpio;
        json["dallas_parasite"] = EMSESP_Status.dallas_parasite;
        json["thermostat_type"] = EMS_Thermostat.type_id;
        json["boiler_type"]     = EMS_Boiler.type_id;
        json["silent_mode"]     = EMSESP_Status.silent_mode;
        json["shower_timer"]    = EMSESP_Status.shower_timer;
        json["shower_alert"]    = EMSESP_Status.shower_alert;
        json["publish_wait"]    = EMSESP_Status.publish_wait;

        return true;
    }

    return false;
}

// callback for custom settings when showing Stored Settings with the 'set' command
// wc is number of arguments after the 'set' command
// returns true if the setting was recognized and changed and should be saved back to SPIFFs
bool SettingsCallback(MYESP_FSACTION action, uint8_t wc, const char * setting, const char * value) {
    bool ok = false;

    if (action == MYESP_FSACTION_SET) {
        // led
        if ((strcmp(setting, "led") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Status.led = true;
                ok                = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Status.led = false;
                ok                = true;
                // let's make sure LED is really off - For onboard high=off
                digitalWrite(EMSESP_Status.led_gpio, (EMSESP_Status.led_gpio == LED_BUILTIN) ? HIGH : LOW);
            } else {
                myDebug("Error. Usage: set led <on | off>");
            }
        }

        // test mode
        if ((strcmp(setting, "silent_mode") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Status.silent_mode = true;
                ok                        = true;
                myDebug("* in Silent mode. All Tx is disabled.");
                ems_setTxDisabled(true);
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Status.silent_mode = false;
                ok                        = true;
                ems_setTxDisabled(false);
                myDebug("* out of Silent mode. Tx is enabled.");
            } else {
                myDebug("Error. Usage: set silent_mode <on | off>");
            }
        }

        // led_gpio
        if ((strcmp(setting, "led_gpio") == 0) && (wc == 2)) {
            EMSESP_Status.led_gpio = atoi(value);
            // reset pin
            pinMode(EMSESP_Status.led_gpio, OUTPUT);
            digitalWrite(EMSESP_Status.led_gpio, (EMSESP_Status.led_gpio == LED_BUILTIN) ? HIGH : LOW); // light off. For onboard high=off
            ok = true;
        }

        // dallas_gpio
        if ((strcmp(setting, "dallas_gpio") == 0) && (wc == 2)) {
            EMSESP_Status.dallas_gpio = atoi(value);
            ok                        = true;
        }

        // dallas_parasite
        if ((strcmp(setting, "dallas_parasite") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Status.dallas_parasite = true;
                ok                            = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Status.dallas_parasite = false;
                ok                            = true;
            } else {
                myDebug("Error. Usage: set dallas_parasite <on | off>");
            }
        }

        // thermostat_type
        if (strcmp(setting, "thermostat_type") == 0) {
            EMS_Thermostat.type_id = ((wc == 2) ? (uint8_t)strtol(value, 0, 16) : EMS_ID_NONE);
            ok                     = true;
        }

        // boiler_type
        if (strcmp(setting, "boiler_type") == 0) {
            EMS_Boiler.type_id = ((wc == 2) ? (uint8_t)strtol(value, 0, 16) : EMS_ID_NONE);
            ok                 = true;
        }

        // shower timer
        if ((strcmp(setting, "shower_timer") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Status.shower_timer = true;
                ok                         = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Status.shower_timer = false;
                ok                         = true;
            } else {
                myDebug("Error. Usage: set shower_timer <on | off>");
            }
        }

        // shower alert
        if ((strcmp(setting, "shower_alert") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Status.shower_alert = true;
                ok                         = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Status.shower_alert = false;
                ok                         = true;
            } else {
                myDebug("Error. Usage: set shower_alert <on | off>");
            }
        }

        // publish_wait
        if ((strcmp(setting, "publish_wait") == 0) && (wc == 2)) {
            EMSESP_Status.publish_wait = atoi(value);
            ok                         = true;
        }
    }

    if (action == MYESP_FSACTION_LIST) {
        myDebug("  led=%s", EMSESP_Status.led ? "on" : "off");
        myDebug("  led_gpio=%d", EMSESP_Status.led_gpio);
        myDebug("  dallas_gpio=%d", EMSESP_Status.dallas_gpio);
        myDebug("  dallas_parasite=%s", EMSESP_Status.dallas_parasite ? "on" : "off");

        if (EMS_Thermostat.type_id == EMS_ID_NONE) {
            myDebug("  thermostat_type=<not set>");

        } else {
            myDebug("  thermostat_type=%02X", EMS_Thermostat.type_id);
        }

        if (EMS_Boiler.type_id == EMS_ID_NONE) {
            myDebug("  boiler_type=<not set>");

        } else {
            myDebug("  boiler_type=%02X", EMS_Boiler.type_id);
        }

        myDebug("  silent_mode=%s", EMSESP_Status.silent_mode ? "on" : "off");
        myDebug("  shower_timer=%s", EMSESP_Status.shower_timer ? "on" : "off");
        myDebug("  shower_alert=%s", EMSESP_Status.shower_alert ? "on" : "off");
        myDebug("  publish_wait=%d", EMSESP_Status.publish_wait);
    }

    return ok;
}

// call back when a telnet client connects or disconnects
// we set the logging here
void TelnetCallback(uint8_t event) {
    if (event == TELNET_EVENT_CONNECT) {
        ems_setLogging(EMS_SYS_LOGGING_DEFAULT);
    } else if (event == TELNET_EVENT_DISCONNECT) {
        ems_setLogging(EMS_SYS_LOGGING_NONE);
    }
}

// extra commands options for telnet debug window
// wc is the word count, i.e. number of arguments. Everything is in lower case.
void TelnetCommandCallback(uint8_t wc, const char * commandLine) {
    bool ok = false;
    // get first command argument
    char * first_cmd = strtok((char *)commandLine, ", \n");

    if (strcmp(first_cmd, "info") == 0) {
        showInfo();
        ok = true;
    }

    if (strcmp(first_cmd, "publish") == 0) {
        publishValues(true);
        ok = true;
    }

    if (strcmp(first_cmd, "refresh") == 0) {
        myDebug("Fetching data from EMS devices...");
        do_regularUpdates();
        ok = true;
    }

    if (strcmp(first_cmd, "types") == 0) {
        ems_printAllTypes();
        ok = true;
    }

    if (strcmp(first_cmd, "queue") == 0) {
        ems_printTxQueue();
        ok = true;
    }

    if (strcmp(first_cmd, "autodetect") == 0) {
        ems_scanDevices();
        ok = true;
    }

    if (strcmp(first_cmd, "startup") == 0) {
        ems_startupTelegrams();
        ok = true;
    }

    // shower settings
    if ((strcmp(first_cmd, "shower") == 0) && (wc == 2)) {
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "timer") == 0) {
            EMSESP_Status.shower_timer = !EMSESP_Status.shower_timer;
            myESP.mqttPublish(TOPIC_SHOWER_TIMER, EMSESP_Status.shower_timer ? "1" : "0");
            ok = true;
        } else if (strcmp(second_cmd, "alert") == 0) {
            EMSESP_Status.shower_alert = !EMSESP_Status.shower_alert;
            myESP.mqttPublish(TOPIC_SHOWER_ALERT, EMSESP_Status.shower_alert ? "1" : "0");
            ok = true;
        }
    }

    // logging
    if ((strcmp(first_cmd, "log") == 0) && (wc == 2)) {
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "v") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_VERBOSE);
            ok = true;
        } else if (strcmp(second_cmd, "b") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_BASIC);
            ok = true;
        } else if (strcmp(second_cmd, "t") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_THERMOSTAT);
            ok = true;
        } else if (strcmp(second_cmd, "r") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_RAW);
            ok = true;
        } else if (strcmp(second_cmd, "n") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_NONE);
            ok = true;
        }
    }

    // thermostat commands
    if ((strcmp(first_cmd, "thermostat") == 0) && (wc == 3)) {
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "temp") == 0) {
            ems_setThermostatTemp(_readFloatNumber());
            ok = true;
        } else if (strcmp(second_cmd, "mode") == 0) {
            ems_setThermostatMode(_readIntNumber());
            ok = true;
        } else if (strcmp(second_cmd, "read") == 0) {
            ems_doReadCommand(_readHexNumber(), EMS_Thermostat.type_id);
            ok = true;
        } else if (strcmp(second_cmd, "scan") == 0) {
            startThermostatScan(_readIntNumber());
            ok = true;
        }
    }

    // boiler commands
    if ((strcmp(first_cmd, "boiler") == 0) && (wc == 3)) {
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "wwtemp") == 0) {
            ems_setWarmWaterTemp(_readIntNumber());
            ok = true;
        } else if (strcmp(second_cmd, "comfort") == 0) {
            char * third_cmd = _readWord();
            if (strcmp(third_cmd, "hot") == 0) {
                ems_setWarmWaterModeComfort(1);
                ok = true;
            } else if (strcmp(third_cmd, "eco") == 0) {
                ems_setWarmWaterModeComfort(2);
                ok = true;
            } else if (strcmp(third_cmd, "intelligent") == 0) {
                ems_setWarmWaterModeComfort(3);
                ok = true;
            }
        } else if (strcmp(second_cmd, "read") == 0) {
            ems_doReadCommand(_readHexNumber(), EMS_Boiler.type_id);
            ok = true;
        } else if (strcmp(second_cmd, "tapwater") == 0) {
            char * third_cmd = _readWord();
            if (strcmp(third_cmd, "on") == 0) {
                ems_setWarmTapWaterActivated(true);
                ok = true;
            } else if (strcmp(third_cmd, "off") == 0) {
                ems_setWarmTapWaterActivated(false);
                ok = true;
            }
        }
    }

    // send raw
    if (strcmp(first_cmd, "send") == 0) {
        ems_sendRawTelegram((char *)&commandLine[5]);
        ok = true;
    }

    // check for invalid command
    if (!ok) {
        myDebug("Unknown command. Use ? for help.");
    }
}

// OTA callback when the OTA process starts
// so we can disable the EMS to avoid any noise
void OTACallback_pre() {
    emsuart_stop();
}

// OTA callback when the OTA process finishes
// so we can re-enable the UART
void OTACallback_post() {
    emsuart_start();
}

// MQTT Callback to handle incoming/outgoing changes
void MQTTCallback(unsigned int type, const char * topic, const char * message) {
    // we're connected. lets subscribe to some topics
    if (type == MQTT_CONNECT_EVENT) {
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_TEMP);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_MODE);
        myESP.mqttSubscribe(TOPIC_BOILER_WWACTIVATED);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWTEMP);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_COMFORT);
        myESP.mqttSubscribe(TOPIC_SHOWER_TIMER);
        myESP.mqttSubscribe(TOPIC_SHOWER_ALERT);
        myESP.mqttSubscribe(TOPIC_SHOWER_COLDSHOT);

        // publish the status of the Shower parameters
        myESP.mqttPublish(TOPIC_SHOWER_TIMER, EMSESP_Status.shower_timer ? "1" : "0");
        myESP.mqttPublish(TOPIC_SHOWER_ALERT, EMSESP_Status.shower_alert ? "1" : "0");
    }

    // handle incoming MQTT publish events
    if (type == MQTT_MESSAGE_EVENT) {
        // thermostat temp changes
        if (strcmp(topic, TOPIC_THERMOSTAT_CMD_TEMP) == 0) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug("MQTT topic: thermostat temperature value %s", _float_to_char(s, f));
            ems_setThermostatTemp(f);
            publishValues(true); // publish back immediately
        }

        // thermostat mode changes
        if (strcmp(topic, TOPIC_THERMOSTAT_CMD_MODE) == 0) {
            myDebug("MQTT topic: thermostat mode value %s", message);
            if (strcmp((char *)message, "auto") == 0) {
                ems_setThermostatMode(2);
            } else if (strcmp((char *)message, "day") == 0) {
                ems_setThermostatMode(1);
            } else if (strcmp((char *)message, "night") == 0) {
                ems_setThermostatMode(0);
            }
        }

        // wwActivated
        if (strcmp(topic, TOPIC_BOILER_WWACTIVATED) == 0) {
            if (message[0] == '1') {
                ems_setWarmWaterActivated(true);
            } else if (message[0] == '0') {
                ems_setWarmWaterActivated(false);
            }
        }

        // boiler wwtemp changes
        if (strcmp(topic, TOPIC_BOILER_CMD_WWTEMP) == 0) {
            uint8_t t = atoi((char *)message);
            myDebug("MQTT topic: boiler warm water temperature value %d", t);
            ems_setWarmWaterTemp(t);
            publishValues(true); // publish back immediately
        }

        // boiler ww comfort setting
        if (strcmp(topic, TOPIC_BOILER_CMD_COMFORT) == 0) {
            myDebug("MQTT topic: boiler warm water comfort value is %s", message);
            if (strcmp((char *)message, "hot") == 0) {
                ems_setWarmWaterModeComfort(1);
            } else if (strcmp((char *)message, "comfort") == 0) {
                ems_setWarmWaterModeComfort(2);
            } else if (strcmp((char *)message, "intelligent") == 0) {
                ems_setWarmWaterModeComfort(3);
            }
        }

        // shower timer
        if (strcmp(topic, TOPIC_SHOWER_TIMER) == 0) {
            if (message[0] == '1') {
                EMSESP_Status.shower_timer = true;
            } else if (message[0] == '0') {
                EMSESP_Status.shower_timer = false;
            }
            set_showerTimer();
        }

        // shower alert
        if (strcmp(topic, TOPIC_SHOWER_ALERT) == 0) {
            if (message[0] == '1') {
                EMSESP_Status.shower_alert = true;
            } else if (message[0] == '0') {
                EMSESP_Status.shower_alert = false;
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
    // This is where we enable the UART service to scan the incoming serial Tx/Rx bus signals
    // This is done after we have a WiFi signal to avoid any resource conflicts

    if (myESP.getUseSerial()) {
        myDebug("Warning! EMS bus disabled when in Serial mode. Use 'set serial off' to start EMS.");
    } else {
        emsuart_init();
        myDebug("[UART] Opened Rx/Tx connection");
        if (!EMSESP_Status.silent_mode) {
            // go and find the boiler and thermostat types, if not in silent mode
            ems_discoverModels();
        }
    }
}

// Initialize the boiler settings and shower settings
// Most of these will be overwritten after the SPIFFS config file is loaded
void initEMSESP() {
    // general settings
    EMSESP_Status.shower_timer = false;
    EMSESP_Status.shower_alert = false;
    EMSESP_Status.led          = true; // LED is on by default
    EMSESP_Status.silent_mode  = false;
    EMSESP_Status.publish_wait = DEFAULT_PUBLISHWAIT;

    EMSESP_Status.timestamp      = millis();
    EMSESP_Status.dallas_sensors = 0;

    EMSESP_Status.led_gpio    = EMSESP_LED_GPIO;
    EMSESP_Status.dallas_gpio = EMSESP_DALLAS_GPIO;

    // shower settings
    EMSESP_Shower.timerStart    = 0;
    EMSESP_Shower.timerPause    = 0;
    EMSESP_Shower.duration      = 0;
    EMSESP_Shower.doingColdShot = false;
}

/*
 *  Shower Logic
 */
void showerCheck() {
    // if already in cold mode, ignore all this logic until we're out of the cold blast
    if (!EMSESP_Shower.doingColdShot) {
        // is the hot water running?
        if (EMS_Boiler.tapwaterActive == 1) {
            // if heater was previously off, start the timer
            if (EMSESP_Shower.timerStart == 0) {
                // hot water just started...
                EMSESP_Shower.timerStart    = EMSESP_Status.timestamp;
                EMSESP_Shower.timerPause    = 0; // remove any last pauses
                EMSESP_Shower.doingColdShot = false;
                EMSESP_Shower.duration      = 0;
                EMSESP_Shower.showerOn      = false;
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!EMSESP_Shower.showerOn && (EMSESP_Status.timestamp - EMSESP_Shower.timerStart) > SHOWER_MIN_DURATION) {
                    EMSESP_Shower.showerOn = true;
                    myDebugLog("[Shower] hot water still running, starting shower timer");
                }
                // check if the shower has been on too long
                else if ((((EMSESP_Status.timestamp - EMSESP_Shower.timerStart) > SHOWER_MAX_DURATION) && !EMSESP_Shower.doingColdShot)
                         && EMSESP_Status.shower_alert) {
                    myDebugLog("[Shower] exceeded max shower time");
                    _showerColdShotStart();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if ((EMSESP_Shower.timerStart != 0) && (EMSESP_Shower.timerPause == 0)) {
                EMSESP_Shower.timerPause = EMSESP_Status.timestamp;
            }

            // if shower has been off for longer than the wait time
            if ((EMSESP_Shower.timerPause != 0) && ((EMSESP_Status.timestamp - EMSESP_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
                // it is over the wait period, so assume that the shower has finished and calculate the total time and publish
                // because its unsigned long, can't have negative so check if length is less than OFFSET_TIME
                if ((EMSESP_Shower.timerPause - EMSESP_Shower.timerStart) > SHOWER_OFFSET_TIME) {
                    EMSESP_Shower.duration = (EMSESP_Shower.timerPause - EMSESP_Shower.timerStart - SHOWER_OFFSET_TIME);
                    if (EMSESP_Shower.duration > SHOWER_MIN_DURATION) {
                        char s[50]      = {0};
                        char buffer[16] = {0};
                        strlcpy(s, itoa((uint8_t)((EMSESP_Shower.duration / (1000 * 60)) % 60), buffer, 10), sizeof(s));
                        strlcat(s, " minutes and ", sizeof(s));
                        strlcat(s, itoa((uint8_t)((EMSESP_Shower.duration / 1000) % 60), buffer, 10), sizeof(s));
                        strlcat(s, " seconds", sizeof(s));
                        if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                            myDebug("[Shower] finished with duration %s", s);
                        }
                        myESP.mqttPublish(TOPIC_SHOWERTIME, s); // publish to MQTT
                    }
                }

                // reset everything
                EMSESP_Shower.timerStart = 0;
                EMSESP_Shower.timerPause = 0;
                EMSESP_Shower.showerOn   = false;
                _showerColdShotStop(); // turn hot water back on in case its off
            }
        }
    }
}

//
// SETUP
//
void setup() {
    // init our own parameters
    initEMSESP();

    // call ems.cpp's init function to set all the internal params
    ems_init();

    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck); // check if Boiler is online

    // set up myESP for Wifi, MQTT, MDNS and Telnet
    myESP.setTelnet(project_cmds, ArraySize(project_cmds), TelnetCommandCallback, TelnetCallback); // set up Telnet commands
#ifdef WIFI_SSID
    myESP.setWIFI(WIFI_SSID, WIFI_PASSWORD, WIFICallback);
#else
    myESP.setWIFI(NULL, NULL, WIFICallback); // pull the wifi settings from the SPIFFS stored settings
#endif

    // MQTT host, username and password taken from the SPIFFS settings
    myESP.setMQTT(NULL,
                  NULL,
                  NULL,
                  MQTT_BASE,
                  MQTT_KEEPALIVE,
                  MQTT_QOS,
                  MQTT_RETAIN,
                  MQTT_WILL_TOPIC,
                  MQTT_WILL_ONLINE_PAYLOAD,
                  MQTT_WILL_OFFLINE_PAYLOAD,
                  MQTTCallback);

    // OTA callback which is called when OTA is starting and stopping
    myESP.setOTA(OTACallback_pre, OTACallback_post);

    // custom settings in SPIFFS
    myESP.setSettings(FSCallback, SettingsCallback);

    // start up all the services
    myESP.begin(APP_HOSTNAME, APP_NAME, APP_VERSION);

    // at this point we have the settings from our internall SPIFFS config file

    // enable regular checks if not in test mode
    if (!EMSESP_Status.silent_mode) {
        publishValuesTimer.attach(EMSESP_Status.publish_wait, do_publishValues);             // post MQTT EMS values
        publishSensorValuesTimer.attach(EMSESP_Status.publish_wait, do_publishSensorValues); // post MQTT sensor values
        regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates);                  // regular reads from the EMS
    }

    // set pin for LED
    if (EMSESP_Status.led_gpio != EMS_VALUE_INT_NOTSET) {
        pinMode(EMSESP_Status.led_gpio, OUTPUT);
        digitalWrite(EMSESP_Status.led_gpio, (EMSESP_Status.led_gpio == LED_BUILTIN) ? HIGH : LOW); // light off. For onboard high=off
        ledcheckTimer.attach_ms(LEDCHECK_TIME, do_ledcheck);                                        // blink heartbeat LED
    }

    // check for Dallas sensors
    EMSESP_Status.dallas_sensors = ds18.setup(EMSESP_Status.dallas_gpio, EMSESP_Status.dallas_parasite); // returns #sensors
}

//
// Main loop
//
void loop() {
    EMSESP_Status.timestamp = millis();

    // the main loop
    myESP.loop();

    // check Dallas sensors, every 2 seconds
    // these values are published to MQTT seperately via the timer publishSensorValuesTimer
    if (EMSESP_Status.dallas_sensors != 0) {
        ds18.loop();
    }

    // publish the values to MQTT, only if the values have changed
    // although we don't want to publish when doing a deep scan of the thermostat
    if (ems_getEmsRefreshed() && (scanThermostat_count == 0) && (!EMSESP_Status.silent_mode)) {
        publishValues(false);
        ems_setEmsRefreshed(false); // reset
    }

    // do shower logic, if enabled
    if (EMSESP_Status.shower_timer) {
        showerCheck();
    }

    if (EMSESP_DELAY != 0) {
        delay(EMSESP_DELAY); // some time to WiFi and everything else to catch up, and prevent overheating
    }
}
