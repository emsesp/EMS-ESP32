/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMSESP_THERMOSTAT_H
#define EMSESP_THERMOSTAT_H

#include "emsesp.h"

namespace emsesp {

class Thermostat : public EMSdevice {
  public:
    Thermostat(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const std::string & name, uint8_t flags, uint8_t brand);
    class HeatingCircuit {
      public:
        HeatingCircuit(const uint8_t hc_num, const uint8_t model)
            : hc_num_(hc_num)
            , model_(model) {
        }
        ~HeatingCircuit() = default;

        int16_t selTemp;
        int16_t roomTemp;
        int16_t remotetemp; // for readback
        uint8_t tempautotemp;
        int8_t  remoteseltemp;
        uint8_t mode;
        uint8_t modetype;
        uint8_t summermode;
        uint8_t holidaymode;
        uint8_t daytemp;
        uint8_t daylowtemp;
        uint8_t daymidtemp;
        uint8_t nighttemp;
        uint8_t holidaytemp;
        uint8_t heatingtype; // type of heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
        uint8_t targetflowtemp;
        uint8_t summertemp;
        int8_t  nofrosttemp; // signed -20°C to +10°C
        uint8_t designtemp;  // heating curve design temp at MinExtTemp
        int8_t  offsettemp;  // heating curve offest temp at roomtemp signed!
        uint8_t manualtemp;
        uint8_t summersetmode;
        uint8_t hpoperatingmode;
        uint8_t hpoperatingstate;
        uint8_t roominfluence;
        uint8_t roominfl_factor;
        int16_t curroominfl;
        uint8_t flowtempoffset;
        uint8_t minflowtemp;
        uint8_t maxflowtemp;
        uint8_t reducemode;
        uint8_t nofrostmode;
        uint8_t program;
        uint8_t controlmode;
        uint8_t control;
        uint8_t pause;
        uint8_t party;
        int8_t  noreducetemp; // signed -20°C to +10°C
        int8_t  reducetemp;
        int8_t  vacreducetemp;
        uint8_t vacreducemode;
        uint8_t wwprio;
        uint8_t fastHeatup;
        char    holiday[26];
        char    vacation[26];
        char    switchtime1[16];
        char    switchtime2[16];
        uint8_t climate;
        uint8_t switchonoptimization;
        uint8_t statusbyte; // from RC300monitor

        // RC 10
        uint8_t  reducehours;   // night reduce duration
        uint16_t reduceminutes; // remaining minutes to night->day

        uint8_t hc_num() const {
            return hc_num_;
        }

        uint8_t hc() const {
            return hc_num_ - 1;
        }

        uint8_t get_model() const {
            return model_;
        }

        // determines if the heating circuit is actually present and has data
        bool is_active() const {
            return Helpers::hasValue(selTemp);
        }

        uint8_t get_mode() const;
        uint8_t get_mode_type() const;

        enum Mode : uint8_t {
            OFF,
            MANUAL,
            AUTO,
            DAY,
            NIGHT,
            HEAT,
            NOFROST,
            ECO,
            HOLIDAY,
            COMFORT,
            OFFSET,
            DESIGN,
            SUMMER,
            FLOWOFFSET,
            MINFLOW,
            MAXFLOW,
            ROOMINFLUENCE,
            TEMPAUTO,
            NOREDUCE,
            REDUCE,
            VACREDUCE,
            ON,
            DAYLOW,
            DAYMID,
            REMOTESELTEMP,
            UNKNOWN

        };

        // for sorting based on hc number
        friend inline bool operator<(const std::shared_ptr<HeatingCircuit> & a, const std::shared_ptr<HeatingCircuit> & b) {
            return (a->hc_num_ < b->hc_num_);
        }

      private:
        uint8_t hc_num_; // heating circuit number 1..10
        uint8_t model_;  // the model type
    };

    static std::string mode_tostring(uint8_t mode);

  private:
    static uuid::log::Logger logger_;

    void register_device_values();
    void register_device_values(uint8_t hc_num);

    // specific thermostat characteristics, stripping the top 4 bits
    inline uint8_t model() const {
        return (flags() & 0x0F);
    }

