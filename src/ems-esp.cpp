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

// public libraries
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

// standard arduino libs
#include <Ticker.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

// default APP params
#define APP_NAME "EMS-ESP"
#define APP_HOSTNAME "ems-esp"
#define APP_URL "https://github.com/proddy/EMS-ESP"
#define APP_URL_API "https://api.github.com/repos/proddy/EMS-ESP"

// timers, all values are in seconds
#define DEFAULT_PUBLISHTIME 10 // 10 seconds
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
    bool     shower_timer;      // true if we want to report back on shower times
    bool     shower_alert;      // true if we want the alert of cold water
    bool     led;               // LED on/off
    bool     listen_mode;       // stop automatic Tx on/off
    uint16_t publish_time;      // frequency of MQTT publish in seconds
    uint8_t  led_gpio;          // pin for LED
    uint8_t  dallas_gpio;       // pin for attaching external dallas temperature sensors
    bool     dallas_parasite;   // on/off is using parasite
    uint8_t  tx_mode;           // TX mode 1,2 or 3
    uint8_t  master_thermostat; // Product ID of master thermostat to use, 0 for automatic
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
    {true, "publish_time <seconds>", "set frequency for publishing data to MQTT"},
    {true, "tx_mode <n>", "changes Tx logic. 1=EMS generic, 2=EMS+, 3=HT3"},
    {true, "master_thermostat [product id]", "set default thermostat to use. No argument lists options"},

    {false, "info", "show current values deciphered from the EMS messages"},
    {false, "log <n | b | t | s | r | j | v | w [ID] | d [ID]>", "logging: none, basic, thermo, solar, raw, jabber, verbose, watch a type or device"},

#ifdef TESTS
    {false, "test <n>", "insert a test telegram on to the EMS bus"},
