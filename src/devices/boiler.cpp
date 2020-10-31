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
    this->reserve_mem(20); // reserve some space for the telegram registries, to avoid memory fragmentation

    LOG_DEBUG(F("Adding new Boiler with device ID 0x%02X"), device_id);

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

    // MQTT commands for boiler topic
    register_mqtt_cmd(F("comfort"), [&](const char * value, const int8_t id) { return set_warmwater_mode(value, id); });
    register_mqtt_cmd(F("wwactivated"), [&](const char * value, const int8_t id) { return set_warmwater_activated(value, id); });
    register_mqtt_cmd(F("wwtapactivated"), [&](const char * value, const int8_t id) { return set_tapwarmwater_activated(value, id); });
    register_mqtt_cmd(F("wwonetime"), [&](const char * value, const int8_t id) { return set_warmwater_onetime(value, id); });
    register_mqtt_cmd(F("wwcircpump"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_pump(value, id); });
    register_mqtt_cmd(F("wwcirculation"), [&](const char * value, const int8_t id) { return set_warmwater_circulation(value, id); });
    register_mqtt_cmd(F("wwcircmode"), [&](const char * value, const int8_t id) { return set_warmwater_circulation_mode(value, id); });
    register_mqtt_cmd(F("flowtemp"), [&](const char * value, const int8_t id) { return set_flow_temp(value, id); });
    register_mqtt_cmd(F("wwtemp"), [&](const char * value, const int8_t id) { return set_warmwater_temp(value, id); });
    register_mqtt_cmd(F("heatingactivated"), [&](const char * value, const int8_t id) { return set_heating_activated(value, id); });
    register_mqtt_cmd(F("heatingtemp"), [&](const char * value, const int8_t id) { return set_heating_temp(value, id); });
    register_mqtt_cmd(F("burnmaxpower"), [&](const char * value, const int8_t id) { return set_max_power(value, id); });
    register_mqtt_cmd(F("burnminpower"), [&](const char * value, const int8_t id) { return set_min_power(value, id); });
    register_mqtt_cmd(F("boilhyston"), [&](const char * value, const int8_t id) { return set_hyst_on(value, id); });
    register_mqtt_cmd(F("boilhystoff"), [&](const char * value, const int8_t id) { return set_hyst_off(value, id); });
    register_mqtt_cmd(F("burnperiod"), [&](const char * value, const int8_t id) { return set_burn_period(value, id); });
    register_mqtt_cmd(F("pumpdelay"), [&](const char * value, const int8_t id) { return set_pump_delay(value, id); });
}

// create the config topics for Home Assistant MQTT Discovery
// for each of the main elements
void Boiler::register_mqtt_ha_config(bool force) {
    if ((mqtt_ha_config_ && !force)) {
        return;
    }

    if (!Mqtt::connected()) {
        return;
    }

    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    doc["name"]    = F("Service Code");
    doc["uniq_id"] = F("boiler");
    doc["ic"]      = F("mdi:home-thermometer-outline");

    char stat_t[50];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/boiler_data"), System::hostname().c_str());
    doc["stat_t"] = stat_t;

    doc["val_tpl"] = F("{{value_json.serviceCode}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Boiler");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-boiler");
    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/boiler/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag

    Mqtt::register_mqtt_ha_binary_sensor(F_(tapwaterActive), this->device_type(), "tapwater_active");
    Mqtt::register_mqtt_ha_binary_sensor(F_(heatingActive), this->device_type(), "heating_active");

    // main
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(serviceCodeNumber), this->device_type(), "serviceCodeNumber", nullptr, F_(iconpower));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(selFlowTemp), this->device_type(), "selFlowTemp", F_(degrees), F_(iconcruise));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(selBurnPow), this->device_type(), "selBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(curBurnPow), this->device_type(), "curBurnPow", F_(percent), F_(iconfire));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpMod), this->device_type(), "pumpMod", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpMod2), this->device_type(), "pumpMod2", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(outdoorTemp), this->device_type(), "outdoorTemp", F_(degrees), F_(iconexport));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(curFlowTemp), this->device_type(), "curFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(retTemp), this->device_type(), "retTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(switchTemp), this->device_type(), "switchTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(sysPress), this->device_type(), "sysPress", F_(bar), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(boilTemp), this->device_type(), "boilTemp", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnGas), this->device_type(), "burnGas", nullptr, F_(iconfire));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(flameCurr), this->device_type(), "flameCurr", F_(uA), F_(iconflash));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(heatPump), this->device_type(), "heatPump", nullptr, F_(iconwaterpump));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(fanWork), this->device_type(), "fanWork", nullptr, F_(iconfan));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(ignWork), this->device_type(), "ignWork", nullptr, F_(iconflash));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(exhaustTemp), this->device_type(), "exhaustTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(heatingActivated), this->device_type(), "heatingActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(heatingTemp), this->device_type(), "heatingTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpModMax), this->device_type(), "pumpModMax", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpModMin), this->device_type(), "pumpModMin", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(pumpDelay), this->device_type(), "pumpDelay", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnMinPeriod), this->device_type(), "burnMinPeriod", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnMinPower), this->device_type(), "burnMinPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnMaxPower), this->device_type(), "burnMaxPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(boilHystOn), this->device_type(), "boilHystOn", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(boilHystOff), this->device_type(), "boilHystOff", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(setFlowTemp), this->device_type(), "setFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(setBurnPow), this->device_type(), "setBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnStarts), this->device_type(), "burnStarts", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(burnWorkMin), this->device_type(), "burnWorkMin", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(heatWorkMin), this->device_type(), "heatWorkMin", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, nullptr, F_(UBAuptime), this->device_type(), "UBAuptime", F_(min), nullptr);

    // ww
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWSelTemp), this->device_type(), "wWSelTemp", F_(degrees), F_(iconcruise));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWSetTemp), this->device_type(), "wWSetTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWDisinfectionTemp), this->device_type(), "wWDisinfectionTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWType), this->device_type(), "wWType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWChargeType), this->device_type(), "wWChargeType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCircPump), this->device_type(), "wWCircPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCircPumpMode), this->device_type(), "wWCircPumpMode", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCirc), this->device_type(), "wWCirc", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCurTemp), this->device_type(), "wWCurTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCurTemp2), this->device_type(), "wWCurTemp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCurFlow), this->device_type(), "wWCurFlow", F("l/min"), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWHeat), this->device_type(), "wWHeat", nullptr, F_(iconvalve));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wwStorageTemp1), this->device_type(), "wwStorageTemp1", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wwStorageTemp2), this->device_type(), "wwStorageTemp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWActivated), this->device_type(), "wWActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWOneTime), this->device_type(), "wWOneTime", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWDisinfecting), this->device_type(), "wWDisinfecting", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWCharging), this->device_type(), "wWCharging", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWRecharging), this->device_type(), "wWRecharging", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWTempOK), this->device_type(), "wWTempOK", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWActive), this->device_type(), "wWActive", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWSetPumpPower), this->device_type(), "wWSetPumpPower", F_(percent), F_(iconwaterpump));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wwMixTemperature), this->device_type(), "wwMixTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wwBufferTemperature), this->device_type(), "wwBufferTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWStarts), this->device_type(), "wWStarts", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(mqtt_suffix_ww), F_(wWWorkM), this->device_type(), "wWWorkM", F_(min), nullptr);

    mqtt_ha_config_ = true; // done
}

