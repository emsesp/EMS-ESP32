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
#include "ems_utils.h"
#include "emsuart.h"
#include "my_config.h"
#include "version.h"

// Dallas external temp sensors
#include "ds18.h"
DS18 ds18;
#define DS18_MQTT_PAYLOAD_MAXSIZE 600

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// default APP params
#define APP_NAME "EMS-ESP"
#define APP_HOSTNAME "ems-esp"
#define APP_URL "https://github.com/proddy/EMS-ESP"
#define APP_URL_API "https://api.github.com/repos/proddy/EMS-ESP"

#define DEFAULT_PUBLISHTIME 0         // 0=automatic
#define DEFAULT_SENSOR_PUBLISHTIME 10 // 10 seconds to post MQTT topics on Dallas sensors when in automatic mode
Ticker publishValuesTimer;

bool _need_first_publish = true; // this ensures on boot we always send out MQTT messages

#define SYSTEMCHECK_TIME 30 // every 30 seconds check if EMS can be reached
Ticker systemCheckTimer;

#define REGULARUPDATES_TIME 60 // every minute a call is made to fetch data from EMS devices manually
Ticker regularUpdatesTimer;

#define LEDCHECK_TIME 500 // every 1/2 second blink the heartbeat LED
Ticker ledcheckTimer;

Ticker showerColdShotStopTimer;

// if using the shower timer, change these settings
#define SHOWER_PAUSE_TIME 15000     // in ms. 15 seconds, max time if water is switched off & on during a shower
#define SHOWER_MIN_DURATION 120000  // in ms. 2 minutes, before recognizing its a shower
#define SHOWER_OFFSET_TIME 5000     // in ms. 5 seconds grace time, to calibrate actual time under the shower
#define SHOWER_COLDSHOT_DURATION 10 // in seconds. 10 seconds for cold water before turning back hot water
#define SHOWER_MAX_DURATION 420000  // in ms. 7 minutes, before trigger a shot of cold water

// set this if using an external temperature sensor like a DS18B20
#define EMSESP_DALLAS_GPIO D5
#define EMSESP_DALLAS_PARASITE false

// Set LED pin used for showing the EMS bus connection status. Solid means EMS bus working, flashing is an error
// can be either the onboard LED on the ESP8266 (LED_BULLETIN) or external via an external pull-up LED (e.g. D1 on a bbqkees' board)
// can be enabled and disabled via the 'set led' command and pin set by 'set led_gpio'
#define EMSESP_LED_GPIO LED_BUILTIN

typedef struct {
    uint8_t dallas_sensors; // count of dallas sensors

    // custom params
    bool    shower_timer;      // true if we want to report back on shower times
    bool    shower_alert;      // true if we want the alert of cold water
    bool    led;               // LED on/off
    bool    listen_mode;       // stop automatic Tx on/off
    int16_t publish_time;      // frequency of MQTT publish in seconds, -1 for off
    uint8_t led_gpio;          // pin for LED
    uint8_t dallas_gpio;       // pin for attaching external dallas temperature sensors
    bool    dallas_parasite;   // on/off is using parasite
    uint8_t tx_mode;           // TX mode 1,2 or 3
    uint8_t bus_id;            // BUS ID, defaults to 0x0B for the service key
    uint8_t master_thermostat; // Product ID of master thermostat to use, 0 for automatic
    char *  known_devices;     // list of known deviceIDs for quick boot
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
    {true, "publish_time <seconds>", "set frequency for publishing data to MQTT (-1=off, 0=automatic)"},
    {true, "tx_mode <n>", "changes Tx logic. 1=EMS generic, 2=EMS+, 3=HT3"},
    {true, "bus_id <ID>", "EMS-ESP's deviceID. 0B=Service Key (default), 0D=Modem, 0A=Hand terminal, 0F=Time module, 12=Error module"},
    {true, "master_thermostat [product id]", "set default thermostat to use. No argument lists options"},
    {false, "info", "show current values deciphered from the EMS messages"},
    {false, "log <n | b | t | s | m | r | j | v | w [ID] | d [ID]>", "logging: none, basic, thermo, solar, mixing, raw, jabber, verbose, watch a type or device"},
    {false, "publish", "publish all values to MQTT"},
    {false, "refresh", "fetch values from the EMS devices"},
    {false, "devices [scan] | [scan deep] | [save]", "list detected devices, quick scan or deep scan and save as known devices"},
    {false, "queue", "show current Tx queue"},
    {false, "send XX ...", "send raw telegram data to EMS bus (XX are hex values)"},
    {false, "thermostat read <type ID>", "send read request to the thermostat for heating circuit hc 1-4"},
    {false, "thermostat temp <degrees> [mode] [hc]", "set thermostat temperature. mode is manual,auto,heat,day,night,eco,comfort,holiday,nofrost"},
    {false, "thermostat mode <mode> [hc]", "set mode (manual,auto,heat,day,night,eco,comfort,holiday,nofrost)"},
    {false, "boiler read <type ID>", "send read request to boiler"},
    {false, "boiler wwtemp <degrees>", "set boiler warm water temperature"},
    {false, "boiler wwactive <on | off>", "set boiler warm water on/off"},
    {false, "boiler wwonetime <on | off>", "set boiler warm water onetime on/off"},
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

// figures out the thermostat mode (manual/auto) depending on the thermostat type
// returns {EMS_THERMOSTAT_MODE_UNKNOWN, EMS_THERMOSTAT_MODE_OFF, EMS_THERMOSTAT_MODE_MANUAL, EMS_THERMOSTAT_MODE_AUTO, EMS_THERMOSTAT_MODE_NIGHT, EMS_THERMOSTAT_MODE_DAY}
// hc_num is 1 to 4
_EMS_THERMOSTAT_MODE _getThermostatMode(uint8_t hc_num) {
    _EMS_THERMOSTAT_MODE thermoMode = EMS_THERMOSTAT_MODE_UNKNOWN;

    uint8_t mode  = EMS_Thermostat.hc[hc_num - 1].mode;
    uint8_t model = ems_getThermostatFlags();

    if (model == EMS_DEVICE_FLAG_RC20) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_OFF;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_MANUAL;
        } else if (mode == 2) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    } else if ((model == EMS_DEVICE_FLAG_RC300) || (model == EMS_DEVICE_FLAG_RC100)) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_MANUAL;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    } else if ((model == EMS_DEVICE_FLAG_JUNKERS1) || (model == EMS_DEVICE_FLAG_JUNKERS2)) {
        if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_MANUAL;
        } else if (mode == 2) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    } else { // default for all other thermostats
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_NIGHT;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_DAY;
        } else if (mode == 2) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    }

    return thermoMode;
}

// figures out the thermostat day/night mode depending on the thermostat type
// returns {EMS_THERMOSTAT_MODE_NIGHT, EMS_THERMOSTAT_MODE_DAY, etc...}
// hc_num is 1 to 4
_EMS_THERMOSTAT_MODE _getThermostatMode2(uint8_t hc_num) {
    _EMS_THERMOSTAT_MODE thermoMode = EMS_THERMOSTAT_MODE_UNKNOWN;
    uint8_t              model      = ems_getThermostatFlags();

    uint8_t mode = EMS_Thermostat.hc[hc_num - 1].mode_type;

    if ((model == EMS_DEVICE_FLAG_JUNKERS1) || (model == EMS_DEVICE_FLAG_JUNKERS2)) {
        if (mode == 3) {
            thermoMode = EMS_THERMOSTAT_MODE_DAY;
        } else if (mode == 2) {
            thermoMode = EMS_THERMOSTAT_MODE_NIGHT;
        }
    } else if ((model == EMS_DEVICE_FLAG_RC35) || (model == EMS_DEVICE_FLAG_RC30N)) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_NIGHT;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_DAY;
        }
    } else if (model == EMS_DEVICE_FLAG_RC300) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_ECO;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_COMFORT;
        }
    } else if (model == EMS_DEVICE_FLAG_RC100) {
        thermoMode = EMS_THERMOSTAT_MODE_DAY; // no modes on these devices
    }

    return thermoMode;
}

