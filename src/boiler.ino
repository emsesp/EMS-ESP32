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
void publishValues();
void _showerColdShotStart();
void _showerColdShotStop();
void _toggleHeartbeat();

// timers
Ticker publishValuesTimer;
Ticker systemCheckTimer;
Ticker heartbeatTimer;
Ticker showerResetTimer;
#define publishValuesTime 300 // every 5 mins post HA values
#define systemCheckTime 10    // every 10 seconds check if Boiler is online
#define heartbeatTime 1       // every second blink heartbeat LED

// hostname is also used as the MQTT topic identifier (home/<hostname>)
#define HOSTNAME "boiler"

// Project commands for telnet
// Note: ?, *, $, ! and & are reserved
#define PROJECT_CMDS                                                                                                   \
    "s=show statistics\n\r"                                                                                            \
    "*  q=toggle Verbose telegram logging\n\r"                                                                         \
    "*  m=publish stats to MQTT\n\r"                                                                                   \
    "*  p=toggle Poll response\n\r"                                                                                    \
    "*  T=toggle Thermostat suport on/off\n\r"                                                                         \
    "*  S=toggle Shower Timer on/off\n\r"                                                                              \
    "*  r [n] to request for data from EMS "                                                                           \
    "(33=UBAParameterWW, 18=UBAMonitorFast, 19=UBAMonitorSlow, "                                                       \
    "34=UBAMonitorWWMessage, 91=RC20StatusMessage, 6=RC20Time, 2=Version)\n\r"                                         \
    "*  t [n] set thermostat temperature to n\n\r"                                                                     \
    "*  w [n] set boiler warm water temperature to n (min 30)\n\r"                                                     \
    "*  a [n] activate boiler warm water on (n=1) or off (n=0)"

// GPIOs
#define LED_RX D1  // (GPIO5 on nodemcu)
#define LED_TX D2  // (GPIO4 on nodemcu)
#define LED_ERR D3 // (GPIO0 on nodemcu)

// app specific - do not change
#define MQTT_BOILER MQTT_BASE HOSTNAME "/"
#define TOPIC_START MQTT_BOILER MQTT_TOPIC_START

#define TOPIC_THERMOSTAT_TEMP MQTT_BOILER "thermostat_temp"         // for received thermostat temp changes
#define TOPIC_THERMOSTAT_CURRTEMP MQTT_BOILER "thermostat_currtemp" // current temperature
#define TOPIC_THERMOSTAT_SELTEMP MQTT_BOILER "thermostat_seltemp"   // selected temperature

#define TOPIC_BOILER_DATA MQTT_BOILER "boiler_data" // for sending boiler values
#define TOPIC_SHOWERTIME MQTT_BOILER "showertime"   // for sending shower time results

// thermostat support, shower timing and shower alert all enabled
#define BOILER_THERMOSTAT_ENABLED 1
#define BOILER_SHOWER_ENABLED 1
#define BOILER_SHOWER_TIMER 0

// shower settings
const unsigned long SHOWER_PAUSE_TIME    = 15000;  // 15 seconds, max time if water is switched off & on during a shower
const unsigned long SHOWER_MIN_DURATION  = 120000; // 2 minutes, before recognizing its a shower
const unsigned long SHOWER_MAX_DURATION  = 420000; // 7 minutes, before trigger a shot of cold water
const unsigned long SHOWER_OFF_DURATION  = 3000;   // 3 seconds long for cold water
const uint8_t       SHOWER_BURNPOWER_MIN = 80;

