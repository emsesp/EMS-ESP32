/*
 * EMS-ESP
 *
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 * See ChangeLog.md for history
 * See wiki at https://github.com/proddy/EMS-ESP/Wiki for Acknowledgments
 */

// local libraries
#include "MyESP.h"
#include "ems.h"
#include "ems_devices.h"
#include "emsuart.h"
#include "my_config.h"
#include "version.h"

// Dallas external temp sensors
#include "ds18.h"
DS18 ds18;

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <CRC32.h>       // https://github.com/bakercp/CRC32

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// default APP params
#define APP_NAME "EMS-ESP"
#define APP_HOSTNAME "ems-esp"
#define APP_URL "https://github.com/proddy/EMS-ESP"
#define APP_UPDATEURL "https://api.github.com/repos/proddy/EMS-ESP/releases/latest"

// macros for easy debugging
#define myDebug(...) myESP.myDebug(__VA_ARGS__)
#define myDebug_P(...) myESP.myDebug_P(__VA_ARGS__)

// set to value >0 if the ESP is overheating or there are timing issues. Recommend a value of 1.
#define EMSESP_DELAY 0 // initially set to 0 for no delay. Change to 1 if getting WDT resets from wifi

// timers, all values are in seconds
#define DEFAULT_PUBLISHTIME 120 // every 2 minutes publish MQTT values, including Dallas sensors
Ticker publishValuesTimer;
Ticker publishSensorValuesTimer;

#define SYSTEMCHECK_TIME 30 // every 30 seconds check if EMS can be reached
Ticker systemCheckTimer;

#define REGULARUPDATES_TIME 60 // every minute a call is made to fetch data from EMS devices manually
Ticker regularUpdatesTimer;

#define LEDCHECK_TIME 500 // every 1/2 second blink the heartbeat LED
Ticker ledcheckTimer;

// thermostat scan - for debugging
Ticker scanThermostat;
#define SCANTHERMOSTAT_TIME 1
uint8_t scanThermostat_count = 0;

// ems bus scan
Ticker scanDevices;
#define SCANDEVICES_TIME 350 // ms
uint8_t scanDevices_count;

Ticker showerColdShotStopTimer;

// if using the shower timer, change these settings
#define SHOWER_PAUSE_TIME 15000     // in ms. 15 seconds, max time if water is switched off & on during a shower
#define SHOWER_MIN_DURATION 120000  // in ms. 2 minutes, before recognizing its a shower
#define SHOWER_OFFSET_TIME 5000     // in ms. 5 seconds grace time, to calibrate actual time under the shower
#define SHOWER_COLDSHOT_DURATION 10 // in seconds. 10 seconds for cold water before turning back hot water
#define SHOWER_MAX_DURATION 420000  // in ms. 7 minutes, before trigger a shot of cold water

#ifdef LOGICANALYZER
#define EMSESP_DALLAS_GPIO D1
#define EMSESP_DALLAS_PARASITE false
#else
// set this if using an external temperature sensor like a DS18B20
// D5 is the default on a bbqkees board
#define EMSESP_DALLAS_GPIO D5
#define EMSESP_DALLAS_PARASITE false
#endif

// Set LED pin used for showing the EMS bus connection status. Solid means EMS bus working, flashing is an error
// can be either the onboard LED on the ESP8266 (LED_BULLETIN) or external via an external pull-up LED (e.g. D1 on a bbqkees' board)
// can be enabled and disabled via the 'set led' command and pin set by 'set led_gpio'
#define EMSESP_LED_GPIO LED_BUILTIN

typedef struct {
    uint32_t timestamp;      // for internal timings, via millis()
    uint8_t  dallas_sensors; // count of dallas sensors

    // custom params
    bool     shower_timer;    // true if we want to report back on shower times
    bool     shower_alert;    // true if we want the alert of cold water
    bool     led;             // LED on/off
    bool     listen_mode;     // stop automatic Tx on/off
    uint16_t publish_time;    // frequency of MQTT publish in seconds
    uint8_t  led_gpio;        // pin for LED
    uint8_t  dallas_gpio;     // pin for attaching external dallas temperature sensors
    bool     dallas_parasite; // on/off is using parasite
    uint8_t  tx_mode;         // TX mode 1,2 or 3
} _EMSESP_Settings;

typedef struct {
    bool     showerOn;
    uint32_t timerStart;    // ms
    uint32_t timerPause;    // ms
    uint32_t duration;      // ms
    bool     doingColdShot; // true if we've just sent a jolt of cold water
} _EMSESP_Shower;

static const command_t project_cmds[] PROGMEM = {

    {true, "led <on | off>", "toggle status LED on/off"},
    {true, "led_gpio <gpio>", "set the LED pin. Default is the onboard LED 2. For external D1 use 5"},
    {true, "dallas_gpio <gpio>", "set the external Dallas temperature sensors pin. Default is 14 for D5"},
    {true, "dallas_parasite <on | off>", "set to on if powering Dallas sensors via parasite power"},
    {true, "listen_mode <on | off>", "when set to on all automatic Tx are disabled"},
    {true, "shower_timer <on | off>", "send MQTT notification on all shower durations"},
    {true, "shower_alert <on | off>", "stop hot water to send 3 cold burst warnings after max shower time is exceeded"},
    {true, "publish_time <seconds>", "set frequency for publishing data to MQTT (0=off)"},
    {true, "tx_mode <n>", "changes Tx logic. 1=ems generic, 2=ems+, 3=Junkers HT3"},

    {false, "info", "show current captured on the devices"},
    {false, "log <n | b | t | s | r | j | v>", "set logging mode to none, basic, thermostat only, solar module only, raw, jabber or verbose"},

#ifdef TESTS
    {false, "test <n>", "insert a test telegram on to the EMS bus"},
#endif

    {false, "publish", "publish all values to MQTT"},
    {false, "refresh", "fetch values from the EMS devices"},
    {false, "devices", "list all supported and detected EMS devices and types IDs"},
    {false, "queue", "show current Tx queue"},
    {false, "autodetect [deep]", "detect EMS devices and attempt to automatically set boiler and thermostat types"},
    {false, "shower <timer | alert>", "toggle either timer or alert on/off"},
    {false, "send XX ...", "send raw telegram data to EMS bus (XX are hex values)"},
    {false, "thermostat read <type ID>", "send read request to the thermostat for heating circuit hc 1-4"},
    {false, "thermostat temp [hc] <degrees>", "set current thermostat temperature"},
    {false, "thermostat mode [hc] <mode>", "set mode (0=low/night, 1=manual/day, 2=auto) for heating circuit hc 1-4"},
    {false, "thermostat scan <type ID>", "probe thermostat on all type id responses"},
    {false, "boiler read <type ID>", "send read request to boiler"},
    {false, "boiler wwtemp <degrees>", "set boiler warm water temperature"},
    {false, "boiler tapwater <on | off>", "set boiler warm tap water on/off"},
    {false, "boiler flowtemp <degrees>", "set boiler flow temperature"},
    {false, "boiler comfort <hot | eco | intelligent>", "set boiler warm water comfort setting"}

};
uint8_t _project_cmds_count = ArraySize(project_cmds);

// store for overall system status
_EMSESP_Settings EMSESP_Settings;
_EMSESP_Shower   EMSESP_Shower;

// logging messages with fixed strings
void myDebugLog(const char * s) {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug(s);
    }
}

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision = 2) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret   = a;
    long   whole = (long)f;
    itoa(whole, a, 10);
    while (*a != '\0')
        a++;
    *a++         = '.';
    long decimal = abs((long)((f - whole) * p[precision]));
    itoa(decimal, a, 10);

    return ret;
}

// convert bool to text. bools are stored as bytes
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

