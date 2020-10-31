/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

#include <Arduino.h>
#include <ArduinoJson.h>

#include <uuid/log.h>

#include "emsdevice.h"
#include "telegram.h"
#include "emsesp.h"
#include "helpers.h"
#include "mqtt.h"

#include <vector>
#include <time.h>

namespace emsesp {

class Thermostat : public EMSdevice {
  public:
    Thermostat(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);
    class HeatingCircuit {
      public:
        HeatingCircuit(const uint8_t hc_num)
            : hc_num_(hc_num)
            , ha_registered_(false) {
        }
        ~HeatingCircuit() = default;

        int16_t setpoint_roomTemp = EMS_VALUE_SHORT_NOTSET;
        int16_t curr_roomTemp     = EMS_VALUE_SHORT_NOTSET;
        uint8_t mode              = EMS_VALUE_UINT_NOTSET;
        uint8_t mode_type         = EMS_VALUE_UINT_NOTSET;
        uint8_t summer_mode       = EMS_VALUE_UINT_NOTSET;
        uint8_t holiday_mode      = EMS_VALUE_UINT_NOTSET;
        uint8_t daytemp           = EMS_VALUE_UINT_NOTSET;
        uint8_t nighttemp         = EMS_VALUE_UINT_NOTSET;
        uint8_t holidaytemp       = EMS_VALUE_UINT_NOTSET;
        uint8_t heatingtype       = EMS_VALUE_UINT_NOTSET; // type of heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
        uint8_t targetflowtemp    = EMS_VALUE_UINT_NOTSET;
        uint8_t summertemp        = EMS_VALUE_UINT_NOTSET;
        uint8_t nofrosttemp       = EMS_VALUE_UINT_NOTSET;
        uint8_t designtemp        = EMS_VALUE_UINT_NOTSET; // heating curve design temp at MinExtTemp
        int8_t  offsettemp        = EMS_VALUE_INT_NOTSET;  // heating curve offest temp at roomtemp signed!
        uint8_t manualtemp        = EMS_VALUE_UINT_NOTSET;
        uint8_t summer_setmode    = EMS_VALUE_UINT_NOTSET;
        uint8_t roominfluence     = EMS_VALUE_UINT_NOTSET;
        uint8_t flowtempoffset    = EMS_VALUE_UINT_NOTSET;

        uint8_t hc_num() const {
            return hc_num_;
        }

        bool ha_registered() const {
            return ha_registered_;
        }

        void ha_registered(bool b) {
            ha_registered_ = b;
        }

        // determines if the heating circuit is actually present and has data
        bool is_active() {
            return Helpers::hasValue(setpoint_roomTemp);
        }

        uint8_t get_mode(uint8_t flags) const;
        uint8_t get_mode_type(uint8_t flags) const;

        enum Mode : uint8_t { UNKNOWN, OFF, MANUAL, AUTO, DAY, NIGHT, HEAT, NOFROST, ECO, HOLIDAY, COMFORT, OFFSET, DESIGN, SUMMER, FLOWOFFSET };

        // for sorting based on hc number
        friend inline bool operator<(const std::shared_ptr<HeatingCircuit> & lhs, const std::shared_ptr<HeatingCircuit> & rhs) {
            return (lhs->hc_num_ < rhs->hc_num_);
        }

      private:
        uint8_t hc_num_;        // heating circuit number 1..10
        bool    ha_registered_; // whether it has been registered for HA MQTT Discovery
    };

    static std::string mode_tostring(uint8_t mode);

    virtual void show_values(uuid::console::Shell & shell);
    virtual void publish_values(JsonObject & json, bool force);
    virtual bool export_values(JsonObject & json);
    virtual void device_info_web(JsonArray & root);
    virtual bool updated_values();

  private:
    static uuid::log::Logger logger_;

    void add_commands();
    bool export_values_main(JsonObject & doc);
    bool export_values_hc(uint8_t mqtt_format, JsonObject & doc);

    bool ha_registered() const {
        return ha_registered_;
    }

    void ha_registered(bool b) {
        ha_registered_ = b;
    }

    // specific thermostat characteristics, stripping the last 4 bits
    inline uint8_t model() const {
        return (this->flags() & 0x0F);
    }