#endif

    {false, "publish", "publish all values to MQTT"},
    {false, "refresh", "fetch values from the EMS devices"},
    {false, "devices", "list detected EMS devices"},
    {false, "queue", "show current Tx queue"},
    {false, "autodetect [scan]", "scan for EMS devices and external sensors. scan uses brute-force"},
    {false, "send XX ...", "send raw telegram data to EMS bus (XX are hex values)"},
    {false, "thermostat read <type ID>", "send read request to the thermostat for heating circuit hc 1-4"},
    {false, "thermostat temp [hc] <degrees>", "set current thermostat temperature"},
    {false, "thermostat mode [hc] <mode>", "set mode (0=off, 1=manual, 2=auto) for heating circuit hc 1-4"},
    {false, "boiler read <type ID>", "send read request to boiler"},
    {false, "boiler wwtemp <degrees>", "set boiler warm water temperature"},
    {false, "boiler wwactive <on | off>", "set boiler warm water on/off"},
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
    uint8_t model = ems_getThermostatModel();

    if (model == EMS_DEVICE_FLAG_RC20) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_OFF;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_MANUAL;
        } else if (mode == 2) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    } else if (model == EMS_DEVICE_FLAG_RC300) {
        if (mode == 0) {
            thermoMode = EMS_THERMOSTAT_MODE_MANUAL;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_AUTO;
        }
    } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
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
// returns {EMS_THERMOSTAT_MODE_NIGHT, EMS_THERMOSTAT_MODE_DAY}
// hc_num is 1 to 4
_EMS_THERMOSTAT_MODE _getThermostatDayMode(uint8_t hc_num) {
    _EMS_THERMOSTAT_MODE thermoMode = EMS_THERMOSTAT_MODE_UNKNOWN;
    uint8_t              model      = ems_getThermostatModel();

    uint8_t mode = EMS_Thermostat.hc[hc_num - 1].day_mode;

    if (model == EMS_DEVICE_FLAG_JUNKERS) {
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
            thermoMode = EMS_THERMOSTAT_MODE_NIGHT;
        } else if (mode == 1) {
            thermoMode = EMS_THERMOSTAT_MODE_DAY;
        }
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
              (ems_getMixingDeviceEnabled() ? "enabled" : "disabled"));

    myDebug_P(PSTR("  Shower Timer: %s, Shower Alert: %s"),
              ((EMSESP_Settings.shower_timer) ? "enabled" : "disabled"),
              ((EMSESP_Settings.shower_alert) ? "enabled" : "disabled"));

    myDebug_P(PSTR("\n%sEMS Bus status:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);

    if (ems_getBusConnected()) {
        myDebug_P(PSTR("  Bus is connected, protocol: %s"), (ems_isHT3() ? "HT3" : "Buderus"));
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
        if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Hot) {
            myDebug_P(PSTR("  Warm Water comfort setting: Hot"));
        } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Eco) {
            myDebug_P(PSTR("  Warm Water comfort setting: Eco"));
        } else if (EMS_Boiler.wWComfort == EMS_VALUE_UBAParameterWW_wwComfort_Intelligent) {
            myDebug_P(PSTR("  Warm Water comfort setting: Intelligent"));
        }

        _renderIntValue("Warm Water selected temperature", "C", EMS_Boiler.wWSelTemp);
        _renderIntValue("Warm Water desinfection temperature", "C", EMS_Boiler.wWDesinfectTemp);
        _renderBoolValue("Warm Water Circulation active", EMS_Boiler.wWCirc);

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
        if (EMS_Boiler.extTemp > EMS_VALUE_SHORT_NOTSET) {
            _renderShortValue("Outside temperature", "C", EMS_Boiler.extTemp);
        }
        _renderUShortValue("Boiler temperature", "C", EMS_Boiler.boilTemp);
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
        uint8_t model = ems_getThermostatModel();
        if ((model != EMS_DEVICE_FLAG_EASY)) {
            myDebug_P(PSTR("  Thermostat time is %s"), EMS_Thermostat.datetime);
        }

        uint8_t _m_setpoint, _m_curr;
        switch (model) {
        case EMS_DEVICE_FLAG_EASY:
            _m_setpoint = 10; // *100
            _m_curr     = 10; // *100
            break;
        case EMS_DEVICE_FLAG_JUNKERS:
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

                // Render Thermostat Day Mode
                thermoMode = _getThermostatDayMode(hc_num);
                if (thermoMode == EMS_THERMOSTAT_MODE_NIGHT) {
                    myDebug_P(PSTR("   Day Mode is set to night"));
                } else if (thermoMode == EMS_THERMOSTAT_MODE_DAY) {
                    myDebug_P(PSTR("   Day Mode is set to day"));
                }
            }
        }
    }

    // Mixing modules sensors
    if (ems_getMixingDeviceEnabled()) {
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
        char buffer[128] = {0};
        char valuestr[8] = {0}; // for formatting temp
        myDebug_P(PSTR("%sExternal temperature sensors:%s"), COLOR_BOLD_ON, COLOR_BOLD_OFF);
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            float sensorValue = ds18.getValue(i);
            if (sensorValue != DS18_DISCONNECTED) {
                myDebug_P(PSTR("  Sensor #%d %s: %s C"), i + 1, ds18.getDeviceString(buffer, i), _float_to_char(valuestr, sensorValue));
            }
        }
    }

    myDebug_P(PSTR("")); // newline
}

// scan for external Dallas sensors and display
void scanDallas() {
    EMSESP_Settings.dallas_sensors = ds18.scan();
    if (EMSESP_Settings.dallas_sensors) {
        char buffer[128] = {0};
        for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
            myDebug_P(PSTR("External temperature sensor %s found"), ds18.getDeviceString(buffer, i));
        }
    }
}

// send all dallas sensor values as a JSON package to MQTT
void publishSensorValues() {
    // don't send if MQTT is connected
    if (!myESP.isMQTTConnected()) {
        return;
    }

    if (!EMSESP_Settings.dallas_sensors) {
        return; // no sensors attached
    }

    StaticJsonDocument<200> doc;
    JsonObject              sensors = doc.to<JsonObject>();

    bool hasdata  = false;
    char label[8] = {0};

    // see if the sensor values have changed, if so send it on
    for (uint8_t i = 0; i < EMSESP_Settings.dallas_sensors; i++) {
        float sensorValue = ds18.getValue(i);
        if (sensorValue != DS18_DISCONNECTED) {
            sprintf(label, PAYLOAD_EXTERNAL_SENSORS, (i + 1));
            sensors[label] = sensorValue;
            hasdata        = true;
        }
    }

    if (!hasdata) {
        return; // nothing to send
    }

    char data[200] = {0};
    serializeJson(doc, data, sizeof(data));
    myDebugLog("Publishing external sensor data via MQTT");
    myESP.mqttPublish(TOPIC_EXTERNAL_SENSORS, data);
}