// convert short (two bytes) to text string
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
// negative values are assumed stored as 1-compliment (https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c)
char * _short_to_char(char * s, int16_t value, uint8_t decimals = 1) {
    // remove errors or invalid values
    if (value == EMS_VALUE_SHORT_NOTSET) {
        strlcpy(s, "?", 10);
        return (s);
    }

    // just print
    if (decimals == 0) {
        ltoa(value, s, 10);
        return (s);
    }

    // do floating point
    char s2[10] = {0};
    // check for negative values
    if (value < 0) {
        strlcpy(s, "-", 10);
        value *= -1; // convert to positive
    }

    if (decimals == 2) {
        // divide by 2
        strlcpy(s, ltoa(value / 2, s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ((value & 0x01) ? "5" : "0"), 10);

    } else {
        strlcpy(s, ltoa(value / (decimals * 10), s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ltoa(value % (decimals * 10), s2, 10), 10);
    }

    return s;
}

// convert short (two bytes) to text string
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
char * _ushort_to_char(char * s, uint16_t value, uint8_t decimals = 1) {
    // remove errors or invalid values
    if (value == EMS_VALUE_USHORT_NOTSET) {
        strlcpy(s, "?", 10);
        return (s);
    }

    // just print
    if (decimals == 0) {
        ltoa(value, s, 10);
        return (s);
    }

    // do floating point
    char s2[10] = {0};

    if (decimals == 2) {
        // divide by 2
        strlcpy(s, ltoa(value / 2, s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ((value & 0x01) ? "5" : "0"), 10);

    } else {
        strlcpy(s, ltoa(value / (decimals * 10), s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ltoa(value % (decimals * 10), s2, 10), 10);
    }

    return s;
}

// takes a signed short value (2 bytes), converts to a fraction
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
void _renderShortValue(const char * prefix, const char * postfix, int16_t value, uint8_t decimals = 1) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _short_to_char(s, value, decimals), sizeof(buffer));

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a unsigned short value (2 bytes), converts to a fraction
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
void _renderUShortValue(const char * prefix, const char * postfix, uint16_t value, uint8_t decimals = 1) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _ushort_to_char(s, value, decimals), sizeof(buffer));

    if (postfix != nullptr) {
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

    static char s2[5] = {0};

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
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _int_to_char(s, value, div), sizeof(buffer));

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a long value at prints it to debug log
void _renderLongValue(const char * prefix, const char * postfix, uint32_t value) {
    static char buffer[200] = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    if (value == EMS_VALUE_LONG_NOTSET) {
        strlcat(buffer, "?", sizeof(buffer));
    } else {
        char s[20] = {0};
        strlcat(buffer, ltoa(value, s, 10), sizeof(buffer));
    }

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a bool value at prints it to debug log
void _renderBoolValue(const char * prefix, uint8_t value) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _bool_to_char(s, value), sizeof(buffer));

    myDebug(buffer);
}

// figures out the thermostat mode
// returns 0xFF=unknown, 0=low, 1=manual, 2=auto, 3=night, 4=day
// hc_num is 1 to 4
uint8_t _getThermostatMode(uint8_t hc_num) {
    int     thermoMode = EMS_VALUE_INT_NOTSET;
    uint8_t model      = ems_getThermostatModel();

    uint8_t mode = EMS_Thermostat.hc[hc_num - 1].mode;

    if (model == EMS_MODEL_RC20) {
        if (mode == 0) {
            thermoMode = 0; // low
        } else if (mode == 1) {
            thermoMode = 1; // manual
        } else if (mode == 2) {
            thermoMode = 2; // auto
        }
    } else if (model == EMS_MODEL_RC300) {
        if (mode == 0) {
            thermoMode = 1; // manual
        } else if (mode == 1) {
            thermoMode = 2; // auto
        }
    } else { // default for all other thermostats
        if (mode == 0) {
            thermoMode = 3; // night
        } else if (mode == 1) {
            thermoMode = 4; // day
        } else if (mode == 2) {
            thermoMode = 2; // auto
        }
    }

    return thermoMode;
}

// Show command - display stats on an 's' command
void showInfo() {
    // General stats from EMS bus

    static char buffer_type[128] = {0};

    myDebug_P(PSTR("%sEMS-ESP system stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug_P(PSTR("  System logging set to Basic"));
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug_P(PSTR("  System logging set to Verbose"));
    } else if (sysLog == EMS_SYS_LOGGING_THERMOSTAT) {
        myDebug_P(PSTR("  System logging set to Thermostat only"));
    } else if (sysLog == EMS_SYS_LOGGING_SOLARMODULE) {
        myDebug_P(PSTR("  System logging set to Solar Module only"));
    } else if (sysLog == EMS_SYS_LOGGING_JABBER) {
        myDebug_P(PSTR("  System logging set to Jabber"));
    } else {
        myDebug_P(PSTR("  System logging set to None"));
    }

    myDebug_P(PSTR("  LED is %s, Listen mode is %s"), EMSESP_Settings.led ? "on" : "off", EMSESP_Settings.listen_mode ? "on" : "off");
    if (EMSESP_Settings.dallas_sensors > 0) {
        myDebug_P(PSTR("  %d external temperature sensor%s found"), EMSESP_Settings.dallas_sensors, (EMSESP_Settings.dallas_sensors == 1) ? "" : "s");
    }

    myDebug_P(PSTR("  Boiler is %s, Thermostat is %s, Solar Module is %s, Shower Timer is %s, Shower Alert is %s"),
              (ems_getBoilerEnabled() ? "enabled" : "disabled"),
              (ems_getThermostatEnabled() ? "enabled" : "disabled"),
              (ems_getSolarModuleEnabled() ? "enabled" : "disabled"),
              ((EMSESP_Settings.shower_timer) ? "enabled" : "disabled"),
              ((EMSESP_Settings.shower_alert) ? "enabled" : "disabled"));

    myDebug_P(PSTR("\n%sEMS Bus stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);

    if (ems_getBusConnected()) {
        myDebug_P(PSTR("  Bus is connected"));
        myDebug_P(PSTR("  Rx: # successful read requests=%d, # CRC errors=%d"), EMS_Sys_Status.emsRxPgks, EMS_Sys_Status.emxCrcErr);

        if (ems_getTxCapable()) {
            char valuestr[8] = {0}; // for formatting floats
            myDebug_P(PSTR("  Tx: Last poll=%s seconds ago, # successful write requests=%d"),
                      _float_to_char(valuestr, (ems_getPollFrequency() / (float)1000000), 3),
                      EMS_Sys_Status.emsTxPkgs);
        } else {
            myDebug_P(PSTR("  Tx: no signal"));
        }
    } else {
        myDebug_P(PSTR("  No connection can be made to the EMS bus"));
    }

    myDebug_P(PSTR(""));
    myDebug_P(PSTR("%sBoiler stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);

    // version details
    myDebug_P(PSTR("  Boiler: %s"), ems_getBoilerDescription(buffer_type));

    // active stats
    if (ems_getBusConnected()) {
        if (EMS_Boiler.tapwaterActive != EMS_VALUE_INT_NOTSET) {
            myDebug_P(PSTR("  Hot tap water: %s"), EMS_Boiler.tapwaterActive ? "running" : "off");
        }

        if (EMS_Boiler.heatingActive != EMS_VALUE_INT_NOTSET) {
            myDebug_P(PSTR("  Central heating: %s"), EMS_Boiler.heatingActive ? "active" : "off");
        }
    }

    // UBAParameterWW
    _renderBoolValue("Warm Water activated", EMS_Boiler.wWActivated);
    _renderBoolValue("Warm Water circulation pump available", EMS_Boiler.wWCircPump);
    if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
        myDebug_P(PSTR("  Warm Water comfort setting: Hot"));
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
        myDebug_P(PSTR("  Warm Water comfort setting: Eco"));
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
        myDebug_P(PSTR("  Warm Water comfort setting: Intelligent"));
    }

    _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
    _renderIntValue("Warm Water desired temperature", "C", EMS_Boiler.wWDesiredTemp);

    // UBAMonitorWWMessage
    _renderUShortValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
    _renderIntValue("Warm Water current tap water flow", "l/min", EMS_Boiler.wWCurFlow, 10);
    _renderLongValue("Warm Water # starts", "times", EMS_Boiler.wWStarts);
    if (EMS_Boiler.wWWorkM != EMS_VALUE_LONG_NOTSET) {
        myDebug_P(PSTR("  Warm Water active time: %d days %d hours %d minutes"),
                  EMS_Boiler.wWWorkM / 1440,
                  (EMS_Boiler.wWWorkM % 1440) / 60,
                  EMS_Boiler.wWWorkM % 60);
    }
    _renderBoolValue("Warm Water 3-way valve", EMS_Boiler.wWHeat);

    // UBAMonitorFast
    _renderIntValue("Selected flow temperature", "C", EMS_Boiler.selFlowTemp);
    _renderUShortValue("Current flow temperature", "C", EMS_Boiler.curFlowTemp);
    _renderUShortValue("Return temperature", "C", EMS_Boiler.retTemp);
    _renderBoolValue("Gas", EMS_Boiler.burnGas);
    _renderBoolValue("Boiler pump", EMS_Boiler.heatPmp);
    _renderBoolValue("Fan", EMS_Boiler.fanWork);
    _renderBoolValue("Ignition", EMS_Boiler.ignWork);
    _renderBoolValue("Circulation pump", EMS_Boiler.wWCirc);
    _renderIntValue("Burner selected max power", "%", EMS_Boiler.selBurnPow);
    _renderIntValue("Burner current power", "%", EMS_Boiler.curBurnPow);
    _renderShortValue("Flame current", "uA", EMS_Boiler.flameCurr);
    _renderIntValue("System pressure", "bar", EMS_Boiler.sysPress, 10);
    if (EMS_Boiler.serviceCode == EMS_VALUE_USHORT_NOTSET) {
        myDebug_P(PSTR("  System service code: %s"), EMS_Boiler.serviceCodeChar);
    } else {
        myDebug_P(PSTR("  System service code: %s (%d)"), EMS_Boiler.serviceCodeChar, EMS_Boiler.serviceCode);
    }

    // UBAParametersMessage
    _renderIntValue("Heating temperature setting on the boiler", "C", EMS_Boiler.heating_temp);
    _renderIntValue("Boiler circuit pump modulation max power", "%", EMS_Boiler.pump_mod_max);
    _renderIntValue("Boiler circuit pump modulation min power", "%", EMS_Boiler.pump_mod_min);

    // UBAMonitorSlow
    if (EMS_Boiler.extTemp != EMS_VALUE_SHORT_NOTSET) {
        _renderShortValue("Outside temperature", "C", EMS_Boiler.extTemp);
    }
    _renderUShortValue("Boiler temperature", "C", EMS_Boiler.boilTemp);
    _renderIntValue("Pump modulation", "%", EMS_Boiler.pumpMod);
    _renderLongValue("Burner # starts", "times", EMS_Boiler.burnStarts);
    if (EMS_Boiler.burnWorkMin != EMS_VALUE_LONG_NOTSET) {
        myDebug_P(PSTR("  Total burner operating time: %d days %d hours %d minutes"),
                  EMS_Boiler.burnWorkMin / 1440,
                  (EMS_Boiler.burnWorkMin % 1440) / 60,
                  EMS_Boiler.burnWorkMin % 60);
    }
    if (EMS_Boiler.heatWorkMin != EMS_VALUE_LONG_NOTSET) {
        myDebug_P(PSTR("  Total heat operating time: %d days %d hours %d minutes"),
                  EMS_Boiler.heatWorkMin / 1440,
                  (EMS_Boiler.heatWorkMin % 1440) / 60,
                  EMS_Boiler.heatWorkMin % 60);
    }
    if (EMS_Boiler.UBAuptime != EMS_VALUE_LONG_NOTSET) {
        myDebug_P(PSTR("  Total UBA working time: %d days %d hours %d minutes"),
                  EMS_Boiler.UBAuptime / 1440,
                  (EMS_Boiler.UBAuptime % 1440) / 60,
                  EMS_Boiler.UBAuptime % 60);
    }

    // For SM10/SM100 Solar Module
    if (ems_getSolarModuleEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sSolar Module stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Solar module: %s"), ems_getSolarModuleDescription(buffer_type));
        _renderShortValue("Collector temperature", "C", EMS_SolarModule.collectorTemp);
        _renderShortValue("Bottom temperature", "C", EMS_SolarModule.bottomTemp);
        _renderIntValue("Pump modulation", "%", EMS_SolarModule.pumpModulation);
        _renderBoolValue("Pump active", EMS_SolarModule.pump);
        if (EMS_SolarModule.pumpWorkMin != EMS_VALUE_LONG_NOTSET) {
            myDebug_P(PSTR("  Pump working time: %d days %d hours %d minutes"),
                      EMS_SolarModule.pumpWorkMin / 1440,
                      (EMS_SolarModule.pumpWorkMin % 1440) / 60,
                      EMS_SolarModule.pumpWorkMin % 60);
        }
        _renderUShortValue("Energy last hour", "Wh", EMS_SolarModule.EnergyLastHour, 1); // *10
        _renderUShortValue("Energy today", "Wh", EMS_SolarModule.EnergyToday, 0);
        _renderUShortValue("Energy total", "kWh", EMS_SolarModule.EnergyTotal, 1); // *10
    }

    // For HeatPumps
    if (ems_getHeatPumpEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sHeat Pump stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Heat Pump module: %s"), ems_getHeatPumpDescription(buffer_type));
        _renderIntValue("Pump modulation", "%", EMS_HeatPump.HPModulation);
        _renderIntValue("Pump speed", "%", EMS_HeatPump.HPSpeed);
    }

    // Thermostat stats
    if (ems_getThermostatEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sThermostat stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Thermostat: %s"), ems_getThermostatDescription(buffer_type, false));

        // Render Thermostat Date & Time
        uint8_t model = ems_getThermostatModel();
        if ((model != EMS_MODEL_EASY)) {
            myDebug_P(PSTR("  Thermostat time is %s"), EMS_Thermostat.datetime);
        }

        uint8_t _m_setpoint, _m_curr;
        switch (model) {
        case EMS_MODEL_EASY:
            _m_setpoint = 10; // *100
            _m_curr     = 10; // *100
            break;
        case EMS_MODEL_FR10:
        case EMS_MODEL_FW100:
        case EMS_MODEL_FW120:
            _m_setpoint = 1; // *10
            _m_curr     = 1; // *10
            break;
        default:             // RC30,RC35 etc...
            _m_setpoint = 2; // *2
            _m_curr     = 1; // *10
            break;
        }

        // go through all Heating Circuits
        for (uint8_t hc_num = 1; hc_num <= EMS_THERMOSTAT_MAXHC; hc_num++) {
            // only show if we have data for the Heating Circuit
            if (EMS_Thermostat.hc[hc_num - 1].active) {
                myDebug_P(PSTR("  Heating Circuit %d"), hc_num);
                _renderShortValue(" Current room temperature", "C", EMS_Thermostat.hc[hc_num - 1].curr_roomTemp, _m_curr);

                // Render Day/Night/Holiday Temperature on RC35s
                // there is no single setpoint temp, but one for day, night and vacation
                if (model == EMS_MODEL_RC35) {
                    if (EMS_Thermostat.hc[hc_num - 1].summer_mode) {
                        myDebug_P(PSTR("   Program is set to Summer mode"));
                    } else if (EMS_Thermostat.hc[hc_num - 1].holiday_mode) {
                        myDebug_P(PSTR("   Program is set to Holiday mode"));
                    }

                    _renderIntValue(" Day temperature", "C", EMS_Thermostat.hc[hc_num - 1].daytemp, 2);          // convert to a single byte * 2
                    _renderIntValue(" Night temperature", "C", EMS_Thermostat.hc[hc_num - 1].nighttemp, 2);      // convert to a single byte * 2
                    _renderIntValue(" Vacation temperature", "C", EMS_Thermostat.hc[hc_num - 1].holidaytemp, 2); // convert to a single byte * 2
                } else {
                    _renderShortValue(" Setpoint room temperature", "C", EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp, _m_setpoint);
                }

                // Render Termostat Mode, if we have a mode
                uint8_t thermoMode = _getThermostatMode(hc_num); // 0xFF=unknown, 0=low, 1=manual, 2=auto, 3=night, 4=day
                if (thermoMode == 0) {
                    myDebug_P(PSTR("   Mode is set to low"));
                } else if (thermoMode == 1) {
                    myDebug_P(PSTR("   Mode is set to manual"));
                } else if (thermoMode == 2) {
                    myDebug_P(PSTR("   Mode is set to auto"));
                } else if (thermoMode == 3) {
                    myDebug_P(PSTR("   Mode is set to night"));
                } else if (thermoMode == 4) {
                    myDebug_P(PSTR("   Mode is set to day"));
                }
            }
        }
    }

    // Dallas external temp sensors
    if (EMSESP_Settings.dallas_sensors != 0) {
        myDebug_P(PSTR("")); // newline
        char buffer[128] = {0};
        char valuestr[8] = {0}; // for formatting temp
        myDebug_P(PSTR("%sExternal temperature sensors:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            myDebug_P(PSTR("  Sensor #%d %s: %s C"), i + 1, ds18.getDeviceString(buffer, i), _float_to_char(valuestr, ds18.getValue(i)));
        }
    }

    // show the Shower Info
    if (EMSESP_Settings.shower_timer) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sShower stats:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Shower is %s"), (EMSESP_Shower.showerOn ? "running" : "off"));
    }

    myDebug_P(PSTR("")); // newline
}

// send all dallas sensor values as a JSON package to MQTT
void publishSensorValues() {
    // don't send if MQTT is connected
    if (!myESP.isMQTTConnected()) {
        return;
    }

    StaticJsonDocument<200> doc;
    JsonObject              sensors = doc.to<JsonObject>();

    bool hasdata     = false;
    char label[8]    = {0};
    char valuestr[8] = {0}; // for formatting temp

    // see if the sensor values have changed, if so send
    for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
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
// a check is done against the previous values and if there are changes only then they are published. Unless force=true
void publishValues(bool force) {
    // don't send if MQTT is not connected
    if (!myESP.isMQTTConnected()) {
        return;
    }

    // don't publish is publish time is set to 0
    if (EMSESP_Settings.publish_time == 0) {
        return;
    }

    char                                      s[20] = {0}; // for formatting strings
    StaticJsonDocument<MQTT_MAX_PAYLOAD_SIZE> doc;
    char                                      data[MQTT_MAX_PAYLOAD_SIZE] = {0};
    CRC32                                     crc;
    uint32_t                                  fchecksum;
    uint8_t                                   jsonSize;

    static uint8_t  last_boilerActive            = 0xFF; // for remembering last setting of the tap water or heating on/off
    static uint32_t previousBoilerPublishCRC     = 0;    // CRC check for boiler values
    static uint32_t previousThermostatPublishCRC = 0;    // CRC check for thermostat values
    static uint32_t previousSMPublishCRC         = 0;    // CRC check for Solar Module values (e.g. SM10)

    JsonObject rootBoiler = doc.to<JsonObject>();

    if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
        rootBoiler["wWComfort"] = "Hot";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
        rootBoiler["wWComfort"] = "Eco";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
        rootBoiler["wWComfort"] = "Intelligent";
    }

    if (EMS_Boiler.wWSelTemp != EMS_VALUE_INT_NOTSET)
        rootBoiler["wWSelTemp"] = EMS_Boiler.wWSelTemp;
    if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET)
        rootBoiler["selFlowTemp"] = EMS_Boiler.selFlowTemp;
    if (EMS_Boiler.selBurnPow != EMS_VALUE_INT_NOTSET)
        rootBoiler["selBurnPow"] = EMS_Boiler.selBurnPow;
    if (EMS_Boiler.curBurnPow != EMS_VALUE_INT_NOTSET)
        rootBoiler["curBurnPow"] = EMS_Boiler.curBurnPow;
    if (EMS_Boiler.pumpMod != EMS_VALUE_INT_NOTSET)
        rootBoiler["pumpMod"] = EMS_Boiler.pumpMod;

    if (EMS_Boiler.extTemp != EMS_VALUE_SHORT_NOTSET)
        rootBoiler["outdoorTemp"] = (double)EMS_Boiler.extTemp / 10;
    if (EMS_Boiler.wWCurTmp != EMS_VALUE_USHORT_NOTSET)
        rootBoiler["wWCurTmp"] = (double)EMS_Boiler.wWCurTmp / 10;
    if (EMS_Boiler.wWCurFlow != EMS_VALUE_INT_NOTSET)
        rootBoiler["wWCurFlow"] = (double)EMS_Boiler.wWCurFlow / 10;
    if (EMS_Boiler.curFlowTemp != EMS_VALUE_USHORT_NOTSET)
        rootBoiler["curFlowTemp"] = (double)EMS_Boiler.curFlowTemp / 10;
    if (EMS_Boiler.retTemp != EMS_VALUE_USHORT_NOTSET)
        rootBoiler["retTemp"] = (double)EMS_Boiler.retTemp / 10;
    if (EMS_Boiler.sysPress != EMS_VALUE_INT_NOTSET)
        rootBoiler["sysPress"] = (double)EMS_Boiler.sysPress / 10;
    if (EMS_Boiler.boilTemp != EMS_VALUE_USHORT_NOTSET)
        rootBoiler["boilTemp"] = (double)EMS_Boiler.boilTemp / 10;

    if (EMS_Boiler.wWActivated != EMS_VALUE_INT_NOTSET)
        rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);

    if (EMS_Boiler.burnGas != EMS_VALUE_INT_NOTSET)
        rootBoiler["burnGas"] = _bool_to_char(s, EMS_Boiler.burnGas);

    if (EMS_Boiler.heatPmp != EMS_VALUE_INT_NOTSET)
        rootBoiler["heatPmp"] = _bool_to_char(s, EMS_Boiler.heatPmp);

    if (EMS_Boiler.fanWork != EMS_VALUE_INT_NOTSET)
        rootBoiler["fanWork"] = _bool_to_char(s, EMS_Boiler.fanWork);

    if (EMS_Boiler.ignWork != EMS_VALUE_INT_NOTSET)
        rootBoiler["ignWork"] = _bool_to_char(s, EMS_Boiler.ignWork);

    if (EMS_Boiler.wWCirc != EMS_VALUE_INT_NOTSET)
        rootBoiler["wWCirc"] = _bool_to_char(s, EMS_Boiler.wWCirc);

    if (EMS_Boiler.wWHeat != EMS_VALUE_INT_NOTSET)
        rootBoiler["wWHeat"] = _bool_to_char(s, EMS_Boiler.wWHeat);

    // **** also add burnStarts, burnWorkMin, heatWorkMin
    if (abs(EMS_Boiler.burnStarts) != EMS_VALUE_LONG_NOTSET)
        rootBoiler["burnStarts"] = (double)EMS_Boiler.burnStarts;
    if (abs(EMS_Boiler.burnWorkMin) != EMS_VALUE_LONG_NOTSET)
        rootBoiler["burnWorkMin"] = (double)EMS_Boiler.burnWorkMin;
    if (abs(EMS_Boiler.heatWorkMin) != EMS_VALUE_LONG_NOTSET)
        rootBoiler["heatWorkMin"] = (double)EMS_Boiler.heatWorkMin;

    if (EMS_Boiler.serviceCode != EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["ServiceCode"]       = EMS_Boiler.serviceCodeChar;
        rootBoiler["ServiceCodeNumber"] = EMS_Boiler.serviceCode;
    }

    serializeJson(doc, data, sizeof(data));

    // check for empty json
    jsonSize = measureJson(doc);
    if (jsonSize > 2) {
        // calculate hash and send values if something has changed, to save unnecessary wifi traffic
        for (uint8_t i = 0; i < (jsonSize - 1); i++) {
            crc.update(data[i]);
        }
        fchecksum = crc.finalize();
        if ((previousBoilerPublishCRC != fchecksum) || force) {
            previousBoilerPublishCRC = fchecksum;
            myDebugLog("Publishing boiler data via MQTT");

            // send values via MQTT
            myESP.mqttPublish(TOPIC_BOILER_DATA, data);
        }
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    if ((last_boilerActive != ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive)) || force) {
        myDebugLog("Publishing hot water and heating states via MQTT");
        myESP.mqttPublish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive == 1 ? "1" : "0");
        myESP.mqttPublish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive == 1 ? "1" : "0");

        last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
    }

    // handle the thermostat values
    if (ems_getThermostatEnabled()) {
        uint8_t total_active_hc = 0;                           // number of HCs
        bool    hc_1_active     = EMS_Thermostat.hc[0].active; // do we have HC1 active?
        for (uint8_t hc_v = 1; hc_v <= EMS_THERMOSTAT_MAXHC; hc_v++) {
            _EMS_Thermostat_HC * thermostat = &EMS_Thermostat.hc[hc_v - 1];

            // only send if we have an active Heating Circuit with real data
            if ((thermostat->active) && (thermostat->curr_roomTemp != EMS_VALUE_SHORT_NOTSET) && (thermostat->setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)) {
                total_active_hc++; // increase count for #HCs we encounter

                // build new json object
                doc.clear();
                JsonObject rootThermostat = doc.to<JsonObject>();

                rootThermostat[THERMOSTAT_HC] = _int_to_char(s, thermostat->hc); // heating circuit 1..4

                // different logic depending on thermostat types
                if (ems_getThermostatModel() == EMS_MODEL_EASY) {
                    if (thermostat->setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_SELTEMP] = (double)thermostat->setpoint_roomTemp / 100;
                    if (thermostat->curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_CURRTEMP] = (double)thermostat->curr_roomTemp / 100;
                } else if ((ems_getThermostatModel() == EMS_MODEL_FR10) || (ems_getThermostatModel() == EMS_MODEL_FW100)
                           || (ems_getThermostatModel() == EMS_MODEL_FW120)) {
                    if (thermostat->setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_SELTEMP] = (double)thermostat->setpoint_roomTemp / 10;
                    if (thermostat->curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_CURRTEMP] = (double)thermostat->curr_roomTemp / 10;
                } else {
                    if (thermostat->setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_SELTEMP] = (double)thermostat->setpoint_roomTemp / 2;
                    if (thermostat->curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                        rootThermostat[THERMOSTAT_CURRTEMP] = (double)thermostat->curr_roomTemp / 10;

                    if (thermostat->daytemp != EMS_VALUE_INT_NOTSET)
                        rootThermostat[THERMOSTAT_DAYTEMP] = (double)thermostat->daytemp / 2;
                    if (thermostat->nighttemp != EMS_VALUE_INT_NOTSET)
                        rootThermostat[THERMOSTAT_NIGHTTEMP] = (double)thermostat->nighttemp / 2;
                    if (thermostat->holidaytemp != EMS_VALUE_INT_NOTSET)
                        rootThermostat[THERMOSTAT_HOLIDAYTEMP] = (double)thermostat->holidaytemp / 2;

                    if (thermostat->heatingtype != EMS_VALUE_INT_NOTSET)
                        rootThermostat[THERMOSTAT_HEATINGTYPE] = thermostat->heatingtype;

                    if (thermostat->circuitcalctemp != EMS_VALUE_INT_NOTSET)
                        rootThermostat[THERMOSTAT_CIRCUITCALCTEMP] = thermostat->circuitcalctemp;
                }

                uint8_t thermoMode = _getThermostatMode(hc_v); // 0xFF=unknown, 0=low, 1=manual, 2=auto, 3=night, 4=day

                // Termostat Mode
                if (thermoMode == 0) {
                    rootThermostat[THERMOSTAT_MODE] = "low";
                } else if (thermoMode == 1) {
                    rootThermostat[THERMOSTAT_MODE] = "manual";
                } else if (thermoMode == 2) {
                    rootThermostat[THERMOSTAT_MODE] = "auto";
                } else if (thermoMode == 3) {
                    rootThermostat[THERMOSTAT_MODE] = "night";
                } else if (thermoMode == 4) {
                    rootThermostat[THERMOSTAT_MODE] = "day";
                }

                data[0] = '\0'; // reset data for next package
                serializeJson(doc, data, sizeof(data));

                // check for empty json
                jsonSize = measureJson(doc);
                if (jsonSize > 2) {
                    // calculate new CRC
                    crc.reset();
                    for (uint8_t i = 0; i < (jsonSize - 1); i++) {
                        crc.update(data[i]);
                    }
                    fchecksum = crc.finalize();
                    if ((previousThermostatPublishCRC != fchecksum) || force) {
                        previousThermostatPublishCRC = fchecksum;
                        char thermostat_topicname[20];
                        strlcpy(thermostat_topicname, TOPIC_THERMOSTAT_DATA, sizeof(thermostat_topicname)); // "thermostat_data"
                        // if we have more than 1 active Heating Circuit
                        // or this is single HC and its not HC1
                        // append topic name with the HC number
                        if ((total_active_hc > 1) || ((total_active_hc == 1) && (!hc_1_active))) {
                            char buffer[4];
                            strlcat(thermostat_topicname, itoa(total_active_hc, buffer, 10), sizeof(thermostat_topicname));
                        }
                        myDebugLog("Publishing thermostat data via MQTT");
                        myESP.mqttPublish(thermostat_topicname, data);
                    }
                }
            }
        }
    }

    // For SM10 and SM100 Solar Modules
    if (ems_getSolarModuleEnabled()) {
        // build new json object
        doc.clear();
        JsonObject rootSM = doc.to<JsonObject>();

        if (EMS_SolarModule.collectorTemp != EMS_VALUE_SHORT_NOTSET)
            rootSM[SM_COLLECTORTEMP] = (double)EMS_SolarModule.collectorTemp / 10;

        if (EMS_SolarModule.bottomTemp != EMS_VALUE_SHORT_NOTSET)
            rootSM[SM_BOTTOMTEMP] = (double)EMS_SolarModule.bottomTemp / 10;

        if (EMS_SolarModule.pumpModulation != EMS_VALUE_INT_NOTSET)
            rootSM[SM_PUMPMODULATION] = EMS_SolarModule.pumpModulation;

        if (EMS_SolarModule.pump != EMS_VALUE_INT_NOTSET) {
            rootSM[SM_PUMP] = _bool_to_char(s, EMS_SolarModule.pump);
        }

        if (EMS_SolarModule.pumpWorkMin != EMS_VALUE_LONG_NOTSET) {
            rootSM[SM_PUMPWORKMIN] = (double)EMS_SolarModule.pumpWorkMin;
        }

        if (EMS_SolarModule.EnergyLastHour != EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYLASTHOUR] = (double)EMS_SolarModule.EnergyLastHour / 10;

        if (EMS_SolarModule.EnergyToday != EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYTODAY] = EMS_SolarModule.EnergyToday;

        if (EMS_SolarModule.EnergyTotal != EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYTOTAL] = (double)EMS_SolarModule.EnergyTotal / 10;

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));

        // check for empty json
        jsonSize = measureJson(doc);
        if (jsonSize > 2) {
            // calculate new CRC
            crc.reset();
            for (uint8_t i = 0; i < (jsonSize - 1); i++) {
                crc.update(data[i]);
            }
            fchecksum = crc.finalize();
            if ((previousSMPublishCRC != fchecksum) || force) {
                previousSMPublishCRC = fchecksum;
                myDebugLog("Publishing SM data via MQTT");

                // send values via MQTT
                myESP.mqttPublish(TOPIC_SM_DATA, data);
            }
        }
    }

    // handle HeatPump
    if (ems_getHeatPumpEnabled()) {
        // build new json object
        doc.clear();
        JsonObject rootSM = doc.to<JsonObject>();

        if (EMS_HeatPump.HPModulation != EMS_VALUE_INT_NOTSET)
            rootSM[HP_PUMPMODULATION] = EMS_HeatPump.HPModulation;

        if (EMS_HeatPump.HPSpeed != EMS_VALUE_INT_NOTSET)
            rootSM[HP_PUMPSPEED] = EMS_HeatPump.HPSpeed;

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));

        myDebugLog("Publishing HeatPump data via MQTT");

        // send values via MQTT
        myESP.mqttPublish(TOPIC_HP_DATA, data);
    }
}

