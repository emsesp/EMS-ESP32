/*
 * Boiler Project
 * Paul Derbyshire - May 2018 - https://github.com/proddy/EMS-ESP-Boiler
 *
 * See Readme for Acknowledgments
 */

// local libraries
#include "ESPHelper.h"
#include "ems.h"
#include "emsuart.h"

// public libraries
#include <ArduinoJson.h>
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// these are set as -D build flags. If you're not using PlatformIO hard code them
//#define WIFI_SSID "<my_ssid>"
//#define WIFI_PASSWORD "<my_password>"
//#define MQTT_IP "<broker_ip>"
//#define MQTT_USER "<broker_username>"
//#define MQTT_PASS "<broker_password>"

// private function prototypes
void heartbeat();
void systemCheck();
void regulatUpdates();
void publishValues();
void _showerColdShotStart();
void _showerColdShotStop();

char * _float_to_char(char * a, float f, uint8_t precision = 1);

// timers
#define PUBLISHVALUES_TIME 300 // every 5 mins post HA values
#define SYSTEMCHECK_TIME 10    // every 10 seconds check if Boiler is online and execute other requests
#define REGULARUPDATES_TIME 60 // every minute a call is made
#define HEARTBEAT_TIME 1       // every second blink heartbeat LED
Ticker  publishValuesTimer;
Ticker  systemCheckTimer;
Ticker  regularUpdatesTimer;
Ticker  heartbeatTimer;
Ticker  showerResetTimer;
uint8_t regularUpdatesCount = 0;
#define MAX_MANUAL_CALLS 2 // number of ems reads we do during the fetch cycle (in regularUpdates)

// Project commands for telnet
// Note: ?, *, $, ! and & are reserved
#define PROJECT_CMDS                                                                                                   \
    "*  s=show statistics\n\r"                                                                                         \
    "*  P=publish stats to MQTT\n\r"                                                                                   \
    "*  v [n] set logging to 0=none, 1=basic, 2=verbose\n\r"                                                           \
    "*  p=Poll response on/off\n\r"                                                                                    \
    "*  T=Thermostat Support on/off\n\r"                                                                               \
    "*  S=Shower Timer on/off\n\r"                                                                                     \
    "*  r [n] to request for data from EMS, some examples:\n\r"                                                        \
    "*     from Boiler: 33=UBAParameterWW, 18=UBAMonitorFast, 19=UBAMonitorSlow, 34=UBAMonitorWWMessage\n\r"           \
    "*     from Thermostat: 91=RC20StatusMessage, A8=RC20Temperature, 6=RC20Time, 2=Version\n\r"                       \
    "*  t [n] set thermostat temperature to n\n\r"                                                                     \
    "*  m [n] set thermostat mode (0=low, 1=manual, 2=clock)\n\r"                                                      \
    "*  w [n] set boiler warm water temperature to n (min 30)\n\r"                                                     \
    "*  a [n] activate boiler warm water on (n=1) or off (n=0)"

// GPIOs
#define LED_RX D1                 // (GPIO5 on nodemcu)
#define LED_TX D2                 // (GPIO4 on nodemcu)
#define LED_ERR D3                // (GPIO0 on nodemcu)
#define LED_HEARTBEAT LED_BUILTIN // onboard LED

// hostname is also used as the MQTT topic identifier (home/<hostname>)
#define HOSTNAME "boiler"

// app specific - do not change
#define MQTT_BOILER MQTT_BASE HOSTNAME "/"
#define TOPIC_START MQTT_BOILER MQTT_TOPIC_START

#define TOPIC_THERMOSTAT_TEMP MQTT_BOILER "thermostat_temp"         // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CURRTEMP MQTT_BOILER "thermostat_currtemp" // current temperature
#define TOPIC_THERMOSTAT_SELTEMP MQTT_BOILER "thermostat_seltemp"   // selected temperature
#define TOPIC_THERMOSTAT_MODE MQTT_BOILER "thermostat_mode"         // selected temperature

#define TOPIC_BOILER_DATA MQTT_BOILER "boiler_data" // for sending boiler values
#define TOPIC_SHOWERTIME MQTT_BOILER "showertime"   // for sending shower time results

// default values
// thermostat support, shower timing and shower alert all enabled
#define BOILER_THERMOSTAT_ENABLED 1
#define BOILER_SHOWER_ENABLED 1
#define BOILER_SHOWER_TIMER 0
#define BOILER_DEFAULT_LOGGING EMS_SYS_LOGGING_BASIC // or EMS_SYS_LOGGING_VERBOSE

// shower settings
const unsigned long SHOWER_PAUSE_TIME    = 15000;  // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION  = 120000; // 2 minutes, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION  = 420000; // 7 minutes, before trigger a shot of cold water
const unsigned long SHOWER_OFF_DURATION  = 3000;   // 3 seconds long for cold water
const uint8_t       SHOWER_BURNPOWER_MIN = 80;
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