    // each thermostat has a list of heating controller type IDs for reading and writing
    std::vector<uint16_t> monitor_typeids;
    std::vector<uint16_t> set_typeids;
    std::vector<uint16_t> set2_typeids;
    std::vector<uint16_t> timer_typeids;
    std::vector<uint16_t> timer2_typeids;
    std::vector<uint16_t> summer_typeids;
    std::vector<uint16_t> summer2_typeids;
    std::vector<uint16_t> curve_typeids;

    // standard for all thermostats
    char     status_[20];    // online or offline
    char     dateTime_[25];  // date and time stamp
    char     errorCode_[15]; // code from 0xA2 as string i.e. "A22(816)"
    uint16_t errorNumber_;   // used internally to build error code
    char     lastCode_[50];  // error log

    // Installation parameters
    uint8_t ibaMainDisplay_; // display on Thermostat: 0 int temp, 1 int setpoint, 2 ext temp, 3 burner temp, 4 ww temp, 5 functioning mode, 6 time, 7 data, 9 smoke temp
    uint8_t ibaLanguage_;          // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    int8_t  ibaCalIntTemperature_; // offset int. temperature sensor, by * 0.1 Kelvin (-5.0 to 5.0K)
    int8_t  ibaMinExtTemperature_; // min ext temp for heating curve, in deg., 0xF6=-10, 0x0 = 0, 0xFF=-1
    uint8_t ibaBuildingType_;      // building type: 0 = light, 1 = medium, 2 = heavy
    int8_t  ibaClockOffset_;       // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
    uint8_t ibaDamping_;           // damping 0-off, 0xff-on
    uint8_t backlight_;
    uint8_t heatingpid_;
    int8_t  brightness_;   // Screen brightness 0F=dark F1=light
    uint8_t preheating_;   // Preheating in the clock program: (0x00 = off, 0xFF = on)
    uint8_t autodst_;      // Automatic change Daylight Saving time: (0x00 = off, 0xFF = on)
    uint8_t offtemp_;      // Set Temperature when mode is Off / 10 (e.g.: 0x0F = 7.5 degrees Celsius)
    uint8_t mixingvalves_; // Number of Mixing Valves: (0x00=0, 0x01=1, 0x02=2)

    int8_t  dampedoutdoortemp_;
    int16_t tempsensor1_;
    int16_t tempsensor2_;
    int16_t dampedoutdoortemp2_;
    uint8_t floordrystatus_;
    uint8_t floordrytemp_;
    uint8_t dewtemperature_;
    uint8_t humidity_;

    uint8_t wwExtra1_; // wwExtra active for wwSystem 1
    uint8_t wwExtra2_;
    uint8_t wwMode_;
    uint8_t wwCircPump_;
    uint8_t wwCircMode_;
    uint8_t wwSetTemp_;
    uint8_t wwSetTempLow_;
    uint8_t wwCharge_;
    uint8_t wwChargeDuration_;
    uint8_t wwDisinfecting_;
    uint8_t wwDisinfectDay_;
    uint8_t wwDisinfectHour_;
    uint8_t wwMaxTemp_;
    uint8_t wwOneTimeKey_;
    uint8_t wwProgMode_;
    uint8_t wwCircProg_;
    char    wwSwitchTime_[16];
    char    wwCircSwitchTime_[16];
    uint8_t wwDailyHeating_;
    uint8_t wwDailyHeatTime_;
    uint8_t wwWhenModeOff_;
    char    wwHoliday_[26];
    char    wwVacation_[26];

    // HybridHP
    uint8_t hybridStrategy_;  // co2 = 1, cost = 2, temperature = 3, mix = 4
    int8_t  switchOverTemp_;  // degrees
    uint8_t energyCostRatio_; // is *10
    uint8_t fossileFactor_;   // is * 10
    uint8_t electricFactor_;  // is * 10
    uint8_t delayBoiler_;     // minutes
    uint8_t tempDiffBoiler_;  // relative temperature degrees

    std::vector<std::shared_ptr<HeatingCircuit>> heating_circuits_; // each thermostat can have multiple heating circuits

    uint8_t zero_value_ = 0; // for fixing current room temperature to 0 for HA

    // Generic Types
    static constexpr uint16_t EMS_TYPE_RCTime        = 0x06; // time
    static constexpr uint16_t EMS_TYPE_RCOutdoorTemp = 0xA3; // is an automatic thermostat broadcast, outdoor external temp