    // each thermostat has a list of heating controller type IDs for reading and writing
    std::vector<uint16_t> monitor_typeids;
    std::vector<uint16_t> set_typeids;
    std::vector<uint16_t> timer_typeids;
    std::vector<uint16_t> summer_typeids;

    std::string datetime_;  // date and time stamp
    std::string errorCode_; // code as string i.e. "A22(816)"

    bool changed_       = false;
    bool ha_registered_ = false;

    // Installation parameters
    uint8_t ibaMainDisplay_ =
        EMS_VALUE_UINT_NOTSET; // display on Thermostat: 0 int temp, 1 int setpoint, 2 ext temp, 3 burner temp, 4 ww temp, 5 functioning mode, 6 time, 7 data, 9 smoke temp
    uint8_t ibaLanguage_          = EMS_VALUE_UINT_NOTSET; // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    int8_t  ibaCalIntTemperature_ = EMS_VALUE_INT_NOTSET;  // offset int. temperature sensor, by * 0.1 Kelvin (-5.0 to 5.0K)
    int8_t  ibaMinExtTemperature_ = EMS_VALUE_INT_NOTSET;  // min ext temp for heating curve, in deg., 0xF6=-10, 0x0 = 0, 0xFF=-1
    uint8_t ibaBuildingType_      = EMS_VALUE_UINT_NOTSET; // building type: 0 = light, 1 = medium, 2 = heavy
    uint8_t ibaClockOffset_       = EMS_VALUE_UINT_NOTSET; // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s

    uint16_t errorNumber_        = EMS_VALUE_USHORT_NOTSET;
    int8_t   dampedoutdoortemp_  = EMS_VALUE_INT_NOTSET;
    uint16_t tempsensor1_        = EMS_VALUE_USHORT_NOTSET;
    uint16_t tempsensor2_        = EMS_VALUE_USHORT_NOTSET;
    int16_t  dampedoutdoortemp2_ = EMS_VALUE_SHORT_NOTSET;
    uint8_t  floordrystatus_     = EMS_VALUE_UINT_NOTSET;
    uint8_t  floordrytemp_       = EMS_VALUE_UINT_NOTSET;

    uint8_t wwExtra1_   = EMS_VALUE_UINT_NOTSET; // wwExtra active for wwSystem 1
    uint8_t wwExtra2_   = EMS_VALUE_UINT_NOTSET;
    uint8_t wwMode_     = EMS_VALUE_UINT_NOTSET;
    uint8_t wwCircMode_ = EMS_VALUE_UINT_NOTSET;
    uint8_t wwTemp_     = EMS_VALUE_UINT_NOTSET;
    uint8_t wwTempLow_  = EMS_VALUE_UINT_NOTSET;

    std::vector<std::shared_ptr<HeatingCircuit>> heating_circuits_; // each thermostat can have multiple heating circuits

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
    static constexpr uint8_t EMS_OFFSET_RC20Set_temp               = 28; // position of thermostat setpoint temperature

    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_mode       = 3; // ES72 - see https://github.com/proddy/EMS-ESP/issues/334
    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_temp_night = 1; // ES72
    static constexpr uint8_t EMS_OFFSET_RC20_2_Set_temp_day   = 2; // ES72

    static constexpr uint8_t EMS_OFFSET_RC30StatusMessage_setpoint = 1;  // setpoint temp
    static constexpr uint8_t EMS_OFFSET_RC30StatusMessage_curr     = 2;  // current temp
    static constexpr uint8_t EMS_OFFSET_RC30Set_mode               = 23; // position of thermostat mode
    static constexpr uint8_t EMS_OFFSET_RC30Set_temp               = 28; // position of thermostat setpoint temperature

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
    static constexpr uint8_t EMS_TYPE_IBASettings = 0xA5; // installation settings
    static constexpr uint8_t EMS_TYPE_wwSettings  = 0x37; // ww settings
    static constexpr uint8_t EMS_TYPE_time        = 0x06; // time

    std::shared_ptr<Thermostat::HeatingCircuit> heating_circuit(std::shared_ptr<const Telegram> telegram);
    std::shared_ptr<Thermostat::HeatingCircuit> heating_circuit(const uint8_t hc_num);

    void register_mqtt_ha_config();
    void register_mqtt_ha_config(uint8_t hc_num);
    void ha_config(bool force = false);
    bool thermostat_ha_cmd(const char * message, uint8_t hc_num);