// ESPHelper
netInfo homeNet = {.mqttHost = MQTT_IP,
                   .mqttUser = MQTT_USER,
                   .mqttPass = MQTT_PASS,
                   .mqttPort = 1883, // this is the default, change if using another port
                   .ssid     = WIFI_SSID,
                   .pass     = WIFI_PASSWORD};

ESPHelper myESP(&homeNet);

// store for overall system status
_Boiler_Status Boiler_Status;
_Boiler_Shower Boiler_Shower;

// Debugger to telnet
#define myDebug(x, ...) myESP.printf(x, ##__VA_ARGS__);

// Times
const unsigned long POLL_TIMEOUT_ERR = 10000; // if no signal from boiler for last 10 seconds, assume its offline
const unsigned long TX_HOLD_LED_TIME = 2000;  // how long to hold the Tx LED because its so quick

unsigned long timestamp; // for internal timings, via millis()
static int    connectionStatus = NO_CONNECTION;
bool          startMQTTsent    = false;

// toggle for heartbeat LED
bool heartbeatEnabled;

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision) {
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

    myDebug("%sEMS-ESP-Boiler general info:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
    myDebug("  System Logging is set to ");
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug("Basic");
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug("Verbose");
    } else {
        myDebug("None");
    }

    myDebug("\n  Thermostat is %s, Poll is %s, Shower is %s, Shower timer is %s\n",
            ((Boiler_Status.thermostat_enabled) ? "enabled" : "disabled"),
            ((EMS_Sys_Status.emsPollEnabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_enabled) ? "enabled" : "disabled"),
            ((Boiler_Status.shower_timer) ? "enabled" : "disabled"));

    myDebug("  EMS Bus Stats: RxPgks=%d, TxPkgs=%d, #CrcErrors=%d, ",
            EMS_Sys_Status.emsRxPgks,
            EMS_Sys_Status.emsTxPkgs,
            EMS_Sys_Status.emxCrcErr);

    myDebug("RxStatus=");
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
    _renderIntValue("Burner max power", "%", EMS_Boiler.selBurnPow);
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
            myDebug("clock/auto\n");
        } else {
            myDebug("?\n");
        }
    }

    // show the Shower Info
    if (Boiler_Status.shower_enabled) {
        myDebug("\n%sShower stats:%s\n", COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug("  Shower is %s\n", (Boiler_Shower.showerOn ? "on" : "off"));
        char    s[70];
        uint8_t sec = (uint8_t)((Boiler_Shower.duration / 1000) % 60);
        uint8_t min = (uint8_t)((Boiler_Shower.duration / (1000 * 60)) % 60);
        sprintf(s, "  Last shower duration was %d minutes and %d %s\n", min, sec, (sec == 1) ? "second" : "seconds");
        myDebug(s);
    }

    myDebug("\n");
}