// Info - display status and data on an 'info' command
void showInfo() {
    static char buffer_type[200] = {0};

    myDebug_P(PSTR("%sEMS-ESP system status:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
    _EMS_SYS_LOGGING sysLog = ems_getLogging();
    if (sysLog == EMS_SYS_LOGGING_BASIC) {
        myDebug_P(PSTR("  System logging set to Basic"));
    } else if (sysLog == EMS_SYS_LOGGING_VERBOSE) {
        myDebug_P(PSTR("  System logging set to Verbose"));
    } else if (sysLog == EMS_SYS_LOGGING_THERMOSTAT) {
        myDebug_P(PSTR("  System logging set to Thermostat only"));
    } else if (sysLog == EMS_SYS_LOGGING_SOLARMODULE) {
        myDebug_P(PSTR("  System logging set to Solar Module only"));
    } else if (sysLog == EMS_SYS_LOGGING_MIXINGMODULE) {
        myDebug_P(PSTR("  System logging set to Mixing Module only"));
    } else if (sysLog == EMS_SYS_LOGGING_JABBER) {
        myDebug_P(PSTR("  System logging set to Jabber"));
    } else if (sysLog == EMS_SYS_LOGGING_WATCH) {
        myDebug_P(PSTR("  System logging set to Watch"));
    } else if (sysLog == EMS_SYS_LOGGING_DEVICE) {
        myDebug_P(PSTR("  System logging set to device"));
    } else {
        myDebug_P(PSTR("  System logging set to None"));
    }

    myDebug_P(PSTR("  LED: %s, Listen mode: %s"), EMSESP_Settings.led ? "on" : "off", EMSESP_Settings.listen_mode ? "on" : "off");
    if (EMSESP_Settings.dallas_sensors > 0) {
        myDebug_P(PSTR("  %d external temperature sensor%s found"), EMSESP_Settings.dallas_sensors, (EMSESP_Settings.dallas_sensors == 1) ? "" : "s");
    }

    myDebug_P(PSTR("  Boiler: %s, Thermostat: %s, Solar Module: %s, Mixing Module: %s"),
              (ems_getBoilerEnabled() ? "enabled" : "disabled"),
              (ems_getThermostatEnabled() ? "enabled" : "disabled"),
              (ems_getSolarModuleEnabled() ? "enabled" : "disabled"),
              (ems_getMixingModuleEnabled() ? "enabled" : "disabled"));

    myDebug_P(PSTR("  Shower Timer: %s, Shower Alert: %s"),
              ((EMSESP_Settings.shower_timer) ? "enabled" : "disabled"),
              ((EMSESP_Settings.shower_alert) ? "enabled" : "disabled"));

    myDebug_P(PSTR("\n%sEMS Bus status:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);

    if (ems_getBusConnected()) {
        myDebug_P(PSTR("  Bus is connected, protocol: %s"), (ems_isHT3() ? "HT3" : "Buderus"));
        myDebug_P(PSTR("  Rx: # successful read requests=%d, # CRC errors=%d"), EMS_Sys_Status.emsRxPgks, EMS_Sys_Status.emxCrcErr);
        if (strlen(EMSESP_Settings.known_devices) > 0) {
            myDebug_P(PSTR("  Saved known device IDs: %s"), EMSESP_Settings.known_devices);
        }

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

    // show boiler data if connected
    if (ems_getBoilerEnabled()) {
        myDebug_P(PSTR("%sBoiler data:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);

        // version details
        myDebug_P(PSTR("  Boiler: %s"), ems_getDeviceDescription(EMS_DEVICE_TYPE_BOILER, buffer_type));

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
        myDebug_P(PSTR("  Warm Water circulation pump type: %s"), EMS_Boiler.wWCircPumpType ? "charge pump" : "3-way pump");
        if (EMS_Boiler.wWCircPumpMode == 7) {
            myDebug_P(PSTR("  Warm Water circulation pump freq: continuous"));
        } else {
            char s[7];
            char buffer[2];
            buffer[0] = (EMS_Boiler.wWCircPumpMode % 10) + '0';
            buffer[1] = '\0';
            strlcpy(s, buffer, 3);
            strlcat(s, "x3min", sizeof(s));
            myDebug_P(PSTR("  Warm Water circulation pump freq: %s"), s);
        }
        if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
            myDebug_P(PSTR("  Warm Water comfort setting: Hot"));
        } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
            myDebug_P(PSTR("  Warm Water comfort setting: Eco"));
        } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
            myDebug_P(PSTR("  Warm Water comfort setting: Intelligent"));
        }

        _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
        _renderIntValue("Warm Water desinfection temperature", "C", EMS_Boiler.wWDesinfectTemp);
        _renderBoolValue("Warm Water circulation active", EMS_Boiler.wWCirc);

        // UBAMonitorWWMessage
        _renderIntValue("Warm Water set temperature", "C", EMS_Boiler.wWSetTmp);
        _renderUShortValue("Warm Water current temperature", "C", EMS_Boiler.wWCurTmp);
        _renderUShortValue("Warm water temperature (intern)", "C", EMS_Boiler.wwStorageTemp1);
        _renderUShortValue("Warm water temperature (extern)", "C", EMS_Boiler.wwStorageTemp2);
        _renderUShortValue("Warm Water current temperature (extern)", "C", EMS_Boiler.wWCurTmp2);
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
        _renderUShortValue("Max boiler temperature", "C", EMS_Boiler.boilTemp);
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
        if (EMS_Boiler.extTemp > EMS_VALUE_SHORT_NOTSET) {
            _renderShortValue("Outside temperature", "C", EMS_Boiler.extTemp);
        }

        _renderUShortValue("Exhaust temperature", "C", EMS_Boiler.exhaustTemp);
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
    }

    // For SM10/SM100/SM200 Solar Module
    if (ems_getSolarModuleEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sSolar Module data:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Solar module: %s"), ems_getDeviceDescription(EMS_DEVICE_TYPE_SOLAR, buffer_type));
        _renderShortValue("Collector temperature (TS1)", "C", EMS_SolarModule.collectorTemp);
        _renderShortValue("Bottom temperature (TS2)", "C", EMS_SolarModule.bottomTemp);
        _renderShortValue("Bottom temperature (TS5)", "C", EMS_SolarModule.bottomTemp2);
        _renderIntValue("Pump modulation", "%", EMS_SolarModule.pumpModulation);
        _renderBoolValue("Valve (VS2) status", EMS_SolarModule.valveStatus);
        _renderBoolValue("Pump (PS1) active", EMS_SolarModule.pump);
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
        myDebug_P(PSTR("%sHeat Pump data:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Heat Pump module: %s"), ems_getDeviceDescription(EMS_DEVICE_TYPE_HEATPUMP, buffer_type));
        _renderIntValue("Pump modulation", "%", EMS_HeatPump.HPModulation);
        _renderIntValue("Pump speed", "%", EMS_HeatPump.HPSpeed);
    }

    // Thermostat data
    if (ems_getThermostatEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sThermostat data:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Thermostat: %s"), ems_getDeviceDescription(EMS_DEVICE_TYPE_THERMOSTAT, buffer_type, false));

        // Render Thermostat Date & Time
        uint8_t model = ems_getThermostatFlags();
        if (strlen(EMS_Thermostat.datetime) > 2) {
            myDebug_P(PSTR("  Thermostat time is %s"), EMS_Thermostat.datetime);
        }

        uint8_t _m_setpoint, _m_curr;
        switch (model) {
        case EMS_DEVICE_FLAG_EASY:
            _m_setpoint = 10; // *100
            _m_curr     = 10; // *100
            break;
        case EMS_DEVICE_FLAG_JUNKERS1:
        case EMS_DEVICE_FLAG_JUNKERS2:
            _m_setpoint = 1; // *10
            _m_curr     = 1; // *10
            break;
        default:             // RC30, RC35 etc...
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
                _renderShortValue(" Setpoint room temperature", "C", EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp, _m_setpoint);

                // Render Day/Night/Holiday Temperature on RC35s
                // there is no single setpoint temp, but one for day, night and vacation
                if ((model == EMS_DEVICE_FLAG_RC35) || (model == EMS_DEVICE_FLAG_RC30N)) {
                    if (EMS_Thermostat.hc[hc_num - 1].summer_mode) {
                        myDebug_P(PSTR("   Program is set to Summer mode"));
                    } else if (EMS_Thermostat.hc[hc_num - 1].holiday_mode) {
                        myDebug_P(PSTR("   Program is set to Holiday mode"));
                    }

                    _renderIntValue(" Day temperature", "C", EMS_Thermostat.hc[hc_num - 1].daytemp, 2);          // convert to a single byte * 2
                    _renderIntValue(" Night temperature", "C", EMS_Thermostat.hc[hc_num - 1].nighttemp, 2);      // convert to a single byte * 2
                    _renderIntValue(" Vacation temperature", "C", EMS_Thermostat.hc[hc_num - 1].holidaytemp, 2); // convert to a single byte * 2
                }

                // show flow temp if we have it
                if (EMS_Thermostat.hc[hc_num - 1].circuitcalctemp < EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Calculated flow temperature", "C", EMS_Thermostat.hc[hc_num - 1].circuitcalctemp);

                // Render Thermostat Mode
                _EMS_THERMOSTAT_MODE thermoMode;
                thermoMode = _getThermostatMode(hc_num);
                if (thermoMode == EMS_THERMOSTAT_MODE_OFF) {
                    myDebug_P(PSTR("   Mode is set to off"));
                } else if (thermoMode == EMS_THERMOSTAT_MODE_MANUAL) {
                    myDebug_P(PSTR("   Mode is set to manual"));
                } else if (thermoMode == EMS_THERMOSTAT_MODE_AUTO) {
                    myDebug_P(PSTR("   Mode is set to auto"));
                } else if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
                    myDebug_P(PSTR("   Mode is set to night"));
                } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
                    myDebug_P(PSTR("   Mode is set to day"));
                }

                // Render Thermostat Mode2, only if its in Auto mode
                if (thermoMode == EMS_THERMOSTAT_MODE_AUTO) {
                    thermoMode = _getThermostatMode2(hc_num);
                    if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
                        myDebug_P(PSTR("   Mode type is set to night"));
                    } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
                        myDebug_P(PSTR("   Mode type is set to day"));
                    } else if (thermoMode == EMS_THERMOSTAT_MODE_COMFORT) {
                        myDebug_P(PSTR("   Mode type is set to comfort"));
                    } else if (thermoMode == EMS_THERMOSTAT_MODE_ECO) {
                        myDebug_P(PSTR("   Mode type is set to eco"));
                    }
                }
            }
        }
    }

    // Mixing modules sensors
    if (ems_getMixingModuleEnabled()) {
        myDebug_P(PSTR("")); // newline
        myDebug_P(PSTR("%sMixing module data:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        myDebug_P(PSTR("  Mixing Module: %s"), ems_getDeviceDescription(EMS_DEVICE_TYPE_MIXING, buffer_type, false));
        if ((EMS_Boiler.switchTemp < EMS_VALUE_USHORT_NOTSET) && (EMS_Boiler.switchTemp != 0)) {
            _renderUShortValue("Switch temperature", "C", EMS_Boiler.switchTemp);
        }

        for (uint8_t hc_num = 1; hc_num <= EMS_MIXING_MAXHC; hc_num++) {
            if (EMS_MixingModule.hc[hc_num - 1].active) {
                myDebug_P(PSTR("  Mixing Circuit %d"), hc_num);
                if (EMS_MixingModule.hc[hc_num - 1].flowTemp < EMS_VALUE_USHORT_NOTSET)
                    _renderUShortValue(" Current flow temperature", "C", EMS_MixingModule.hc[hc_num - 1].flowTemp);
                if (EMS_MixingModule.hc[hc_num - 1].flowSetTemp != EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Setpoint flow temperature", "C", EMS_MixingModule.hc[hc_num - 1].flowSetTemp);
                if (EMS_MixingModule.hc[hc_num - 1].pumpMod != EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Current pump modulation", "%", EMS_MixingModule.hc[hc_num - 1].pumpMod);
                if (EMS_MixingModule.hc[hc_num - 1].valveStatus != EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Current valve status", "", EMS_MixingModule.hc[hc_num - 1].valveStatus);
            }
        }

        for (uint8_t wwc_num = 1; wwc_num <= EMS_MIXING_MAXWWC; wwc_num++) {
            if (EMS_MixingModule.wwc[wwc_num - 1].active) {
                myDebug_P(PSTR("  Warm Water Circuit %d"), wwc_num);
                if (EMS_MixingModule.wwc[wwc_num - 1].flowTemp < EMS_VALUE_USHORT_NOTSET)
                    _renderUShortValue(" Current warm water temperature", "C", EMS_MixingModule.wwc[wwc_num - 1].flowTemp);
                if (EMS_MixingModule.wwc[wwc_num - 1].pumpMod != EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Current pump status", "", EMS_MixingModule.wwc[wwc_num - 1].pumpMod);
                if (EMS_MixingModule.wwc[wwc_num - 1].tempStatus != EMS_VALUE_INT_NOTSET)
                    _renderIntValue(" Current temp status", "", EMS_MixingModule.wwc[wwc_num - 1].tempStatus);
            }
        }
    }

    // Dallas external temp sensors
    if (EMSESP_Settings.dallas_sensors) {
        myDebug_P(PSTR("")); // newline
        char buffer[128]  = {0};
        char buffer2[128] = {0};
        char valuestr[8]  = {0}; // for formatting temp
        myDebug_P(PSTR("%sExternal temperature sensors:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            float sensorValue = ds18.getValue(i);
            if (sensorValue != DS18_DISCONNECTED) {
                myDebug_P(PSTR("  Sensor #%d type: %s id: %s temperature: %s C"),
                          i + 1,
                          ds18.getDeviceType(buffer, i),
                          ds18.getDeviceID(buffer2, i),
                          _float_to_char(valuestr, sensorValue));
            }
        }
    }

    myDebug_P(PSTR("")); // newline
}

// scan for external Dallas sensors and display
void scanDallas() {
    EMSESP_Settings.dallas_sensors = ds18.scan();
    if (EMSESP_Settings.dallas_sensors) {
        char buffer[128];
        char buffer2[128];
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            myDebug_P(PSTR("External temperature sensor found, type: %s id: %s"), ds18.getDeviceType(buffer, i), ds18.getDeviceID(buffer2, i));
        }
    }
}

// send all dallas sensor values as a JSON package to MQTT
bool publishSensorValues() {
    // don't send if MQTT is connected
    if (!myESP.isMQTTConnected() || (EMSESP_Settings.publish_time == -1)) {
        return false;
    }

    if (!EMSESP_Settings.dallas_sensors) {
        return false; // no sensors attached
    }

    // each payload per sensor is 30 bytes so calculate if we have enough space
    if ((EMSESP_Settings.dallas_sensors * 50) > DS18_MQTT_PAYLOAD_MAXSIZE) {
        myDebug_P(PSTR("Error: too many Dallas sensors for MQTT payload"));
    }

    StaticJsonDocument<DS18_MQTT_PAYLOAD_MAXSIZE> doc;
    JsonObject                                    sensors = doc.to<JsonObject>();

    bool hasdata = false;
    char buffer[128]; // temp string buffer

    // if we're not using nested JSON, send each sensor out seperately
    if (!myESP.mqttUseNestedJson()) {
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            float sensorValue = ds18.getValue(i);
            if (sensorValue != DS18_DISCONNECTED) {
                hasdata = true;
                char topic[30];                                        // sensors{1-n}
                strlcpy(topic, TOPIC_EXTERNAL_SENSORS, sizeof(topic)); // create topic
                strlcat(topic, _int_to_char(buffer, i + 1), sizeof(topic));
                sensors[PAYLOAD_EXTERNAL_SENSOR_ID]   = ds18.getDeviceID(buffer, i); // add ID
                sensors[PAYLOAD_EXTERNAL_SENSOR_TEMP] = sensorValue;                 // add temp value
                myESP.mqttPublish(topic, doc);                                       // and publish
            }
        }
        if (hasdata) {
            myDebugLog("Publishing external sensor data via MQTT");
        }
        return true; // exit
    }

    // group all sensors together - https://github.com/proddy/EMS-ESP/issues/327
    for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
        float sensorValue = ds18.getValue(i);
        if (sensorValue != DS18_DISCONNECTED) {
            hasdata = true;
#ifdef SENSOR_MQTT_USEID
            sensors[ds18.getDeviceID(buffer, i)] = sensorValue;
#else
            char sensorID[10]; // sensor{1-n}
            strlcpy(sensorID, PAYLOAD_EXTERNAL_SENSOR_NUM, sizeof(sensorID));
            strlcat(sensorID, _int_to_char(buffer, i + 1), sizeof(sensorID));
            JsonObject dataSensor                    = sensors.createNestedObject(sensorID);
            dataSensor[PAYLOAD_EXTERNAL_SENSOR_ID]   = ds18.getDeviceID(buffer, i);
            dataSensor[PAYLOAD_EXTERNAL_SENSOR_TEMP] = sensorValue;
#endif
        }
    }

    if (hasdata) {
        myDebugLog("Publishing external sensor data via MQTT");
        return (myESP.mqttPublish(TOPIC_EXTERNAL_SENSORS, doc));
    }

    return false;
}

// publish Boiler data via MQTT
bool publishEMSValues_boiler() {
    const size_t        capacity = JSON_OBJECT_SIZE(47); // must recalculate if more objects addded https://arduinojson.org/v6/assistant/
    DynamicJsonDocument doc(capacity);
    JsonObject          rootBoiler = doc.to<JsonObject>();

    char s[20]; // for formatting strings

    if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
        rootBoiler["wWComfort"] = "Hot";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
        rootBoiler["wWComfort"] = "Eco";
    } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
        rootBoiler["wWComfort"] = "Intelligent";
    }

    if (EMS_Boiler.wWSelTemp != EMS_VALUE_INT_NOTSET) {
        rootBoiler["wWSelTemp"] = EMS_Boiler.wWSelTemp;
    }
    if (EMS_Boiler.wWDesinfectTemp != EMS_VALUE_INT_NOTSET) {
        rootBoiler["wWDesinfectionTemp"] = EMS_Boiler.wWDesinfectTemp;
    }
    if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET) {
        rootBoiler["selFlowTemp"] = EMS_Boiler.selFlowTemp;
    }
    if (EMS_Boiler.selBurnPow != EMS_VALUE_INT_NOTSET) {
        rootBoiler["selBurnPow"] = EMS_Boiler.selBurnPow;
    }
    if (EMS_Boiler.curBurnPow != EMS_VALUE_INT_NOTSET) {
        rootBoiler["curBurnPow"] = EMS_Boiler.curBurnPow;
    }
    if (EMS_Boiler.pumpMod != EMS_VALUE_INT_NOTSET) {
        rootBoiler["pumpMod"] = EMS_Boiler.pumpMod;
    }
    if (EMS_Boiler.wWCircPump != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWCircPump"] = EMS_Boiler.wWCircPump;
    }
    if (EMS_Boiler.wWCircPumpType != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWCiPuType"] = EMS_Boiler.wWCircPumpType;
    }
    if (EMS_Boiler.wWCircPumpMode != EMS_VALUE_INT_NOTSET) {
        rootBoiler["wWCiPuMode"] = EMS_Boiler.wWCircPumpMode;
    }
    if (EMS_Boiler.extTemp > EMS_VALUE_SHORT_NOTSET) {
        rootBoiler["outdoorTemp"] = (float)EMS_Boiler.extTemp / 10;
    }
    if (EMS_Boiler.wWCurTmp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["wWCurTmp"] = (float)EMS_Boiler.wWCurTmp / 10;
    }
    if (EMS_Boiler.wWCurFlow != EMS_VALUE_INT_NOTSET) {
        rootBoiler["wWCurFlow"] = (float)EMS_Boiler.wWCurFlow / 10;
    }
    if (EMS_Boiler.curFlowTemp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["curFlowTemp"] = (float)EMS_Boiler.curFlowTemp / 10;
    }
    if (EMS_Boiler.retTemp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["retTemp"] = (float)EMS_Boiler.retTemp / 10;
    }
    if (EMS_Boiler.switchTemp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["switchTemp"] = (float)EMS_Boiler.switchTemp / 10;
    }
    if (EMS_Boiler.sysPress != EMS_VALUE_INT_NOTSET) {
        rootBoiler["sysPress"] = (float)EMS_Boiler.sysPress / 10;
    }
    if (EMS_Boiler.boilTemp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["boilTemp"] = (float)EMS_Boiler.boilTemp / 10;
    }
    if (EMS_Boiler.wwStorageTemp1 < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["wwStorageTemp1"] = (float)EMS_Boiler.wwStorageTemp1 / 10;
    }
    if (EMS_Boiler.wwStorageTemp2 < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["wwStorageTemp2"] = (float)EMS_Boiler.wwStorageTemp2 / 10;
    }
    if (EMS_Boiler.exhaustTemp < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["exhaustTemp"] = (float)EMS_Boiler.exhaustTemp / 10;
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWOnetime"] = _bool_to_char(s, EMS_Boiler.wWOneTime);
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWDesinfecting"] = _bool_to_char(s, EMS_Boiler.wWDesinfecting);
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWReady"] = _bool_to_char(s, EMS_Boiler.wWReadiness);
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWRecharge"] = _bool_to_char(s, EMS_Boiler.wWRecharging);
    }
    if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWTempOK"] = _bool_to_char(s, EMS_Boiler.wWTemperatureOK);
    }
    if (EMS_Boiler.wWCirc != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWCirc"] = _bool_to_char(s, EMS_Boiler.wWCirc);
    }
    if (EMS_Boiler.burnGas != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["burnGas"] = _bool_to_char(s, EMS_Boiler.burnGas);
    }
    if (EMS_Boiler.flameCurr < EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["flameCurr"] = (float)(int16_t)EMS_Boiler.flameCurr / 10;
    }
    if (EMS_Boiler.heatPmp != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["heatPmp"] = _bool_to_char(s, EMS_Boiler.heatPmp);
    }
    if (EMS_Boiler.fanWork != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["fanWork"] = _bool_to_char(s, EMS_Boiler.fanWork);
    }
    if (EMS_Boiler.ignWork != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["ignWork"] = _bool_to_char(s, EMS_Boiler.ignWork);
    }
    if (EMS_Boiler.heating_temp != EMS_VALUE_INT_NOTSET) {
        rootBoiler["heating_temp"] = EMS_Boiler.heating_temp;
    }
    if (EMS_Boiler.pump_mod_max != EMS_VALUE_INT_NOTSET) {
        rootBoiler["pump_mod_max"] = EMS_Boiler.pump_mod_max;
    }
    if (EMS_Boiler.pump_mod_min != EMS_VALUE_INT_NOTSET) {
        rootBoiler["pump_mod_min"] = EMS_Boiler.pump_mod_min;
    }
    if (EMS_Boiler.wWHeat != EMS_VALUE_BOOL_NOTSET) {
        rootBoiler["wWHeat"] = _bool_to_char(s, EMS_Boiler.wWHeat);
    }
    if (abs(EMS_Boiler.wWStarts) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["wWStarts"] = (float)EMS_Boiler.wWStarts;
    }
    if (abs(EMS_Boiler.wWWorkM) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["wWWorkM"] = (float)EMS_Boiler.wWWorkM;
    }
    if (abs(EMS_Boiler.UBAuptime) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["UBAuptime"] = (float)EMS_Boiler.UBAuptime;
    }
    if (abs(EMS_Boiler.burnStarts) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["burnStarts"] = (float)EMS_Boiler.burnStarts;
    }
    if (abs(EMS_Boiler.burnWorkMin) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["burnWorkMin"] = (float)EMS_Boiler.burnWorkMin;
    }
    if (abs(EMS_Boiler.heatWorkMin) != EMS_VALUE_LONG_NOTSET) {
        rootBoiler["heatWorkMin"] = (float)EMS_Boiler.heatWorkMin;
    }

    if (EMS_Boiler.serviceCode != EMS_VALUE_USHORT_NOTSET) {
        rootBoiler["ServiceCode"]       = EMS_Boiler.serviceCodeChar;
        rootBoiler["ServiceCodeNumber"] = EMS_Boiler.serviceCode;
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    static uint8_t last_boilerActive = 0xFF; // for remembering last setting of the tap water or heating on/off
    if (last_boilerActive != ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive)) {
        myESP.mqttPublish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive ? "1" : "0");
        myESP.mqttPublish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive ? "1" : "0");
        last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
    }

    return (myESP.mqttPublish(TOPIC_BOILER_DATA, doc));
}