// send stuff to the Web UI
void Boiler::device_info_web(JsonArray & root) {
    // fetch the values into a JSON document
    DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_LARGE);
    JsonObject          json = doc.to<JsonObject>();
    if (!export_values_main(json)) {
        return; // empty
    }
    export_values_ww(json); // append ww values

    print_value_json(root, F("heatingActive"), nullptr, F_(heatingActive), nullptr, json);
    print_value_json(root, F("tapwaterActive"), nullptr, F_(tapwaterActive), nullptr, json);
    print_value_json(root, F("serviceCode"), nullptr, F_(serviceCode), nullptr, json);
    print_value_json(root, F("serviceCodeNumber"), nullptr, F_(serviceCodeNumber), nullptr, json);
    print_value_json(root, F("selFlowTemp"), nullptr, F_(selFlowTemp), F_(degrees), json);
    print_value_json(root, F("selBurnPow"), nullptr, F_(selBurnPow), F_(percent), json);
    print_value_json(root, F("curBurnPow"), nullptr, F_(curBurnPow), F_(percent), json);
    print_value_json(root, F("pumpMod"), nullptr, F_(pumpMod), F_(percent), json);
    print_value_json(root, F("pumpMod2"), nullptr, F_(pumpMod2), F_(percent), json);
    print_value_json(root, F("outdoorTemp"), nullptr, F_(outdoorTemp), F_(degrees), json);
    print_value_json(root, F("curFlowTemp"), nullptr, F_(curFlowTemp), F_(degrees), json);
    print_value_json(root, F("retTemp"), nullptr, F_(retTemp), F_(degrees), json);
    print_value_json(root, F("switchTemp"), nullptr, F_(switchTemp), F_(degrees), json);
    print_value_json(root, F("sysPress"), nullptr, F_(sysPress), nullptr, json);
    print_value_json(root, F("boilTemp"), nullptr, F_(boilTemp), F_(degrees), json);
    print_value_json(root, F("burnGas"), nullptr, F_(burnGas), nullptr, json);
    print_value_json(root, F("flameCurr"), nullptr, F_(flameCurr), F_(uA), json);
    print_value_json(root, F("heatPump"), nullptr, F_(heatPump), nullptr, json);
    print_value_json(root, F("fanWork"), nullptr, F_(fanWork), nullptr, json);
    print_value_json(root, F("ignWork"), nullptr, F_(ignWork), nullptr, json);
    print_value_json(root, F("heatingActivated"), nullptr, F_(heatingActivated), nullptr, json);
    print_value_json(root, F("heatingTemp"), nullptr, F_(heatingTemp), F_(degrees), json);
    print_value_json(root, F("pumpModMax"), nullptr, F_(pumpModMax), F_(percent), json);
    print_value_json(root, F("pumpModMin"), nullptr, F_(pumpModMin), F_(percent), json);
    print_value_json(root, F("pumpDelay"), nullptr, F_(pumpDelay), F_(min), json);
    print_value_json(root, F("burnMinPeriod"), nullptr, F_(burnMinPeriod), F_(min), json);
    print_value_json(root, F("burnMinPower"), nullptr, F_(burnMinPower), F_(percent), json);
    print_value_json(root, F("burnMaxPower"), nullptr, F_(burnMaxPower), F_(percent), json);
    print_value_json(root, F("boilHystOn"), nullptr, F_(boilHystOn), F_(degrees), json);
    print_value_json(root, F("boilHystOff"), nullptr, F_(boilHystOff), F_(degrees), json);
    print_value_json(root, F("setFlowTemp"), nullptr, F_(setFlowTemp), F_(degrees), json);
    print_value_json(root, F("setBurnPow"), nullptr, F_(setBurnPow), F_(percent), json);
    print_value_json(root, F("burnStarts"), nullptr, F_(burnStarts), nullptr, json);
    print_value_json(root, F("burnWorkMin"), nullptr, F_(burnWorkMin), F_(min), json);
    print_value_json(root, F("heatWorkMin"), nullptr, F_(heatWorkMin), F_(min), json);
    print_value_json(root, F("UBAuptime"), nullptr, F_(UBAuptime), F_(min), json);

    // ww
    print_value_json(root, F("wWSelTemp"), nullptr, F_(wWSelTemp), F_(degrees), json);
    print_value_json(root, F("wWSetTemp"), nullptr, F_(wWSetTemp), F_(degrees), json);
    print_value_json(root, F("wWDisinfectionTemp"), nullptr, F_(wWDisinfectionTemp), F_(degrees), json);
    print_value_json(root, F("wWType"), nullptr, F_(wWType), nullptr, json);
    print_value_json(root, F("wWChargeType"), nullptr, F_(wWChargeType), nullptr, json);
    print_value_json(root, F("wWCircPump"), nullptr, F_(wWCircPump), nullptr, json);
    print_value_json(root, F("wWCircPumpMode"), nullptr, F_(wWCircPumpMode), nullptr, json);
    print_value_json(root, F("wWCirc"), nullptr, F_(wWCirc), nullptr, json);
    print_value_json(root, F("wWCurTemp"), nullptr, F_(wWCurTemp), F_(degrees), json);
    print_value_json(root, F("wWCurTemp2"), nullptr, F_(wWCurTemp2), F_(degrees), json);
    print_value_json(root, F("wWCurFlow"), nullptr, F_(wWCurFlow), F("l/min"), json);
    print_value_json(root, F("wwStorageTemp1"), nullptr, F_(wwStorageTemp1), F_(degrees), json);
    print_value_json(root, F("wwStorageTemp2"), nullptr, F_(wwStorageTemp2), F_(degrees), json);
    print_value_json(root, F("exhaustTemp"), nullptr, F_(exhaustTemp), F_(degrees), json);
    print_value_json(root, F("wWActivated"), nullptr, F_(wWActivated), nullptr, json);
    print_value_json(root, F("wWOneTime"), nullptr, F_(wWOneTime), nullptr, json);
    print_value_json(root, F("wWDisinfecting"), nullptr, F_(wWDisinfecting), nullptr, json);
    print_value_json(root, F("wWCharging"), nullptr, F_(wWCharging), nullptr, json);
    print_value_json(root, F("wWRecharging"), nullptr, F_(wWRecharging), nullptr, json);
    print_value_json(root, F("wWTempOK"), nullptr, F_(wWTempOK), nullptr, json);
    print_value_json(root, F("wWActive"), nullptr, F_(wWActive), nullptr, json);
    print_value_json(root, F("wWHeat"), nullptr, F_(wWHeat), nullptr, json);
    print_value_json(root, F("wWSetPumpPower"), nullptr, F_(wWSetPumpPower), F_(percent), json);
    print_value_json(root, F("wwMixTemperature"), nullptr, F_(wwMixTemperature), F_(degrees), json);
    print_value_json(root, F("wwBufferTemperature"), nullptr, F_(wwBufferTemperature), F_(degrees), json);
    print_value_json(root, F("wWStarts"), nullptr, F_(wWStarts), nullptr, json);
    print_value_json(root, F("wWWorkM"), nullptr, F_(wWWorkM), F_(min), json);
}

