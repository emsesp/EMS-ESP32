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

#include "boiler.h"

namespace emsesp {

REGISTER_FACTORY(Boiler, EMSdevice::DeviceType::BOILER)

uuid::log::Logger Boiler::logger_{F_(boiler), uuid::log::Facility::CONSOLE};

Boiler::Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Boiler with device ID 0x%02X"), device_id);

    reserve_telgram_functions(25); // reserve some space for the telegram registries, to avoid memory fragmentation

    // the telegram handlers...
    register_telegram_type(0x10, F("UBAErrorMessage1"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAErrorMessage(t); });
    register_telegram_type(0x11, F("UBAErrorMessage2"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAErrorMessage(t); });
    register_telegram_type(0x14, F("UBATotalUptime"), true, [&](std::shared_ptr<const Telegram> t) { process_UBATotalUptime(t); });
    register_telegram_type(0x15, F("UBAMaintenanceData"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMaintenanceData(t); });
    register_telegram_type(0x16, F("UBAParameters"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameters(t); });
    register_telegram_type(0x18, F("UBAMonitorFast"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorFast(t); });
    register_telegram_type(0x19, F("UBAMonitorSlow"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlow(t); });
    register_telegram_type(0x1A, F("UBASetPoints"), false, [&](std::shared_ptr<const Telegram> t) { process_UBASetPoints(t); });
    register_telegram_type(0x1C, F("UBAMaintenanceStatus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMaintenanceStatus(t); });
    register_telegram_type(0x2A, F("MC10Status"), false, [&](std::shared_ptr<const Telegram> t) { process_MC10Status(t); });
    register_telegram_type(0x33, F("UBAParameterWW"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameterWW(t); });
    register_telegram_type(0x34, F("UBAMonitorWW"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorWW(t); });
    register_telegram_type(0x35, F("UBAFlags"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAFlags(t); });
    register_telegram_type(0xD1, F("UBAOutdoorTemp"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAOutdoorTemp(t); });
    register_telegram_type(0xE3, F("UBAMonitorSlowPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlowPlus2(t); });
    register_telegram_type(0xE4, F("UBAMonitorFastPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorFastPlus(t); });
    register_telegram_type(0xE5, F("UBAMonitorSlowPlus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAMonitorSlowPlus(t); });
    register_telegram_type(0xE6, F("UBAParametersPlus"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParametersPlus(t); });
    register_telegram_type(0xE9, F("UBADHWStatus"), false, [&](std::shared_ptr<const Telegram> t) { process_UBADHWStatus(t); });
    register_telegram_type(0xEA, F("UBAParameterWWPlus"), true, [&](std::shared_ptr<const Telegram> t) { process_UBAParameterWWPlus(t); });
    register_telegram_type(0x494, F("UBAEnergySupplied"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAEnergySupplied(t); });
    register_telegram_type(0x495, F("UBAInformation"), false, [&](std::shared_ptr<const Telegram> t) { process_UBAInformation(t); });

    EMSESP::send_read_request(0x10,
                              device_id); // read last errorcode on start (only published on errors)
    EMSESP::send_read_request(0x11,
                              device_id); // read last errorcode on start (only published on errors)
    EMSESP::send_read_request(0x15,
                              device_id); // read maintenace data on start (only published on change)
    EMSESP::send_read_request(0x1C,
                              device_id); // read maintenace status on start (only published on change)

    // MQTT commands for boiler topic
    register_mqtt_cmd(F("comfort"), [&](const char * value, const int8_t id) { return set_warmwater_mode(value, id); });
    register_mqtt_cmd(F("wwactivated"), [&](const char * value, const int8_t id) { return set_warmwater_activated(value, id); });
    register_mqtt_cmd(F("wwtapactivated"), [&](const char * value, const int8_t id) { return set_tapwarmwater_activated(value, id); });
    register_mqtt_cmd(F("wwonetime"), [&](const char * value, const int8_t id) { return set_warmwater_onetime(value, id); });
    register_mqtt_cmd(F("wwcircpump"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_pump(value, id); });
    register_mqtt_cmd(F("wwcirculation"), [&](const char * value, const int8_t id) { return set_warmwater_circulation(value, id); });
    register_mqtt_cmd(F("wwcircmode"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_mode(value, id); });
    register_mqtt_cmd(F("flowtemp"), [&](const char * value, const int8_t id) { return set_flow_temp(value, id); });
    register_mqtt_cmd(F("wwsettemp"), [&](const char * value, const int8_t id) { return set_warmwater_temp(value, id); });
    register_mqtt_cmd(F("heatingactivated"), [&](const char * value, const int8_t id) { return set_heating_activated(value, id); });
    register_mqtt_cmd(F("heatingtemp"), [&](const char * value, const int8_t id) { return set_heating_temp(value, id); });
    register_mqtt_cmd(F("burnmaxpower"), [&](const char * value, const int8_t id) { return set_max_power(value, id); });
    register_mqtt_cmd(F("burnminpower"), [&](const char * value, const int8_t id) { return set_min_power(value, id); });
    register_mqtt_cmd(F("boilhyston"), [&](const char * value, const int8_t id) { return set_hyst_on(value, id); });
    register_mqtt_cmd(F("boilhystoff"), [&](const char * value, const int8_t id) { return set_hyst_off(value, id); });
    register_mqtt_cmd(F("burnperiod"), [&](const char * value, const int8_t id) { return set_burn_period(value, id); });
    register_mqtt_cmd(F("pumpdelay"), [&](const char * value, const int8_t id) { return set_pump_delay(value, id); });
    register_mqtt_cmd(F("maintenance"), [&](const char * value, const int8_t id) { return set_maintenance(value, id); });
    register_mqtt_cmd(F("pumpmodmax"), [&](const char * value, const int8_t id) { return set_max_pump(value, id); });
    register_mqtt_cmd(F("pumpmodmin"), [&](const char * value, const int8_t id) { return set_min_pump(value, id); });
    // register_mqtt_cmd(F("reset"), [&](const char * value, const int8_t id) { return set_reset(value, id); });

    // add values
    reserve_device_values(50);

    // main - boiler_data topic
    register_device_value(TAG_BOILER_DATA, &id_, DeviceValueType::UINT, nullptr, F("id"), nullptr); // empty full name to prevent being shown in web or console
    id_ = product_id;

    register_device_value(TAG_BOILER_DATA, &heatingActive_, DeviceValueType::BOOL, nullptr, F("heatingActive"), F("heating active"));
    register_device_value(TAG_BOILER_DATA, &tapwaterActive_, DeviceValueType::BOOL, nullptr, F("tapwaterActive"), F("warm water/DHW active"));

    register_device_value(TAG_BOILER_DATA, &selFlowTemp_, DeviceValueType::UINT, nullptr, F("selFlowTemp"), F("selected flow temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &selBurnPow_, DeviceValueType::UINT, nullptr, F("selBurnPow"), F("burner selected max power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &heatingPumpMod_, DeviceValueType::UINT, nullptr, F("heatingPumpMod"), F("heating pump modulation"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &heatingPump2Mod_, DeviceValueType::UINT, nullptr, F("heatingPump2Mod"), F("heating pump 2 modulation"), DeviceValueUOM::PERCENT);

    register_device_value(TAG_BOILER_DATA, &outdoorTemp_, DeviceValueType::SHORT, FL_(div10), F("outdoorTemp"), F("outside temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &curFlowTemp_, DeviceValueType::USHORT, FL_(div10), F("curFlowTemp"), F("current flow temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &retTemp_, DeviceValueType::USHORT, FL_(div10), F("retTemp"), F("return temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &switchTemp_, DeviceValueType::USHORT, FL_(div10), F("switchTemp"), F("mixing switch temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &sysPress_, DeviceValueType::UINT, FL_(div10), F("sysPress"), F("system pressure"), DeviceValueUOM::BAR);
    register_device_value(TAG_BOILER_DATA, &boilTemp_, DeviceValueType::USHORT, FL_(div10), F("boilTemp"), F("max boiler temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &exhaustTemp_, DeviceValueType::USHORT, FL_(div10), F("exhaustTemp"), F("exhaust temperature"), DeviceValueUOM::DEGREES);

    register_device_value(TAG_BOILER_DATA, &burnGas_, DeviceValueType::BOOL, nullptr, F("burnGas"), F("gas"));
    register_device_value(TAG_BOILER_DATA, &flameCurr_, DeviceValueType::USHORT, FL_(div10), F("flameCurr"), F("flame current"), DeviceValueUOM::UA);
    register_device_value(TAG_BOILER_DATA, &heatingPump_, DeviceValueType::BOOL, nullptr, F("heatingPump"), F("heating pump"), DeviceValueUOM::PUMP);
    register_device_value(TAG_BOILER_DATA, &fanWork_, DeviceValueType::BOOL, nullptr, F("fanWork"), F("fan"));
    register_device_value(TAG_BOILER_DATA, &ignWork_, DeviceValueType::BOOL, nullptr, F("ignWork"), F("ignition"));
    register_device_value(TAG_BOILER_DATA, &heatingActivated_, DeviceValueType::BOOL, nullptr, F("heatingActivated"), F("heating activated"));
    register_device_value(TAG_BOILER_DATA, &heatingTemp_, DeviceValueType::UINT, nullptr, F("heatingTemp"), F("heating temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &pumpModMax_, DeviceValueType::UINT, nullptr, F("pumpModMax"), F("burner pump max power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &pumpModMin_, DeviceValueType::UINT, nullptr, F("pumpModMin"), F("burner pump min power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &pumpDelay_, DeviceValueType::UINT, nullptr, F("pumpDelay"), F("pump delay"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA, &burnMinPeriod_, DeviceValueType::UINT, nullptr, F("burnMinPeriod"), F("burner min period"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA, &burnMinPower_, DeviceValueType::UINT, nullptr, F("burnMinPower"), F("burner min power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &burnMaxPower_, DeviceValueType::UINT, nullptr, F("burnMaxPower"), F("burner max power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &boilHystOn_, DeviceValueType::INT, nullptr, F("boilHystOn"), F("hysteresis on temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &boilHystOff_, DeviceValueType::INT, nullptr, F("boilHystOff"), F("hysteresis off temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &setFlowTemp_, DeviceValueType::UINT, nullptr, F("setFlowTemp"), F("set flow temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA, &setBurnPow_, DeviceValueType::UINT, nullptr, F("setBurnPow"), F("burner set power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &curBurnPow_, DeviceValueType::UINT, nullptr, F("curBurnPow"), F("burner current power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA, &burnStarts_, DeviceValueType::ULONG, nullptr, F("burnStarts"), F("burner # starts"));
    register_device_value(TAG_BOILER_DATA, &burnWorkMin_, DeviceValueType::TIME, nullptr, F("burnWorkMin"), F("total burner operating time"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA, &heatWorkMin_, DeviceValueType::TIME, nullptr, F("heatWorkMin"), F("total heat operating time"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA, &UBAuptime_, DeviceValueType::TIME, nullptr, F("UBAuptime"), F("total UBA operating time"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA, &lastCode_, DeviceValueType::TEXT, nullptr, F("lastCode"), F("last error code"));
    register_device_value(TAG_BOILER_DATA, &serviceCode_, DeviceValueType::TEXT, nullptr, F("serviceCode"), F("service code"));
    register_device_value(TAG_BOILER_DATA, &serviceCodeNumber_, DeviceValueType::USHORT, nullptr, F("serviceCodeNumber"), F("service code number"));

    // ww - boiler_data_ww topic
    register_device_value(TAG_BOILER_DATA_WW, &wWSelTemp_, DeviceValueType::UINT, nullptr, F("wWSelTemp"), F("selected temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWSetTemp_, DeviceValueType::UINT, nullptr, F("wWSetTemp"), F("set temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWType_, DeviceValueType::ENUM, FL_(enum_flow), F("wWType"), F("type"));
    register_device_value(TAG_BOILER_DATA_WW, &wWComfort_, DeviceValueType::ENUM, FL_(enum_comfort), F("wWComfort"), F("comfort"));
    register_device_value(TAG_BOILER_DATA_WW, &wWCircPump_, DeviceValueType::BOOL, nullptr, F("wWCircPump"), F("circulation pump available"));
    register_device_value(TAG_BOILER_DATA_WW, &wWChargeType_, DeviceValueType::BOOL, FL_(enum_charge), F("wWChargeType"), F("charging type"));
    register_device_value(TAG_BOILER_DATA_WW, &wWDisinfectionTemp_, DeviceValueType::UINT, nullptr, F("wWDisinfectionTemp"), F("disinfection temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWCircPumpMode_, DeviceValueType::ENUM, FL_(enum_freq), F("wWCircPumpMode"), F("circulation pump freq"));
    register_device_value(TAG_BOILER_DATA_WW, &wWCirc_, DeviceValueType::BOOL, nullptr, F("wWCirc"), F("circulation active"));
    register_device_value(TAG_BOILER_DATA_WW, &wWCurTemp_, DeviceValueType::USHORT, FL_(div10), F("wWCurTemp"), F("current temperature (intern)"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWCurTemp2_, DeviceValueType::USHORT, FL_(div10), F("wWCurTemp2"), F("current temperature (extern)"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWCurFlow_, DeviceValueType::UINT, FL_(div10), F("wWCurFlow"), F("current tap water flow"), DeviceValueUOM::LMIN);
    register_device_value(TAG_BOILER_DATA_WW, &wWStorageTemp1_, DeviceValueType::USHORT, FL_(div10), F("wWStorageTemp1"), F("storage temperature (intern)"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWStorageTemp2_, DeviceValueType::USHORT, FL_(div10), F("wWStorageTemp2"), F("storage temperature (extern)"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWActivated_, DeviceValueType::BOOL, nullptr, F("wWActivated"), F("activated"));
    register_device_value(TAG_BOILER_DATA_WW, &wWOneTime_, DeviceValueType::BOOL, nullptr, F("wWOneTime"), F("one time charging"));
    register_device_value(TAG_BOILER_DATA_WW, &wWDisinfecting_, DeviceValueType::BOOL, nullptr, F("wWDisinfecting"), F("disinfecting"));
    register_device_value(TAG_BOILER_DATA_WW, &wWCharging_, DeviceValueType::BOOL, nullptr, F("wWCharging"), F("charging"));
    register_device_value(TAG_BOILER_DATA_WW, &wWRecharging_, DeviceValueType::BOOL, nullptr, F("wWRecharging"), F("recharging"));
    register_device_value(TAG_BOILER_DATA_WW, &wWTempOK_, DeviceValueType::BOOL, nullptr, F("wWTempOK"), F("temperature ok"));
    register_device_value(TAG_BOILER_DATA_WW, &wWActive_, DeviceValueType::BOOL, nullptr, F("wWActive"), F("active"));
    register_device_value(TAG_BOILER_DATA_WW, &wWHeat_, DeviceValueType::BOOL, nullptr, F("wWHeat"), F("heating"));
    register_device_value(TAG_BOILER_DATA_WW, &wWSetPumpPower_, DeviceValueType::UINT, nullptr, F("wWSetPumpPower"), F("pump set power"), DeviceValueUOM::PERCENT);
    register_device_value(TAG_BOILER_DATA_WW, &mixerTemp_, DeviceValueType::USHORT, FL_(div10), F("mixerTemp"), F("mixer temperature"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &tankMiddleTemp_, DeviceValueType::USHORT, FL_(div10), F("tankMiddleTemp"), F("tank middle temperature (TS3)"), DeviceValueUOM::DEGREES);
    register_device_value(TAG_BOILER_DATA_WW, &wWStarts_, DeviceValueType::ULONG, nullptr, F("wWStarts"), F("# starts"));
    register_device_value(TAG_BOILER_DATA_WW, &wWStarts2_, DeviceValueType::ULONG, nullptr, F("wWStarts2"), F("# control starts"));
    register_device_value(TAG_BOILER_DATA_WW, &wWWorkM_, DeviceValueType::TIME, nullptr, F("wWWorkM"), F("active time"), DeviceValueUOM::MINUTES);

    // info - boiler_data_info topic
    register_device_value(TAG_BOILER_DATA_INFO, &upTimeControl_, DeviceValueType::TIME, FL_(div60), F("upTimeControl"), F("operating time total heat"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA_INFO, &upTimeCompHeating_, DeviceValueType::TIME, FL_(div60), F("upTimeCompHeating"), F("operating time compressor heating"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA_INFO, &upTimeCompCooling_, DeviceValueType::TIME, FL_(div60), F("upTimeCompCooling"), F("operating time compressor cooling"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA_INFO, &upTimeCompWw_, DeviceValueType::TIME, FL_(div60), F("upTimeCompWw"), F("operating time compressor warm water"), DeviceValueUOM::MINUTES);
    register_device_value(TAG_BOILER_DATA_INFO, &heatingStarts_, DeviceValueType::ULONG, nullptr, F("heatingStarts"), F("# heating starts (control)"));
    register_device_value(TAG_BOILER_DATA_INFO, &coolingStarts_, DeviceValueType::ULONG, nullptr, F("coolingStarts"), F("# cooling starts (control)"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgConsTotal_, DeviceValueType::ULONG, nullptr, F("nrgConsTotal"), F("total energy consumption"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgConsCompTotal_, DeviceValueType::ULONG, nullptr, F("nrgConsCompTotal"), F("energy consumption compressor total"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgConsCompHeating_, DeviceValueType::ULONG, nullptr, F("nrgConsCompHeating"), F("energy consumption compressor heating"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgConsCompWw_, DeviceValueType::ULONG, nullptr, F("nrgConsCompWw"), F("energy consumption compressor warm water"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgConsCompCooling_, DeviceValueType::ULONG, nullptr, F("nrgConsCompCooling"), F("energy consumption compressor cooling"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgSuppTotal_, DeviceValueType::ULONG, nullptr, F("nrgSuppTotal"), F("total energy supplied"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgSuppHeating_, DeviceValueType::ULONG, nullptr, F("nrgSuppHeating"), F("total energy supplied heating"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgSuppWw_, DeviceValueType::ULONG, nullptr, F("nrgSuppWw"), F("total energy warm supplied warm water"));
    register_device_value(TAG_BOILER_DATA_INFO, &nrgSuppCooling_, DeviceValueType::ULONG, nullptr, F("nrgSuppCooling"), F("total energy supplied cooling"));
    register_device_value(TAG_BOILER_DATA_INFO, &auxElecHeatNrgConsTotal_, DeviceValueType::ULONG, nullptr, F("auxElecHeatNrgConsTotal"), F("auxiliary electrical heater energy consumption total"));
    register_device_value(TAG_BOILER_DATA_INFO, &auxElecHeatNrgConsHeating_, DeviceValueType::ULONG, nullptr, F("auxElecHeatNrgConsHeating"), F("auxiliary electrical heater energy consumption heating"));
    register_device_value(TAG_BOILER_DATA_INFO, &auxElecHeatNrgConsDHW_, DeviceValueType::ULONG, nullptr, F("auxElecHeatNrgConsDHW"), F("auxiliary electrical heater energy consumption DHW"));
    register_device_value(TAG_BOILER_DATA_INFO, &maintenanceMessage_, DeviceValueType::TEXT, nullptr, F("maintenanceMessage"), F("maintenance message"));
    register_device_value(TAG_BOILER_DATA_INFO, &maintenanceDate_, DeviceValueType::TEXT, nullptr, F("maintenanceDate"), F("maintenance set date"));
    register_device_value(TAG_BOILER_DATA_INFO, &maintenanceType_, DeviceValueType::ENUM, FL_(enum_off_time_date), F("maintenanceType"), F("scheduled maintenance"));
    register_device_value(TAG_BOILER_DATA_INFO, &maintenanceTime_, DeviceValueType::USHORT, nullptr, F("maintenanceTime"), F("maintenance set time"), DeviceValueUOM::HOURS);
}

// publish HA config
bool Boiler::publish_ha_config() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(boiler);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/boiler_data"), Mqtt::base().c_str());
    doc["stat_t"] = stat_t;

    doc["name"]    = FJSON("ID");
    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Boiler");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-boiler");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/%s/boiler/config"), Mqtt::base().c_str());
    Mqtt::publish_ha(topic,
                     doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// Check if hot tap water or heating is active
// Values will always be posted first time as heatingActive_ and tapwaterActive_ will have values EMS_VALUE_BOOL_NOTSET
void Boiler::check_active(const bool force) {
    if (!Helpers::hasValue(boilerState_)) {
        return;
    }

    bool    b;
    uint8_t val;

    // check if heating is active, bits 2 and 4 must be set
    b   = ((boilerState_ & 0x09) == 0x09);
    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (heatingActive_ != val || force) {
        heatingActive_ = val;
        char s[7];
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s, b));
    }

    // check if tap water is active, bits 1 and 4 must be set
    // also check if there is a flowsensor and flow-type
    static bool flowsensor = false;
    if (Helpers::hasValue(wWCurFlow_) && (wWCurFlow_ > 0) && (wWType_ == 1)) {
        flowsensor = true;
    }
    if (flowsensor) {
        b = ((wWCurFlow_ > 0) && ((boilerState_ & 0x0A) == 0x0A));
    } else {
        b = ((boilerState_ & 0x0A) == 0x0A);
    }

    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (tapwaterActive_ != val || force) {
        tapwaterActive_ = val;
        char s[7];
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, b));
        EMSESP::tap_water_active(b); // let EMS-ESP know, used in the Shower class
    }
}

// 0x33
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wWActivated_, 1)); // 0xFF means on
    has_update(telegram->read_value(wWCircPump_, 6));  // 0xFF means on
    has_update(telegram->read_value(wWCircPumpMode_,
                                    7)); // 1=1x3min... 6=6x3min, 7=continuous
    has_update(telegram->read_value(wWChargeType_,
                                    10)); // 0 = charge pump, 0xff = 3-way valve
    has_update(telegram->read_value(wWSelTemp_, 2));
    has_update(telegram->read_value(wWDisinfectionTemp_, 8));

    telegram->read_value(wWComfort_, 9);
    if (wWComfort_ == 0x00) {
        wWComfort_ = 0; // Hot
    } else if (wWComfort_ == 0xD8) {
        wWComfort_ = 1; // Eco
    } else if (wWComfort_ == 0xEC) {
        wWComfort_ = 2; // Intelligent
    } else {
        wWComfort_ = EMS_VALUE_UINT_NOTSET;
    }
}

// 0x18
void Boiler::process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(selFlowTemp_, 0));
    has_update(telegram->read_value(curFlowTemp_, 1));
    has_update(telegram->read_value(selBurnPow_, 3)); // burn power max setting
    has_update(telegram->read_value(curBurnPow_, 4));
    has_update(telegram->read_value(boilerState_, 5));

    has_update(telegram->read_bitvalue(burnGas_, 7, 0));
    has_update(telegram->read_bitvalue(fanWork_, 7, 2));
    has_update(telegram->read_bitvalue(ignWork_, 7, 3));
    has_update(telegram->read_bitvalue(heatingPump_, 7, 5));
    has_update(telegram->read_bitvalue(wWHeat_, 7, 6));
    has_update(telegram->read_bitvalue(wWCirc_, 7, 7));

    // warm water storage sensors (if present)
    // wWStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/proddy/EMS-ESP/issues/206
    has_update(telegram->read_value(wWStorageTemp1_, 9)); // 0x8300 if not available
    has_update(telegram->read_value(wWStorageTemp2_,
                                    11)); // 0x8000 if not available - this is boiler temp

    has_update(telegram->read_value(retTemp_, 13));
    has_update(telegram->read_value(flameCurr_, 15));

    // system pressure. FF means missing
    has_update(telegram->read_value(sysPress_, 17)); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        serviceCode_[0] = (serviceCode_[0] == '~') ? 0xF0 : serviceCode_[0];
        has_update(telegram->read_value(serviceCode_[0], 18));
        serviceCode_[0] = (serviceCode_[0] == (char)0xF0) ? '~' : serviceCode_[0];
        has_update(telegram->read_value(serviceCode_[1], 19));
        serviceCode_[2] = '\0'; // null terminate string
    }

    has_update(telegram->read_value(serviceCodeNumber_, 20));

    check_active(); // do a quick check to see if the hot water or heating is active
}

/*
 * UBATotalUptime - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void Boiler::process_UBATotalUptime(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(UBAuptime_, 0, 3)); // force to 3 bytes
}

/*
 * UBAParameters - type 0x16
 */
void Boiler::process_UBAParameters(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(heatingActivated_, 0));
    has_update(telegram->read_value(heatingTemp_, 1));
    has_update(telegram->read_value(burnMaxPower_, 2));
    has_update(telegram->read_value(burnMinPower_, 3));
    has_update(telegram->read_value(boilHystOff_, 4));
    has_update(telegram->read_value(boilHystOn_, 5));
    has_update(telegram->read_value(burnMinPeriod_, 6));
    has_update(telegram->read_value(pumpDelay_, 8));
    has_update(telegram->read_value(pumpModMax_, 9));
    has_update(telegram->read_value(pumpModMin_, 10));
}

/*
 * UBAMonitorWW - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wWSetTemp_, 0));
    has_update(telegram->read_value(wWCurTemp_, 1));
    has_update(telegram->read_value(wWCurTemp2_, 3));
    has_update(telegram->read_value(wWCurFlow_, 9));
    has_update(telegram->read_value(wWType_, 8));

    has_update(telegram->read_value(wWWorkM_, 10, 3));  // force to 3 bytes
    has_update(telegram->read_value(wWStarts_, 13, 3)); // force to 3 bytes

    has_update(telegram->read_bitvalue(wWOneTime_, 5, 1));
    has_update(telegram->read_bitvalue(wWDisinfecting_, 5, 2));
    has_update(telegram->read_bitvalue(wWCharging_, 5, 3));
    has_update(telegram->read_bitvalue(wWRecharging_, 5, 4));
    has_update(telegram->read_bitvalue(wWTempOK_, 5, 5));
    has_update(telegram->read_bitvalue(wWActive_, 5, 6));
}

/*
 * UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
 * temperatures at 7 and 23 always identical
+ * Bosch Logamax Plus GB122: issue #620
+ * 88 00 E4 00 00 2D 2D 00 00 C9 34 02 21 64 3D 05 02 01 DE 00 00 00 00 03 62 14 00 02 21 00 00 00 00 00 00 00 2B 2B 83
+ * GB125/Logamatic MC110: issue #650: add retTemp & sysPress
+ * 08 00 E4 00 10 20 2D 48 00 C8 38 02 37 3C 27 03 00 00 00 00 00 01 7B 01 8F 11 00 02 37 80 00 02 1B 80 00 7F FF 80 00
 */
void Boiler::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(selFlowTemp_, 6));
    has_update(telegram->read_bitvalue(burnGas_, 11, 0));
    // has_update(telegram->read_bitvalue(heatingPump_, 11, 1)); // heating active? see SlowPlus
    has_update(telegram->read_bitvalue(wWHeat_, 11, 2));
    has_update(telegram->read_value(curBurnPow_, 10));
    has_update(telegram->read_value(selBurnPow_, 9));
    has_update(telegram->read_value(curFlowTemp_, 7));
    has_update(telegram->read_value(flameCurr_, 19));
    has_update(telegram->read_value(retTemp_,
                                    17)); // can be 0 if no sensor, handled in export_values
    has_update(telegram->read_value(sysPress_, 21));

    //has_update(telegram->read_value(temperatur_, 13)); // unknown temperature
    //has_update(telegram->read_value(temperatur_, 27)); // unknown temperature

    // read 3 char service code / installation status as appears on the display
    if ((telegram->message_length > 3) && (telegram->offset == 0)) {
        serviceCode_[0] = (serviceCode_[0] == '~') ? 0xF0 : serviceCode_[0];
        has_update(telegram->read_value(serviceCode_[0], 1));
        serviceCode_[0] = (serviceCode_[0] == (char)0xF0) ? '~' : serviceCode_[0];
        has_update(telegram->read_value(serviceCode_[1], 2));
        has_update(telegram->read_value(serviceCode_[2], 3));
        serviceCode_[3] = '\0';
    }
    has_update(telegram->read_value(serviceCodeNumber_, 4));

    // at this point do a quick check to see if the hot water or heating is active
    uint8_t state = EMS_VALUE_UINT_NOTSET;
    if (telegram->read_value(state, 11)) {
        boilerState_ = state & 0x01 ? 0x08 : 0;
        boilerState_ |= state & 0x02 ? 0x01 : 0;
        boilerState_ |= state & 0x04 ? 0x02 : 0;
    }

    check_active(); // do a quick check to see if the hot water or heating is active
}

/*
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 * e.g. 08 00 19 00 80 00 02 41 80 00 00 00 00 00 03 91 7B 05 B8 40 00 00 00 04 92 AD 00 5E EE 80 00
 *      08 0B 19 00 FF EA 02 47 80 00 00 00 00 62 03 CA 24 2C D6 23 00 00 00 27 4A B6 03 6E 43 
 *                  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 17 19 20 21 22 23 24
 */
void Boiler::process_UBAMonitorSlow(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(outdoorTemp_, 0));
    has_update(telegram->read_value(boilTemp_, 2));
    has_update(telegram->read_value(exhaustTemp_, 4));
    has_update(telegram->read_value(switchTemp_,
                                    25)); // only if there is a mixer module present
    has_update(telegram->read_value(heatingPumpMod_, 9));
    has_update(telegram->read_value(burnStarts_, 10, 3));  // force to 3 bytes
    has_update(telegram->read_value(burnWorkMin_, 13, 3)); // force to 3 bytes
    has_update(telegram->read_value(heatWorkMin_, 19, 3)); // force to 3 bytes
}

/*
 * UBAMonitorSlowPlus2 - type 0xE3
 * 88 00 E3 00 04 00 00 00 00 01 00 00 00 00 00 02 22 2B 64 46 01 00 00 61
 */
void Boiler::process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(heatingPump2Mod_, 13)); // Heat Pump Modulation
}

/*
 * UBAMonitorSlowPlus - type 0xE5 - central heating monitor EMS+
 * Boiler(0x08) -> Me(0x0B), UBAMonitorSlowPlus(0xE5),
 * data: 01 00 20 00 00 78 00 00 00 00 00 1E EB 00 9D 3E 00 00 00 00 6B 5E 00 06 4C 64 00 00 00 00 8A A3
 */
void Boiler::process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_bitvalue(fanWork_, 2, 2));
    has_update(telegram->read_bitvalue(ignWork_, 2, 3));
    has_update(telegram->read_bitvalue(heatingPump_, 2, 5));
    has_update(telegram->read_bitvalue(wWCirc_, 2, 7));
    has_update(telegram->read_value(exhaustTemp_, 6));
    has_update(telegram->read_value(burnStarts_, 10, 3));  // force to 3 bytes
    has_update(telegram->read_value(burnWorkMin_, 13, 3)); // force to 3 bytes
    has_update(telegram->read_value(heatWorkMin_, 19, 3)); // force to 3 bytes
    has_update(telegram->read_value(heatingPumpMod_, 25));
    // temperature measurements at 4, see #620
}

/*
 * UBAParametersPlus - type 0xe6
 * 88 0B E6 00 01 46 00 00 46 0A 00 01 06 FA 0A 01 02 64 01 00 00 1E 00 3C 01 00 00 00 01 00 9A
 */
void Boiler::process_UBAParametersPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(heatingActivated_, 0));
    has_update(telegram->read_value(heatingTemp_, 1));
    has_update(telegram->read_value(burnMaxPower_, 4));
    has_update(telegram->read_value(burnMinPower_, 5));
    has_update(telegram->read_value(boilHystOff_, 8));
    has_update(telegram->read_value(boilHystOn_, 9));
    has_update(telegram->read_value(burnMinPeriod_, 10));
    // changed_ |= telegram->read_value(pumpModMax_, 13); // guess
    // changed_ |= telegram->read_value(pumpModMin_, 14); // guess
}

// 0xEA
void Boiler::process_UBAParameterWWPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wWActivated_, 5));     // 0x01 means on
    has_update(telegram->read_value(wWCircPump_, 10));     // 0x01 means yes
    has_update(telegram->read_value(wWCircPumpMode_, 11)); // 1=1x3min... 6=6x3min, 7=continuous
    // has_update(telegram->read_value(wWDisinfectTemp_, 12)); // settings, status in E9
    // has_update(telegram->read_value(wWSelTemp_, 6));        // settings, status in E9
}

// 0xE9 - DHW Status
// e.g. 08 00 E9 00 37 01 F6 01 ED 00 00 00 00 41 3C 00 00 00 00 00 00 00 00 00 00 00 00 37 00 00 00 (CRC=77) #data=27
void Boiler::process_UBADHWStatus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wWSetTemp_, 0));
    has_update(telegram->read_value(wWCurTemp_, 1));
    has_update(telegram->read_value(wWCurTemp2_, 3));

    has_update(telegram->read_value(wWWorkM_, 14, 3));  // force to 3 bytes
    has_update(telegram->read_value(wWStarts_, 17, 3)); // force to 3 bytes

    has_update(telegram->read_bitvalue(wWOneTime_, 12, 2));
    has_update(telegram->read_bitvalue(wWDisinfecting_, 12, 3));
    has_update(telegram->read_bitvalue(wWCharging_, 12, 4));
    has_update(telegram->read_bitvalue(wWRecharging_, 13, 4));
    has_update(telegram->read_bitvalue(wWTempOK_, 13, 5));
    has_update(telegram->read_bitvalue(wWCirc_, 13, 2));

    // has_update(telegram->read_value(wWActivated_, 20)); // Activated is in 0xEA, this is something other 0/100%
    has_update(telegram->read_value(wWSelTemp_, 10));
    has_update(telegram->read_value(wWDisinfectionTemp_, 9));
}

/*
 * UBAInformation - type 0x495
 * all values 32 bit
 * 08 0B FF 00 03 95 01 01 AB 83 00 27 78 EB 00 84 FA 39 FF FF FF 00 00 53 7D 8D 00 00 0F 04 1C
 * 08 00 FF 00 03 95 01 01 AB 83 00 27 78 EB 00 84 FA 39 FF FF FF 00 00 53 7D 8D 00 00 0F 04 63
 * 08 00 FF 18 03 95 00 00 05 84 00 00 07 22 FF FF FF FF 00 00 02 5C 00 00 03 C0 00 00 01 98 64
 * 08 00 FF 30 03 95 00 00 00 D4 FF FF FF FF 00 00 1C 70 FF FF FF FF 00 00 20 30 00 00 0E 06 FB
 * 08 00 FF 48 03 95 00 00 06 C0 00 00 07 66 FF FF FF FF 2E
 */
void Boiler::process_UBAInformation(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(upTimeControl_, 0));
    has_update(telegram->read_value(upTimeCompHeating_, 8));
    has_update(telegram->read_value(upTimeCompCooling_, 16));
    has_update(telegram->read_value(upTimeCompWw_, 4));

    has_update(telegram->read_value(heatingStarts_, 28));
    has_update(telegram->read_value(coolingStarts_, 36));
    has_update(telegram->read_value(wWStarts2_, 24));

    has_update(telegram->read_value(nrgConsTotal_, 64));

    has_update(telegram->read_value(auxElecHeatNrgConsTotal_, 40));
    has_update(telegram->read_value(auxElecHeatNrgConsHeating_, 48));
    has_update(telegram->read_value(auxElecHeatNrgConsDHW_, 44));

    has_update(telegram->read_value(nrgConsCompTotal_, 56));
    has_update(telegram->read_value(nrgConsCompHeating_, 68));
    has_update(telegram->read_value(nrgConsCompWw_, 72));
    has_update(telegram->read_value(nrgConsCompCooling_, 76));
}

/*
 * UBAEnergy - type 0x494
 * Energy-values all 32bit
 * 08 00 FF 00 03 94 03 31 21 59 00 00 7C 70 00 00 15 B8 00 00 40 E3 00 00 27 23 FF FF FF FF EA
 * 08 00 FF 18 03 94 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 00 7E
 * 08 00 FF 31 03 94 00 00 00 00 00 00 00 38
 */
void Boiler::process_UBAEnergySupplied(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(nrgSuppTotal_, 4));
    has_update(telegram->read_value(nrgSuppHeating_, 12));
    has_update(telegram->read_value(nrgSuppWw_, 8));
    has_update(telegram->read_value(nrgSuppCooling_, 16));
}

// 0x2A - MC10Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/proddy/EMS-ESP/issues/397
void Boiler::process_MC10Status(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(mixerTemp_, 14));
    has_update(telegram->read_value(tankMiddleTemp_, 18));
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(outdoorTemp_, 0));
}

// UBASetPoint 0x1A
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(setFlowTemp_,
                                    0));                  // boiler set temp from thermostat
    has_update(telegram->read_value(setBurnPow_, 1));     // max json power in %
    has_update(telegram->read_value(wWSetPumpPower_, 2)); // ww pump speed/power?
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// 0x35 - not yet implemented
void Boiler::process_UBAFlags(std::shared_ptr<const Telegram> telegram) {
}
#pragma GCC diagnostic pop

// 0x1C
// 08 00 1C 94 0B 0A 1D 31 08 00 80 00 00 00 -> message for 29.11.2020
// 08 00 1C 94 0B 0A 1D 31 00 00 00 00 00 00 -> message reset
void Boiler::process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram) {
    // 5. byte: Maintenance due (0 = no, 3 = yes, due to operating hours, 8 = yes, due to date)
    uint8_t message_code = maintenanceMessage_[2] - '0';
    has_update(telegram->read_value(message_code, 5));

    // ignore if 0, which means all is ok
    if (Helpers::hasValue(message_code) && message_code > 0) {
        snprintf_P(maintenanceMessage_, sizeof(maintenanceMessage_), PSTR("H%02d"), message_code);
    }
}

// 0x10, 0x11
void Boiler::process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 9) {
        return;
    }
    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
    if (telegram->message_data[4] & 0x80) { // valid date

        static uint32_t lastCodeDate_ = 0; // last code date
        char            code[3];
        uint16_t        codeNo;
        code[0] = telegram->message_data[0];
        code[1] = telegram->message_data[1];
        code[2] = 0;
        telegram->read_value(codeNo, 2);
        uint16_t year  = (telegram->message_data[4] & 0x7F) + 2000;
        uint8_t  month = telegram->message_data[5];
        uint8_t  day   = telegram->message_data[7];
        uint8_t  hour  = telegram->message_data[6];
        uint8_t  min   = telegram->message_data[8];
        uint32_t date  = (year - 2000) * 535680UL + month * 44640UL + day * 1440UL + hour * 60 + min;
        // store only the newest code from telegrams 10 and 11
        if (date > lastCodeDate_) {
            snprintf_P(lastCode_, sizeof(lastCode_), PSTR("%s(%d) %02d.%02d.%d %02d:%02d"), code, codeNo, day, month, year, hour, min);
            lastCodeDate_ = date;
        }
    }
}

// 0x15
void Boiler::process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }
    // first byte: Maintenance messages (0 = none, 1 = by operating hours, 2 = by date)

    has_update(telegram->read_value(maintenanceType_, 0));

    uint8_t time = (maintenanceTime_ == EMS_VALUE_USHORT_NOTSET) ? EMS_VALUE_UINT_NOTSET : maintenanceTime_ / 100;
    has_update(telegram->read_value(time, 1));
    maintenanceTime_ = (time == EMS_VALUE_UINT_NOTSET) ? EMS_VALUE_USHORT_NOTSET : time * 100;
    // telegram->read_value(maintenanceTime_, 1, 1);
    // maintenanceTime_ = maintenanceTime * 100;

    // date only
    uint8_t day   = telegram->message_data[2];
    uint8_t month = telegram->message_data[3];
    uint8_t year  = telegram->message_data[4];
    if (day > 0 && month > 0) {
        snprintf_P(maintenanceDate_, sizeof(maintenanceDate_), PSTR("%02d.%02d.%04d"), day, month, year + 2000);
    }
}

// Set the warm water temperature 0x33
bool Boiler::set_warmwater_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler warm water temperature: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler warm water temperature to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 6, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAFlags, 3, v, 0x34); // for i9000, see #397
        write_command(EMS_TYPE_UBAParameterWW, 2, v,
                      EMS_TYPE_UBAParameterWW);       // read seltemp back
    }

    return true;
}

// flow temp
bool Boiler::set_flow_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler flow temperature: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler flow temperature to %d C"), v);
    // some boiler have it in 0x1A, some in 0x35, but both telegrams are sometimes writeonly
    write_command(EMS_TYPE_UBASetPoints, 0, v, EMS_TYPE_UBASetPoints);
    // write_command(0x35, 3, v, 0x35);


    return true;
}

// set min boiler output
bool Boiler::set_heating_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set boiler heating: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler heating %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 0, v ? 0x01 : 0, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 0, v ? 0xFF : 0, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set heating maximum temperature
bool Boiler::set_heating_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler heating temperature: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler heating temperature to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 1, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 1, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min boiler output
bool Boiler::set_min_power(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler min power: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler min power to %d %%"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 5, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 3, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set max temp
bool Boiler::set_max_power(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler max power: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler max power to %d %%"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 4, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 2, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min pump modulation
bool Boiler::set_min_pump(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set pump min: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting pump min to %d %%"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 14, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 10, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set max pump modulation
bool Boiler::set_max_pump(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set pump max: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting pump max to %d %%"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 13, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 9, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set boiler on hysteresis
bool Boiler::set_hyst_on(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler hysteresis: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler hysteresis on to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 9, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 5, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set boiler off hysteresis
bool Boiler::set_hyst_off(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler hysteresis: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler hysteresis off to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 8, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 4, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min burner period
bool Boiler::set_burn_period(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set burner min. period: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting burner min. period to %d min"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 10, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 6, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set pump delay
bool Boiler::set_pump_delay(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set boiler pump delay: Invalid value"));
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameters)) {
        LOG_INFO(F("Setting boiler pump delay to %d min"), v);
        write_command(EMS_TYPE_UBAParameters, 8, v, EMS_TYPE_UBAParameters);
        return true;
    }

    return false;
}

// note some boilers do not have this setting, than it's done by thermostat
// on a RC35 it's by EMSESP::send_write_request(0x37, 0x10, 2, &set, 1, 0); (set is 1,2,3) 1=hot, 2=eco, 3=intelligent
bool Boiler::set_warmwater_mode(const char * value, const int8_t id) {
    uint8_t set;
    if (!Helpers::value2enum(value, set, {F("hot"), F("eco"), F("intelligent")})) {
        LOG_WARNING(F("Set boiler warm water mode: Invalid value"));
        return false;
    }

    if (!get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        return false;
    }

    if (set == 0) {
        LOG_INFO(F("Setting boiler warm water to Hot"));
    } else if (set == 1) {
        LOG_INFO(F("Setting boiler warm water to Eco"));
        set = 0xD8;
    } else if (set == 2) {
        LOG_INFO(F("Setting boiler warm water to Intelligent"));
        set = 0xEC;
    } else {
        return false; // do nothing
    }

    write_command(EMS_TYPE_UBAParameterWW, 9, set, EMS_TYPE_UBAParameterWW);
    return true;
}

// turn on/off warm water
bool Boiler::set_warmwater_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set boiler warm water active: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting boiler warm water active %s"), v ? "on" : "off");

    // https://github.com/proddy/EMS-ESP/issues/268
    uint8_t n;
    if (EMSbus::is_ht3()) {
        n = (v ? 0x08 : 0x00); // 0x08 is on, 0x00 is off
    } else {
        n = (v ? 0xFF : 0x00); // 0xFF is on, 0x00 is off
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 1, v ? 1 : 0, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 1, n, 0x34);
    }

    return true;
}

// Activate / De-activate the Warm Tap Water
// Note: Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
bool Boiler::set_tapwarmwater_activated(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set warm tap water: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting warm tap water %s"), v ? "on" : "off");
    uint8_t message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];
    for (uint8_t i = 0; i < sizeof(message_data); i++) {
        message_data[i] = 0x00;
    }

    // we use the special test mode 0x1D for this. Setting the first data to 5A puts the system into test mode and
    // a setting of 0x00 puts it back into normal operating mode
    // when in test mode we're able to mess around with the 3-way valve settings
    if (!v) {
        // on
        message_data[0] = 0x5A; // test mode on
        message_data[1] = 0x00; // burner output 0%
        message_data[3] = 0x64; // boiler pump capacity 100%
        message_data[4] = 0xFF; // 3-way valve hot water only
    } else {
        // get out of test mode. Send all zeros.
        // telegram: 0B 08 1D 00 00
    }

    write_command(EMS_TYPE_UBAFunctionTest, 0, message_data, sizeof(message_data), 0);

    return true;
}

// Activate / De-activate One Time warm water 0x35
// true = on, false = off
// See also https://github.com/proddy/EMS-ESP/issues/341#issuecomment-596245458 for Junkers
bool Boiler::set_warmwater_onetime(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set warm water OneTime loading: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting warm water OneTime loading %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02),
                      0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02), 0x34);
    }

    return true;
}