// handle the thermostat values
bool publishEMSValues_thermostat() {
    StaticJsonDocument<MYESP_JSON_MAXSIZE_MEDIUM> doc;
    JsonObject                                    rootThermostat = doc.to<JsonObject>();
    JsonObject                                    dataThermostat;

    bool has_data = false;

    for (uint8_t hc_v = 1; hc_v <= EMS_THERMOSTAT_MAXHC; hc_v++) {
        _EMS_Thermostat_HC * thermostat = &EMS_Thermostat.hc[hc_v - 1];

        // only send if we have an active Heating Circuit with an actual setpoint temp temperature values
        if ((thermostat->active) && (thermostat->setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)) {
            uint8_t model = ems_getThermostatFlags(); // fetch model flags
            has_data      = true;

            if (myESP.mqttUseNestedJson()) {
                // create nested json for each HC
                char hc[10]; // hc{1-4}
                strlcpy(hc, THERMOSTAT_HC, sizeof(hc));
                char s[20]; // for formatting strings
                strlcat(hc, _int_to_char(s, thermostat->hc), sizeof(hc));
                dataThermostat = rootThermostat.createNestedObject(hc);
            } else {
                dataThermostat = rootThermostat;
            }

            // different logic depending on thermostat types
            if (model == EMS_DEVICE_FLAG_EASY) {
                if (thermostat->setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_SELTEMP] = (float)thermostat->setpoint_roomTemp / 100;
                if (thermostat->curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_CURRTEMP] = (float)thermostat->curr_roomTemp / 100;
            } else if ((model == EMS_DEVICE_FLAG_JUNKERS1) || (model == EMS_DEVICE_FLAG_JUNKERS2)) {
                if (thermostat->setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_SELTEMP] = (float)thermostat->setpoint_roomTemp / 10;
                if (thermostat->curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_CURRTEMP] = (float)thermostat->curr_roomTemp / 10;
            } else {
                if (thermostat->setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_SELTEMP] = (float)thermostat->setpoint_roomTemp / 2;
                if (thermostat->curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                    dataThermostat[THERMOSTAT_CURRTEMP] = (float)thermostat->curr_roomTemp / 10;

                if (thermostat->daytemp != EMS_VALUE_INT_NOTSET)
                    dataThermostat[THERMOSTAT_DAYTEMP] = (float)thermostat->daytemp / 2;
                if (thermostat->nighttemp != EMS_VALUE_INT_NOTSET)
                    dataThermostat[THERMOSTAT_NIGHTTEMP] = (float)thermostat->nighttemp / 2;
                if (thermostat->holidaytemp != EMS_VALUE_INT_NOTSET)
                    dataThermostat[THERMOSTAT_HOLIDAYTEMP] = (float)thermostat->holidaytemp / 2;

                if (thermostat->heatingtype != EMS_VALUE_INT_NOTSET)
                    dataThermostat[THERMOSTAT_HEATINGTYPE] = thermostat->heatingtype;

                if (thermostat->circuitcalctemp != EMS_VALUE_INT_NOTSET)
                    dataThermostat[THERMOSTAT_CIRCUITCALCTEMP] = thermostat->circuitcalctemp;
            }

            // Thermostat Mode
            _EMS_THERMOSTAT_MODE thermoMode = _getThermostatMode(hc_v);
            if (thermoMode == EMS_THERMOSTAT_MODE_OFF) {
                dataThermostat[THERMOSTAT_MODE] = "off";
            } else if (thermoMode == EMS_THERMOSTAT_MODE_MANUAL) {
                dataThermostat[THERMOSTAT_MODE] = "manual";
            } else if (thermoMode == EMS_THERMOSTAT_MODE_AUTO) {
                dataThermostat[THERMOSTAT_MODE] = "auto";
            } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
                dataThermostat[THERMOSTAT_MODE] = "day";
            } else if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
                dataThermostat[THERMOSTAT_MODE] = "night";
            }
            // Render Thermostat Mode2, only if its in Auto mode
            if (thermoMode == EMS_THERMOSTAT_MODE_AUTO) {
                thermoMode = _getThermostatMode2(hc_v);
                if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
                    dataThermostat[THERMOSTAT_MODETYPE] = "night";
                } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
                    dataThermostat[THERMOSTAT_MODETYPE] = "day";
                } else if (thermoMode == EMS_THERMOSTAT_MODE_COMFORT) {
                    dataThermostat[THERMOSTAT_MODETYPE] = "comfort";
                } else if (thermoMode == EMS_THERMOSTAT_MODE_ECO) {
                    dataThermostat[THERMOSTAT_MODETYPE] = "eco";
                }
            }

            // if its not nested, send immediately
            if (!myESP.mqttUseNestedJson()) {
                char topic[30];
                char s[20]; // for formatting strings
                strlcpy(topic, TOPIC_THERMOSTAT_DATA, sizeof(topic));
                strlcat(topic, _int_to_char(s, thermostat->hc), sizeof(topic)); // append hc to topic
                char data[MYESP_JSON_MAXSIZE_MEDIUM];
                serializeJson(doc, data);
                myESP.mqttPublish(topic, data);
            }
        }
    }

    // if we're using nested json, send all in one go
    if (myESP.mqttUseNestedJson() && has_data) {
        char data[MYESP_JSON_MAXSIZE_MEDIUM];
        serializeJson(doc, data);
        myESP.mqttPublish(TOPIC_THERMOSTAT_DATA, data);
    }

    return (has_data);
}