bool Boiler::export_values(JsonObject & json) {
    if (!export_values_main(json)) {
        return false;
    }
    export_values_ww(json); // append ww values
    return true;
}

// creates JSON doc from values
// returns false if empty
bool Boiler::export_values_ww(JsonObject & json) {
    char s[10]; // for formatting strings

    // Warm Water comfort setting
    if (Helpers::hasValue(wWComfort_)) {
        if (wWComfort_ == 0x00) {
            json["wWComfort"] = F("Hot");
        } else if (wWComfort_ == 0xD8) {
            json["wWComfort"] = F("Eco");
        } else if (wWComfort_ == 0xEC) {
            json["wWComfort"] = F("Intelligent");
        }
    }

    // Warm Water selected temperature
    if (Helpers::hasValue(wWSelTemp_)) {
        json["wWSelTemp"] = wWSelTemp_;
    }

    // Warm Water set temperature
    if (Helpers::hasValue(wWSetTemp_)) {
        json["wWSetTemp"] = wWSetTemp_;
    }

    // Warm Water disinfection temperature
    if (Helpers::hasValue(wWDisinfectionTemp_)) {
        json["wWDisinfectionTemp"] = wWDisinfectionTemp_;
    }

    // Warm Water type
    if (wWType_ == 0) { // no json if not set
        json["wWType"] = F("off");
    } else if (wWType_ == 1) {
        json["wWType"] = F("flow");
    } else if (wWType_ == 2) {
        json["wWType"] = F("buffered flow");
    } else if (wWType_ == 3) {
        json["wWType"] = F("buffer");
    } else if (wWType_ == 4) {
        json["wWType"] = F("layered buffer");
    }

    // Warm Water charging type
    if (Helpers::hasValue(wWChargeType_, EMS_VALUE_BOOL)) {
        json["wWChargeType"] = wWChargeType_ ? F("3-way valve") : F("charge pump");
    }

    // Warm Water circulation pump available bool
    if (Helpers::hasValue(wWCircPump_, EMS_VALUE_BOOL)) {
        json["wWCircPump"] = Helpers::render_value(s, wWCircPump_, EMS_VALUE_BOOL);
    }

    // Warm Water circulation pump freq
    if (Helpers::hasValue(wWCircPumpMode_)) {
        if (wWCircPumpMode_ == 7) {
            json["wWCircPumpMode"] = F("continuous");
        } else {
            char s[7];
            char buffer[2];
            buffer[0] = (wWCircPumpMode_ % 10) + '0';
            buffer[1] = '\0';
            strlcpy(s, buffer, 7);
            strlcat(s, "x3min", 7);
            json["wWCircPumpMode"] = s;
        }
    }

    // Warm Water circulation active bool
    if (Helpers::hasValue(wWCirc_, EMS_VALUE_BOOL)) {
        json["wWCirc"] = Helpers::render_value(s, wWCirc_, EMS_VALUE_BOOL);
    }

    // Warm Water current temperature (intern)
    if (Helpers::hasValue(wWCurTemp_)) {
        json["wWCurTemp"] = (float)wWCurTemp_ / 10;
    }

    // Warm Water current temperature (extern)
    if (Helpers::hasValue(wWCurTemp2_)) {
        json["wWCurTemp2"] = (float)wWCurTemp2_ / 10;
    }

    // Warm Water current tap water flow l/min
    if (Helpers::hasValue(wWCurFlow_)) {
        json["wWCurFlow"] = (float)wWCurFlow_ / 10;
    }

    // Warm water storage temperature (intern)
    if (Helpers::hasValue(wwStorageTemp1_)) {
        json["wwStorageTemp1"] = (float)wwStorageTemp1_ / 10;
    }

    // Warm water storage temperature (extern)
    if (Helpers::hasValue(wwStorageTemp2_)) {
        json["wwStorageTemp2"] = (float)wwStorageTemp2_ / 10;
    }

    // Warm Water activated bool
    if (Helpers::hasValue(wWActivated_, EMS_VALUE_BOOL)) {
        json["wWActivated"] = Helpers::render_value(s, wWActivated_, EMS_VALUE_BOOL);
    }

    // Warm Water one time charging bool
    if (Helpers::hasValue(wWOneTime_, EMS_VALUE_BOOL)) {
        json["wWOneTime"] = Helpers::render_value(s, wWOneTime_, EMS_VALUE_BOOL);
    }

    // Warm Water disinfecting bool
    if (Helpers::hasValue(wWDisinfecting_, EMS_VALUE_BOOL)) {
        json["wWDisinfecting"] = Helpers::render_value(s, wWDisinfecting_, EMS_VALUE_BOOL);
    }

    // Warm water charging bool
    if (Helpers::hasValue(wWCharging_, EMS_VALUE_BOOL)) {
        json["wWCharging"] = Helpers::render_value(s, wWCharging_, EMS_VALUE_BOOL);
    }

    // Warm water recharge bool
    if (Helpers::hasValue(wWRecharging_, EMS_VALUE_BOOL)) {
        json["wWRecharging"] = Helpers::render_value(s, wWRecharging_, EMS_VALUE_BOOL);
    }

    // Warm water temperature ok bool
    if (Helpers::hasValue(wWTempOK_, EMS_VALUE_BOOL)) {
        json["wWTempOK"] = Helpers::render_value(s, wWTempOK_, EMS_VALUE_BOOL);
    }

    // Warm water active bool
    if (Helpers::hasValue(wWActive_, EMS_VALUE_BOOL)) {
        json["wWActive"] = Helpers::render_value(s, wWActive_, EMS_VALUE_BOOL);
    }

    // Warm Water charging bool
    if (Helpers::hasValue(wWHeat_, EMS_VALUE_BOOL)) {
        json["wWHeat"] = Helpers::render_value(s, wWHeat_, EMS_VALUE_BOOL);
    }

    // Warm Water pump set power %
    if (Helpers::hasValue(wWSetPumpPower_)) {
        json["wWSetPumpPower"] = wWSetPumpPower_;
    }

    // Warm water mix temperature
    if (Helpers::hasValue(wwMixTemperature_)) {
        json["wwMixTemperature"] = wwMixTemperature_;
    }

    // Warm water buffer boiler temperature
    if (Helpers::hasValue(wwBufferTemperature_)) {
        json["wwBufferTemperature"] = wwBufferTemperature_;
    }

    // Warm Water # starts
    if (Helpers::hasValue(wWStarts_)) {
        json["wWStarts"] = wWStarts_;
    }

    // Warm Water active time
    if (Helpers::hasValue(wWWorkM_)) {
        json["wWWorkM"] = wWWorkM_;
    }

    return (json.size());
}