typedef struct {
    bool wifi_connected;
    bool boiler_online;
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

// Timers
const unsigned long POLL_TIMEOUT_ERR = 10000; // if no signal from boiler for last 10 seconds, assume its offline
bool                heartbeatEnabled = false;

const unsigned long TX_HOLD_LED_TIME = 2000; // how long to hold the Tx LED because its so quick

unsigned long timestamp; // for internal timings, via millis()
static int    connectionStatus = NO_CONNECTION;
bool          startMQTTsent    = false;

// Show command - display stats on an 's' command
void showInfo() {
    char s[10]; // for formatting floats using the _float_to_char() function

    // General stats from EMS bus
    myDebug("EMS Bus stats:\n");
    myDebug("  Poll is %s, Shower is %s, Shower timer is %s, RxPgks=%d, TxPkgs=%d, #CrcErrors=%d",
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
    if (EMS_Sys_Status.emsThermostatEnabled) {
        myDebug("Thermostat stats:\n  Thermostat time is %02d:%02d:%02d %d/%d/%d\n",
                EMS_Thermostat.hour,
                EMS_Thermostat.minute,
                EMS_Thermostat.second,
                EMS_Thermostat.day,
                EMS_Thermostat.month,
                EMS_Thermostat.year + 2000);

        myDebug("  Setpoint room temperature is %s C\n", _float_to_char(s, EMS_Thermostat.setpoint_roomTemp));
        myDebug("  Current room temperature is %s C\n", _float_to_char(s, EMS_Thermostat.curr_roomTemp));
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

// send values to HA via MQTT
void publishValues() {
    myDebug("Publishing data to MQTT topics\n");

    char s[20]; // for formatting strings

    // Boiler values as one JSON object
    StaticJsonBuffer<512> jsonBuffer;
    char                  data[512];
    JsonObject &          root = jsonBuffer.createObject();
    root["wWCurTmp"]           = _float_to_char(s, EMS_Boiler.wWCurTmp);
    root["wWHeat"]             = EMS_Boiler.wWHeat ? "on" : "off";
    root["curFlowTemp"]        = _float_to_char(s, EMS_Boiler.curFlowTemp);
    root["retTemp"]            = _float_to_char(s, EMS_Boiler.retTemp);
    root["burnGas"]            = EMS_Boiler.burnGas ? "on" : "off";
    root["heatPmp"]            = EMS_Boiler.heatPmp ? "on" : "off";
    root["fanWork"]            = EMS_Boiler.fanWork ? "on" : "off";
    root["ignWork"]            = EMS_Boiler.ignWork ? "on" : "off";
    root["wWCirc"]             = EMS_Boiler.wWCirc ? "on" : "off";
    root["selBurnPow"]         = (String)EMS_Boiler.selBurnPow;
    root["curBurnPow"]         = (String)EMS_Boiler.curBurnPow;
    root["sysPress"]           = _float_to_char(s, EMS_Boiler.sysPress);
    root["boilTemp"]           = _float_to_char(s, EMS_Boiler.boilTemp);
    root["pumpMod"]            = (String)EMS_Boiler.pumpMod;

    root.printTo(data, root.measureLength() + 1);
    myESP.publish(TOPIC_BOILER_DATA, data);

    if (EMS_Sys_Status.emsThermostatEnabled) {
        // only send thermostat values if we actually have them
        if (((int)EMS_Thermostat.curr_roomTemp == (int)0) || ((int)EMS_Thermostat.setpoint_roomTemp == (int)0)) {
            return;
        }

        myESP.publish(TOPIC_THERMOSTAT_CURRTEMP, _float_to_char(s, EMS_Thermostat.curr_roomTemp));
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
    case 'm':
        publishValues();
        break;
    case 'r': // read command for Boiler or Thermostat
        ems_doReadCommand((uint8_t)strtol(&cmd[2], 0, 16));
        break;
    case 't': // set thermostat temp
        ems_setThermostatTemp(strtof(&cmd[2], 0));
        break;
    case 'w': // set warm water temp
        ems_setWarmWaterTemp((uint8_t)strtol(&cmd[2], 0, 10));
        break;
    case 'q': // quiet
        _toggleHeartbeat();
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

// toggle heartbeat LED
void _toggleHeartbeat() {
    bool b = !ems_getLogVerbose();
    ems_setLogVerbose(b);
    heartbeatEnabled = b;
    digitalWrite(LED_BUILTIN, (b) ? LOW : HIGH); // set the LED
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

    // turn off the LEDs since we've finished the boot loading
    digitalWrite(LED_RX, LOW);
    digitalWrite(LED_TX, LOW);
    digitalWrite(LED_ERR, LOW);

    // when finally we're all set up, we can fire up the uart (this will enable the interrupts)
    emsuart_init();
}

// Initialize the boiler settings
void _initBoiler() {
    // default settings
    ems_setThermostatEnabled(BOILER_THERMOSTAT_ENABLED);
    Boiler_Status.shower_enabled = BOILER_SHOWER_ENABLED;
    Boiler_Status.shower_timer   = BOILER_SHOWER_TIMER;

    // init boiler
    Boiler_Status.wifi_connected = false;
    Boiler_Status.boiler_online  = true; // assume we have a connection, it will be checked in the loop() anyway

    // init shower
    Boiler_Shower.timerStart = 0;
    Boiler_Shower.timerPause = 0;
    Boiler_Shower.duration   = 0;
    Boiler_Shower.isColdShot = false;
}

//
// SETUP
// Note: we don't init the UART here as we should wait until everything is loaded first. It's done in loop()
//
void setup() {
    // set pin for LEDs - start up with all lit up while we sort stuff out
    pinMode(LED_RX, OUTPUT);
    pinMode(LED_TX, OUTPUT);
    pinMode(LED_ERR, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_RX, HIGH);
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_ERR, HIGH);

    // Timers
    publishValuesTimer.attach(publishValuesTime, publishValues); // every 5 mins (300000) post HA values
    systemCheckTimer.attach(systemCheckTime, systemCheck);       // every 10 seconds check if Boiler is online
    heartbeatTimer.attach(heartbeatTime, heartbeat);             //  every second blink heartbeat LED

    // set up Wifi, MQTT, Telnet
    myESP.setWifiCallback(WIFIcallback);

    myESP.setMQTTCallback(MQTTcallback);
    myESP.addSubscription(TOPIC_START);
    myESP.addSubscription(TOPIC_THERMOSTAT_TEMP);

    myESP.consoleSetHelpProjectsCmds(PROJECT_CMDS);
    myESP.consoleSetCallBackProjectCmds(myDebugCallback);
    myESP.begin(HOSTNAME);

    // init ems stats
    ems_init();

    // init Boiler specific params
    _initBoiler();

    // heartbeat, only if setting is enabled
    heartbeatEnabled = ems_getLogVerbose();
}

// flash LEDs
// Using a faster way to write to pins as digitalWrite does a lot of overhead like pin checking & disabling interrupts
void showLEDs() {
    if (ems_getLogVerbose()) {
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
}

// heartbeat callback to light up the LED, called via Ticker
void heartbeat() {
    if (heartbeatEnabled) {
        int state = digitalRead(LED_BUILTIN);
        digitalWrite(LED_BUILTIN, !state);
    }
}

// do a healthcheck every now and then to see if we connections
void systemCheck() {
    Boiler_Status.boiler_online = ((timestamp - EMS_Sys_Status.emsLastPoll) < POLL_TIMEOUT_ERR);
    if (!Boiler_Status.boiler_online) {
        myDebug("Error! Boiler unreachable. Please check connection. Retrying in 10 seconds.\n");
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
    showLEDs();

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