// sets the shower timer on/off
void set_showerTimer() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug_P(PSTR("Shower timer has been set to %s"), EMSESP_Settings.shower_timer ? "enabled" : "disabled");
    }
}

// sets the shower alert on/off
void set_showerAlert() {
    if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
        myDebug_P(PSTR("Shower alert has been set to %s"), EMSESP_Settings.shower_alert ? "enabled" : "disabled");
    }
}

// used to read the next string from an input buffer and convert to an 8 bit int
uint8_t _readIntNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atoi(numTextPtr);
}

// used to read the next string from an input buffer and convert to a double
float _readFloatNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atof(numTextPtr);
}

// used to read the next string from an input buffer as a hex value and convert to a 16 bit int
uint16_t _readHexNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return (uint16_t)strtol(numTextPtr, 0, 16);
}

// used to read the next string from an input buffer
char * _readWord() {
    char * word = strtok(nullptr, ", \n");
    return word;
}

// publish external dallas sensor temperature values to MQTT
void do_publishSensorValues() {
    if ((EMSESP_Settings.dallas_sensors != 0) && (EMSESP_Settings.publish_time != 0)) {
        publishSensorValues();
    }
}

// call PublishValues without forcing, so using CRC to see if we really need to publish
void do_publishValues() {
    // don't publish if we're not connected to the EMS bus
    if ((ems_getBusConnected()) && myESP.isMQTTConnected() && EMSESP_Settings.publish_time != 0) {
        publishValues(true); // force publish
    }
}