// send values to HA via MQTT
void publishValues() {
    myDebug("Publishing data to MQTT topics\n");

    char s[20]; // for formatting strings

    // Boiler values as one JSON object
    StaticJsonBuffer<512> jsonBuffer;
    char                  data[512];
    JsonObject &          root = jsonBuffer.createObject();
    root["wWCurTmp"]           = _float_to_char(s, EMS_Boiler.wWCurTmp);
    root["wWHeat"]             = _bool_to_char(s, EMS_Boiler.wWHeat);
    root["curFlowTemp"]        = _float_to_char(s, EMS_Boiler.curFlowTemp);
    root["retTemp"]            = _float_to_char(s, EMS_Boiler.retTemp);
    root["burnGas"]            = _bool_to_char(s, EMS_Boiler.burnGas);
    root["heatPmp"]            = _bool_to_char(s, EMS_Boiler.heatPmp);
    root["fanWork"]            = _bool_to_char(s, EMS_Boiler.fanWork);
    root["ignWork"]            = _bool_to_char(s, EMS_Boiler.ignWork);
    root["wWCirc"]             = _bool_to_char(s, EMS_Boiler.wWCirc);
    root["selBurnPow"]         = _int_to_char(s, EMS_Boiler.selBurnPow);
    root["curBurnPow"]         = _int_to_char(s, EMS_Boiler.curBurnPow);
    root["sysPress"]           = _float_to_char(s, EMS_Boiler.sysPress);
    root["boilTemp"]           = _float_to_char(s, EMS_Boiler.boilTemp);
    root["pumpMod"]            = _float_to_char(s, EMS_Boiler.pumpMod);

    root.printTo(data, root.measureLength() + 1);
    myESP.publish(TOPIC_BOILER_DATA, data);

    if (EMS_Sys_Status.emsThermostatEnabled) {
        // only send thermostat values if we actually have them
        if (((int)EMS_Thermostat.curr_roomTemp == (int)0) || ((int)EMS_Thermostat.setpoint_roomTemp == (int)0)) {
            return;
        }

        myESP.publish(TOPIC_THERMOSTAT_CURRTEMP, _float_to_char(s, EMS_Thermostat.curr_roomTemp));
        myESP.publish(TOPIC_THERMOSTAT_SELTEMP, _float_to_char(s, EMS_Thermostat.setpoint_roomTemp));

        // send mode 0=low, 1=manual, 2=clock/auto
        if (EMS_Thermostat.mode == 0) {
            myESP.publish(TOPIC_THERMOSTAT_MODE, "low");
        } else if (EMS_Thermostat.mode == 1) {
            myESP.publish(TOPIC_THERMOSTAT_MODE, "manual");
        } else {
            myESP.publish(TOPIC_THERMOSTAT_MODE, "auto");
        }
        myESP.publish(TOPIC_THERMOSTAT_SELTEMP, _float_to_char(s, EMS_Thermostat.setpoint_roomTemp));
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
        publishValues();
        break;
    case 'r': // read command for Boiler or Thermostat
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16));
        break;
    case 't': // set thermostat temp
        ems_setThermostatTemp(strtof(&cmd[2], 0));
        break;
    case 'm': // set thermostat mode
        ems_setThermostatMode(cmd[2] - '0');
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
    case 'T': // toggle Thermostat
        b = !ems_getThermostatEnabled();
        ems_setThermostatEnabled(b);
        Boiler_Status.thermostat_enabled = b;
        break;
    case 'S': // toggle Shower timer support
        Boiler_Status.shower_enabled = !Boiler_Status.shower_enabled;
        myDebug("Shower timer is %s\n", Boiler_Status.shower_enabled ? "enabled" : "disabled");
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
        myDebug("MQTT topic boottime: %s\n", payload);
        myESP.setBoottime((char *)payload);
        return;
    }

    // thermostat_temp
    if (strcmp(topic, TOPIC_THERMOSTAT_TEMP) == 0) {
        float f = strtof((char *)payload, 0);
        char  s[10];
        myDebug("MQTT topic: thermostat_temp value %s\n", _float_to_char(s, f));
        ems_setThermostatTemp(f);
        return;
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
        // ...and turn off LED
        digitalWrite(LED_HEARTBEAT, HIGH);
    }
}

// Initialize the boiler settings
void _initBoiler() {
    // default settings
    Boiler_Status.shower_enabled     = BOILER_SHOWER_ENABLED;
    Boiler_Status.shower_timer       = BOILER_SHOWER_TIMER;
    Boiler_Status.thermostat_enabled = BOILER_THERMOSTAT_ENABLED;
    ems_setThermostatEnabled(Boiler_Status.thermostat_enabled);

    // init boiler
    Boiler_Status.wifi_connected = false;
    Boiler_Status.boiler_online  = true; // assume we have a connection, it will be checked in the loop() anyway

    // init shower
    Boiler_Shower.timerStart = 0;
    Boiler_Shower.timerPause = 0;
    Boiler_Shower.duration   = 0;
    Boiler_Shower.isColdShot = false;

    // heartbeat only if verbose logging
    ems_setLogging(BOILER_DEFAULT_LOGGING);
    updateHeartbeat();
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
    publishValuesTimer.attach(PUBLISHVALUES_TIME, publishValues);                         // post HA values
    systemCheckTimer.attach(SYSTEMCHECK_TIME, systemCheck);                               // check if Boiler is online
    regularUpdatesTimer.attach((REGULARUPDATES_TIME / MAX_MANUAL_CALLS), regularUpdates); // regular reads from the EMS

    // set up WiFi
    myESP.setWifiCallback(WIFIcallback);

    // set up MQTT
    myESP.setMQTTCallback(MQTTcallback);
    myESP.addSubscription(TOPIC_START);
    myESP.addSubscription(TOPIC_THERMOSTAT_TEMP);

    // set up Telnet
    myESP.consoleSetHelpProjectsCmds(PROJECT_CMDS);
    myESP.consoleSetCallBackProjectCmds(myDebugCallback);
    myESP.begin(HOSTNAME);

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
        int state = digitalRead(LED_HEARTBEAT);
        digitalWrite(LED_HEARTBEAT, !state);
    }
}

// do a healthcheck every now and then to see if we connections
void systemCheck() {
    // first do a system check to see if there is still a connection to the EMS
    Boiler_Status.boiler_online = ((timestamp - EMS_Sys_Status.emsLastPoll) < POLL_TIMEOUT_ERR);
    if (!Boiler_Status.boiler_online) {
        myDebug("Error! Boiler unreachable. Please check connection. Retrying in 10 seconds.\n");
    }
}