// publish mixing data
// only sending if we have an active hc
bool publishEMSValues_mixing() {
    char                                          s[20]; // for formatting strings
    StaticJsonDocument<MYESP_JSON_MAXSIZE_MEDIUM> doc;
    JsonObject                                    rootMixing = doc.to<JsonObject>();
    bool                                          has_data   = false;

    for (uint8_t hc_v = 1; hc_v <= EMS_MIXING_MAXHC; hc_v++) {
        _EMS_MixingModule_HC * mixingHC = &EMS_MixingModule.hc[hc_v - 1];

        if (mixingHC->active) {
            has_data = true;
            char hc[10]; // hc{1-4}
            strlcpy(hc, MIXING_HC, sizeof(hc));
            strlcat(hc, _int_to_char(s, mixingHC->hc), sizeof(hc));
            JsonObject dataMixingHC = rootMixing.createNestedObject(hc);
            if (mixingHC->flowTemp < EMS_VALUE_USHORT_NOTSET)
                dataMixingHC["flowTemp"] = (float)mixingHC->flowTemp / 10;
            if (mixingHC->flowSetTemp != EMS_VALUE_INT_NOTSET)
                dataMixingHC["setflowTemp"] = mixingHC->flowSetTemp;
            if (mixingHC->pumpMod != EMS_VALUE_INT_NOTSET)
                dataMixingHC["pumpMod"] = mixingHC->pumpMod;
            if (mixingHC->valveStatus != EMS_VALUE_INT_NOTSET)
                dataMixingHC["valveStatus"] = mixingHC->valveStatus;
        }
    }

    for (uint8_t wwc_v = 1; wwc_v <= EMS_MIXING_MAXWWC; wwc_v++) {
        _EMS_MixingModule_WWC * mixingWWC = &EMS_MixingModule.wwc[wwc_v - 1];

        if (mixingWWC->active) {
            has_data = true;
            char wwc[10]; // wwc{1-2}
            strlcpy(wwc, MIXING_WWC, sizeof(wwc));
            strlcat(wwc, _int_to_char(s, mixingWWC->wwc), sizeof(wwc));
            JsonObject dataMixing = rootMixing.createNestedObject(wwc);
            if (mixingWWC->flowTemp < EMS_VALUE_USHORT_NOTSET)
                dataMixing["wwTemp"] = (float)mixingWWC->flowTemp / 10;
            if (mixingWWC->pumpMod != EMS_VALUE_INT_NOTSET)
                dataMixing["pumpStatus"] = mixingWWC->pumpMod;
            if (mixingWWC->tempStatus != EMS_VALUE_INT_NOTSET)
                dataMixing["tempStatus"] = mixingWWC->tempStatus;
        }
    }

    if (has_data) {
        myESP.mqttPublish(TOPIC_MIXING_DATA, doc);
        return true;
    }

    return false;
}

// For SM10 and SM100/SM200 Solar Modules
bool publishEMSValues_solar() {
    StaticJsonDocument<MYESP_JSON_MAXSIZE_SMALL> doc;
    JsonObject                                   rootSM = doc.to<JsonObject>();

    if (EMS_SolarModule.collectorTemp > EMS_VALUE_SHORT_NOTSET) {
        rootSM[SM_COLLECTORTEMP] = (float)EMS_SolarModule.collectorTemp / 10;
    }
    if (EMS_SolarModule.bottomTemp > EMS_VALUE_SHORT_NOTSET) {
        rootSM[SM_BOTTOMTEMP] = (float)EMS_SolarModule.bottomTemp / 10;
    }
    if (EMS_SolarModule.bottomTemp2 > EMS_VALUE_SHORT_NOTSET) {
        rootSM[SM_BOTTOMTEMP2] = (float)EMS_SolarModule.bottomTemp2 / 10;
    }
    if (EMS_SolarModule.pumpModulation != EMS_VALUE_INT_NOTSET) {
        rootSM[SM_PUMPMODULATION] = EMS_SolarModule.pumpModulation;
    }
    if (EMS_SolarModule.pump != EMS_VALUE_BOOL_NOTSET) {
        char s[20];
        rootSM[SM_PUMP] = _bool_to_char(s, EMS_SolarModule.pump);
    }
    if (EMS_SolarModule.valveStatus != EMS_VALUE_BOOL_NOTSET) {
        char s[20];
        rootSM[SM_VALVESTATUS] = _bool_to_char(s, EMS_SolarModule.valveStatus);
    }
    if (EMS_SolarModule.pumpWorkMin != EMS_VALUE_LONG_NOTSET) {
        rootSM[SM_PUMPWORKMIN] = (float)EMS_SolarModule.pumpWorkMin;
    }
    if (EMS_SolarModule.EnergyLastHour < EMS_VALUE_USHORT_NOTSET) {
        rootSM[SM_ENERGYLASTHOUR] = (float)EMS_SolarModule.EnergyLastHour / 10;
    }
    if (EMS_SolarModule.EnergyToday < EMS_VALUE_USHORT_NOTSET) {
        rootSM[SM_ENERGYTODAY] = EMS_SolarModule.EnergyToday;
    }
    if (EMS_SolarModule.EnergyTotal < EMS_VALUE_USHORT_NOTSET) {
        rootSM[SM_ENERGYTOTAL] = (float)EMS_SolarModule.EnergyTotal / 10;
    }

    return (myESP.mqttPublish(TOPIC_SM_DATA, doc));
}