    void process_RCOutdoorTemp(std::shared_ptr<const Telegram> telegram);
    void process_IBASettings(std::shared_ptr<const Telegram> telegram);
    void process_RCTime(std::shared_ptr<const Telegram> telegram);
    void process_RCError(std::shared_ptr<const Telegram> telegram);
    void process_RC35wwSettings(std::shared_ptr<const Telegram> telegram);
    void process_RC35Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC35Set(std::shared_ptr<const Telegram> telegram);
    void process_RC30Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC30Set(std::shared_ptr<const Telegram> telegram);
    void process_RC20Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC20Set(std::shared_ptr<const Telegram> telegram);
    void process_RC20Remote(std::shared_ptr<const Telegram> telegram);
    void process_RC20Monitor_2(std::shared_ptr<const Telegram> telegram);
    void process_RC20Set_2(std::shared_ptr<const Telegram> telegram);
    void process_RC10Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC10Set(std::shared_ptr<const Telegram> telegram);
    void process_RC300Monitor(std::shared_ptr<const Telegram> telegram);
    void process_RC300Set(std::shared_ptr<const Telegram> telegram);
    void process_RC300Summer(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWmode(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWmode2(std::shared_ptr<const Telegram> telegram);
    void process_RC300WWtemp(std::shared_ptr<const Telegram> telegram);
    void process_RC300OutdoorTemp(std::shared_ptr<const Telegram> telegram);
    void process_RC300Settings(std::shared_ptr<const Telegram> telegram);
    void process_RC300Floordry(std::shared_ptr<const Telegram> telegram);
    void process_JunkersMonitor(std::shared_ptr<const Telegram> telegram);
    void process_JunkersSet(std::shared_ptr<const Telegram> telegram);
    void process_JunkersSet2(std::shared_ptr<const Telegram> telegram);
    void process_EasyMonitor(std::shared_ptr<const Telegram> telegram);

    // internal helper functions
    bool set_mode_n(const uint8_t mode, const uint8_t hc_num);

    bool set_temperature_value(const char * value, const int8_t id, const uint8_t mode);
    bool set_temperature(const float temperature, const std::string & mode, const uint8_t hc_num);
    bool set_temperature(const float temperature, const uint8_t mode, const uint8_t hc_num);

    // set functions - these use the id/hc
    bool set_mode(const char * value, const int8_t id);
    bool set_control(const char * value, const int8_t id);
    bool set_holiday(const char * value, const int8_t id);
    bool set_pause(const char * value, const int8_t id);
    bool set_party(const char * value, const int8_t id);
    bool set_summermode(const char * value, const int8_t id);

    bool set_temp(const char * value, const int8_t id);
    bool set_nighttemp(const char * value, const int8_t id);
    bool set_daytemp(const char * value, const int8_t id);
    bool set_comforttemp(const char * value, const int8_t id);
    bool set_nofrosttemp(const char * value, const int8_t id);
    bool set_ecotemp(const char * value, const int8_t id);
    bool set_heattemp(const char * value, const int8_t id);
    bool set_summertemp(const char * value, const int8_t id);
    bool set_designtemp(const char * value, const int8_t id);
    bool set_offsettemp(const char * value, const int8_t id);
    bool set_holidaytemp(const char * value, const int8_t id);
    bool set_manualtemp(const char * value, const int8_t id);
    bool set_remotetemp(const char * value, const int8_t id);
    bool set_roominfluence(const char * value, const int8_t id);
    bool set_flowtempoffset(const char * value, const int8_t id);

    // set functions - these don't use the id/hc, the parameters are ignored
    bool set_wwmode(const char * value, const int8_t id);
    bool set_wwtemp(const char * value, const int8_t id);
    bool set_wwtemplow(const char * value, const int8_t id);
    bool set_wwcircmode(const char * value, const int8_t id);
    bool set_datetime(const char * value, const int8_t id);
    bool set_minexttemp(const char * value, const int8_t id);
    bool set_clockoffset(const char * value, const int8_t id);
    bool set_calinttemp(const char * value, const int8_t id);
    bool set_display(const char * value, const int8_t id);
    bool set_building(const char * value, const int8_t id);
    bool set_language(const char * value, const int8_t id);
};

} // namespace emsesp

#endif