// creates JSON doc from values
// returns false if empty
bool Boiler::export_values_main(JsonObject & json) {
    char s[10]; // for formatting strings

    // Hot tap water bool
    if (Helpers::hasValue(heatingActive_, EMS_VALUE_BOOL)) {
        json["heatingActive"] = Helpers::render_value(s, heatingActive_, EMS_VALUE_BOOL);
    }

    // Central heating bool
    if (Helpers::hasValue(tapwaterActive_, EMS_VALUE_BOOL)) {
        json["tapwaterActive"] = Helpers::render_value(s, tapwaterActive_, EMS_VALUE_BOOL);
    }

    // Selected flow temperature deg
    if (Helpers::hasValue(selFlowTemp_)) {
        json["selFlowTemp"] = selFlowTemp_;
    }

    // Burner selected max power %
    if (Helpers::hasValue(selBurnPow_)) {
        json["selBurnPow"] = selBurnPow_;
    }

    // Burner current power %
    if (Helpers::hasValue(curBurnPow_)) {
        json["curBurnPow"] = curBurnPow_;
    }

    // Pump modulation %
    if (Helpers::hasValue(pumpMod_)) {
        json["pumpMod"] = pumpMod_;
    }

    // Heat Pump modulation %
    if (Helpers::hasValue(pumpMod2_)) {
        json["pumpMod2"] = pumpMod2_;
    }

    // Outside temperature
    if (Helpers::hasValue(outdoorTemp_)) {
        json["outdoorTemp"] = (float)outdoorTemp_ / 10;
    }

    // Current flow temperature
    if (Helpers::hasValue(curFlowTemp_)) {
        json["curFlowTemp"] = (float)curFlowTemp_ / 10;
    }

    // Return temperature
    if (Helpers::hasValue(retTemp_)) {
        json["retTemp"] = (float)retTemp_ / 10;
    }

    // Mixing switch temperature
    if (Helpers::hasValue(switchTemp_)) {
        json["switchTemp"] = (float)switchTemp_ / 10;
    }

    // System pressure
    if (Helpers::hasValue(sysPress_)) {
        json["sysPress"] = (float)sysPress_ / 10;
    }

    // Max boiler temperature
    if (Helpers::hasValue(boilTemp_)) {
        json["boilTemp"] = (float)boilTemp_ / 10;
    }

    // Exhaust temperature
    if (Helpers::hasValue(exhaustTemp_)) {
        json["exhaustTemp"] = (float)exhaustTemp_ / 10;
    }

    // Gas bool
    if (Helpers::hasValue(burnGas_, EMS_VALUE_BOOL)) {
        json["burnGas"] = Helpers::render_value(s, burnGas_, EMS_VALUE_BOOL);
    }

    // Flame current uA
    if (Helpers::hasValue(flameCurr_)) {
        json["flameCurr"] = (float)(int16_t)flameCurr_ / 10;
    }

    // Boiler pump bool
    if (Helpers::hasValue(heatPump_, EMS_VALUE_BOOL)) {
        json["heatPump"] = Helpers::render_value(s, heatPump_, EMS_VALUE_BOOL);
    }

    // Fan bool
    if (Helpers::hasValue(fanWork_, EMS_VALUE_BOOL)) {
        json["fanWork"] = Helpers::render_value(s, fanWork_, EMS_VALUE_BOOL);
    }

    // Ignition bool
    if (Helpers::hasValue(ignWork_, EMS_VALUE_BOOL)) {
        json["ignWork"] = Helpers::render_value(s, ignWork_, EMS_VALUE_BOOL);
    }

    // heating activated bool
    if (Helpers::hasValue(heatingActivated_, EMS_VALUE_BOOL)) {
        json["heatingActivated"] = Helpers::render_value(s, heatingActivated_, EMS_VALUE_BOOL);
    }

    // Heating temperature setting on the boiler
    if (Helpers::hasValue(heatingTemp_)) {
        json["heatingTemp"] = heatingTemp_;
    }

    // Boiler circuit pump modulation max power %
    if (Helpers::hasValue(pumpModMax_)) {
        json["pumpModMax"] = pumpModMax_;
    }

    // Boiler circuit pump modulation min power %
    if (Helpers::hasValue(pumpModMin_)) {
        json["pumpModMin"] = pumpModMin_;
    }

    // Boiler circuit pump delay time min
    if (Helpers::hasValue(pumpDelay_)) {
        json["pumpDelay"] = pumpDelay_;
    }

    // Boiler burner min period min
    if (Helpers::hasValue(burnMinPeriod_)) {
        json["burnMinPeriod"] = burnMinPeriod_;
    }

    // Boiler burner min power %
    if (Helpers::hasValue(burnMinPower_)) {
        json["burnMinPower"] = burnMinPower_;
    }

    // Boiler burner max power %
    if (Helpers::hasValue(burnMaxPower_)) {
        json["burnMaxPower"] = burnMaxPower_;
    }

    // Boiler temp hysteresis on degrees
    if (Helpers::hasValue(boilHystOn_)) {
        json["boilHystOn"] = boilHystOn_;
    }

    // Boiler temp hysteresis off degrees
    if (Helpers::hasValue(boilHystOff_)) {
        json["boilHystOff"] = boilHystOff_;
    }

    // Set Flow temperature
    if (Helpers::hasValue(setFlowTemp_)) {
        json["setFlowTemp"] = setFlowTemp_;
    }

    // Total UBA working time
    if (Helpers::hasValue(UBAuptime_)) {
        json["UBAuptime"] = UBAuptime_;
    }

    if (Helpers::hasValue(setBurnPow_)) {
        json["setBurnPow"] = setBurnPow_;
    }

    // Burner # starts
    if (Helpers::hasValue(burnStarts_)) {
        json["burnStarts"] = burnStarts_;
    }

    // Total burner operating time
    if (Helpers::hasValue(burnWorkMin_)) {
        json["burnWorkMin"] = burnWorkMin_;
    }

    // Total heat operating time
    if (Helpers::hasValue(heatWorkMin_)) {
        json["heatWorkMin"] = heatWorkMin_;
    }

    // Service Code
    // Service Code Number
    if (Helpers::hasValue(serviceCodeNumber_)) {
        json["serviceCode"]       = serviceCode_;
        json["serviceCodeNumber"] = serviceCodeNumber_;
    }

    return (json.size());
}