// callback to light up the LED, called via Ticker every second
// when ESP is booting up, ignore this as the LED is being used for something else
void do_ledcheck() {
    if ((EMSESP_Settings.led) && (myESP.getSystemBootStatus() == MYESP_BOOTSTATUS_BOOTED)) {
        if (ems_getBusConnected()) {
            digitalWrite(EMSESP_Settings.led_gpio, (EMSESP_Settings.led_gpio == LED_BUILTIN) ? LOW : HIGH); // light on. For onboard LED high=off
        } else {
            int state = digitalRead(EMSESP_Settings.led_gpio);
            digitalWrite(EMSESP_Settings.led_gpio, !state);
        }
    }
}

// Thermostat scan
void do_scanThermostat() {
    if (ems_getBusConnected()) {
        myDebug_P(PSTR("> Scanning thermostat message type #0x%02X..."), scanThermostat_count);
        ems_doReadCommand(scanThermostat_count, EMS_Thermostat.device_id);
        scanThermostat_count++;
    }
}

// do a system health check every now and then to see if we all connections
void do_systemCheck() {
    if (!ems_getBusConnected() && !myESP.getUseSerial()) {
        myDebug_P(PSTR("Error! Unable to read the EMS bus."));
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
// only if we have a EMS connection
void do_regularUpdates() {
    if (ems_getBusConnected() && !ems_getTxDisabled()) {
        myDebugLog("Starting scheduled query from EMS devices");
        ems_getThermostatValues();
        ems_getBoilerValues();
        ems_getSolarModuleValues();
    }
}

// stop devices scan and restart all other timers
void stopDeviceScan() {
    publishValuesTimer.attach(EMSESP_Settings.publish_time, do_publishValues);             // post MQTT EMS values
    publishSensorValuesTimer.attach(EMSESP_Settings.publish_time, do_publishSensorValues); // post MQTT sensor values
    regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates);                    // regular reads from the EMS
    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck);                             // check if Boiler is online
    scanThermostat_count = 0;
    scanThermostat.detach();
}

