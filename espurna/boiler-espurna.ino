// Boiler
// Espurna version
// Paul Derbyshire - https://github.com/proddy/EMS-ESP-Boiler

#include "emsuart.h"
#include <ArduinoJson.h>
#include <ems.h>

#define myDebug(...) debugSend(__VA_ARGS__)

#define BOILER_THERMOSTAT_ENABLED 1
#define BOILER_SHOWER_ENABLED 1
#define BOILER_POLLING_ENABLED 0
#define BOILER_LOGGING_NONE 1

// home/boiler/
#define TOPIC_BOILER_DATA "boiler_data"                              // for sending boiler values
#define TOPIC_THERMOSTAT_TEMP "thermostat_temp"                      // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CURRTEMP "thermostat_currtemp"              // current temperature
#define TOPIC_THERMOSTAT_SELTEMP "thermostat_seltemp"                // selected temperature
#define TOPIC_THERMOSTAT_MODE "thermostat_mode"                      // selected temperature
#define TOPIC_BOILER_WARM_WATER_SELECTED_TEMPERATURE "boiler_wwtemp" // warm water selected temp

#define BOILERSEND_INTERVAL 60000 // send every minute to HA

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

// store for overall system status
_Boiler_Status Boiler_Status;
_Boiler_Shower Boiler_Shower;

// Config
void _boilerConfigure() {
    Boiler_Status.thermostat_enabled = getSetting("boilerThermostat", BOILER_THERMOSTAT_ENABLED).toInt() == 1;
    ems_setThermostatEnabled(Boiler_Status.thermostat_enabled);

    bool _boilerPolling = getSetting("boilerPolling", BOILER_POLLING_ENABLED).toInt() == 1;
    ems_setPoll(_boilerPolling);

    uint8_t _boilerLogging = getSetting("boilerLogging", BOILER_LOGGING_NONE).toInt();
    ems_setLogging((_EMS_SYS_LOGGING)_boilerLogging);

    Boiler_Status.shower_timer = getSetting("boilerShower", BOILER_SHOWER_ENABLED).toInt() == 1;
}

// WEB callbacks
bool _boilerWebSocketOnReceive(const char * key, JsonVariant & value) {
    return (strncmp(key, "boiler", 6) == 0);
}

void _boilerWebSocketOnSend(JsonObject & root) {
    root["boilerThermostat"] = getSetting("boilerThermostat", BOILER_THERMOSTAT_ENABLED).toInt() == 1;
    root["boilerShower"]     = getSetting("boilerShower", BOILER_SHOWER_ENABLED).toInt() == 1;
    root["boilerPolling"]    = getSetting("boilerPolling", BOILER_POLLING_ENABLED).toInt() == 1;
    root["boilerLogging"]    = getSetting("boilerLogging", BOILER_LOGGING_NONE).toInt();
}

// used if we have a button
void _boilerWebSocketOnAction(uint32_t client_id, const char * action, JsonObject & data) {
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
            myDebug("?\n");
        }
    }

    myDebug("\n");
}

// Init telnet commands
void _boilerInitCommands() {
    settingsRegisterCommand(F("BOILER.INFO"), [](Embedis * e) {
        showInfo();
        DEBUG_MSG(_boilerGetConfig().c_str());
        DEBUG_MSG_P(PSTR("+OK\n"));
    });

    settingsRegisterCommand(F("BOILER.POLLING"), [](Embedis * e) {
        if (e->argc < 2) {
            DEBUG_MSG_P(PSTR("-ERROR: arg is 0 or 1\n"));
            return;
        }

        int param = String(e->argv[1]).toInt();

        bool b = setSetting("boilerPolling", param);
        if (b) {
            _boilerConfigure();
        }

        wsSend(_boilerWebSocketOnSend); // update web

        DEBUG_MSG_P(PSTR("+OK\n"));
    });

    settingsRegisterCommand(F("BOILER.LOGGING"), [](Embedis * e) {
        if (e->argc < 2) {
            DEBUG_MSG_P(PSTR("-ERROR: arg is 0, 1 or 2\n"));
            return;
        }

        int param = String(e->argv[1]).toInt();

        bool b = setSetting("boilerLogging", param);
        if (b) {
            _boilerConfigure();
        }

        wsSend(_boilerWebSocketOnSend); // update web

        DEBUG_MSG_P(PSTR("+OK\n"));
    });

    settingsRegisterCommand(F("BOILER.SEND"), [](Embedis * e) {
        if (e->argc < 2) {
            DEBUG_MSG_P(PSTR("-ERROR: Wrong arguments\n"));
            return;
        }
        int cmd = String(e->argv[1]).toInt();
        DEBUG_MSG_P(PSTR("Sending %d\n"), cmd);
        DEBUG_MSG_P(PSTR("+OK\n"));
    });
}