// publish values via MQTT
void Boiler::publish_values(JsonObject & json, bool force) {
    // handle HA first
    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config(force);
    }

    DynamicJsonDocument doc_main(EMSESP_MAX_JSON_SIZE_LARGE);
    JsonObject          json_main = doc_main.to<JsonObject>();
    if (export_values_main(json_main)) {
        Mqtt::publish(F("boiler_data"), doc_main.as<JsonObject>());
    }

    DynamicJsonDocument doc_ww(EMSESP_MAX_JSON_SIZE_LARGE);
    JsonObject          json_ww = doc_ww.to<JsonObject>();
    if (export_values_ww(json_ww)) {
        Mqtt::publish(F("boiler_data_ww"), doc_ww.as<JsonObject>());
    }

    // send out heating and tapwater status
    check_active();
}

// called after a process command is called, to check values and see if we need to force an MQTT publish
bool Boiler::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// print values to shell console
void Boiler::show_values(uuid::console::Shell & shell) {
    EMSdevice::show_values(shell); // for showing the header

    // fetch the values into a JSON document
    DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_LARGE);
    JsonObject          json = doc.to<JsonObject>();
    if (!export_values_main(json)) {
        return; // empty
    }
    export_values_ww(json); // append ww values
    doc.shrinkToFit();

    print_value_json(shell, F("heatingActive"), nullptr, F_(heatingActive), nullptr, json);
    print_value_json(shell, F("tapwaterActive"), nullptr, F_(tapwaterActive), nullptr, json);
    print_value_json(shell, F("serviceCode"), nullptr, F_(serviceCode), nullptr, json);
    print_value_json(shell, F("serviceCodeNumber"), nullptr, F_(serviceCodeNumber), nullptr, json);
    print_value_json(shell, F("wWSelTemp"), nullptr, F_(wWSelTemp), F_(degrees), json);
    print_value_json(shell, F("wWSetTemp"), nullptr, F_(wWSetTemp), F_(degrees), json);
    print_value_json(shell, F("wWDisinfectionTemp"), nullptr, F_(wWDisinfectionTemp), F_(degrees), json);
    print_value_json(shell, F("selFlowTemp"), nullptr, F_(selFlowTemp), F_(degrees), json);
    print_value_json(shell, F("selBurnPow"), nullptr, F_(selBurnPow), F_(percent), json);
    print_value_json(shell, F("curBurnPow"), nullptr, F_(curBurnPow), F_(percent), json);
    print_value_json(shell, F("pumpMod"), nullptr, F_(pumpMod), F_(percent), json);
    print_value_json(shell, F("pumpMod2"), nullptr, F_(pumpMod2), F_(percent), json);
    print_value_json(shell, F("wWType"), nullptr, F_(wWType), nullptr, json);
    print_value_json(shell, F("wWChargeType"), nullptr, F_(wWChargeType), nullptr, json);
    print_value_json(shell, F("wWCircPump"), nullptr, F_(wWCircPump), nullptr, json);
    print_value_json(shell, F("wWCircPumpMode"), nullptr, F_(wWCircPumpMode), nullptr, json);
    print_value_json(shell, F("wWCirc"), nullptr, F_(wWCirc), nullptr, json);
    print_value_json(shell, F("outdoorTemp"), nullptr, F_(outdoorTemp), F_(degrees), json);
    print_value_json(shell, F("wWCurTemp"), nullptr, F_(wWCurTemp), F_(degrees), json);
    print_value_json(shell, F("wWCurTemp2"), nullptr, F_(wWCurTemp2), F_(degrees), json);
    print_value_json(shell, F("wWCurFlow"), nullptr, F_(wWCurFlow), F("l/min"), json);
    print_value_json(shell, F("curFlowTemp"), nullptr, F_(curFlowTemp), F_(degrees), json);
    print_value_json(shell, F("retTemp"), nullptr, F_(retTemp), F_(degrees), json);
    print_value_json(shell, F("switchTemp"), nullptr, F_(switchTemp), F_(degrees), json);
    print_value_json(shell, F("sysPress"), nullptr, F_(sysPress), nullptr, json);
    print_value_json(shell, F("boilTemp"), nullptr, F_(boilTemp), F_(degrees), json);
    print_value_json(shell, F("wwStorageTemp1"), nullptr, F_(wwStorageTemp1), F_(degrees), json);
    print_value_json(shell, F("wwStorageTemp2"), nullptr, F_(wwStorageTemp2), F_(degrees), json);
    print_value_json(shell, F("exhaustTemp"), nullptr, F_(exhaustTemp), F_(degrees), json);
    print_value_json(shell, F("wWActivated"), nullptr, F_(wWActivated), nullptr, json);
    print_value_json(shell, F("wWOneTime"), nullptr, F_(wWOneTime), nullptr, json);
    print_value_json(shell, F("wWDisinfecting"), nullptr, F_(wWDisinfecting), nullptr, json);
    print_value_json(shell, F("wWCharging"), nullptr, F_(wWCharging), nullptr, json);
    print_value_json(shell, F("wWRecharging"), nullptr, F_(wWRecharging), nullptr, json);
    print_value_json(shell, F("wWTempOK"), nullptr, F_(wWTempOK), nullptr, json);
    print_value_json(shell, F("wWActive"), nullptr, F_(wWActive), nullptr, json);
    print_value_json(shell, F("burnGas"), nullptr, F_(burnGas), nullptr, json);
    print_value_json(shell, F("flameCurr"), nullptr, F_(flameCurr), F_(uA), json);
    print_value_json(shell, F("heatPump"), nullptr, F_(heatPump), nullptr, json);
    print_value_json(shell, F("fanWork"), nullptr, F_(fanWork), nullptr, json);
    print_value_json(shell, F("ignWork"), nullptr, F_(ignWork), nullptr, json);
    print_value_json(shell, F("wWHeat"), nullptr, F_(wWHeat), nullptr, json);
    print_value_json(shell, F("heatingActivated"), nullptr, F_(heatingActivated), nullptr, json);
    print_value_json(shell, F("heatingTemp"), nullptr, F_(heatingTemp), F_(degrees), json);
    print_value_json(shell, F("pumpModMax"), nullptr, F_(pumpModMax), F_(percent), json);
    print_value_json(shell, F("pumpModMin"), nullptr, F_(pumpModMin), F_(percent), json);
    print_value_json(shell, F("pumpDelay"), nullptr, F_(pumpDelay), F_(min), json);
    print_value_json(shell, F("burnMinPeriod"), nullptr, F_(burnMinPeriod), F_(min), json);
    print_value_json(shell, F("burnMinPower"), nullptr, F_(burnMinPower), F_(percent), json);
    print_value_json(shell, F("burnMaxPower"), nullptr, F_(burnMaxPower), F_(percent), json);
    print_value_json(shell, F("boilHystOn"), nullptr, F_(boilHystOn), F_(degrees), json);
    print_value_json(shell, F("boilHystOff"), nullptr, F_(boilHystOff), F_(degrees), json);
    print_value_json(shell, F("setFlowTemp"), nullptr, F_(setFlowTemp), F_(degrees), json);
    print_value_json(shell, F("wWSetPumpPower"), nullptr, F_(wWSetPumpPower), F_(percent), json);
    print_value_json(shell, F("wwMixTemperature"), nullptr, F_(wwMixTemperature), F_(degrees), json);
    print_value_json(shell, F("wwBufferTemperature"), nullptr, F_(wwBufferTemperature), F_(degrees), json);
    print_value_json(shell, F("wWStarts"), nullptr, F_(wWStarts), nullptr, json);
    print_value_json(shell, F("wWWorkM"), nullptr, F_(wWWorkM), nullptr, json);
    print_value_json(shell, F("setBurnPow"), nullptr, F_(setBurnPow), F_(percent), json);
    print_value_json(shell, F("burnStarts"), nullptr, F_(burnStarts), nullptr, json);

    if (Helpers::hasValue(wWWorkM_)) {
        shell.printfln(F("  Warm Water active time: %d days %d hours %d minutes"), wWWorkM_ / 1440, (wWWorkM_ % 1440) / 60, wWWorkM_ % 60);
    }

    if (Helpers::hasValue(burnWorkMin_)) {
        shell.printfln(F("  Total burner operating time: %d days %d hours %d minutes"), burnWorkMin_ / 1440, (burnWorkMin_ % 1440) / 60, burnWorkMin_ % 60);
    }
    if (Helpers::hasValue(heatWorkMin_)) {
        shell.printfln(F("  Total heat operating time: %d days %d hours %d minutes"), heatWorkMin_ / 1440, (heatWorkMin_ % 1440) / 60, heatWorkMin_ % 60);
    }
    if (Helpers::hasValue(UBAuptime_)) {
        shell.printfln(F("  Total UBA working time: %d days %d hours %d minutes"), UBAuptime_ / 1440, (UBAuptime_ % 1440) / 60, UBAuptime_ % 60);
    }

    shell.println();
}