// EMS device scan
void do_scanDevices() {
    if (scanDevices_count == 0) {
        // we're at the finish line
        myDebug_P(PSTR("Finished the deep EMS device scan."));
        stopDeviceScan();
        ems_printDevices();
        ems_setLogging(EMS_SYS_LOGGING_NONE);
        return;
    }

    if (ems_getBusConnected()) {
        ems_doReadCommand(EMS_TYPE_Version, scanDevices_count++); // ask for version
    }
}

// initiate a force scan by sending a version command to all type ids
void startDeviceScan() {
    publishValuesTimer.detach();
    systemCheckTimer.detach();
    regularUpdatesTimer.detach();
    publishSensorValuesTimer.detach();
    scanDevices_count = 1; // starts at 1
    ems_clearDeviceList(); // empty the current list
    ems_setLogging(EMS_SYS_LOGGING_NONE);
    myDebug_P(PSTR("Starting a deep EMS device scan. This can take up to 2 minutes. Please wait..."));
    scanThermostat.attach_ms(SCANDEVICES_TIME, do_scanDevices);
}

// initiate a force scan by sending type read requests from 0 to FF to the thermostat
// used to analyze responses for debugging
void startThermostatScan(uint8_t start) {
    ems_setLogging(EMS_SYS_LOGGING_THERMOSTAT);
    publishValuesTimer.detach();
    systemCheckTimer.detach();
    regularUpdatesTimer.detach();
    scanThermostat_count = start;
    myDebug_P(PSTR("Starting a deep message scan on thermostat"));
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
    if (EMSESP_Settings.shower_alert) {
        myDebugLog("[Shower] doing a shot of cold water");
        ems_setWarmTapWaterActivated(false);
        EMSESP_Shower.doingColdShot = true;
        // start the timer for n seconds which will reset the water back to hot
        showerColdShotStopTimer.attach(SHOWER_COLDSHOT_DURATION, _showerColdShotStop);
    }
}

// run tests to validate handling of telegrams by injecting fake telegrams
void runUnitTest(uint8_t test_num) {
    ems_setLogging(EMS_SYS_LOGGING_VERBOSE);
    publishValuesTimer.detach();
    systemCheckTimer.detach();
    regularUpdatesTimer.detach();
    // EMSESP_Settings.listen_mode = true; // temporary go into listen mode to disable Tx
    ems_testTelegram(test_num);
}

// callback for loading/saving settings to the file system (SPIFFS)
bool LoadSaveCallback(MYESP_FSACTION action, JsonObject settings) {
    if (action == MYESP_FSACTION_LOAD) {
        // check for valid json
        if (settings.isNull()) {
            myDebug_P(PSTR("Error processing json settings"));
            return false;
        }

        // serializeJsonPretty(settings, Serial); // for debugging

        EMSESP_Settings.led             = settings["led"];
        EMSESP_Settings.led_gpio        = settings["led_gpio"] | EMSESP_LED_GPIO;
        EMSESP_Settings.dallas_gpio     = settings["dallas_gpio"] | EMSESP_DALLAS_GPIO;
        EMSESP_Settings.dallas_parasite = settings["dallas_parasite"] | EMSESP_DALLAS_PARASITE;
        EMSESP_Settings.shower_timer    = settings["shower_timer"];
        EMSESP_Settings.shower_alert    = settings["shower_alert"];
        EMSESP_Settings.publish_time    = settings["publish_time"] | DEFAULT_PUBLISHTIME;

        EMSESP_Settings.listen_mode = settings["listen_mode"];
        ems_setTxDisabled(EMSESP_Settings.listen_mode);

        EMSESP_Settings.tx_mode = settings["tx_mode"] | EMS_TXMODE_DEFAULT; // default to 1 (generic)
        ems_setTxMode(EMSESP_Settings.tx_mode);

        return true;
    }

    if (action == MYESP_FSACTION_SAVE) {
        settings["led"]             = EMSESP_Settings.led;
        settings["led_gpio"]        = EMSESP_Settings.led_gpio;
        settings["dallas_gpio"]     = EMSESP_Settings.dallas_gpio;
        settings["dallas_parasite"] = EMSESP_Settings.dallas_parasite;
        settings["listen_mode"]     = EMSESP_Settings.listen_mode;
        settings["shower_timer"]    = EMSESP_Settings.shower_timer;
        settings["shower_alert"]    = EMSESP_Settings.shower_alert;
        settings["publish_time"]    = EMSESP_Settings.publish_time;
        settings["tx_mode"]         = EMSESP_Settings.tx_mode;

        return true;
    }

    return false;
}