// Activate / De-activate circulation of warm water 0x35
// true = on, false = off
bool Boiler::set_warmwater_circulation(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set warm water circulation: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting warm water circulation %s"), v ? "on" : "off");
    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02),
                      0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0x34);
    }

    return true;
}

// configuration of warm water circulation pump
bool Boiler::set_warmwater_circulation_pump(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        LOG_WARNING(F("Set warm water circulation pump: Invalid value"));
        return false;
    }

    LOG_INFO(F("Setting warm water circulation pump %s"), v ? "on" : "off");

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 10, v ? 0x01 : 0x00, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 6, v ? 0xFF : 0x00, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// Set the mode of circulation, 1x3min, ... 6x3min, continuos
// true = on, false = off
bool Boiler::set_warmwater_circulation_mode(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        LOG_WARNING(F("Set warm water circulation mode: Invalid value"));
        return false;
    }

    if (v < 7) {
        LOG_INFO(F("Setting warm water circulation mode %dx3min"), v);
    } else if (v == 7) {
        LOG_INFO(F("Setting warm water circulation mode continuos"));
    } else {
        LOG_WARNING(F("Set warm water circulation mode: Invalid value"));
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 11, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 7, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}
// Reset command
// 0 & 1        Reset-Mode (Manuel, others)
// 8            reset maintenance message Hxx
// 12 & 13      Reset that Error-memory
bool Boiler::set_reset(const char * value, const int8_t id) {
    std::string s(12, '\0');
    if (!Helpers::value2string(value, s)) {
        return false;
    }

    if (s == "maintenance") {
        LOG_INFO(F("Reset boiler maintenance message"));
        write_command(0x05, 0x08, 0xFF, 0x1C);
        return true;
    } else if (s == "error") {
        LOG_INFO(F("Reset boiler error message"));
        write_command(0x05, 0x00, 0x5A); // error reset
        return true;
    }
    return false;
}