/*
 * Check if hot tap water or heating is active
 * If a value has changed, post it immediately to MQTT so we get real time data
 * Values will always be posted first time as heatingActive_ and tapwaterActive_ will have values EMS_VALUE_BOOL_NOTSET
 */
void Boiler::check_active() {
    bool    b;
    uint8_t val;

    // check if heating is active, bits 2 and 4 must be set
    b   = ((boilerState_ & 0x09) == 0x09);
    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (heatingActive_ != val) {
        heatingActive_ = val;
        char s[7];
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s, b));
    }

    // check if tap water is active, bits 1 and 4 must be set
    b   = ((boilerState_ & 0x0A) == 0x0A);
    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (tapwaterActive_ != val) {
        tapwaterActive_ = val;
        char s[7];
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, b));
        EMSESP::tap_water_active(b); // let EMS-ESP know, used in the Shower class
    }
}

// 0x33
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWActivated_, 1);    // 0xFF means on
    changed_ |= telegram->read_value(wWCircPump_, 6);     // 0xFF means on
    changed_ |= telegram->read_value(wWCircPumpMode_, 7); // 1=1x3min... 6=6x3min, 7=continuous
    changed_ |= telegram->read_value(wWChargeType_, 10);  // 0 = charge pump, 0xff = 3-way valve
    changed_ |= telegram->read_value(wWSelTemp_, 2);
    changed_ |= telegram->read_value(wWDisinfectionTemp_, 8);
    changed_ |= telegram->read_value(wWComfort_, 9);
}