// send values to HA via MQTT
void publishValues() {
    char s[20]; // for formatting strings

    // Boiler values as one JSON object
    StaticJsonBuffer<512> jsonBuffer;
    char                  data[512];
    JsonObject &          root = jsonBuffer.createObject();

    root["wWSelTemp"]   = _int_to_char(s, EMS_Boiler.wWSelTemp);
    root["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);
    root["wWCurTmp"]    = _float_to_char(s, EMS_Boiler.wWCurTmp);
    root["wWHeat"]      = _bool_to_char(s, EMS_Boiler.wWHeat);
    root["curFlowTemp"] = _float_to_char(s, EMS_Boiler.curFlowTemp);
    root["retTemp"]     = _float_to_char(s, EMS_Boiler.retTemp);
    root["burnGas"]     = _bool_to_char(s, EMS_Boiler.burnGas);
    root["heatPmp"]     = _bool_to_char(s, EMS_Boiler.heatPmp);
    root["fanWork"]     = _bool_to_char(s, EMS_Boiler.fanWork);
    root["ignWork"]     = _bool_to_char(s, EMS_Boiler.ignWork);
    root["wWCirc"]      = _bool_to_char(s, EMS_Boiler.wWCirc);
    root["selBurnPow"]  = _int_to_char(s, EMS_Boiler.selBurnPow);
    root["curBurnPow"]  = _int_to_char(s, EMS_Boiler.curBurnPow);
    root["sysPress"]    = _float_to_char(s, EMS_Boiler.sysPress);
    root["boilTemp"]    = _float_to_char(s, EMS_Boiler.boilTemp);
    root["pumpMod"]     = _int_to_char(s, EMS_Boiler.pumpMod);

    size_t len = root.measureLength();
    root.printTo(data, len + 1); // form the json string
    mqttSend(TOPIC_BOILER_DATA, data);

    // handle the thermostat values separately
    if (EMS_Sys_Status.emsThermostatEnabled) {
        // only send thermostat values if we actually have them
        if (((int)EMS_Thermostat.curr_roomTemp == (int)0) || ((int)EMS_Thermostat.setpoint_roomTemp == (int)0)) {
            return;
        }

        mqttSend(TOPIC_THERMOSTAT_CURRTEMP, _float_to_char(s, EMS_Thermostat.curr_roomTemp));
        mqttSend(TOPIC_THERMOSTAT_SELTEMP, _float_to_char(s, EMS_Thermostat.setpoint_roomTemp));

        // send mode 0=low, 1=manual, 2=auto
        if (EMS_Thermostat.mode == 0) {
            mqttSend(TOPIC_THERMOSTAT_MODE, "low");
        } else if (EMS_Thermostat.mode == 1) {
            mqttSend(TOPIC_THERMOSTAT_MODE, "manual");
        } else {
            mqttSend(TOPIC_THERMOSTAT_MODE, "auto");
        }
    }
}


void _boilerMQTTCallback(unsigned int type, const char * topic, const char * payload) {
}

// TELNET commands
String _boilerGetConfig() {
    String output;

    // get values and print them
    output = String("Thermostat is ") + getSetting("boilerThermostat") + String(", Polling is ")
             + getSetting("boilerPolling") + String(", Logging is ") + getSetting("boilerLogging")
             + String(", Shower is ") + getSetting("boilerShower") + String("\n");

    return output;
}

// SETUP
void extraSetup() {
    boilerSetup();
}

void boilerSetup() {
    // configure
    _boilerConfigure();

    wsOnSendRegister(_boilerWebSocketOnSend);
    wsOnActionRegister(_boilerWebSocketOnAction);
    wsOnReceiveRegister(_boilerWebSocketOnReceive);

    mqttRegister(_boilerMQTTCallback);

    _boilerInitCommands(); // telnet

    // init shower
    Boiler_Shower.timerStart    = 0;
    Boiler_Shower.timerPause    = 0;
    Boiler_Shower.duration      = 0;
    Boiler_Shower.doingColdShot = false;

    // ems init values
    ems_init();

    // start uart
    emsuart_init();

    // Register loop
    espurnaRegisterLoop(_boilerLoop);

    espurnaRegisterReload([]() { _boilerConfigure(); });
}

// LOOP
void _boilerLoop() {
    static unsigned long last_boilersend = 0;
    if ((last_boilersend == 0) || (millis() - last_boilersend > BOILERSEND_INTERVAL)) {
        last_boilersend = millis();

        // get 0x33 WW values manually
        ems_doReadCommand(EMS_TYPE_UBAParameterWW);

#if MQTT_SUPPORT
        // send MQTT
        publishValues();
#endif
    }
}