    // Type offsets
    static constexpr uint8_t EMS_OFFSET_RC10StatusMessage_setpoint = 1; // setpoint temp
    static constexpr uint8_t EMS_OFFSET_RC10StatusMessage_curr     = 2; // current temp
    static constexpr uint8_t EMS_OFFSET_RC10Set_temp               = 4; // position of thermostat setpoint temperature

    static constexpr uint8_t EMS_OFFSET_RC20StatusMessage_setpoint = 1;  // setpoint temp
    static constexpr uint8_t EMS_OFFSET_RC20StatusMessage_curr     = 2;  // current temp
    static constexpr uint8_t EMS_OFFSET_RC20Set_mode               = 23; // position of thermostat mode
    static constexpr uint8_t EMS_OFFSET_RC20Set_temp_auto          = 28; // position of thermostat setpoint temperature
    static constexpr uint8_t EMS_OFFSET_RC20Set_temp_manual        = 29; // position of thermostat setpoint temperature

    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_mode       = 3; // ES72 - see https://github.com/emsesp/EMS-ESP/issues/334
    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_temp_night = 1; // ES72
    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_temp_day   = 2; // ES72

    static constexpr uint8_t EMS_OFFSET_RC30StatusMessage_setpoint = 1;  // setpoint temp
    static constexpr uint8_t EMS_OFFSET_RC30StatusMessage_curr     = 2;  // current temp
    static constexpr uint8_t EMS_OFFSET_RC30Set_mode               = 23; // position of thermostat mode
    static constexpr uint8_t EMS_OFFSET_RC30Set_temp               = 28; // position of thermostat setpoint temperature
    static constexpr uint8_t EMS_OFFSET_RC30Temp_temp_night        = 3;  // position of thermostat setpoint temperature for night time (T1)
    static constexpr uint8_t EMS_OFFSET_RC30Temp_temp_daylow       = 4;  // position of thermostat setpoint temperature for daylow time (T2)
    static constexpr uint8_t EMS_OFFSET_RC30Temp_temp_daymid       = 5;  // position of thermostat setpoint temperature for daymid time (T3)
    static constexpr uint8_t EMS_OFFSET_RC30Temp_temp_day          = 6;  // position of thermostat setpoint temperature for day time (T4)
    static constexpr uint8_t EMS_OFFSET_RC30Temp_temp_holiday      = 7;  // temp during holiday mode

    static constexpr uint8_t EMS_OFFSET_RC35StatusMessage_setpoint = 2;  // desired temp
    static constexpr uint8_t EMS_OFFSET_RC35StatusMessage_curr     = 3;  // current temp
    static constexpr uint8_t EMS_OFFSET_RC35StatusMessage_mode     = 1;  // day mode, also summer on RC3's
    static constexpr uint8_t EMS_OFFSET_RC35StatusMessage_mode1    = 0;  // for holiday mode
    static constexpr uint8_t EMS_OFFSET_RC35Set_mode               = 7;  // position of thermostat mode
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_day           = 2;  // position of thermostat setpoint temperature for day time
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_night         = 1;  // position of thermostat setpoint temperature for night time
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_holiday       = 3;  // temp during holiday mode
    static constexpr uint8_t EMS_OFFSET_RC35Set_heatingtype        = 0;  // e.g. floor heating = 3
    static constexpr uint8_t EMS_OFFSET_RC35Set_targetflowtemp     = 14; // target flow temperature
    static constexpr uint8_t EMS_OFFSET_RC35Set_seltemp            = 37; // selected temp
    static constexpr uint8_t EMS_OFFSET_RC35Set_noreducetemp       = 38; // temp to stop reducing
    static constexpr uint8_t EMS_OFFSET_RC35Set_reducetemp         = 39; // temp reducing/hold
    static constexpr uint8_t EMS_OFFSET_RC35Set_vacreducetemp      = 40; // temp reducing/hold in vacations
    static constexpr uint8_t EMS_OFFSET_RC35Set_vacreducemode      = 41; // reduce mode in vacations
    static constexpr uint8_t EMS_OFFSET_RC35Set_reducemode         = 25; // reduce mode in normal operation
    static constexpr uint8_t EMS_OFFSET_RC35Set_nofrostmode        = 28; // 0-off, 1-room, 2-outdoor
    static constexpr uint8_t EMS_OFFSET_RC35Set_control            = 26; // 0-off, 1-RC20, 2-RC3x
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_offset        = 6;
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_flowoffset    = 24;
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_design        = 17;
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_design_floor  = 36;
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_summer        = 22;
    static constexpr uint8_t EMS_OFFSET_RC35Set_temp_nofrost       = 23;