// 0x18
void Boiler::process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(selFlowTemp_, 0);
    changed_ |= telegram->read_value(curFlowTemp_, 1);
    changed_ |= telegram->read_value(selBurnPow_, 3); // burn power max setting
    changed_ |= telegram->read_value(curBurnPow_, 4);
    changed_ |= telegram->read_value(boilerState_, 5);

    changed_ |= telegram->read_bitvalue(burnGas_, 7, 0);
    changed_ |= telegram->read_bitvalue(fanWork_, 7, 2);
    changed_ |= telegram->read_bitvalue(ignWork_, 7, 3);
    changed_ |= telegram->read_bitvalue(heatPump_, 7, 5);
    changed_ |= telegram->read_bitvalue(wWHeat_, 7, 6);
    changed_ |= telegram->read_bitvalue(wWCirc_, 7, 7);

    // warm water storage sensors (if present)
    // wwStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/proddy/EMS-ESP/issues/206
    changed_ |= telegram->read_value(wwStorageTemp1_, 9);  // 0x8300 if not available
    changed_ |= telegram->read_value(wwStorageTemp2_, 11); // 0x8000 if not available - this is boiler temp

    changed_ |= telegram->read_value(retTemp_, 13);
    changed_ |= telegram->read_value(flameCurr_, 15);

    // system pressure. FF means missing
    changed_ |= telegram->read_value(sysPress_, 17); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        changed_ |= telegram->read_value(serviceCode_[0], 18);
        changed_ |= telegram->read_value(serviceCode_[1], 19);
        serviceCode_[2] = '\0'; // null terminate string
    }

    changed_ |= telegram->read_value(serviceCodeNumber_, 20);

    // at this point do a quick check to see if the hot water or heating is active
    check_active();
}

/*
 * UBATotalUptime - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void Boiler::process_UBATotalUptime(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(UBAuptime_, 0, 3); // force to 3 bytes
}

/*
 * UBAParameters - type 0x16
 */
void Boiler::process_UBAParameters(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(heatingActivated_, 0);
    changed_ |= telegram->read_value(heatingTemp_, 1);
    changed_ |= telegram->read_value(burnMaxPower_, 2);
    changed_ |= telegram->read_value(burnMinPower_, 3);
    changed_ |= telegram->read_value(boilHystOff_, 4);
    changed_ |= telegram->read_value(boilHystOn_, 5);
    changed_ |= telegram->read_value(burnMinPeriod_, 6);
    changed_ |= telegram->read_value(pumpDelay_, 8);
    changed_ |= telegram->read_value(pumpModMax_, 9);
    changed_ |= telegram->read_value(pumpModMin_, 10);
}

/*
 * UBAMonitorWW - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWSetTemp_, 0);
    changed_ |= telegram->read_value(wWCurTemp_, 1);
    changed_ |= telegram->read_value(wWCurTemp2_, 3);
    changed_ |= telegram->read_value(wWCurFlow_, 9);
    changed_ |= telegram->read_value(wWType_, 8);

    changed_ |= telegram->read_value(wWWorkM_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 13, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 5, 1);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 5, 2);
    changed_ |= telegram->read_bitvalue(wWCharging_, 5, 3);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 5, 4);
    changed_ |= telegram->read_bitvalue(wWTempOK_, 5, 5);
    changed_ |= telegram->read_bitvalue(wWActive_, 5, 6);
}

/*
 * UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
 * Still to figure out are: serviceCode, retTemp, sysPress
 */
void Boiler::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(selFlowTemp_, 6);
    changed_ |= telegram->read_bitvalue(burnGas_, 11, 0);
    changed_ |= telegram->read_bitvalue(wWHeat_, 11, 2);
    changed_ |= telegram->read_value(curBurnPow_, 10);
    changed_ |= telegram->read_value(selBurnPow_, 9);
    changed_ |= telegram->read_value(curFlowTemp_, 7);
    changed_ |= telegram->read_value(flameCurr_, 19);

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 4) && (telegram->offset == 0)) {
        changed_ |= telegram->read_value(serviceCode_[0], 4);
        changed_ |= telegram->read_value(serviceCode_[1], 5);
        serviceCode_[2] = '\0';
    }

    // at this point do a quick check to see if the hot water or heating is active
    check_active();
}

/*
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 * e.g. 08 00 19 00 80 00 02 41 80 00 00 00 00 00 03 91 7B 05 B8 40 00 00 00 04 92 AD 00 5E EE 80 00
 *      08 0B 19 00 FF EA 02 47 80 00 00 00 00 62 03 CA 24 2C D6 23 00 00 00 27 4A B6 03 6E 43 
 *                  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 17 19 20 21 22 23 24
 */