// callback for custom settings when showing Stored Settings with the 'set' command
// wc is number of arguments after the 'set' command
// returns true if the setting was recognized and changed and should be saved back to SPIFFs
bool SetListCallback(MYESP_FSACTION action, uint8_t wc, const char * setting, const char * value) {
    bool ok = false;

    if (action == MYESP_FSACTION_SET) {
        // led
        if ((strcmp(setting, "led") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.led = true;
                ok                  = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.led = false;
                ok                  = true;
                // let's make sure LED is really off - For onboard high=off
                digitalWrite(EMSESP_Settings.led_gpio, (EMSESP_Settings.led_gpio == LED_BUILTIN) ? HIGH : LOW);
            } else {
                myDebug_P(PSTR("Error. Usage: set led <on | off>"));
            }
        }

        // test mode
        if ((strcmp(setting, "listen_mode") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.listen_mode = true;
                ok                          = true;
                myDebug_P(PSTR("* in listen mode. All Tx is disabled."));
                ems_setTxDisabled(true);
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.listen_mode = false;
                ok                          = true;
                ems_setTxDisabled(false);
                myDebug_P(PSTR("* out of listen mode. Tx is now enabled."));
            } else {
                myDebug_P(PSTR("Error. Usage: set listen_mode <on | off>"));
            }
        }

        // led_gpio
        if ((strcmp(setting, "led_gpio") == 0) && (wc == 2)) {
            EMSESP_Settings.led_gpio = atoi(value);
            // reset pin
            pinMode(EMSESP_Settings.led_gpio, OUTPUT);
            digitalWrite(EMSESP_Settings.led_gpio, (EMSESP_Settings.led_gpio == LED_BUILTIN) ? HIGH : LOW); // light off. For onboard high=off
            ok = true;
        }

        // dallas_gpio
        if ((strcmp(setting, "dallas_gpio") == 0) && (wc == 2)) {
            EMSESP_Settings.dallas_gpio = atoi(value);
            ok                          = true;
        }

        // dallas_parasite
        if ((strcmp(setting, "dallas_parasite") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.dallas_parasite = true;
                ok                              = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.dallas_parasite = false;
                ok                              = true;
            } else {
                myDebug_P(PSTR("Error. Usage: set dallas_parasite <on | off>"));
            }
        }

        // shower timer
        if ((strcmp(setting, "shower_timer") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.shower_timer = true;
                ok                           = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.shower_timer = false;
                ok                           = true;
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_timer <on | off>"));
            }
        }

        // shower alert
        if ((strcmp(setting, "shower_alert") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.shower_alert = true;
                ok                           = true;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.shower_alert = false;
                ok                           = true;
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_alert <on | off>"));
            }
        }

        // publish_time
        if ((strcmp(setting, "publish_time") == 0) && (wc == 2)) {
            EMSESP_Settings.publish_time = atoi(value);
            ok                           = true;
        }

        // tx_mode
        if ((strcmp(setting, "tx_mode") == 0) && (wc == 2)) {
            uint8_t mode = atoi(value);
            if ((mode >= 1) && (mode <= 3)) { // see ems.h for definitions
                EMSESP_Settings.tx_mode = mode;
                ems_setTxMode(mode);
                ok = true;
            } else {
                myDebug_P(PSTR("Error. Usage: set tx_mode <1 | 2 | 3>"));
            }
        }
    }

    if (action == MYESP_FSACTION_LIST) {
        myDebug_P(PSTR("  led=%s"), EMSESP_Settings.led ? "on" : "off");
        myDebug_P(PSTR("  led_gpio=%d"), EMSESP_Settings.led_gpio);
        myDebug_P(PSTR("  dallas_gpio=%d"), EMSESP_Settings.dallas_gpio);
        myDebug_P(PSTR("  dallas_parasite=%s"), EMSESP_Settings.dallas_parasite ? "on" : "off");
        myDebug_P(PSTR("  tx_mode=%d"), EMSESP_Settings.tx_mode);
        myDebug_P(PSTR("  listen_mode=%s"), EMSESP_Settings.listen_mode ? "on" : "off");
        myDebug_P(PSTR("  shower_timer=%s"), EMSESP_Settings.shower_timer ? "on" : "off");
        myDebug_P(PSTR("  shower_alert=%s"), EMSESP_Settings.shower_alert ? "on" : "off");
        myDebug_P(PSTR("  publish_time=%d"), EMSESP_Settings.publish_time);
    }

    return ok;
}

// print settings
void _showCommands(uint8_t event) {
    bool      mode = (event == TELNET_EVENT_SHOWSET); // show set commands or normal commands
    command_t cmd;

    // find the longest key length so we can right-align the text
    uint8_t max_len = 0;
    uint8_t i;
    for (i = 0; i < _project_cmds_count; i++) {
        memcpy_P(&cmd, &project_cmds[i], sizeof(cmd));
        if ((strlen(cmd.key) > max_len) && (cmd.set == mode)) {
            max_len = strlen(cmd.key);
        }
    }

    char line[200] = {0};
    for (i = 0; i < _project_cmds_count; i++) {
        memcpy_P(&cmd, &project_cmds[i], sizeof(cmd));
        if (cmd.set == mode) {
            if (event == TELNET_EVENT_SHOWSET) {
                strlcpy(line, "  set ", sizeof(line));
            } else {
                strlcpy(line, "*  ", sizeof(line));
            }
            strlcat(line, cmd.key, sizeof(line));
            for (uint8_t j = 0; j < ((max_len + 5) - strlen(cmd.key)); j++) { // account for longest string length
                strlcat(line, " ", sizeof(line));                             // padding
            }
            strlcat(line, cmd.description, sizeof(line));
            myDebug(line); // print the line
        }
    }
}