// handle Heat Pump
// returns true if added to MQTT queue went ok
bool publishEMSValues_heatpump() {
    StaticJsonDocument<MYESP_JSON_MAXSIZE_SMALL> doc;
    JsonObject                                   rootHP = doc.to<JsonObject>();

    if (EMS_HeatPump.HPModulation != EMS_VALUE_INT_NOTSET) {
        rootHP[HP_PUMPMODULATION] = EMS_HeatPump.HPModulation;
    }
    if (EMS_HeatPump.HPSpeed != EMS_VALUE_INT_NOTSET) {
        rootHP[HP_PUMPSPEED] = EMS_HeatPump.HPSpeed;
    }

    myDebugLog("Publishing heat pump data via MQTT");
    return (myESP.mqttPublish(TOPIC_HP_DATA, doc));
}

// Publish shower data
// returns true if added to MQTT queue went ok
bool do_publishShowerData() {
    StaticJsonDocument<MYESP_JSON_MAXSIZE_SMALL> doc;
    JsonObject                                   rootShower = doc.to<JsonObject>();
    rootShower[TOPIC_SHOWER_TIMER]                          = EMSESP_Settings.shower_timer ? "1" : "0";
    rootShower[TOPIC_SHOWER_ALERT]                          = EMSESP_Settings.shower_alert ? "1" : "0";

    // only publish shower duration if there is a value
    char s[50] = {0};
    if (EMSESP_Shower.duration > SHOWER_MIN_DURATION) {
        char buffer[16] = {0};
        strlcpy(s, itoa((uint8_t)((EMSESP_Shower.duration / (1000 * 60)) % 60), buffer, 10), sizeof(s));
        strlcat(s, " minutes and ", sizeof(s));
        strlcat(s, itoa((uint8_t)((EMSESP_Shower.duration / 1000) % 60), buffer, 10), sizeof(s));
        strlcat(s, " seconds", sizeof(s));
        rootShower[TOPIC_SHOWER_DURATION] = s;
    }

    myDebugLog("Publishing shower data via MQTT");
    return (myESP.mqttPublish(TOPIC_SHOWER_DATA, doc, false)); // Publish MQTT forcing retain to be off
}

// send values via MQTT
// a json object is created for each device type
bool publishEMSValues(bool force) {
    // don't send if MQTT is not connected or EMS bus is not connected
    if (!myESP.isMQTTConnected() || (!ems_getBusConnected()) || (EMSESP_Settings.publish_time == -1)) {
        return false;
    }

    bool thermo   = false;
    bool boiler   = false;
    bool mixing   = false;
    bool solar    = false;
    bool heatpump = false;

    if (ems_getThermostatEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_THERMOSTAT) || force)) {
        thermo = publishEMSValues_thermostat();
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_THERMOSTAT); // unset flag
    }

    if (ems_getBoilerEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_BOILER) || force)) {
        boiler = publishEMSValues_boiler();
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_BOILER); // unset flag
    }

    if (ems_getMixingModuleEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_MIXING) || force)) {
        mixing = publishEMSValues_mixing();
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_MIXING); // unset flag
    }

    if (ems_getSolarModuleEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_SOLAR) || force)) {
        solar = publishEMSValues_solar();
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_SOLAR); // unset flag
    }

    if (ems_getHeatPumpEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_HEATPUMP) || force)) {
        heatpump = publishEMSValues_heatpump();
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_HEATPUMP); // unset flag
    }

    if (!thermo && !boiler && !mixing && !solar && !heatpump) {
        return false; // nothing was sent
    }

    // print
    char log_s[50];
    strlcpy(log_s, "Publishing MQTT data for:", sizeof(log_s));
    if (thermo) {
        strlcat(log_s, " thermostat", sizeof(log_s));
    }
    if (boiler) {
        strlcat(log_s, " boiler", sizeof(log_s));
    }
    if (mixing) {
        strlcat(log_s, " mixing", sizeof(log_s));
    }
    if (solar) {
        strlcat(log_s, " solar", sizeof(log_s));
    }
    if (heatpump) {
        strlcat(log_s, " heatpump", sizeof(log_s));
    }
    myDebugLog(log_s);

    return true;
}

// publishes value via MQTT
void publishValues(bool force, bool send_sensor) {
    if (EMSESP_Settings.publish_time == -1) {
        return;
    }

    // myDebugLog("Starting scheduled MQTT publish...");
    publishEMSValues(force);
    if (send_sensor) {
        publishSensorValues();
    }
    // myESP.heartbeatCheck(true);
}