    static constexpr uint8_t EMS_OFFSET_EasyStatusMessage_setpoint = 10; // setpoint temp
    static constexpr uint8_t EMS_OFFSET_EasyStatusMessage_curr     = 8;  // current temp

    static constexpr uint8_t EMS_OFFSET_RCPLUSStatusMessage_mode         = 10; // thermostat mode (auto, manual)
    static constexpr uint8_t EMS_OFFSET_RCPLUSStatusMessage_setpoint     = 3;  // setpoint temp
    static constexpr uint8_t EMS_OFFSET_RCPLUSStatusMessage_curr         = 0;  // current temp
    static constexpr uint8_t EMS_OFFSET_RCPLUSStatusMessage_currsetpoint = 6;  // target setpoint temp
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_mode                   = 0;  // operation mode(Auto=0xFF, Manual=0x00)
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_temp_comfort3          = 1;  // comfort3 level
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_temp_comfort2          = 2;  // comfort2 level
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_temp_comfort1          = 3;  // comfort1 level
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_temp_eco               = 4;  // eco level
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_temp_setpoint          = 8;  // temp setpoint, when changing of templevel (in auto) value is reset to FF
    static constexpr uint8_t EMS_OFFSET_RCPLUSSet_manual_setpoint        = 10; // manual setpoint

    static constexpr uint8_t EMS_OFFSET_JunkersStatusMessage_daymode     = 0;  // 3 = day, 2 = night, 1 = nofrost
    static constexpr uint8_t EMS_OFFSET_JunkersStatusMessage_mode        = 1;  // current mode, 1 = manual, 2 = auto
    static constexpr uint8_t EMS_OFFSET_JunkersStatusMessage_setpoint    = 2;  // setpoint temp
    static constexpr uint8_t EMS_OFFSET_JunkersStatusMessage_curr        = 4;  // current temp
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage_day_temp       = 17; // EMS offset to set temperature on thermostat for day mode
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage_night_temp     = 16; // EMS offset to set temperature on thermostat for night mode
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage_no_frost_temp  = 15; // EMS offset to set temperature on thermostat for no frost mode
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage_set_mode       = 14; // EMS offset to set mode on thermostat
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage2_set_mode      = 4;  // EMS offset to set mode on thermostat
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage2_no_frost_temp = 5;
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage2_eco_temp      = 6;
    static constexpr uint8_t EMS_OFFSET_JunkersSetMessage2_heat_temp     = 7;

    static constexpr uint8_t AUTO_HEATING_CIRCUIT = 0;

    // Installation settings
    static constexpr uint8_t EMS_TYPE_IBASettings    = 0xA5; // installation settings
    static constexpr uint8_t EMS_TYPE_RC30Settings   = 0xA7; // RC30 settings
    static constexpr uint8_t EMS_TYPE_wwSettings     = 0x37; // ww settings
    static constexpr uint8_t EMS_TYPE_RC30wwSettings = 0x3A; // RC30 ww settings
    static constexpr uint8_t EMS_TYPE_time           = 0x06; // time

    std::shared_ptr<Thermostat::HeatingCircuit> heating_circuit(std::shared_ptr<const Telegram> telegram);
    std::shared_ptr<Thermostat::HeatingCircuit> heating_circuit(const uint8_t hc_num);

    void register_device_values_hc(std::shared_ptr<Thermostat::HeatingCircuit> hc);

    void add_ha_climate(std::shared_ptr<HeatingCircuit> hc) const;