// call back when a telnet client connects or disconnects
// we set the logging here
void TelnetCallback(uint8_t event) {
    if (event == TELNET_EVENT_CONNECT) {
        ems_setLogging(EMS_SYS_LOGGING_DEFAULT);
    } else if (event == TELNET_EVENT_DISCONNECT) {
        ems_setLogging(EMS_SYS_LOGGING_NONE);
    } else if ((event == TELNET_EVENT_SHOWCMD) || (event == TELNET_EVENT_SHOWSET)) {
        _showCommands(event);
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
        do_regularUpdates();
        ok = true;
    }

    if (strcmp(first_cmd, "devices") == 0) {
        ems_printAllDevices();
        ok = true;
    }

    if (strcmp(first_cmd, "queue") == 0) {
        ems_printTxQueue();
        ok = true;
    }

    if (strcmp(first_cmd, "autodetect") == 0) {
        if (wc == 2) {
            char * second_cmd = _readWord();
            if (strcmp(second_cmd, "deep") == 0) {
                startDeviceScan();
                ok = true;
            }
        } else {
            ems_scanDevices();
            ok = true;
        }
    }

    // shower settings
    if ((strcmp(first_cmd, "shower") == 0) && (wc == 2)) {
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "timer") == 0) {
            EMSESP_Settings.shower_timer = !EMSESP_Settings.shower_timer;
            myESP.mqttPublish(TOPIC_SHOWER_TIMER, EMSESP_Settings.shower_timer ? "1" : "0");
            ok = true;
        } else if (strcmp(second_cmd, "alert") == 0) {
            EMSESP_Settings.shower_alert = !EMSESP_Settings.shower_alert;
            myESP.mqttPublish(TOPIC_SHOWER_ALERT, EMSESP_Settings.shower_alert ? "1" : "0");
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
        } else if (strcmp(second_cmd, "s") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_SOLARMODULE);
            ok = true;
        } else if (strcmp(second_cmd, "r") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_RAW);
            ok = true;
        } else if (strcmp(second_cmd, "n") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_NONE);
            ok = true;
        } else if (strcmp(second_cmd, "j") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_JABBER);
            ok = true;
        }
    }

    // thermostat commands
    if ((strcmp(first_cmd, "thermostat") == 0) && (wc >= 3)) {
        char *  second_cmd = _readWord();
        uint8_t hc         = EMS_THERMOSTAT_DEFAULTHC;

        if (strcmp(second_cmd, "temp") == 0) {
            if (wc == 4) {
                hc = _readIntNumber(); // next parameter is the heating circuit
            }
            ems_setThermostatTemp(_readFloatNumber(), hc);
            ok = true;
        } else if (strcmp(second_cmd, "mode") == 0) {
            if (wc == 4) {
                hc = _readIntNumber(); // next parameter is the heating circuit
            }
            ems_setThermostatMode(_readIntNumber(), hc);
            ok = true;
        } else if (strcmp(second_cmd, "read") == 0) {
            ems_doReadCommand(_readHexNumber(), EMS_Thermostat.device_id);
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
            ems_doReadCommand(_readHexNumber(), EMS_Boiler.device_id);
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
        } else if (strcmp(second_cmd, "flowtemp") == 0) {
            ems_setFlowTemp(_readIntNumber());
            ok = true;
        }
    }

    // send raw
    if ((strcmp(first_cmd, "send") == 0) && (wc > 1)) {
        ems_sendRawTelegram((char *)&commandLine[5]);
        ok = true;
    }

    // test commands
    if ((strcmp(first_cmd, "test") == 0) && (wc == 2)) {
        runUnitTest(_readIntNumber());
        ok = true;
    }

    // check for invalid command
    if (!ok) {
        myDebug_P(PSTR("Unknown command or wrong number of arguments. Use ? for help."));
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


// see's if a topic is appended with an interger value
// used to identify a heating circuit
// returns HC number
uint8_t _hasHCspecified(const char * topic, const char * input) {
    int orig_len = strlen(topic); // original length of the topic we're comparing too

    // check if the strings match ignoring any suffix
    if (strncmp(input, topic, orig_len) == 0) {
        // see if we have additional chars at the end, we want none or 1
        uint8_t diff = (strlen(input) - orig_len);
        if (diff > 1) {
            return 0; // invalid
        }

        if (diff == 0) {
            return EMS_THERMOSTAT_DEFAULTHC; // identical, use default
        }

        // return the value of the last char, 0-9
        return input[orig_len] - '0';
    }

    return 0; // invalid
}

// MQTT Callback to handle incoming/outgoing changes
void MQTTCallback(unsigned int type, const char * topic, const char * message) {
    // we're connected. lets subscribe to some topics
    if (type == MQTT_CONNECT_EVENT) {
        // subscribe to the 4 heating circuits
        char topic_s[50];
        char buffer[4];

        // subscribe to the normal batch to be backwards compatible
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_TEMP);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_MODE);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_DAYTEMP);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_NIGHTTEMP);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP);

        for (uint8_t hc = 1; hc <= EMS_THERMOSTAT_MAXHC; hc++) {
            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_TEMP, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s)); // add 1-4 at the end
            myESP.mqttSubscribe(topic_s);

            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_MODE, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s)); // add 1-4 at the end
            myESP.mqttSubscribe(topic_s);

            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_DAYTEMP, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s)); // add 1-4 at the end
            myESP.mqttSubscribe(topic_s);

            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_NIGHTTEMP, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s)); // add 1-4 at the end
            myESP.mqttSubscribe(topic_s);

            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s)); // add 1-4 at the end
            myESP.mqttSubscribe(topic_s);
        }

        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWACTIVATED);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWTEMP);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_COMFORT);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_FLOWTEMP);

        myESP.mqttSubscribe(TOPIC_SHOWER_TIMER);
        myESP.mqttSubscribe(TOPIC_SHOWER_ALERT);
        myESP.mqttSubscribe(TOPIC_SHOWER_COLDSHOT);

        // publish the status of the Shower parameters
        myESP.mqttPublish(TOPIC_SHOWER_TIMER, EMSESP_Settings.shower_timer ? "1" : "0");
        myESP.mqttPublish(TOPIC_SHOWER_ALERT, EMSESP_Settings.shower_alert ? "1" : "0");
    }

    // handle incoming MQTT publish events
    if (type == MQTT_MESSAGE_EVENT) {
        uint8_t hc;
        // thermostat temp changes
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_TEMP, topic);
        if (hc) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug_P(PSTR("MQTT topic: thermostat HC%d temperature value %s"), hc, _float_to_char(s, f));
            ems_setThermostatTemp(f, hc);
            publishValues(true); // publish back immediately
            return;
        }

        // thermostat mode changes
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_MODE, topic);
        if (hc) {
            myDebug_P(PSTR("MQTT topic: thermostat HC%d mode value %s"), hc, message);
            if (strcmp((char *)message, "auto") == 0) {
                ems_setThermostatMode(2, hc);
            } else if (strcmp((char *)message, "day") == 0 || (strcmp((char *)message, "manual") == 0) || (strcmp((char *)message, "heat") == 0)) {
                ems_setThermostatMode(1, hc);
            } else if (strcmp((char *)message, "night") == 0 || strcmp((char *)message, "off") == 0) {
                ems_setThermostatMode(0, hc);
            }
            return;
        }

        // set night temp value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_NIGHTTEMP, topic);
        if (hc) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug_P(PSTR("MQTT topic: new thermostat HC%d night temperature value %s"), hc, _float_to_char(s, f));
            ems_setThermostatTemp(f, hc, 1); // night
            return;
        }

        // set daytemp value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_DAYTEMP, topic);
        if (hc) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug_P(PSTR("MQTT topic: new thermostat HC%d day temperature value %s"), hc, _float_to_char(s, f));
            ems_setThermostatTemp(f, hc, 2); // day
            return;
        }

        // set holiday value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP, topic);
        if (hc) {
            float f     = strtof((char *)message, 0);
            char  s[10] = {0};
            myDebug_P(PSTR("MQTT topic: new thermostat HC%d holiday temperature value %s"), hc, _float_to_char(s, f));
            ems_setThermostatTemp(f, hc, 3); // holiday
            return;
        }

        // wwActivated
        if (strcmp(topic, TOPIC_BOILER_CMD_WWACTIVATED) == 0) {
            if ((message[0] == '1' || strcmp(message, "on") == 0) || (strcmp(message, "auto") == 0)) {
                ems_setWarmWaterActivated(true);
            } else if (message[0] == '0' || strcmp(message, "off") == 0) {
                ems_setWarmWaterActivated(false);
            }
            return;
        }

        // boiler wwtemp changes
        if (strcmp(topic, TOPIC_BOILER_CMD_WWTEMP) == 0) {
            uint8_t t = atoi((char *)message);
            myDebug_P(PSTR("MQTT topic: boiler warm water temperature value %d"), t);
            ems_setWarmWaterTemp(t);
            publishValues(true); // publish back immediately, can't remember why I do this?!
            return;
        }

        // boiler ww comfort setting
        if (strcmp(topic, TOPIC_BOILER_CMD_COMFORT) == 0) {
            myDebug_P(PSTR("MQTT topic: boiler warm water comfort value is %s"), message);
            if (strcmp((char *)message, "hot") == 0) {
                ems_setWarmWaterModeComfort(1);
            } else if (strcmp((char *)message, "comfort") == 0) {
                ems_setWarmWaterModeComfort(2);
            } else if (strcmp((char *)message, "intelligent") == 0) {
                ems_setWarmWaterModeComfort(3);
            }
            return;
        }

        // boiler flowtemp setting
        if (strcmp(topic, TOPIC_BOILER_CMD_FLOWTEMP) == 0) {
            uint8_t t = atoi((char *)message);
            myDebug_P(PSTR("MQTT topic: boiler flowtemp value %d"), t);
            ems_setFlowTemp(t);
            return;
        }

        // shower timer
        if (strcmp(topic, TOPIC_SHOWER_TIMER) == 0) {
            if (message[0] == '1') {
                EMSESP_Settings.shower_timer = true;
            } else if (message[0] == '0') {
                EMSESP_Settings.shower_timer = false;
            }
            set_showerTimer();
            return;
        }

        // shower alert
        if (strcmp(topic, TOPIC_SHOWER_ALERT) == 0) {
            if (message[0] == '1') {
                EMSESP_Settings.shower_alert = true;
            } else if (message[0] == '0') {
                EMSESP_Settings.shower_alert = false;
            }
            set_showerAlert();
            return;
        }

        // shower cold shot
        if (strcmp(topic, TOPIC_SHOWER_COLDSHOT) == 0) {
            _showerColdShotStart();
            return;
        }
    }
}

// Init callback, which is used to set functions and call methods after a wifi connection has been established
void WIFICallback() {
    // This is where we enable the UART service to scan the incoming serial Tx/Rx bus signals
    // This is done after we have a WiFi signal to avoid any resource conflicts
    // system_uart_swap();
}

// web information for diagnostics
void WebCallback(JsonObject root) {
    JsonObject emsbus = root.createNestedObject("emsbus");

    if (myESP.getUseSerial()) {
        emsbus["ok"]  = false;
        emsbus["msg"] = "EMS Bus is disabled when in Serial mode. Check Settings->General Settings->Serial Port";
    } else {
        if (ems_getBusConnected()) {
            if (ems_getTxDisabled()) {
                emsbus["ok"]  = false;
                emsbus["msg"] = "EMS Bus Connected with Rx active but Tx has been disabled (in listen only mode).";
            } else if (ems_getTxCapable()) {
                emsbus["ok"]  = true;
                emsbus["msg"] = "EMS Bus Connected with both Rx and Tx active.";
            } else {
                emsbus["ok"]  = false;
                emsbus["msg"] = "EMS Bus Connected but Tx is not working.";
            }
        } else {
            emsbus["ok"]  = false;
            emsbus["msg"] = "EMS Bus is not connected. Check event logs for errors.";
        }
    }

    // send over EMS devices
    JsonArray list = emsbus.createNestedArray("devices");

    for (std::list<_Generic_Device>::iterator it = Devices.begin(); it != Devices.end(); ++it) {
        JsonObject item   = list.createNestedObject();
        item["type"]      = (it)->model_type;
        item["model"]     = (it)->model_string;
        item["version"]   = (it)->version;
        item["productid"] = (it)->product_id;

        char s[10];
        itoa((it)->device_id, s, 16); // convert to hex
        item["deviceid"] = s;
    }

    // send over Thermostat data
    JsonObject thermostat = root.createNestedObject("thermostat");

    if (ems_getThermostatEnabled()) {
        thermostat["ok"] = true;

        char buffer[200];
        thermostat["tm"] = ems_getThermostatDescription(buffer, true);

        uint8_t hc_num = EMS_THERMOSTAT_DEFAULTHC; // default to HC1

        // Render Current & Setpoint Room Temperature
        if (ems_getThermostatModel() == EMS_MODEL_EASY) {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (double)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 100;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (double)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 100;
        } else if ((ems_getThermostatModel() == EMS_MODEL_FR10) || (ems_getThermostatModel() == EMS_MODEL_FW100)
                   || (ems_getThermostatModel() == EMS_MODEL_FW120)) {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (double)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 10;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (double)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 10;
        } else {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (double)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 2;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp != EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (double)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 10;
        }

        // Render Termostat Mode, if we have a mode
        uint8_t thermoMode = _getThermostatMode(hc_num); // 0xFF=unknown, 0=low, 1=manual, 2=auto, 3=night, 4=day
        if (thermoMode == 0) {
            thermostat["tmode"] = "low";
        } else if (thermoMode == 1) {
            thermostat["tmode"] = "manual";
        } else if (thermoMode == 2) {
            thermostat["tmode"] = "auto";
        } else if (thermoMode == 3) {
            thermostat["tmode"] = "night";
        } else if (thermoMode == 4) {
            thermostat["tmode"] = "day";
        }
    } else {
        thermostat["ok"] = false;
    }

    JsonObject boiler = root.createNestedObject("boiler");
    if (ems_getBoilerEnabled()) {
        boiler["ok"] = true;

        char buffer[200];
        boiler["bm"] = ems_getBoilerDescription(buffer, true);

        boiler["b1"] = (EMS_Boiler.tapwaterActive ? "running" : "off");
        boiler["b2"] = (EMS_Boiler.heatingActive ? "active" : "off");

        if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET)
            boiler["b3"] = EMS_Boiler.selFlowTemp;

        if (EMS_Boiler.curFlowTemp != EMS_VALUE_INT_NOTSET)
            boiler["b4"] = EMS_Boiler.curFlowTemp / 10;

        if (EMS_Boiler.boilTemp != EMS_VALUE_USHORT_NOTSET)
            boiler["b5"] = (double)EMS_Boiler.boilTemp / 10;

        if (EMS_Boiler.retTemp != EMS_VALUE_USHORT_NOTSET)
            boiler["b6"] = (double)EMS_Boiler.retTemp / 10;

    } else {
        boiler["ok"] = false;
    }

    // For SM10/SM100 Solar Module
    JsonObject sm = root.createNestedObject("sm");
    if (ems_getSolarModuleEnabled()) {
        sm["ok"] = true;

        char buffer[200];
        sm["sm"] = ems_getSolarModuleDescription(buffer, true);

        if (EMS_SolarModule.collectorTemp != EMS_VALUE_SHORT_NOTSET)
            sm["sm1"] = (double)EMS_SolarModule.collectorTemp / 10; // Collector temperature oC

        if (EMS_SolarModule.bottomTemp != EMS_VALUE_SHORT_NOTSET)
            sm["sm2"] = (double)EMS_SolarModule.bottomTemp / 10; // Bottom temperature oC

        if (EMS_SolarModule.pumpModulation != EMS_VALUE_INT_NOTSET)
            sm["sm3"] = EMS_SolarModule.pumpModulation; // Pump modulation %

        if (EMS_SolarModule.pump != EMS_VALUE_INT_NOTSET) {
            char s[10];
            sm["sm4"] = _bool_to_char(s, EMS_SolarModule.pump); // Pump active on/off
        }

        if (EMS_SolarModule.EnergyLastHour != EMS_VALUE_USHORT_NOTSET)
            sm["sm5"] = (double)EMS_SolarModule.EnergyLastHour / 10; // Energy last hour Wh

        if (EMS_SolarModule.EnergyToday != EMS_VALUE_USHORT_NOTSET) // Energy today Wh
            sm["sm6"] = EMS_SolarModule.EnergyToday;

        if (EMS_SolarModule.EnergyTotal != EMS_VALUE_USHORT_NOTSET) // Energy total KWh
            sm["sm7"] = (double)EMS_SolarModule.EnergyTotal / 10;
    } else {
        sm["ok"] = false;
    }

    // For HeatPumps
    JsonObject hp = root.createNestedObject("hp");
    if (ems_getHeatPumpEnabled()) {
        hp["ok"] = true;
        char buffer[200];
        hp["hm"] = ems_getHeatPumpDescription(buffer, true);

        if (EMS_HeatPump.HPModulation != EMS_VALUE_INT_NOTSET)
            hp["hp1"] = EMS_HeatPump.HPModulation; // Pump modulation %

        if (EMS_HeatPump.HPSpeed != EMS_VALUE_INT_NOTSET)
            hp["hp2"] = EMS_HeatPump.HPSpeed; // Pump speed %
    } else {
        hp["ok"] = false;
    }


    // serializeJsonPretty(root, Serial); // turn on for debugging
}

