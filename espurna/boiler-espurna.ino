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

typedef struct {
    bool wifi_connected;
    bool boiler_online;
    bool thermostat_enabled;
    bool shower_enabled; // true if we want to report back on shower times
    bool shower_timer;   // true if we want the cold water reminder
} _Boiler_Status;

typedef struct {
    bool          showerOn;
    unsigned long timerStart; // ms
    unsigned long timerPause; // ms
    unsigned long duration;   // ms
    bool          isColdShot; // true if we've just sent a jolt of cold water
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

    Boiler_Status.shower_enabled = getSetting("boilerShower", BOILER_SHOWER_ENABLED).toInt() == 1;
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

// send to HA
void sendHA() {
    String topic = getSetting("haPrefix", HOMEASSISTANT_PREFIX);
    String output;
    // assume ha is enabled
    DynamicJsonBuffer jsonBuffer;
    JsonObject &      config = jsonBuffer.createObject();
    String            name   = getSetting("hostname");
    config.set("name", name);
    config.set("platform", "mqtt");
    config.printTo(output);

    mqttSendRaw(topic.c_str(), output.c_str());
    mqttSend(MQTT_TOPIC_STATUS, MQTT_STATUS_ONLINE, true);
}

// Show command - display stats on an 's' command
void showInfo() {
    char s[10]; // for formatting floats using the _float_to_char() function

    // General stats from EMS bus
    myDebug("EMS Bus stats:\n");
    myDebug("  Thermostat is %s, Poll is %s, Shower is %s, Shower timer is %s, RxPgks=%d, TxPkgs=%d, #CrcErrors=%d",
            ((Boiler_Status.thermostat_enabled) ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsPollEnabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_enabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_timer) ? "enabled" : "disabled"),
            EMS_Sys_Status.emsRxPgks,
            EMS_Sys_Status.emsTxPkgs,
            EMS_Sys_Status.emxCrcErr);

    myDebug(", RxStatus=");
    switch (EMS_Sys_Status.emsRxStatus) {
    case EMS_RX_IDLE:
        myDebug("idle");
        break;
    case EMS_RX_ACTIVE:
        myDebug("active");
        break;
    }

    myDebug(", TxStatus=");
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

    myDebug(", TxAction=");
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

    myDebug("\nBoiler stats:\n");

    // UBAMonitorWWMessage & UBAParameterWW
    myDebug("  Warm Water activated: %s\n", (EMS_Boiler.wWActivated ? "yes" : "no"));
    myDebug("  Warm Water selected temperature: %d C\n", EMS_Boiler.wWSelTemp);
    myDebug("  Warm Water circulation pump available: %s\n", (EMS_Boiler.wWCircPump ? "yes" : "no"));
    myDebug("  Warm Water desired temperature: %d C\n", EMS_Boiler.wWDesiredTemp);
    myDebug("  Warm Water current temperature: %s C\n", _float_to_char(s, EMS_Boiler.wWCurTmp));
    myDebug("  Warm Water # starts: %d times\n", EMS_Boiler.wWStarts);
    myDebug("  Warm Water active time: %d days %d hours %d minutes\n",
            EMS_Boiler.wWWorkM / 1440,
            (EMS_Boiler.wWWorkM % 1440) / 60,
            EMS_Boiler.wWWorkM % 60);
    myDebug("  Warm Water 3-way valve: %s\n", EMS_Boiler.wWHeat ? "on" : "off");

    // UBAMonitorFast
    myDebug("  Selected flow temperature: %d C\n", EMS_Boiler.selFlowTemp);
    myDebug("  Current flow temperature: %s C\n", _float_to_char(s, EMS_Boiler.curFlowTemp));
    myDebug("  Return temperature: %s C\n", _float_to_char(s, EMS_Boiler.retTemp));

    myDebug("  Gas: %s\n", EMS_Boiler.burnGas ? "on" : "off");
    myDebug("  Boiler pump: %s\n", EMS_Boiler.heatPmp ? "on" : "off");
    myDebug("  Fan: %s\n", EMS_Boiler.fanWork ? "on" : "off");
    myDebug("  Ignition: %s\n", EMS_Boiler.ignWork ? "on" : "off");
    myDebug("  Circulation pump: %s\n", EMS_Boiler.wWCirc ? "on" : "off");
    myDebug("  Burner max power: %d %%\n", EMS_Boiler.selBurnPow);
    myDebug("  Burner current power: %d %%\n", EMS_Boiler.curBurnPow);
    myDebug("  Flame current: %s uA\n", _float_to_char(s, EMS_Boiler.flameCurr));
    myDebug("  System pressure: %s bar\n", _float_to_char(s, EMS_Boiler.sysPress));

    // UBAMonitorSlow
    myDebug("  Outside temperature: %s C\n", _float_to_char(s, EMS_Boiler.extTemp));
    myDebug("  Boiler temperature: %s C\n", _float_to_char(s, EMS_Boiler.boilTemp));
    myDebug("  Pump modulation: %d %%\n", EMS_Boiler.pumpMod);
    myDebug("  # burner restarts: %d\n", EMS_Boiler.burnStarts);
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
        myDebug("Thermostat stats:\n  Thermostat time is %02d:%02d:%02d %d/%d/%d\n",
                EMS_Thermostat.hour,
                EMS_Thermostat.minute,
                EMS_Thermostat.second,
                EMS_Thermostat.day,
                EMS_Thermostat.month,
                EMS_Thermostat.year + 2000);

        myDebug("  Setpoint room temperature is %s C\n", _float_to_char(s, EMS_Thermostat.setpoint_roomTemp));
        myDebug("  Current room temperature is %s C\n", _float_to_char(s, EMS_Thermostat.curr_roomTemp));
        myDebug("  Mode is set to ");
        if (EMS_Thermostat.mode == 0) {
            myDebug("low\n");
        } else if (EMS_Thermostat.mode == 1) {
            myDebug("manual\n");
        } else if (EMS_Thermostat.mode == 2) {
            myDebug("clock/auto\n");
        } else {
            myDebug("<unknown>\n");
        }
    }

    // show the Shower Info
    if (Boiler_Status.shower_enabled) {
        myDebug("Shower stats:\n  Shower is %s\n", (Boiler_Shower.showerOn ? "on" : "off"));
        char    s[70];
        uint8_t sec = (uint8_t)((Boiler_Shower.duration / 1000) % 60);
        uint8_t min = (uint8_t)((Boiler_Shower.duration / (1000 * 60)) % 60);
        sprintf(s, "  Last shower duration was %d minutes and %d %s\n", min, sec, (sec == 1) ? "second" : "seconds");
        myDebug(s);
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
            DEBUG_MSG_P(PSTR("-ERROR: arg is 0 or 1\n"));
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

void _boilerMQTTCallback(unsigned int type, const char * topic, const char * payload) {
    /*

    if (type == MQTT_CONNECT_EVENT) {
        char buffer[strlen(MQTT_TOPIC_LED) + 3];
        snprintf_P(buffer, sizeof(buffer), PSTR("%s/+"), MQTT_TOPIC_LED);
        mqttSubscribe(buffer);
    }

    if (type == MQTT_MESSAGE_EVENT) {

        // Match topic
        String t = mqttMagnitude((char *) topic);
        if (!t.startsWith(MQTT_TOPIC_LED)) return;

        // Get led ID
        unsigned int ledID = t.substring(strlen(MQTT_TOPIC_LED)+1).toInt();
        if (ledID >= _ledCount()) {
            DEBUG_MSG_P(PSTR("[LED] Wrong ledID (%d)\n"), ledID);
            return;
        }

        // Check if LED is managed
        if (_ledMode(ledID) != LED_MODE_MQTT) return;

        // get value
        unsigned char value = relayParsePayload(payload);

        // Action to perform
        if (value == 2) {
            _ledToggle(ledID);
        } else {
            _ledStatus(ledID, value == 1);
        }

    }
    */
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
    wsOnAfterParseRegister(_boilerConfigure);
    wsOnActionRegister(_boilerWebSocketOnAction);
    wsOnReceiveRegister(_boilerWebSocketOnReceive);

    mqttRegister(_boilerMQTTCallback);

    _boilerInitCommands();

    // init shower
    Boiler_Shower.timerStart = 0;
    Boiler_Shower.timerPause = 0;
    Boiler_Shower.duration   = 0;
    Boiler_Shower.isColdShot = false;

    // ems init values
    ems_init();

    // start uart
    emsuart_init();

    // Register loop
    espurnaRegisterLoop(_boilerLoop);
}

// LOOP
void _boilerLoop() {
}