//maintenance
bool Boiler::set_maintenance(const char * value, const int8_t id) {
    std::string s(12, '\0');
    if (Helpers::value2string(value, s)) {
        if (s == "reset") {
            LOG_INFO(F("Reset boiler maintenance message"));
            write_command(0x05, 0x08, 0xFF, 0x1C);
            return true;
        }
    }

    if (strlen(value) == 10) { // date
        uint8_t day   = (value[0] - '0') * 10 + (value[1] - '0');
        uint8_t month = (value[3] - '0') * 10 + (value[4] - '0');
        uint8_t year  = (uint8_t)(Helpers::atoint(&value[6]) - 2000);
        if (day > 0 && day < 32 && month > 0 && month < 13) {
            LOG_INFO(F("Setting maintenance date to %02d.%02d.%04d"), day, month, year + 2000);
            uint8_t data[5] = {2, (uint8_t)(maintenanceTime_ / 100), day, month, year};
            write_command(0x15, 0, data, 5, 0x15);
        } else {
            LOG_WARNING(F("Setting maintenance: wrong format %d.%d.%d"), day, month, year + 2000);
            return false;
        }
        return true;
    }

    int hrs;
    if (Helpers::value2number(value, hrs)) {
        if (hrs > 99 && hrs < 25600) {
            LOG_INFO(F("Setting maintenance time %d hours"), hrs);
            uint8_t data[2] = {1, (uint8_t)(hrs / 100)};
            write_command(0x15, 0, data, 2, 0x15);
            return true;
        }
    }

    uint8_t num;
    if (Helpers::value2enum(value, num, {F("off"), F("time"), F("date")})) {
        LOG_INFO(F("Setting maintenance type to %s"), value);
        write_command(0x15, 0, num, 0x15);
        return true;
    }

    LOG_WARNING(F("Setting maintenance: wrong format"));
    return false;
}

} // namespace emsesp