// Initialize the boiler settings and shower settings
// Most of these will be overwritten after the SPIFFS config file is loaded
void initEMSESP() {
    // general settings
    EMSESP_Settings.shower_timer   = false;
    EMSESP_Settings.shower_alert   = false;
    EMSESP_Settings.led            = true; // LED is on by default
    EMSESP_Settings.listen_mode    = false;
    EMSESP_Settings.publish_time   = DEFAULT_PUBLISHTIME;
    EMSESP_Settings.timestamp      = millis();
    EMSESP_Settings.dallas_sensors = 0;
    EMSESP_Settings.led_gpio       = EMSESP_LED_GPIO;
    EMSESP_Settings.dallas_gpio    = EMSESP_DALLAS_GPIO;
    EMSESP_Settings.tx_mode        = EMS_TXMODE_DEFAULT; // default tx mode

    // shower settings
    EMSESP_Shower.timerStart    = 0;
    EMSESP_Shower.timerPause    = 0;
    EMSESP_Shower.duration      = 0;
    EMSESP_Shower.doingColdShot = false;

    // call ems.cpp's init function to set all the internal params
    ems_init();
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
                EMSESP_Shower.timerStart    = EMSESP_Settings.timestamp;
                EMSESP_Shower.timerPause    = 0; // remove any last pauses
                EMSESP_Shower.doingColdShot = false;
                EMSESP_Shower.duration      = 0;
                EMSESP_Shower.showerOn      = false;
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!EMSESP_Shower.showerOn && (EMSESP_Settings.timestamp - EMSESP_Shower.timerStart) > SHOWER_MIN_DURATION) {
                    EMSESP_Shower.showerOn = true;
                    myDebugLog("[Shower] hot water still running, starting shower timer");
                }
                // check if the shower has been on too long
                else if ((((EMSESP_Settings.timestamp - EMSESP_Shower.timerStart) > SHOWER_MAX_DURATION) && !EMSESP_Shower.doingColdShot)
                         && EMSESP_Settings.shower_alert) {
                    myDebugLog("[Shower] exceeded max shower time");
                    _showerColdShotStart();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if ((EMSESP_Shower.timerStart != 0) && (EMSESP_Shower.timerPause == 0)) {
                EMSESP_Shower.timerPause = EMSESP_Settings.timestamp;
            }

            // if shower has been off for longer than the wait time
            if ((EMSESP_Shower.timerPause != 0) && ((EMSESP_Settings.timestamp - EMSESP_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
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
                            myDebug_P(PSTR("[Shower] finished with duration %s"), s);
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
    // LA trigger create a small puls to show setup is starting...
    INIT_MARKERS(0);
    LA_PULSE(50);

    // GPIO15 has a pull down, so we must set it to HIGH
    pinMode(15, OUTPUT);
    digitalWrite(15, 1);

    // init our own parameters
    initEMSESP();

    // call ems.cpp's init function to set all the internal params
    ems_init();

    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck); // check if EMS is reachable

    // set up myESP for Wifi, MQTT, MDNS and Telnet callbacks
    myESP.setTelnet(TelnetCommandCallback, TelnetCallback);      // set up Telnet commands
    myESP.setWIFI(WIFICallback);                                 // wifi callback
    myESP.setMQTT(MQTTCallback);                                 // MQTT ip, username and password taken from the SPIFFS settings
    myESP.setSettings(LoadSaveCallback, SetListCallback, false); // default is Serial off
    myESP.setWeb(WebCallback);                                   // web custom settings
    myESP.setOTA(OTACallback_pre, OTACallback_post);             // OTA callback which is called when OTA is starting and stopping
    myESP.begin(APP_HOSTNAME, APP_NAME, APP_VERSION, APP_URL, APP_UPDATEURL);

    // at this point we have all the settings from our internall SPIFFS config file
    // fire up the UART now
    if (myESP.getUseSerial()) {
        myDebug_P(PSTR("Warning! EMS bus communication disabled when Serial mode enabled. Use 'set serial off' to start communication."));
    } else {
        Serial.println("Note: Serial output will now be disabled. Please use Telnet.");
        Serial.flush();
        myESP.setUseSerial(false);
        emsuart_init(); // start EMS bus transmissions
        myDebug_P(PSTR("[UART] Opened Rx/Tx connection"));
        if (!EMSESP_Settings.listen_mode) {
            // go and find the boiler and thermostat types, if not in listen mode
            ems_discoverModels();
        }
    }

    // enable regular checks
    if (!EMSESP_Settings.listen_mode) {
        regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates); // regular reads from the EMS
    }

    // set timers for MQTT publish
    if (EMSESP_Settings.publish_time != 0) {
        publishValuesTimer.attach(EMSESP_Settings.publish_time, do_publishValues);             // post MQTT EMS values
        publishSensorValuesTimer.attach(EMSESP_Settings.publish_time, do_publishSensorValues); // post MQTT dallas sensor values
    }

    // set pin for LED
    if (EMSESP_Settings.led_gpio != EMS_VALUE_INT_NOTSET) {
        pinMode(EMSESP_Settings.led_gpio, OUTPUT);
        digitalWrite(EMSESP_Settings.led_gpio, (EMSESP_Settings.led_gpio == LED_BUILTIN) ? HIGH : LOW); // light off. For onboard high=off
        ledcheckTimer.attach_ms(LEDCHECK_TIME, do_ledcheck);                                            // blink heartbeat LED
    }

    // check for Dallas sensors
    EMSESP_Settings.dallas_sensors = ds18.setup(EMSESP_Settings.dallas_gpio, EMSESP_Settings.dallas_parasite); // returns #sensors

    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck); // check if EMS is reachable
}

//
// Main loop
//
void loop() {
    EMSESP_Settings.timestamp = millis();

    // the main loop
    myESP.loop();

    // check Dallas sensors, every 2 seconds
    // these values are published to MQTT separately via the timer publishSensorValuesTimer
    if (EMSESP_Settings.dallas_sensors != 0) {
        ds18.loop();
    }

    // publish the values to MQTT, only if the values have changed
    // although we don't want to publish when doing a deep scan of the thermostat
    if (ems_getEmsRefreshed() && (scanThermostat_count == 0)) {
        publishValues(false);
        ems_setEmsRefreshed(false); // reset
    }

    // do shower logic, if enabled
    if (EMSESP_Settings.shower_timer) {
        showerCheck();
    }

    if (EMSESP_DELAY != 0) {
        delay(EMSESP_DELAY); // some time to WiFi and everything else to catch up, and prevent overheating
    }
}