// calls to get data from EMS for the types that aren't sent as broadcasts
// number of calls is defined in MAX_MANUAL_CALLS
void regularUpdates() {
    uint8_t cycle = (regularUpdatesCount++ % MAX_MANUAL_CALLS);

    if ((cycle == 0) && Boiler_Status.thermostat_enabled) {
        ems_doReadCommand(EMS_TYPE_RC20Temperature); // to get the thermostat mode which is not broadcasted
    } else if (cycle == 1) {
        ems_doReadCommand(EMS_TYPE_UBAParameterWW); // get Warm Water values
    }
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    myDebug("Shower: exceeded max shower time, doing a shot of cold...\n");
    ems_setWarmWaterActivated(false);
    Boiler_Shower.isColdShot = true;
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (Boiler_Shower.isColdShot) {
        myDebug("Shower: turning back hot shower water.\n");
        ems_setWarmWaterActivated(true);
        Boiler_Shower.isColdShot = false;
        showerResetTimer.detach();
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
        myDebug("Waiting to connect to wifi...\n");
        return;
    }

    // if first time connected to MQTT, send welcome start message
    // which will send all the state values from HA back to the clock via MQTT and return the boottime
    if ((!startMQTTsent) && (connectionStatus == FULL_CONNECTION)) {
        myESP.sendStart();
        startMQTTsent = true;

        // now that we're connected lets get some data from the EMS
        ems_doReadCommand(EMS_TYPE_UBAParameterWW);
    }

    // if we received new data and flagged for pushing, do it
    if (EMS_Sys_Status.emsRefreshed) {
        EMS_Sys_Status.emsRefreshed = false;
        publishValues();
    }

    /*
     * Shower Logic
     */
    if (Boiler_Status.shower_enabled) {
        // if already in cold mode, ignore all this logic until we're out of the cold blast
        if (!Boiler_Shower.isColdShot) {
            // these values come from UBAMonitorFast - type 0x18) which is broadcasted every second so our timings are accurate enough
            // and no need to fetch the values from the boiler
            Boiler_Shower.showerOn =
                ((EMS_Boiler.selBurnPow >= SHOWER_BURNPOWER_MIN) && (EMS_Boiler.selFlowTemp == 0) && EMS_Boiler.burnGas);

            // is the shower on?
            if (Boiler_Shower.showerOn) {
                // if heater was off, start the timer
                if (Boiler_Shower.timerStart == 0) {
                    Boiler_Shower.timerStart = timestamp;
                    Boiler_Shower.timerPause = 0; // remove any last pauses
                    Boiler_Shower.isColdShot = false;
                    Boiler_Shower.duration   = 0;
                    myDebug("Shower: starting timer...\n");
                } else {
                    // check if the shower has been on too long
                    if ((((timestamp - Boiler_Shower.timerStart) > SHOWER_MAX_DURATION) && !Boiler_Shower.isColdShot)
                        && Boiler_Status.shower_timer) {
                        _showerColdShotStart();
                        // start the timer for n seconds which will reset the water back to hot
                        showerResetTimer.attach(SHOWER_OFF_DURATION, _showerColdShotStop);
                    }
                }
            } else { // shower is off
                // if it just turned off, record the time as it could be a pause
                if ((Boiler_Shower.timerStart != 0) && (Boiler_Shower.timerPause == 0)) {
                    Boiler_Shower.timerPause = timestamp;
                    myDebug("Shower: water has just turned off...\n");
                } else {
                    // if shower has been off for longer than the wait time
                    if ((Boiler_Shower.timerPause != 0) && ((timestamp - Boiler_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
                        // its over the wait period, so assume that the shower has finished and calculate the total time and publish
                        Boiler_Shower.duration = (Boiler_Shower.timerPause - Boiler_Shower.timerStart);
                        if (Boiler_Shower.duration > SHOWER_MIN_DURATION) {
                            char s[50];
                            sprintf(s,
                                    "%d minutes and %d seconds",
                                    (uint8_t)((Boiler_Shower.duration / (1000 * 60)) % 60),
                                    (uint8_t)((Boiler_Shower.duration / 1000) % 60));
                            myDebug("Shower: finished, duration was %s\n", s);
                            myESP.publish(TOPIC_SHOWERTIME, s); // publish to HA
                        }

                        // reset
                        myDebug("Shower: resetting timers.\n");
                        Boiler_Shower.timerStart = 0;
                        Boiler_Shower.timerPause = 0;
                        _showerColdShotStop(); // turn heat back on in case its off
                    }
                }
            }
        }
    }

    // yield to prevent watchdog from timing out
    yield();
}