void Boiler::process_UBAMonitorSlow(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(outdoorTemp_, 0);
    changed_ |= telegram->read_value(boilTemp_, 2);
    changed_ |= telegram->read_value(exhaustTemp_, 4);
    changed_ |= telegram->read_value(switchTemp_, 25); // only if there is a mixer module present
    changed_ |= telegram->read_value(pumpMod_, 9);
    changed_ |= telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    changed_ |= telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
}

/*
 * UBAMonitorSlowPlus2 - type 0xE3
 */
void Boiler::process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(pumpMod2_, 13); // Heat Pump Modulation
}

/*
 * UBAMonitorSlowPlus - type 0xE5 - central heating monitor EMS+
 */
void Boiler::process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_bitvalue(fanWork_, 2, 2);
    changed_ |= telegram->read_bitvalue(ignWork_, 2, 3);
    changed_ |= telegram->read_bitvalue(heatPump_, 2, 5);
    changed_ |= telegram->read_bitvalue(wWCirc_, 2, 7);
    changed_ |= telegram->read_value(burnStarts_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(burnWorkMin_, 13, 3); // force to 3 bytes
    changed_ |= telegram->read_value(heatWorkMin_, 19, 3); // force to 3 bytes
    changed_ |= telegram->read_value(pumpMod_, 25);
}

/*
 * UBAParametersPlus - type 0xe6
 */
void Boiler::process_UBAParametersPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(heatingActivated_, 0);
    changed_ |= telegram->read_value(heatingTemp_, 1);
    changed_ |= telegram->read_value(burnMaxPower_, 6);
    changed_ |= telegram->read_value(burnMinPower_, 7);
    changed_ |= telegram->read_value(boilHystOff_, 8);
    changed_ |= telegram->read_value(boilHystOn_, 9);
    changed_ |= telegram->read_value(burnMinPeriod_, 10);
}

// 0xEA
void Boiler::process_UBAParameterWWPlus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWActivated_, 5);     // 0x01 means on
    changed_ |= telegram->read_value(wWCircPump_, 10);     // 0x01 means yes
    changed_ |= telegram->read_value(wWCircPumpMode_, 11); // 1=1x3min... 6=6x3min, 7=continuous
    // changed_ |= telegram->read_value(wWDisinfectTemp_, 12); // settings, status in E9
    // changed_ |= telegram->read_value(wWSelTemp_, 6);        // settings, status in E9
}

// 0xE9 - DHW Status
// e.g. 08 00 E9 00 37 01 F6 01 ED 00 00 00 00 41 3C 00 00 00 00 00 00 00 00 00 00 00 00 37 00 00 00 (CRC=77) #data=27
void Boiler::process_UBADHWStatus(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWSetTemp_, 0);
    changed_ |= telegram->read_value(wWCurTemp_, 1);
    changed_ |= telegram->read_value(wWCurTemp2_, 3);

    changed_ |= telegram->read_value(wWWorkM_, 17, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 14, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 12, 2);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 12, 3);
    changed_ |= telegram->read_bitvalue(wWCharging_, 12, 4);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 13, 4);
    changed_ |= telegram->read_bitvalue(wWTempOK_, 13, 5);
    changed_ |= telegram->read_bitvalue(wWCircPump_, 13, 2);

    // changed_ |= telegram->read_value(wWActivated_, 20); // Activated is in 0xEA, this is something other 0/100%
    changed_ |= telegram->read_value(wWSelTemp_, 10);
    changed_ |= telegram->read_value(wWDisinfectionTemp_, 9);
}

// 0x2A - MC10Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/proddy/EMS-ESP/issues/397
void Boiler::process_MC10Status(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wwMixTemperature_, 14);
    changed_ |= telegram->read_value(wwBufferTemperature_, 18);
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(outdoorTemp_, 0);
}

// UBASetPoint 0x1A
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(setFlowTemp_, 0);    // boiler set temp from thermostat
    changed_ |= telegram->read_value(setBurnPow_, 1);     // max json power in %
    changed_ |= telegram->read_value(wWSetPumpPower_, 2); // ww pump speed/power?
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// 0x35
// not yet implemented
void Boiler::process_UBAFlags(std::shared_ptr<const Telegram> telegram) {
}

// 0x1C
// not yet implemented
void Boiler::process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram) {
    // first byte: Maintenance due (0 = no, 3 = yes, due to operating hours, 8 = yes, due to date)
}

// 0x10, 0x11, 0x12
// not yet implemented
void Boiler::process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram) {
    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
}

#pragma GCC diagnostic pop

// 0x15
void Boiler::process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram) {
    // first byte: Maintenance messages (0 = none, 1 = by operating hours, 2 = by date)
    // I see a value of 3 in the 1st byte when the boiler is booted, so probably a flag
    if (telegram->message_data[0] == 3) {
        LOG_WARNING(F("Boiler has booted."));
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
        write_command(EMS_TYPE_UBAParameterWW, 2, v, EMS_TYPE_UBAParameterWW); // read seltemp back
        write_command(EMS_TYPE_UBAFlags, 3, v, 0x34);                          // for i9000, see #397, read setTemp
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
    write_command(EMS_TYPE_UBASetPoints, 0, v, EMS_TYPE_UBASetPoints);

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

    LOG_INFO(F("Setting boiler heating temperature to "), v);
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

    LOG_INFO(F("Setting boiler min power to "), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 7, v, EMS_TYPE_UBAParametersPlus);
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

    LOG_INFO(F("Setting boiler max power to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 6, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 2, v, EMS_TYPE_UBAParameters);
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
    if (!Helpers::value2enum(value, set, {"hot", "eco", "intelligent"})) {
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
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
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
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
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

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        if (v < 7) {
            LOG_INFO(F("Setting warm water circulation mode %dx3min"), v);
        } else if (v == 7) {
            LOG_INFO(F("Setting warm water circulation mode continuos"));
        } else {
            return false;
        }
        write_command(EMS_TYPE_UBAParameterWW, 6, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

} // namespace emsesp