// calls publishValues fron the Ticker loop, also sending sensor data
// but not using false for force so only data that has changed will be sent
void do_publishValues() {
    publishValues(false, true);
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

// do a system health check every now and then to see if we all connections
void do_systemCheck() {
    if (!ems_getBusConnected() && !myESP.getUseSerial()) {
        myDebug_P(PSTR("Error! Unable to read the EMS bus."));
    }
}

// force calls to get data from EMS for the types that aren't sent as broadcasts
// only if we have a EMS connection
// which will cause the values to get published
void do_regularUpdates() {
    if (ems_getBusConnected() && !ems_getTxDisabled()) {
        myDebugLog("Fetching data from EMS devices");
        ems_getThermostatValues();
        ems_getBoilerValues();
        ems_getSolarModuleValues();
    }
}

// turn back on the hot water for the shower
void _showerColdShotStop() {
    if (EMSESP_Shower.doingColdShot) {
        myDebugLog("[Shower] finished shot of cold water. Hot water turned back on");
        ems_setWarmTapWaterActivated(true);
        EMSESP_Shower.doingColdShot = false;
        showerColdShotStopTimer.detach(); // disable the timer
    }
}

// turn off hot water to send a shot of cold
void _showerColdShotStart() {
    if (EMSESP_Settings.shower_alert) {
        myDebugLog("[Shower] starting shot of cold water");
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
bool LoadSaveCallback(MYESP_FSACTION_t action, JsonObject settings) {
    if (action == MYESP_FSACTION_LOAD) {
        // check for valid json
        if (settings.isNull()) {
            myDebug_P(PSTR("Error processing json settings"));
            return false;
        }

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

        EMSESP_Settings.master_thermostat = settings["master_thermostat"] | 0; // default to 0 (none)
        ems_setMasterThermostat(EMSESP_Settings.master_thermostat);

        EMSESP_Settings.bus_id = settings["bus_id"] | EMS_BUSID_DEFAULT; // default to 0x0B (Service Key)
        ems_setEMSbusid(EMSESP_Settings.bus_id);

        EMSESP_Settings.known_devices = strdup(settings["known_devices"] | "");

        return true;
    }

    if (action == MYESP_FSACTION_SAVE) {
        settings["led"]               = EMSESP_Settings.led;
        settings["led_gpio"]          = EMSESP_Settings.led_gpio;
        settings["dallas_gpio"]       = EMSESP_Settings.dallas_gpio;
        settings["dallas_parasite"]   = EMSESP_Settings.dallas_parasite;
        settings["listen_mode"]       = EMSESP_Settings.listen_mode;
        settings["shower_timer"]      = EMSESP_Settings.shower_timer;
        settings["shower_alert"]      = EMSESP_Settings.shower_alert;
        settings["publish_time"]      = EMSESP_Settings.publish_time;
        settings["tx_mode"]           = EMSESP_Settings.tx_mode;
        settings["bus_id"]            = EMSESP_Settings.bus_id;
        settings["master_thermostat"] = EMSESP_Settings.master_thermostat;
        settings["known_devices"]     = EMSESP_Settings.known_devices;

        return true;
    }

    return false;
}

// callback for custom settings when showing Stored Settings with the 'set' command
// wc is number of arguments after the 'set' command
// returns true if the setting was recognized and changed and should be saved back to SPIFFs
MYESP_FSACTION_t SetListCallback(MYESP_FSACTION_t action, uint8_t wc, const char * setting, const char * value) {
    MYESP_FSACTION_t ok = MYESP_FSACTION_ERR;

    if (action == MYESP_FSACTION_SET) {
        // led
        if ((strcmp(setting, "led") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.led = true;
                ok                  = MYESP_FSACTION_RESTART;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.led = false;
                ok                  = MYESP_FSACTION_RESTART;
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
                ok                          = MYESP_FSACTION_OK;
                myDebug_P(PSTR("* in listen mode. All Tx is disabled."));
                ems_setTxDisabled(true);
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.listen_mode = false;
                ok                          = MYESP_FSACTION_OK;
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
            ok = MYESP_FSACTION_OK;
        }

        // dallas_gpio
        if ((strcmp(setting, "dallas_gpio") == 0) && (wc == 2)) {
            EMSESP_Settings.dallas_gpio = atoi(value);
            ok                          = MYESP_FSACTION_RESTART;
        }

        // dallas_parasite
        if ((strcmp(setting, "dallas_parasite") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.dallas_parasite = true;
                ok                              = MYESP_FSACTION_RESTART;
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.dallas_parasite = false;
                ok                              = MYESP_FSACTION_RESTART;
            } else {
                myDebug_P(PSTR("Error. Usage: set dallas_parasite <on | off>"));
            }
        }

        // shower timer
        if ((strcmp(setting, "shower_timer") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                do_publishShowerData();
                EMSESP_Settings.shower_timer = true;
                ok                           = MYESP_FSACTION_OK;
            } else if (strcmp(value, "off") == 0) {
                do_publishShowerData();
                EMSESP_Settings.shower_timer = false;
                ok                           = MYESP_FSACTION_OK;
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_timer <on | off>"));
            }
        }

        // shower alert
        if ((strcmp(setting, "shower_alert") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                do_publishShowerData();
                EMSESP_Settings.shower_alert = true;
                ok                           = MYESP_FSACTION_OK;
            } else if (strcmp(value, "off") == 0) {
                do_publishShowerData();
                EMSESP_Settings.shower_alert = false;
                ok                           = MYESP_FSACTION_OK;
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_alert <on | off>"));
            }
        }

        // publish_time
        if (strcmp(setting, "publish_time") == 0) {
            if (wc == 1) {
                EMSESP_Settings.publish_time = 0;
            } else {
                EMSESP_Settings.publish_time = atoi(value);
            }
            ok = MYESP_FSACTION_RESTART;
        }

        // tx_mode
        if ((strcmp(setting, "tx_mode") == 0) && (wc == 2)) {
            uint8_t mode = atoi(value);
            if ((mode >= 1) && (mode <= 3)) { // see ems.h for definitions
                EMSESP_Settings.tx_mode = mode;
                ems_setTxMode(mode);
                ok = MYESP_FSACTION_RESTART;
            } else {
                myDebug_P(PSTR("Error. Usage: set tx_mode <1 | 2 | 3>"));
            }
        }

        // bus_id
        if ((strcmp(setting, "bus_id") == 0) && (wc == 2)) {
            uint8_t id = strtoul(value, 0, 16);
            if ((id == 0x0B) || (id == 0x0D) || (id == 0x0A) || (id == 0x0F) || (id == 0x12)) {
                EMSESP_Settings.bus_id = id;
                ems_setEMSbusid(id);
                ok = MYESP_FSACTION_RESTART;
            } else {
                myDebug_P(PSTR("Error. Usage: set bus_id <ID>, with ID=0B, 0D, 0A, 0F or 12"));
            }
        }

        // master_thermostat
        if (strcmp(setting, "master_thermostat") == 0) {
            if (wc == 1) {
                // show list
                char device_string[100];
                myDebug_P(PSTR("Available thermostat Product ids:"));
                for (std::list<_Detected_Device>::iterator it = Devices.begin(); it != Devices.end(); ++it) {
                    if (it->device_type == EMS_DEVICE_TYPE_THERMOSTAT) {
                        strlcpy(device_string, (it)->device_desc_p, sizeof(device_string));
                        myDebug_P(PSTR(" %d = %s"), (it)->product_id, device_string);
                    }
                }
                myDebug_P("");
                myDebug_P(PSTR("Usage: set master_thermostat <product id>"));
                ems_setMasterThermostat(0);            // default value
                EMSESP_Settings.master_thermostat = 0; // back to default
                ok                                = MYESP_FSACTION_OK;
            } else if (wc == 2) {
                uint8_t pid                       = atoi(value);
                EMSESP_Settings.master_thermostat = pid;
                ems_setMasterThermostat(pid);
                // force a scan again to detect and set the thermostat
                Devices.clear();
                ems_doReadCommand(EMS_TYPE_UBADevices, EMS_Boiler.device_id);
                ok = MYESP_FSACTION_OK;
            }
        }
    }

    if (action == MYESP_FSACTION_LIST) {
        myDebug_P(PSTR("  led=%s"), EMSESP_Settings.led ? "on" : "off");
        myDebug_P(PSTR("  led_gpio=%d"), EMSESP_Settings.led_gpio);
        myDebug_P(PSTR("  dallas_gpio=%d"), EMSESP_Settings.dallas_gpio);
        myDebug_P(PSTR("  dallas_parasite=%s"), EMSESP_Settings.dallas_parasite ? "on" : "off");
        myDebug_P(PSTR("  tx_mode=%d"), EMSESP_Settings.tx_mode);
        myDebug_P(PSTR("  bus_id=0x%02X"), EMSESP_Settings.bus_id);
        myDebug_P(PSTR("  listen_mode=%s"), EMSESP_Settings.listen_mode ? "on" : "off");
        myDebug_P(PSTR("  shower_timer=%s"), EMSESP_Settings.shower_timer ? "on" : "off");
        myDebug_P(PSTR("  shower_alert=%s"), EMSESP_Settings.shower_alert ? "on" : "off");

        if (EMSESP_Settings.publish_time == 0) {
            myDebug_P(PSTR("  publish_time=0 (automatic)"));
        } else if (EMSESP_Settings.publish_time == -1) {
            myDebug_P(PSTR("  publish_time=-1 (disabled)"));
        } else {
            myDebug_P(PSTR("  publish_time=%d"), EMSESP_Settings.publish_time);
        }

        if (EMSESP_Settings.master_thermostat) {
            myDebug_P(PSTR("  master_thermostat=%d"), EMSESP_Settings.master_thermostat);
        } else {
            myDebug_P(PSTR("  master_thermostat=0 (use first detected)"));
        }
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
        ems_setLogging(EMS_SYS_LOGGING_DEFAULT, true);
    } else if (event == TELNET_EVENT_DISCONNECT) {
        ems_setLogging(EMS_SYS_LOGGING_NONE, true);
    } else if ((event == TELNET_EVENT_SHOWCMD) || (event == TELNET_EVENT_SHOWSET)) {
        _showCommands(event);
    }
}

// get the list of know devices, as a string, and save them to the config file
void saveEMSDevices() {
    if (Devices.empty()) {
        return;
    }

    char s[100];
    char buffer[16] = {0};
    s[0]            = '\0';

    for (std::list<_Detected_Device>::iterator it = Devices.begin(); it != Devices.end(); ++it) {
        strlcat(s, _hextoa(it->device_id, buffer), sizeof(s));
        strlcat(s, " ", sizeof(s));
    }

    strlcpy(EMSESP_Settings.known_devices, s, sizeof(s));

    myDebug_P(PSTR("The device IDs %s%s%swill be automatically scanned when EMS-ESP boots up."), COLOR_BOLD_ON, EMSESP_Settings.known_devices, COLOR_BOLD_OFF);
    myESP.saveSettings();
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
        do_publishValues();
        ok = true;
    }

    if (strcmp(first_cmd, "refresh") == 0) {
        do_regularUpdates();
        ok = true;
    }

    if (strcmp(first_cmd, "devices") == 0) {
        if (wc == 1) {
            ems_printDevices();
            return;
        }

        // wc = 2 or more. check for "scan"
        char * second_cmd = _readWord();
        if (strcmp(second_cmd, "scan") == 0) {
            if (wc == 2) {
                // just scan use UBA 0x07 telegram
                myDebug_P(PSTR("Requesting EMS bus master for its device list and scanning for external sensors..."));
                scanDallas();
                Devices.clear(); // init the device map
                ems_doReadCommand(EMS_TYPE_UBADevices, EMS_Boiler.device_id);
                return;
            }

            // wc is 3 or more. check for additional "force" argument
            char * third_cmd = _readWord();
            if (strcmp(third_cmd, "deep") == 0) {
                myDebug_P(PSTR("Started deep scan of EMS bus for our known devices. This can take up to 10 seconds..."));
                Devices.clear(); // init the device map
                ems_scanDevices();
                return;
            }
        } else if (strcmp(second_cmd, "save") == 0) {
            saveEMSDevices();
            return;
        }

        ok = false; // unknown command
    }


    if (strcmp(first_cmd, "queue") == 0) {
        ems_printTxQueue();
        ok = true;
    }

    // logging
    if ((strcmp(first_cmd, "log") == 0) && (wc >= 2)) {
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
        } else if (strcmp(second_cmd, "m") == 0) {
            ems_setLogging(EMS_SYS_LOGGING_MIXINGMODULE);
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
        } else if ((strcmp(second_cmd, "w") == 0) && (wc == 3)) {
            ems_setLogging(EMS_SYS_LOGGING_WATCH, _readHexNumber()); // get type_id
            ok = true;
        } else if ((strcmp(second_cmd, "d") == 0) && (wc == 3)) {
            ems_setLogging(EMS_SYS_LOGGING_DEVICE, _readHexNumber()); // get device_id
            ok = true;
        }
    }

    // thermostat commands
    // thermostat temp <tempvalue> <modevalue> <heatingcircuit>
    if ((strcmp(first_cmd, "thermostat") == 0) && (wc >= 3)) {
        char * second_cmd = _readWord();

        if (strcmp(second_cmd, "temp") == 0) {
            float temp = _readFloatNumber(); // read in next param which is the temp

            if (wc == 3) {
                // no more params
                ems_setThermostatTemp(temp, EMS_THERMOSTAT_DEFAULTHC, EMS_THERMOSTAT_MODE_AUTO);
                ok = true;
            } else {
                // get modevalue and heatingcircuit
                char *  mode_s = _readWord(); // get mode string next
                uint8_t hc     = (wc == 4) ? EMS_THERMOSTAT_DEFAULTHC : _readIntNumber();
                ems_setThermostatTemp(temp, hc, mode_s);
                ok = true;
            }
        } else if (strcmp(second_cmd, "mode") == 0) {
            char *  mode_s = _readWord();
            uint8_t hc     = (wc == 3) ? EMS_THERMOSTAT_DEFAULTHC : _readIntNumber();
            ems_setThermostatMode(mode_s, hc);
            ok = true;
        } else if (strcmp(second_cmd, "read") == 0) {
            ems_doReadCommand(_readHexNumber(), EMS_Thermostat.device_id);
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
        } else if (strcmp(second_cmd, "wwactive") == 0) {
            char * third_cmd = _readWord();
            if (strcmp(third_cmd, "on") == 0) {
                ems_setWarmWaterActivated(true);
                ok = true;
            } else if (strcmp(third_cmd, "off") == 0) {
                ems_setWarmWaterActivated(false);
                ok = true;
            }
        } else if (strcmp(second_cmd, "wwonetime") == 0) {
            char * third_cmd = _readWord();
            if (strcmp(third_cmd, "on") == 0) {
                ems_setWarmWaterOnetime(true);
                ok = true;
            } else if (strcmp(third_cmd, "off") == 0) {
                ems_setWarmWaterOnetime(false);
                ok = true;
            }
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


// see's if a topic string is appended with an interger value
// used to identify a heating circuit
// returns HC number 1 - 4
// or the default (1) is no suffix can be found
uint8_t _hasHCspecified(const char * key, const char * input) {
    int orig_len = strlen(key); // original length of the topic we're comparing too

    // check if the strings match ignoring any suffix
    if (strncmp(input, key, orig_len) == 0) {
        // see if we have additional chars at the end, we want none or 1
        uint8_t diff = (strlen(input) - orig_len);
        if (diff > 1) {
            return 0; // invalid
        }

        if (diff == 0) {
            return EMS_THERMOSTAT_DEFAULTHC; // identical, use default which is 1
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
        // subscribe to the 4 heating circuits for receiving setpoint temperature and modes
        char topic_s[50];
        char buffer[4];
        for (uint8_t hc = 1; hc <= EMS_THERMOSTAT_MAXHC; hc++) {
            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_TEMP_HA, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s));
            myESP.mqttSubscribe(topic_s);

            strlcpy(topic_s, TOPIC_THERMOSTAT_CMD_MODE_HA, sizeof(topic_s));
            strlcat(topic_s, itoa(hc, buffer, 10), sizeof(topic_s));
            myESP.mqttSubscribe(topic_s);
        }
        // also subscribe without the HC appended to the end of the topic
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_TEMP_HA);
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD_MODE_HA);

        // generic incoming MQTT command for Thermostat
        // this is used for example for setting daytemp, nighttemp, holidaytemp
        myESP.mqttSubscribe(TOPIC_THERMOSTAT_CMD);

        // generic incoming MQTT command for Boiler
        // this is used for example for comfort, flowtemp
        myESP.mqttSubscribe(TOPIC_BOILER_CMD);

        // these three need to be unique topics
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWACTIVATED);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWONETIME);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWCIRCULATION);
        myESP.mqttSubscribe(TOPIC_BOILER_CMD_WWTEMP);

        // generic incoming MQTT command for EMS-ESP
        // this is used for example for shower_coldshot
        myESP.mqttSubscribe(TOPIC_GENERIC_CMD);

        // shower data
        // for receiving shower_Timer and shower_alert switches
        myESP.mqttSubscribe(TOPIC_SHOWER_DATA);

        // send Shower Alert and Timer switch settings
        do_publishShowerData();

        return;
    }

    // handle incoming MQTT publish events
    if (type != MQTT_MESSAGE_EVENT) {
        return;
    }

    // check first for generic commands
    if (strcmp(topic, TOPIC_GENERIC_CMD) == 0) {
        // convert JSON and get the command
        StaticJsonDocument<100> doc;
        DeserializationError    error = deserializeJson(doc, message); // Deserialize the JSON document
        if (error) {
            myDebug_P(PSTR("[MQTT] Invalid command from topic %s, payload %s, error %s"), topic, message, error.c_str());
            return;
        }
        const char * command = doc["cmd"];

        // Check whatever the command is and act accordingly

        // we only have one now, this is for the shower coldshot
        if (strcmp(command, TOPIC_SHOWER_COLDSHOT) == 0) {
            _showerColdShotStart();
            return;
        }

        return; // no match for generic commands
    }

    // check for shower commands
    if (strcmp(topic, TOPIC_SHOWER_DATA) == 0) {
        StaticJsonDocument<100> doc;
        DeserializationError    error = deserializeJson(doc, message); // Deserialize the JSON document
        if (error) {
            myDebug_P(PSTR("[MQTT] Invalid command from topic %s, payload %s, error %s"), topic, message, error.c_str());
            return;
        }

        // assumes payload is "1" or "0"
        const char * shower_alert = doc[TOPIC_SHOWER_ALERT];
        if (shower_alert) {
            EMSESP_Settings.shower_alert = ((shower_alert[0] - MYESP_MQTT_PAYLOAD_OFF) == 1);
            if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                myDebug_P(PSTR("Shower alert has been set to %s"), EMSESP_Settings.shower_alert ? "enabled" : "disabled");
            }
        }

        // assumes payload is "1" or "0"
        const char * shower_timer = doc[TOPIC_SHOWER_TIMER];
        if (shower_timer) {
            EMSESP_Settings.shower_timer = ((shower_timer[0] - MYESP_MQTT_PAYLOAD_OFF) == 1);
            if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                myDebug_P(PSTR("Shower timer has been set to %s"), EMSESP_Settings.shower_timer ? "enabled" : "disabled");
            }
        }

        return;
    }

    // check for boiler commands
    if (strcmp(topic, TOPIC_BOILER_CMD) == 0) {
        // convert JSON and get the command
        StaticJsonDocument<100> doc;
        DeserializationError    error = deserializeJson(doc, message); // Deserialize the JSON document
        if (error) {
            myDebug_P(PSTR("[MQTT] Invalid command from topic %s, payload %s, error %s"), topic, message, error.c_str());
            return;
        }
        const char * command = doc["cmd"];
        if (command == nullptr) {
            return;
        }

        // boiler ww comfort setting
        if (strcmp(command, TOPIC_BOILER_CMD_COMFORT) == 0) {
            const char * data = doc["data"];
            if (data == nullptr) {
                return;
            }
            if (strcmp((char *)data, "hot") == 0) {
                ems_setWarmWaterModeComfort(1);
            } else if (strcmp((char *)data, "comfort") == 0) {
                ems_setWarmWaterModeComfort(2);
            } else if (strcmp((char *)data, "intelligent") == 0) {
                ems_setWarmWaterModeComfort(3);
            }
            return;
        }

        // boiler flowtemp setting
        if (strcmp(command, TOPIC_BOILER_CMD_FLOWTEMP) == 0) {
            uint8_t t = doc["data"];
            if (t) {
                ems_setFlowTemp(t);
            }
            return;
        }

        return; // unknown boiler command
    }

    // check for unique boiler commands

    // wwActivated
    if (strcmp(topic, TOPIC_BOILER_CMD_WWACTIVATED) == 0) {
        if ((message[0] == MYESP_MQTT_PAYLOAD_ON || strcmp(message, "on") == 0) || (strcmp(message, "auto") == 0)) {
            ems_setWarmWaterActivated(true);
        } else if (message[0] == MYESP_MQTT_PAYLOAD_OFF || strcmp(message, "off") == 0) {
            ems_setWarmWaterActivated(false);
        }
        return;
    }

    // wwOneTime
    if (strcmp(topic, TOPIC_BOILER_CMD_WWONETIME) == 0) {
        if (message[0] == MYESP_MQTT_PAYLOAD_ON || strcmp(message, "on") == 0) {
            ems_setWarmWaterOnetime(true);
        } else if (message[0] == MYESP_MQTT_PAYLOAD_OFF || strcmp(message, "off") == 0) {
            ems_setWarmWaterOnetime(false);
        }
        return;
    }

    // wwCirculation
    if (strcmp(topic, TOPIC_BOILER_CMD_WWCIRCULATION) == 0) {
        if (message[0] == MYESP_MQTT_PAYLOAD_ON || strcmp(message, "on") == 0) {
            ems_setWarmWaterCirculation(true);
        } else if (message[0] == MYESP_MQTT_PAYLOAD_OFF || strcmp(message, "off") == 0) {
            ems_setWarmWaterCirculation(false);
        }
        return;
    }

    // boiler wwtemp changes
    if (strcmp(topic, TOPIC_BOILER_CMD_WWTEMP) == 0) {
        uint8_t t = atoi((char *)message);
        if (t) {
            ems_setWarmWaterTemp(t);
            publishEMSValues(true);
        }
        return;
    }

    uint8_t hc;
    // thermostat temp changes
    hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_TEMP_HA, topic);
    if (hc) {
        if (EMS_Thermostat.hc[hc - 1].active) {
            float f = strtof((char *)message, 0);
            if (f) {
                ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_AUTO);
                publishEMSValues(true); // publish back immediately
                return;
            }
        }
    }

    // thermostat mode changes
    hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_MODE_HA, topic);
    if (hc) {
        if (EMS_Thermostat.hc[hc - 1].active) {
            ems_setThermostatMode(message, hc);
            return;
        }
    }

    // check for generic thermostat commands
    if (strcmp(topic, TOPIC_THERMOSTAT_CMD) == 0) {
        // convert JSON and get the command
        StaticJsonDocument<100> doc;
        DeserializationError    error = deserializeJson(doc, message); // Deserialize the JSON document
        if (error) {
            myDebug_P(PSTR("[MQTT] Invalid command from topic %s, payload %s, error %s"), topic, message, error.c_str());
            return;
        }
        const char * command = doc["cmd"];
        if (command == nullptr) {
            return;
        }

        // thermostat temp changes
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_TEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_AUTO);
                    publishEMSValues(true); // publish back immediately
                    return;
                }
            }
        }

        // thermostat mode changes
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_MODE, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                const char * data_cmd = doc["data"];
                if (data_cmd == nullptr) {
                    return;
                }
                ems_setThermostatMode(data_cmd, hc);
                return;
            }
        }

        // set night temp value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_NIGHTTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_NIGHT); // night
                    return;
                }
            }
        }

        // set daytemp value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_DAYTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_DAY); // day
                }
                return;
            }
        }

        // set holiday value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_HOLIDAYTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_HOLIDAY); // holiday
                }
                return;
            }
        }

        // set eco value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_ECOTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_ECO); // holiday
                }
                return;
            }
        }

        // set heat value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_HEATTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_HEAT); // holiday
                }
                return;
            }
        }

        // set nofrost value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_NOFROSTTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, EMS_THERMOSTAT_MODE_NOFROST); // holiday
                }
                return;
            }
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
            emsbus["msg"] = "EMS Bus is not connected.";
        }
    }

    // send over EMS devices
    JsonArray list = emsbus.createNestedArray("devices");
    char      buffer[50];

    for (std::list<_Detected_Device>::iterator it = Devices.begin(); it != Devices.end(); ++it) {
        JsonObject item = list.createNestedObject();

        (void)ems_getDeviceTypeDescription((it)->device_id, buffer);
        item["type"] = buffer;

        if ((it)->known == true) {
            item["model"] = (it)->device_desc_p;
        } else {
            item["model"] = EMS_MODELTYPE_UNKNOWN_STRING;
        }

        item["version"]   = (it)->version;
        item["productid"] = (it)->product_id;

        char tmp_hex[10];
        // copy of my _hextoa() function from ems.cpp, to convert device_id into a 0xNN hex value string
        char * p         = tmp_hex;
        byte   nib1      = ((it)->device_id >> 4) & 0x0F;
        byte   nib2      = ((it)->device_id >> 0) & 0x0F;
        *p++             = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
        *p++             = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
        *p               = '\0'; // null terminate just in case
        item["deviceid"] = tmp_hex;
    }

    // send over Thermostat data
    JsonObject thermostat = root.createNestedObject("thermostat");

    if (ems_getThermostatEnabled()) {
        thermostat["ok"] = true;

        char buffer[200];
        thermostat["tm"] = ems_getDeviceDescription(EMS_DEVICE_TYPE_THERMOSTAT, buffer, true);

        uint8_t hc_num = 1; // default to HC1
        uint8_t model  = ems_getThermostatFlags();
        while (hc_num < EMS_THERMOSTAT_MAXHC && !EMS_Thermostat.hc[hc_num - 1].active)
            hc_num++; // first active hc
        // Render Current & Setpoint Room Temperature
        if (model == EMS_DEVICE_FLAG_EASY) {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (float)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 100;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (float)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 100;
        } else if ((model == EMS_DEVICE_FLAG_JUNKERS1) || (model == EMS_DEVICE_FLAG_JUNKERS2)) {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (float)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 10;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (float)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 10;
        } else {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (float)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 2;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (float)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 10;
        }

        // Render Thermostat Mode
        _EMS_THERMOSTAT_MODE thermoMode = _getThermostatMode(hc_num);
        if (thermoMode == EMS_THERMOSTAT_MODE_OFF) {
            thermostat["tmode"] = "off";
        } else if (thermoMode == EMS_THERMOSTAT_MODE_MANUAL) {
            thermostat["tmode"] = "manual";
        } else if (thermoMode == EMS_THERMOSTAT_MODE_AUTO) {
            thermostat["tmode"] = "auto";
        } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
            thermostat["tmode"] = "day";
        } else if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
            thermostat["tmode"] = "night";
        }
    } else {
        thermostat["ok"] = false;
    }

    JsonObject boiler = root.createNestedObject("boiler");
    if (ems_getBoilerEnabled()) {
        boiler["ok"] = true;

        char buffer[200];
        boiler["bm"] = ems_getDeviceDescription(EMS_DEVICE_TYPE_BOILER, buffer, true);

        boiler["b1"] = (EMS_Boiler.tapwaterActive ? "running" : "off");
        boiler["b2"] = (EMS_Boiler.heatingActive ? "active" : "off");

        if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET) {
            boiler["b3"] = EMS_Boiler.selFlowTemp;
        }

        if (EMS_Boiler.curFlowTemp != EMS_VALUE_INT_NOTSET) {
            boiler["b4"] = EMS_Boiler.curFlowTemp / 10;
        }

        if (EMS_Boiler.wWCurTmp < EMS_VALUE_USHORT_NOTSET) {
            boiler["b5"] = (float)EMS_Boiler.wWCurTmp / 10;
        }

    } else {
        boiler["ok"] = false;
    }

    // For SM10/SM100/SM200 Solar Module
    JsonObject sm = root.createNestedObject("sm");
    if (ems_getSolarModuleEnabled()) {
        sm["ok"] = true;

        char buffer[200];
        sm["sm"] = ems_getDeviceDescription(EMS_DEVICE_TYPE_SOLAR, buffer, true);

        if (EMS_SolarModule.collectorTemp > EMS_VALUE_SHORT_NOTSET)
            sm["sm1"] = (float)EMS_SolarModule.collectorTemp / 10; // Collector temperature oC

        if (EMS_SolarModule.bottomTemp > EMS_VALUE_SHORT_NOTSET)
            sm["sm2"] = (float)EMS_SolarModule.bottomTemp / 10; // Bottom temperature oC

        if (EMS_SolarModule.pumpModulation != EMS_VALUE_INT_NOTSET)
            sm["sm3"] = EMS_SolarModule.pumpModulation; // Pump modulation %

        if (EMS_SolarModule.pump != EMS_VALUE_BOOL_NOTSET) {
            char s[10];
            sm["sm4"] = _bool_to_char(s, EMS_SolarModule.pump); // Pump active on/off
        }

        if (EMS_SolarModule.EnergyLastHour < EMS_VALUE_USHORT_NOTSET)
            sm["sm5"] = (float)EMS_SolarModule.EnergyLastHour / 10; // Energy last hour Wh

        if (EMS_SolarModule.EnergyToday < EMS_VALUE_USHORT_NOTSET) // Energy today Wh
            sm["sm6"] = EMS_SolarModule.EnergyToday;

        if (EMS_SolarModule.EnergyTotal < EMS_VALUE_USHORT_NOTSET) // Energy total KWh
            sm["sm7"] = (float)EMS_SolarModule.EnergyTotal / 10;
    } else {
        sm["ok"] = false;
    }

    // For HeatPumps
    JsonObject hp = root.createNestedObject("hp");
    if (ems_getHeatPumpEnabled()) {
        hp["ok"] = true;
        char buffer[200];
        hp["hm"] = ems_getDeviceDescription(EMS_DEVICE_TYPE_HEATPUMP, buffer, true);

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
    EMSESP_Settings.shower_timer      = false;
    EMSESP_Settings.shower_alert      = false;
    EMSESP_Settings.led               = true; // LED is on by default
    EMSESP_Settings.listen_mode       = false;
    EMSESP_Settings.publish_time      = DEFAULT_PUBLISHTIME;
    EMSESP_Settings.dallas_sensors    = 0;
    EMSESP_Settings.led_gpio          = EMSESP_LED_GPIO;
    EMSESP_Settings.dallas_gpio       = EMSESP_DALLAS_GPIO;
    EMSESP_Settings.tx_mode           = EMS_TXMODE_DEFAULT; // default tx mode
    EMSESP_Settings.bus_id            = EMS_BUSID_DEFAULT;  // Service Key is default
    EMSESP_Settings.master_thermostat = 0;
    EMSESP_Settings.known_devices     = nullptr;

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
    if (!EMSESP_Settings.shower_timer) {
        return;
    }

    uint32_t time_now = millis();
    // if already in cold mode, ignore all this logic until we're out of the cold blast
    if (!EMSESP_Shower.doingColdShot) {
        // is the hot water running?
        if (EMS_Boiler.tapwaterActive == 1) {
            // if heater was previously off, start the timer
            if (EMSESP_Shower.timerStart == 0) {
                // hot water just started...
                EMSESP_Shower.timerStart    = time_now;
                EMSESP_Shower.timerPause    = 0; // remove any last pauses
                EMSESP_Shower.doingColdShot = false;
                EMSESP_Shower.duration      = 0;
                EMSESP_Shower.showerOn      = false;
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!EMSESP_Shower.showerOn && (time_now - EMSESP_Shower.timerStart) > SHOWER_MIN_DURATION) {
                    EMSESP_Shower.showerOn = true;
                    myDebugLog("[Shower] hot water still running, starting shower timer");
                }
                // check if the shower has been on too long
                else if ((((time_now - EMSESP_Shower.timerStart) > SHOWER_MAX_DURATION) && !EMSESP_Shower.doingColdShot) && EMSESP_Settings.shower_alert) {
                    myDebugLog("[Shower] exceeded max shower time");
                    _showerColdShotStart();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if ((EMSESP_Shower.timerStart) && (EMSESP_Shower.timerPause == 0)) {
                EMSESP_Shower.timerPause = time_now;
            }

            // if shower has been off for longer than the wait time
            if ((EMSESP_Shower.timerPause) && ((time_now - EMSESP_Shower.timerPause) > SHOWER_PAUSE_TIME)) {
                // it is over the wait period, so assume that the shower has finished and calculate the total time and publish
                // because its unsigned long, can't have negative so check if length is less than OFFSET_TIME
                if ((EMSESP_Shower.timerPause - EMSESP_Shower.timerStart) > SHOWER_OFFSET_TIME) {
                    EMSESP_Shower.duration = (EMSESP_Shower.timerPause - EMSESP_Shower.timerStart - SHOWER_OFFSET_TIME);
                    if (EMSESP_Shower.duration > SHOWER_MIN_DURATION) {
                        if (ems_getLogging() != EMS_SYS_LOGGING_NONE) {
                            myDebug_P(PSTR("[Shower] finished with duration %d"), EMSESP_Shower.duration);
                        }
                        do_publishShowerData(); // publish to MQTT
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

// this is called when we've first established an EMS connection
// go send out some spies to figure out what is on the bus and who's driving it
void startupEMSscan() {
    if (EMSESP_Settings.listen_mode) {
        return;
    }

    // First scan anything we may have saved as known devices from a "devices save" command
    if (strlen(EMSESP_Settings.known_devices) > 0) {
        char * p;
        char * temp      = strdup(EMSESP_Settings.known_devices); // because strlok is destructive, make a copy
        char   value[10] = {0};
        // get first value
        if ((p = strtok(temp, " "))) { // delimiter
            strlcpy(value, p, sizeof(value));
            uint8_t val = (uint8_t)strtol(value, 0, 16);
            ems_doReadCommand(EMS_TYPE_Version, val);
        }
        // and iterate until end
        while (p != 0) {
            if ((p = strtok(nullptr, " "))) {
                strlcpy(value, p, sizeof(value));
                uint8_t val = (uint8_t)strtol(value, 0, 16);
                ems_doReadCommand(EMS_TYPE_Version, val);
            }
        }
    }

    // ask the Boiler to show us it's attached devices in case we missed anything
    ems_discoverModels();
}

//
// SETUP
//
void setup() {
    // GPIO15/D8 has an onboard pull down resistor and we use this for Tx, so force it high so it doesn't bring the whole EMS bus down
    pinMode(D8, OUTPUT);
    digitalWrite(D8, HIGH);

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
    myESP.begin(APP_HOSTNAME, APP_NAME, APP_VERSION, APP_URL, APP_URL_API);

    // at this point we have all the settings from our internall SPIFFS config file
    // fire up the UART now
    if (myESP.getUseSerial()) {
        myDebug_P(PSTR("Warning! EMS bus communication disabled when Serial mode enabled. Use 'set serial off' to start communication."));
    } else {
        Serial.println("Note: Serial output will now be disabled. Please use Telnet.");
        Serial.flush();
        myESP.setUseSerial(false);
        emsuart_init(); // start EMS bus transmissions
        myDebug_P(PSTR("[UART] Rx/Tx connection established"));
        startupEMSscan();
    }

    // enable regular checks to fetch data and publish using Tx (unless listen_mode is enabled)
    if (!EMSESP_Settings.listen_mode) {
        regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates); // regular reads from the EMS
    }

    // set timers for MQTT publish
    if (EMSESP_Settings.publish_time > 0) {
        publishValuesTimer.attach(EMSESP_Settings.publish_time, do_publishValues); // post MQTT EMS values
    } else if (EMSESP_Settings.publish_time == 0) {
        // automatic mode. use this Ticker to send out sensor values only. the EMS ones are done in the loop.
        publishValuesTimer.attach(DEFAULT_SENSOR_PUBLISHTIME, publishSensorValues);
    }

    // set pin for LED
    if (EMSESP_Settings.led_gpio != EMS_VALUE_INT_NOTSET) {
        pinMode(EMSESP_Settings.led_gpio, OUTPUT);
        digitalWrite(EMSESP_Settings.led_gpio, (EMSESP_Settings.led_gpio == LED_BUILTIN) ? HIGH : LOW); // light off. For onboard high=off
        ledcheckTimer.attach_ms(LEDCHECK_TIME, do_ledcheck);                                            // blink heartbeat LED
    }

    // system check
    systemCheckTimer.attach(SYSTEMCHECK_TIME, do_systemCheck); // check if EMS is reachable

    // check for Dallas sensors
    ds18.setup(EMSESP_Settings.dallas_gpio, EMSESP_Settings.dallas_parasite);
    scanDallas();
}

//
// Main loop
//
void loop() {
    myESP.loop(); // handle telnet, mqtt, wifi etc

    // get Dallas Sensor readings every 2 seconds
    static uint32_t last_check = 0;
    uint32_t        time_now   = millis();
    if (time_now - last_check > 2000) {
        last_check = time_now;
        if (EMSESP_Settings.dallas_sensors) {
            ds18.loop();
        }
    }

    // if we just have an EMS bus connection go and fetch the data and MQTT publish it to get started
    if (_need_first_publish) {
        publishValues(true, true);
        _need_first_publish = false;
        return;
    }

    // check if we're on auto mode for publishing
    // then send EMS values, only if its been flagged to update.
    // Don't send sensor data as this is done by the Ticker
    if (EMSESP_Settings.publish_time == 0) {
        publishValues(false, false);
    }

    // do shower logic
    showerCheck();
}