// send values via MQTT
// a json object is created for each device type
void publishEMSValues(bool force) {
    // don't send if MQTT is not connected or EMS bus is not connected
    if (!myESP.isMQTTConnected() || (!ems_getBusConnected())) {
        return;
    }

    char                                      s[20] = {0}; // for formatting strings
    StaticJsonDocument<MQTT_MAX_PAYLOAD_SIZE> doc;
    char                                      data[MQTT_MAX_PAYLOAD_SIZE] = {0};

    // do we have boiler changes?
    if (ems_getBoilerEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_BOILER) || force)) {
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
        if (EMS_Boiler.wWDesinfectTemp != EMS_VALUE_INT_NOTSET)
            rootBoiler["wWDesinfectionTemp"] = EMS_Boiler.wWDesinfectTemp;
        if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET)
            rootBoiler["selFlowTemp"] = EMS_Boiler.selFlowTemp;
        if (EMS_Boiler.selBurnPow != EMS_VALUE_INT_NOTSET)
            rootBoiler["selBurnPow"] = EMS_Boiler.selBurnPow;
        if (EMS_Boiler.curBurnPow != EMS_VALUE_INT_NOTSET)
            rootBoiler["curBurnPow"] = EMS_Boiler.curBurnPow;
        if (EMS_Boiler.pumpMod != EMS_VALUE_INT_NOTSET)
            rootBoiler["pumpMod"] = EMS_Boiler.pumpMod;
        if (EMS_Boiler.wWCircPump != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["wWCircPump"] = EMS_Boiler.wWCircPump;

        if (EMS_Boiler.extTemp > EMS_VALUE_SHORT_NOTSET)
            rootBoiler["outdoorTemp"] = (float)EMS_Boiler.extTemp / 10;
        if (EMS_Boiler.wWCurTmp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["wWCurTmp"] = (float)EMS_Boiler.wWCurTmp / 10;
        if (EMS_Boiler.wWCurFlow != EMS_VALUE_INT_NOTSET)
            rootBoiler["wWCurFlow"] = (float)EMS_Boiler.wWCurFlow / 10;
        if (EMS_Boiler.curFlowTemp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["curFlowTemp"] = (float)EMS_Boiler.curFlowTemp / 10;
        if (EMS_Boiler.retTemp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["retTemp"] = (float)EMS_Boiler.retTemp / 10;
        if (EMS_Boiler.switchTemp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["switchTemp"] = (float)EMS_Boiler.switchTemp / 10;
        if (EMS_Boiler.sysPress != EMS_VALUE_INT_NOTSET)
            rootBoiler["sysPress"] = (float)EMS_Boiler.sysPress / 10;
        if (EMS_Boiler.boilTemp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["boilTemp"] = (float)EMS_Boiler.boilTemp / 10;
        if (EMS_Boiler.exhaustTemp < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["exhaustTemp"] = (float)EMS_Boiler.exhaustTemp / 10;
        if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["wWActivated"] = _bool_to_char(s, EMS_Boiler.wWActivated);

        if (EMS_Boiler.wWActivated != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["wWOnetime"] = _bool_to_char(s, EMS_Boiler.wWOneTime);

        if (EMS_Boiler.wWCirc != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["wWCirc"] = _bool_to_char(s, EMS_Boiler.wWCirc);

        if (EMS_Boiler.burnGas != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["burnGas"] = _bool_to_char(s, EMS_Boiler.burnGas);

        if (EMS_Boiler.flameCurr < EMS_VALUE_USHORT_NOTSET)
            rootBoiler["flameCurr"] = (float)(int16_t)EMS_Boiler.flameCurr / 10;

        if (EMS_Boiler.heatPmp != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["heatPmp"] = _bool_to_char(s, EMS_Boiler.heatPmp);

        if (EMS_Boiler.fanWork != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["fanWork"] = _bool_to_char(s, EMS_Boiler.fanWork);

        if (EMS_Boiler.ignWork != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["ignWork"] = _bool_to_char(s, EMS_Boiler.ignWork);

        if (EMS_Boiler.heating_temp != EMS_VALUE_INT_NOTSET)
            rootBoiler["heating_temp"] = EMS_Boiler.heating_temp;
        if (EMS_Boiler.pump_mod_max != EMS_VALUE_INT_NOTSET)
            rootBoiler["pump_mod_max"] = EMS_Boiler.pump_mod_max;
        if (EMS_Boiler.pump_mod_min != EMS_VALUE_INT_NOTSET)
            rootBoiler["pump_mod_min"] = EMS_Boiler.pump_mod_min;

        if (EMS_Boiler.wWHeat != EMS_VALUE_BOOL_NOTSET)
            rootBoiler["wWHeat"] = _bool_to_char(s, EMS_Boiler.wWHeat);

        if (abs(EMS_Boiler.wWStarts) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["wWStarts"] = (float)EMS_Boiler.wWStarts;
        if (abs(EMS_Boiler.wWWorkM) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["wWWorkM"] = (float)EMS_Boiler.wWWorkM;
        if (abs(EMS_Boiler.UBAuptime) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["UBAuptime"] = (float)EMS_Boiler.UBAuptime;

        if (abs(EMS_Boiler.burnStarts) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["burnStarts"] = (float)EMS_Boiler.burnStarts;
        if (abs(EMS_Boiler.burnWorkMin) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["burnWorkMin"] = (float)EMS_Boiler.burnWorkMin;
        if (abs(EMS_Boiler.heatWorkMin) != EMS_VALUE_LONG_NOTSET)
            rootBoiler["heatWorkMin"] = (float)EMS_Boiler.heatWorkMin;

        if (EMS_Boiler.serviceCode != EMS_VALUE_USHORT_NOTSET) {
            rootBoiler["ServiceCode"]       = EMS_Boiler.serviceCodeChar;
            rootBoiler["ServiceCodeNumber"] = EMS_Boiler.serviceCode;
        }

        serializeJson(doc, data, sizeof(data));
        myDebugLog("Publishing boiler data via MQTT");
        myESP.mqttPublish(TOPIC_BOILER_DATA, data);

        // see if the heating or hot tap water has changed, if so send
        // last_boilerActive stores heating in bit 1 and tap water in bit 2
        static uint8_t last_boilerActive = 0xFF; // for remembering last setting of the tap water or heating on/off
        if ((last_boilerActive != ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive)) || force) {
            myDebugLog("Publishing hot water and heating states via MQTT");
            myESP.mqttPublish(TOPIC_BOILER_TAPWATER_ACTIVE, EMS_Boiler.tapwaterActive == 1 ? "1" : "0");
            myESP.mqttPublish(TOPIC_BOILER_HEATING_ACTIVE, EMS_Boiler.heatingActive == 1 ? "1" : "0");

            last_boilerActive = ((EMS_Boiler.tapwaterActive << 1) + EMS_Boiler.heatingActive); // remember last state
        }

        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_BOILER); // unset flag
    }

    // handle the thermostat values
    if (ems_getThermostatEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_THERMOSTAT) || force)) {
        doc.clear();
        JsonObject rootThermostat = doc.to<JsonObject>();

        for (uint8_t hc_v = 1; hc_v <= EMS_THERMOSTAT_MAXHC; hc_v++) {
            _EMS_Thermostat_HC * thermostat = &EMS_Thermostat.hc[hc_v - 1];

            // only send if we have an active Heating Circuit with real data
            if (thermostat->active) {
                // build new json object
                char hc[10]; // hc{1-4}
                strlcpy(hc, THERMOSTAT_HC, sizeof(hc));
                strlcat(hc, _int_to_char(s, thermostat->hc), sizeof(hc));
                JsonObject dataThermostat = rootThermostat.createNestedObject(hc);
                uint8_t    model          = ems_getThermostatModel();

                // different logic depending on thermostat types
                if (model == EMS_DEVICE_FLAG_EASY) {
                    if (thermostat->setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                        dataThermostat[THERMOSTAT_SELTEMP] = (float)thermostat->setpoint_roomTemp / 100;
                    if (thermostat->curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                        dataThermostat[THERMOSTAT_CURRTEMP] = (float)thermostat->curr_roomTemp / 100;
                } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
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
            }
        }

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));
        myDebugLog("Publishing thermostat data via MQTT");
        myESP.mqttPublish(TOPIC_THERMOSTAT_DATA, data);
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_THERMOSTAT); // unset flag
    }

    // handle the mixing values
    if (ems_getMixingDeviceEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_MIXING) || force)) {
        doc.clear();
        JsonObject rootMixing = doc.to<JsonObject>();

        for (uint8_t hc_v = 1; hc_v <= EMS_MIXING_MAXHC; hc_v++) {
            _EMS_MixingModule_HC * mixing = &EMS_MixingModule.hc[hc_v - 1];

            // only send if we have an active Heating Circuit with real data
            if (mixing->active) {
                // build new json object
                char hc[10]; // hc{1-4}
                strlcpy(hc, MIXING_HC, sizeof(hc));
                strlcat(hc, _int_to_char(s, mixing->hc), sizeof(hc));
                JsonObject dataMixing = rootMixing.createNestedObject(hc);
                if (mixing->flowTemp < EMS_VALUE_USHORT_NOTSET)
                    dataMixing["flowTemp"] = (float)mixing->flowTemp / 10;
                if (mixing->flowSetTemp != EMS_VALUE_INT_NOTSET)
                    dataMixing["setflowTemp"] = mixing->flowSetTemp;
                if (mixing->pumpMod != EMS_VALUE_INT_NOTSET)
                    dataMixing["pumpMod"] = mixing->pumpMod;
                if (mixing->valveStatus != EMS_VALUE_INT_NOTSET)
                    dataMixing["valveStatus"] = mixing->valveStatus;
            }
        }

        for (uint8_t wwc_v = 1; wwc_v <= EMS_MIXING_MAXWWC; wwc_v++) {
            _EMS_MixingModule_WWC * mixing = &EMS_MixingModule.wwc[wwc_v - 1];
            // only send if we have an active Warm water Circuit with real data
            if (mixing->active) {
                // build new json object
                char wwc[10]; // wwc{1-2}
                strlcpy(wwc, MIXING_WWC, sizeof(wwc));
                strlcat(wwc, _int_to_char(s, mixing->wwc), sizeof(wwc));
                JsonObject dataMixing = rootMixing.createNestedObject(wwc);
                if (mixing->flowTemp < EMS_VALUE_USHORT_NOTSET)
                    dataMixing["wwTemp"] = (float)mixing->flowTemp / 10;
                if (mixing->pumpMod != EMS_VALUE_INT_NOTSET)
                    dataMixing["pumpStatus"] = mixing->pumpMod;
                if (mixing->tempStatus != EMS_VALUE_INT_NOTSET)
                    dataMixing["tempStatus"] = mixing->tempStatus;
            }
        }

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));
        myDebugLog("Publishing mixing device data via MQTT");
        myESP.mqttPublish(TOPIC_MIXING_DATA, data);
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_MIXING); // unset flag
    }

    // For SM10 and SM100/SM200 Solar Modules
    if (ems_getSolarModuleEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_SOLAR) || force)) {
        // build new json object
        doc.clear();
        JsonObject rootSM = doc.to<JsonObject>();

        if (EMS_SolarModule.collectorTemp > EMS_VALUE_SHORT_NOTSET)
            rootSM[SM_COLLECTORTEMP] = (float)EMS_SolarModule.collectorTemp / 10;

        if (EMS_SolarModule.bottomTemp > EMS_VALUE_SHORT_NOTSET)
            rootSM[SM_BOTTOMTEMP] = (float)EMS_SolarModule.bottomTemp / 10;

        if (EMS_SolarModule.pumpModulation != EMS_VALUE_INT_NOTSET)
            rootSM[SM_PUMPMODULATION] = EMS_SolarModule.pumpModulation;

        if (EMS_SolarModule.pump != EMS_VALUE_BOOL_NOTSET) {
            rootSM[SM_PUMP] = _bool_to_char(s, EMS_SolarModule.pump);
        }

        if (EMS_SolarModule.pumpWorkMin != EMS_VALUE_LONG_NOTSET) {
            rootSM[SM_PUMPWORKMIN] = (float)EMS_SolarModule.pumpWorkMin;
        }

        if (EMS_SolarModule.EnergyLastHour < EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYLASTHOUR] = (float)EMS_SolarModule.EnergyLastHour / 10;

        if (EMS_SolarModule.EnergyToday < EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYTODAY] = EMS_SolarModule.EnergyToday;

        if (EMS_SolarModule.EnergyTotal < EMS_VALUE_USHORT_NOTSET)
            rootSM[SM_ENERGYTOTAL] = (float)EMS_SolarModule.EnergyTotal / 10;

        data[0] = '\0'; // reset data for next package
        serializeJson(doc, data, sizeof(data));
        myDebugLog("Publishing SM data via MQTT");
        myESP.mqttPublish(TOPIC_SM_DATA, data);
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_SOLAR); // unset flag
    }

    // handle HeatPump
    if (ems_getHeatPumpEnabled() && (ems_Device_has_flags(EMS_DEVICE_UPDATE_FLAG_HEATPUMP) || force)) {
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
        myESP.mqttPublish(TOPIC_HP_DATA, data);
        ems_Device_remove_flags(EMS_DEVICE_UPDATE_FLAG_HEATPUMP); // unset flag
    }
}

// Publish shower data
bool do_publishShowerData() {
    StaticJsonDocument<200> doc;
    JsonObject              rootShower = doc.to<JsonObject>();
    rootShower[TOPIC_SHOWER_TIMER]     = EMSESP_Settings.shower_timer ? "1" : "0";
    rootShower[TOPIC_SHOWER_ALERT]     = EMSESP_Settings.shower_alert ? "1" : "0";

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

    char data[300] = {0};
    serializeJson(doc, data, sizeof(data));

    myDebugLog("Publishing shower data via MQTT");

    // Publish MQTT forcing retain to be off
    return (myESP.mqttPublish(TOPIC_SHOWER_DATA, data, false));
}

// call PublishValues with forcing forcing
void do_publishValues() {
    myDebugLog("Starting scheduled MQTT publish...");
    publishEMSValues(false);
    publishSensorValues();
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

        // serializeJsonPretty(settings, Serial); // for debugging

        EMSESP_Settings.led             = settings["led"];
        EMSESP_Settings.led_gpio        = settings["led_gpio"] | EMSESP_LED_GPIO;
        EMSESP_Settings.dallas_gpio     = settings["dallas_gpio"] | EMSESP_DALLAS_GPIO;
        EMSESP_Settings.dallas_parasite = settings["dallas_parasite"] | EMSESP_DALLAS_PARASITE;
        EMSESP_Settings.shower_timer    = settings["shower_timer"];
        EMSESP_Settings.shower_alert    = settings["shower_alert"];
        EMSESP_Settings.publish_time    = settings["publish_time"] | DEFAULT_PUBLISHTIME;

        // can't be 0 which could be the case coming from earlier builds < 1.9.5b12
        if (EMSESP_Settings.publish_time == 0) {
            EMSESP_Settings.publish_time = DEFAULT_PUBLISHTIME;
        }

        EMSESP_Settings.listen_mode = settings["listen_mode"];
        ems_setTxDisabled(EMSESP_Settings.listen_mode);

        EMSESP_Settings.tx_mode = settings["tx_mode"] | EMS_TXMODE_DEFAULT; // default to 1 (generic)
        ems_setTxMode(EMSESP_Settings.tx_mode);

        EMSESP_Settings.master_thermostat = settings["master_thermostat"] | 0; // default to 0 (none)
        ems_setMasterThermostat(EMSESP_Settings.master_thermostat);

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
        settings["master_thermostat"] = EMSESP_Settings.master_thermostat;

        return true;
    }

    return false;
}

// callback for custom settings when showing Stored Settings with the 'set' command
// wc is number of arguments after the 'set' command
// returns true if the setting was recognized and changed and should be saved back to SPIFFs
bool SetListCallback(MYESP_FSACTION_t action, uint8_t wc, const char * setting, const char * value) {
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
                ok                           = do_publishShowerData();
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.shower_timer = false;
                ok                           = do_publishShowerData();
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_timer <on | off>"));
            }
        }

        // shower alert
        if ((strcmp(setting, "shower_alert") == 0) && (wc == 2)) {
            if (strcmp(value, "on") == 0) {
                EMSESP_Settings.shower_alert = true;
                ok                           = do_publishShowerData();
            } else if (strcmp(value, "off") == 0) {
                EMSESP_Settings.shower_alert = false;
                ok                           = do_publishShowerData();
            } else {
                myDebug_P(PSTR("Error. Usage: set shower_alert <on | off>"));
            }
        }

        // publish_time
        if ((strcmp(setting, "publish_time") == 0) && (wc == 2)) {
            int16_t val = atoi(value);
            if (val > 0) {
                EMSESP_Settings.publish_time = atoi(value);
                ok                           = true;
            } else {
                myDebug_P(PSTR("Error. time must be at least 1 second"));
            }
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
                myDebug_P(PSTR("Usage: set master_thermostat <product id>"));
                ok = true;
            } else if (wc == 2) {
                uint8_t pid                       = atoi(value);
                EMSESP_Settings.master_thermostat = pid;
                ems_setMasterThermostat(pid);
                // force a scan
                ems_clearDeviceList();
                ems_doReadCommand(EMS_TYPE_UBADevices, EMS_Boiler.device_id);
                ok = true;
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

        if (EMSESP_Settings.master_thermostat) {
            myDebug_P(PSTR("  master_thermostat=%d"), EMSESP_Settings.master_thermostat);
        } else {
            myDebug_P(PSTR("  master_thermostat=0 (using first one detected)"), EMSESP_Settings.master_thermostat);
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
        ems_printDevices();
        ok = true;
    }

    if (strcmp(first_cmd, "queue") == 0) {
        ems_printTxQueue();
        ok = true;
    }

    if (strcmp(first_cmd, "autodetect") == 0) {
        if (wc == 2) {
            char * second_cmd = _readWord();
            if (strcmp(second_cmd, "scan") == 0) {
                ems_clearDeviceList();
                ems_scanDevices();
                ok = true;
            }
        } else {
            myDebug("Scanning for new EMS devices and attached external sensors...");
            scanDallas();
            ems_clearDeviceList();
            ems_doReadCommand(EMS_TYPE_UBADevices, EMS_Boiler.device_id);
            ok = true;
        }
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
        if (message[0] == '1' || strcmp(message, "on") == 0) {
            ems_setWarmWaterOnetime(true);
        } else if (message[0] == '0' || strcmp(message, "off") == 0) {
            ems_setWarmWaterOnetime(false);
        }
        return;
    }

    // wwCirculation
    if (strcmp(topic, TOPIC_BOILER_CMD_WWCIRCULATION) == 0) {
        if (message[0] == '1' || strcmp(message, "on") == 0) {
            ems_setWarmWaterCirculation(true);
        } else if (message[0] == '0' || strcmp(message, "off") == 0) {
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
                ems_setThermostatTemp(f, hc);
                publishEMSValues(true); // publish back immediately
                return;
            }
        }
    }

    // thermostat mode changes
    hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_MODE_HA, topic);
    if (hc) {
        if (EMS_Thermostat.hc[hc - 1].active) {
            if (strncmp(message, "auto", 4) == 0) {
                ems_setThermostatMode(2, hc);
            } else if ((strncmp(message, "day", 4) == 0) || (strncmp(message, "manual", 6) == 0) || (strncmp(message, "heat", 4) == 0)) {
                ems_setThermostatMode(1, hc);
            } else if ((strncmp(message, "night", 5) == 0) || (strncmp(message, "off", 3) == 0)) {
                ems_setThermostatMode(0, hc);
            }
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
                    ems_setThermostatTemp(f, hc);
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
                if (strncmp(data_cmd, "auto", 4) == 0) {
                    ems_setThermostatMode(2, hc);
                } else if ((strncmp(data_cmd, "day", 4) == 0) || (strncmp(data_cmd, "manual", 6) == 0) || (strncmp(data_cmd, "heat", 4) == 0)) {
                    ems_setThermostatMode(1, hc);
                } else if ((strncmp(data_cmd, "night", 5) == 0) || (strncmp(data_cmd, "off", 3) == 0)) {
                    ems_setThermostatMode(0, hc);
                }
                return;
            }
        }

        // set night temp value
        hc = _hasHCspecified(TOPIC_THERMOSTAT_CMD_NIGHTTEMP, command);
        if (hc) {
            if (EMS_Thermostat.hc[hc - 1].active) {
                float f = doc["data"];
                if (f) {
                    ems_setThermostatTemp(f, hc, 1); // night
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
                    ems_setThermostatTemp(f, hc, 2); // day
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
                    ems_setThermostatTemp(f, hc, 3); // holiday
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
        uint8_t model  = ems_getThermostatModel();
        while (hc_num < EMS_THERMOSTAT_MAXHC && !EMS_Thermostat.hc[hc_num - 1].active)
            hc_num++; // first active hc
        // Render Current & Setpoint Room Temperature
        if (model == EMS_DEVICE_FLAG_EASY) {
            if (EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["ts"] = (float)EMS_Thermostat.hc[hc_num - 1].setpoint_roomTemp / 100;
            if (EMS_Thermostat.hc[hc_num - 1].curr_roomTemp > EMS_VALUE_SHORT_NOTSET)
                thermostat["tc"] = (float)EMS_Thermostat.hc[hc_num - 1].curr_roomTemp / 100;
        } else if (model == EMS_DEVICE_FLAG_JUNKERS) {
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

        if (EMS_Boiler.selFlowTemp != EMS_VALUE_INT_NOTSET)
            boiler["b3"] = EMS_Boiler.selFlowTemp;

        if (EMS_Boiler.curFlowTemp != EMS_VALUE_INT_NOTSET)
            boiler["b4"] = EMS_Boiler.curFlowTemp / 10;

        if (EMS_Boiler.boilTemp < EMS_VALUE_USHORT_NOTSET)
            boiler["b5"] = (float)EMS_Boiler.boilTemp / 10;

        if (EMS_Boiler.retTemp < EMS_VALUE_USHORT_NOTSET)
            boiler["b6"] = (float)EMS_Boiler.retTemp / 10;

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
    EMSESP_Settings.master_thermostat = 0;

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
        if (!EMSESP_Settings.listen_mode) {
            // go and find the boiler and thermostat types, if not in listen mode
            ems_discoverModels();
        }
    }

    // enable regular checks to fetch data and publish using Tx (unless listen_mode is enabled)
    if (!EMSESP_Settings.listen_mode) {
        regularUpdatesTimer.attach(REGULARUPDATES_TIME, do_regularUpdates); // regular reads from the EMS
    }

    // set timers for MQTT publish
    if (EMSESP_Settings.publish_time) {
        publishValuesTimer.attach(EMSESP_Settings.publish_time, do_publishValues); // post MQTT EMS values
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

    // Dallas sensors which are polled every 2 seconds (see DS18_READ_INTERVAL)
    if (EMSESP_Settings.dallas_sensors) {
        ds18.loop();
    }

    // if we have an EMS connect go and fetch some data and MQTT publish it
    if (_need_first_publish) {
        publishEMSValues(false);
        publishSensorValues();
        _need_first_publish = false; // reset flag
    }

    // do shower logic, if enabled
    if (EMSESP_Settings.shower_timer) {
        showerCheck();
    }
}