    void process_RCOutdoorTemp(std::shared_ptr<const Telegram> telegram);
    void process_IBASettings(std::shared_ptr<const Telegram> telegram);
    void process_RCTime(std::shared_ptr<const Telegram> telegram);
    void process_RCError(std::shared_ptr<const Telegram> telegram);
    void process_RCErrorMessage(std::shared_ptr<const Telegram> telegram);
    void process_RC35wwSettings(std::shared_ptr<const Telegram> telegram);
    void process_RC35wwTimer(std::shared_ptr<const Telegram> telegram);
    void process_RC35Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC35Set(std::shared_ptr<const Telegram> telegram);
    void process_RC35Timer(std::shared_ptr<const Telegram> telegram);
    void process_RC30Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC30Set(std::shared_ptr<const Telegram> telegram);
    void process_RC30Temp(std::shared_ptr<const Telegram> telegram);
    void process_RC30wwSettings(std::shared_ptr<const Telegram> telegram);
    void process_RC30Timer(std::shared_ptr<const Telegram> telegram);
    void process_RC20Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC20Set(std::shared_ptr<const Telegram> telegram);
    void process_RC20Temp(std::shared_ptr<const Telegram> telegram);
    void process_RC20Timer(std::shared_ptr<const Telegram> telegram);
    void process_RC20Remote(std::shared_ptr<const Telegram> telegram);
    void process_RC20Monitor_2(std::shared_ptr<const Telegram> telegram);
    void process_RC20Set_2(std::shared_ptr<const Telegram> telegram);
    void process_RC10Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC10Set(std::shared_ptr<const Telegram> telegram);
    void process_RC10Set_2(std::shared_ptr<const Telegram> telegram);
    void process_CRFMonitor(std::shared_ptr<const Telegram> telegram);
    void process_RC300Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC300Set(std::shared_ptr<const Telegram> telegram);
    void process_RC300Set2(std::shared_ptr<const Telegram> telegram);
    void process_RC300Summer(std::shared_ptr<const Telegram> telegram);
    void process_RC300Summer2(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWmode(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWmode2(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWtemp(std::shared_ptr<const Telegram> telegram);
    void process_RC300OutdoorTemp(std::shared_ptr<const Telegram> telegram);
    void process_RC300Settings(std::shared_ptr<const Telegram> telegram);
    void process_RC300Floordry(std::shared_ptr<const Telegram> telegram);
    void process_RC300Curve(std::shared_ptr<const Telegram> telegram);
    void process_JunkersMonitor(std::shared_ptr<const Telegram> telegram);
    void process_JunkersSet(std::shared_ptr<const Telegram> telegram);
    void process_JunkersSet2(std::shared_ptr<const Telegram> telegram);
    void process_EasyMonitor(std::shared_ptr<const Telegram> telegram);
    void process_JunkersRemoteMonitor(std::shared_ptr<const Telegram> telegram);
    void process_JunkersHybridSettings(std::shared_ptr<const Telegram> telegram);
    void process_JunkersSetMixer(std::shared_ptr<const Telegram> telegram);
    void process_RemoteTemp(std::shared_ptr<const Telegram> telegram);
    void process_RemoteHumidity(std::shared_ptr<const Telegram> telegram);
    void process_RemoteCorrection(std::shared_ptr<const Telegram> telegram);

    // internal helper functions
    bool set_mode_n(const uint8_t mode, const uint8_t hc_num);

    bool set_temperature_value(const char * value, const int8_t id, const uint8_t mode, bool relative = false);
    bool set_temperature(const float temperature, const uint8_t mode, const uint8_t hc_num);
    bool set_switchtime(const char * value, const uint16_t type_id, char * out, size_t len);

    // set functions - these use the id/hc
    bool set_mode(const char * value, const int8_t id);
    bool set_control(const char * value, const int8_t id);
    bool set_holiday(const char * value, const int8_t id, const bool vacation = false);
    bool set_vacation(const char * value, const int8_t id) {
        return set_holiday(value, id, true);
    }
    bool set_pause(const char * value, const int8_t id);
    bool set_party(const char * value, const int8_t id);
    bool set_summermode(const char * value, const int8_t id);

    bool set_temp(const char * value, const int8_t id);
    bool set_nighttemp(const char * value, const int8_t id);
    bool set_daytemp(const char * value, const int8_t id);
    bool set_daylowtemp(const char * value, const int8_t id);
    bool set_daymidtemp(const char * value, const int8_t id);
    bool set_comforttemp(const char * value, const int8_t id);
    bool set_nofrosttemp(const char * value, const int8_t id);
    bool set_ecotemp(const char * value, const int8_t id);
    bool set_heattemp(const char * value, const int8_t id);
    bool set_summertemp(const char * value, const int8_t id);
    bool set_designtemp(const char * value, const int8_t id);
    bool set_offsettemp(const char * value, const int8_t id);
    bool set_holidaytemp(const char * value, const int8_t id);
    bool set_manualtemp(const char * value, const int8_t id);
    bool set_tempautotemp(const char * value, const int8_t id);
    bool set_noreducetemp(const char * value, const int8_t id);
    bool set_reducetemp(const char * value, const int8_t id);
    bool set_vacreducetemp(const char * value, const int8_t id);
    bool set_vacreducemode(const char * value, const int8_t id);
    bool set_nofrostmode(const char * value, const int8_t id);
    bool set_remotetemp(const char * value, const int8_t id);
    bool set_roominfluence(const char * value, const int8_t id);
    bool set_roominfl_factor(const char * value, const int8_t id);
    bool set_flowtempoffset(const char * value, const int8_t id);
    bool set_minflowtemp(const char * value, const int8_t id);
    bool set_maxflowtemp(const char * value, const int8_t id);
    bool set_reducemode(const char * value, const int8_t id);
    bool set_switchtime1(const char * value, const int8_t id);
    bool set_switchtime2(const char * value, const int8_t id);
    bool set_program(const char * value, const int8_t id);
    bool set_controlmode(const char * value, const int8_t id);
    bool set_wwprio(const char * value, const int8_t id);
    bool set_fastheatup(const char * value, const int8_t id);
    bool set_switchonoptimization(const char * value, const int8_t id);
    bool set_remoteseltemp(const char * value, const int8_t id);

    // set functions - these don't use the id/hc, the parameters are ignored
    bool set_wwmode(const char * value, const int8_t id);
    bool set_wwtemp(const char * value, const int8_t id);
    bool set_wwtemplow(const char * value, const int8_t id);
    bool set_wwcircmode(const char * value, const int8_t id);
    bool set_wwcharge(const char * value, const int8_t id);
    bool set_wwchargeduration(const char * value, const int8_t id);
    bool set_wwDisinfect(const char * value, const int8_t id);
    bool set_wwDisinfectDay(const char * value, const int8_t id);
    bool set_wwDisinfectHour(const char * value, const int8_t id);
    bool set_wwMaxTemp(const char * value, const int8_t id);
    bool set_wwOneTimeKey(const char * value, const int8_t id);
    bool set_wwProgMode(const char * value, const int8_t id);
    bool set_wwCircProg(const char * value, const int8_t id);
    bool set_wwSwitchTime(const char * value, const int8_t id);
    bool set_wwCircSwitchTime(const char * value, const int8_t id);
    bool set_wwDailyHeating(const char * value, const int8_t id);
    bool set_wwDailyHeatTime(const char * value, const int8_t id);
    bool set_wwwhenmodeoff(const char * value, const int8_t id);
    bool set_wwVacation(const char * value, const int8_t id) {
        return set_holiday(value, DeviceValueTAG::TAG_WWC1, true);
    }
    bool set_wwHoliday(const char * value, const int8_t id) {
        return set_holiday(value, DeviceValueTAG::TAG_WWC1);
    }

    bool set_datetime(const char * value, const int8_t id);
    bool set_minexttemp(const char * value, const int8_t id);
    bool set_clockoffset(const char * value, const int8_t id);
    bool set_calinttemp(const char * value, const int8_t id);
    bool set_display(const char * value, const int8_t id);
    bool set_building(const char * value, const int8_t id);
    bool set_damping(const char * value, const int8_t id);
    bool set_language(const char * value, const int8_t id);
    bool set_heatingtype(const char * value, const int8_t id);
    bool set_reducehours(const char * value, const int8_t id);
    bool set_backlight(const char * value, const int8_t id);
    bool set_heatingpid(const char * value, const int8_t id);
    bool set_brightness(const char * value, const int8_t id);
    bool set_autodst(const char * value, const int8_t id);
    bool set_preheating(const char * value, const int8_t id);
    bool set_mixingvalves(const char * value, const int8_t id);
    bool set_offtemp(const char * value, const int8_t id);

    bool set_hybridStrategy(const char * value, const int8_t id);
    bool set_switchOverTemp(const char * value, const int8_t id);
    bool set_energyCostRatio(const char * value, const int8_t id);
    bool set_fossileFactor(const char * value, const int8_t id);
    bool set_electricFactor(const char * value, const int8_t id);
    bool set_delayBoiler(const char * value, const int8_t id);
    bool set_tempDiffBoiler(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
