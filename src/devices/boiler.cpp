/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

    // API call
    // Command::add_with_json(this->device_type(), F("info"), Boiler::command_info);
    Command::add_with_json(this->device_type(), F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });

    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        register_mqtt_ha_config();
    }
}

// create the config topics for Home Assistant MQTT Discovery
// for each of the main elements
void Boiler::register_mqtt_ha_config() {
    // Create the Master device
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
    doc["name"]    = F("Service Code");
    doc["uniq_id"] = F("boiler");
    doc["ic"]      = F("mdi:home-thermometer-outline");
    doc["stat_t"]  = F("ems-esp/boiler_data");
    doc["val_tpl"] = F("{{value_json.serviceCode}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP Boiler");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = this->brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-boiler");
    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/boiler/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag

    Mqtt::register_mqtt_ha_binary_sensor(F_(tapwaterActive), this->device_type(), "tapwaterActive");
    Mqtt::register_mqtt_ha_binary_sensor(F_(heatingActive), this->device_type(), "heatingActive");

    Mqtt::register_mqtt_ha_sensor(nullptr, F_(serviceCodeNumber), this->device_type(), "serviceCodeNumber", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWSelTemp), this->device_type(), "wWSelTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWSetTemp), this->device_type(), "wWSetTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWDisinfectionTemp), this->device_type(), "wWDisinfectionTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(selFlowTemp), this->device_type(), "selFlowTemp", F_(degrees), F_(icontemperature));

    Mqtt::register_mqtt_ha_sensor(nullptr, F_(selBurnPow), this->device_type(), "selBurnPow", F_(percent), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(curBurnPow), this->device_type(), "curBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(pumpMod), this->device_type(), "pumpMod", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(pumpMod2), this->device_type(), "pumpMod2", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWType), this->device_type(), "wWType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWChargeType), this->device_type(), "wWChargeType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCircPump), this->device_type(), "wWCircPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCiPuMode), this->device_type(), "wWCiPuMode", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCirc), this->device_type(), "wWCirc", nullptr, nullptr);

    Mqtt::register_mqtt_ha_sensor(nullptr, F_(outdoorTemp), this->device_type(), "outdoorTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCurTmp), this->device_type(), "wWCurTmp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCurTmp2), this->device_type(), "wWCurTmp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(wWCurFlow), this->device_type(), "wWCurFlow", F("l/min"), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(curFlowTemp), this->device_type(), "curFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(retTemp), this->device_type(), "retTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(switchTemp), this->device_type(), "switchTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(sysPress), this->device_type(), "sysPress", F_(bar), nullptr);
    Mqtt::register_mqtt_ha_sensor(nullptr, F_(boilTemp), this->device_type(), "boilTemp", F_(degrees), nullptr);

    // TODO add remaining values to MQTT

/*

    Mqtt::register_mqtt_ha_sensor(F_(burnGas), this->device_type(), "burnGas", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(flameCurr), this->device_type(), "flameCurr", F_(uA), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(heatPump), this->device_type(), "heatPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(fanWork), this->device_type(), "fanWork", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(ignWork), this->device_type(), "ignWork", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWHeat), this->device_type(), "wWHeat", nullptr, nullptr);

    Mqtt::register_mqtt_ha_sensor(F_(wwStorageTemp1), this->device_type(), "wwStorageTemp1", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wwStorageTemp2), this->device_type(), "wwStorageTemp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(exhaustTemp), this->device_type(), "exhaustTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWActivated), this->device_type(), "wWActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWOnetime), this->device_type(), "wWOnetime", nullptr, nullptr);

    Mqtt::register_mqtt_ha_sensor(F_(wWDisinfecting), this->device_type(), "wWDisinfecting", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWCharge), this->device_type(), "wWCharge", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWRecharge), this->device_type(), "wWRecharge", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWTempOK), this->device_type(), "wWTempOK", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWActive), this->device_type(), "wWActive", nullptr, nullptr);

    Mqtt::register_mqtt_ha_sensor(F_(heatingActivated), this->device_type(), "heatingActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(heatingTemp), this->device_type(), "heatingTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(pumpModMax), this->device_type(), "pumpModMax", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpModMin), this->device_type(), "pumpModMin", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpDelay), this->device_type(), "pumpDelay", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(burnMinPeriod), this->device_type(), "burnMinPeriod", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(burnMinPower), this->device_type(), "burnMinPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(burnMaxPower), this->device_type(), "burnMaxPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(boilHystOn), this->device_type(), "boilHystOn", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(boilHystOff), this->device_type(), "boilHystOff", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(setFlowTemp), this->device_type(), "setFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWSetPumpPower), this->device_type(), "wWSetPumpPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(wwMixTemperature), this->device_type(), "wwMixTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wwBufferBoilerTemperature), this->device_type(), "wwBufferBoilerTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWStarts), this->device_type(), "wWStarts", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWWorkM), this->device_type(), "wWWorkM", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(setBurnPow), this->device_type(), "setBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(burnStarts), this->device_type(), "burnStarts", nullptr, nullptr);
    */
}

// send stuff to the Web UI
void Boiler::device_info_web(JsonArray & root) {
    JsonObject dataElement;

    render_value_json(root, "", F_(serviceCode), serviceCodeChar_, nullptr);
    render_value_json(root, "", F_(serviceCodeNumber), serviceCode_, nullptr);
    render_value_json(root, "", F_(tapwaterActive), tap_water_active_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F_(heatingActive), heating_active_, nullptr, EMS_VALUE_BOOL);
    render_value_json(root, "", F_(selFlowTemp), selFlowTemp_, F_(degrees));
    render_value_json(root, "", F_(curFlowTemp), curFlowTemp_, F_(degrees), 10);
    render_value_json(root, "", F_(wWSelTemp), wWSelTemp_, F_(degrees));
    render_value_json(root, "", F_(wWSetTemp), wWSetTmp_, F_(degrees));
    render_value_json(root, "", F_(wWCurTmp), wWCurTmp_, F_(degrees), 10);
    render_value_json(root, "", F_(wWCurTmp2), wWCurTmp2_, F_(degrees), 10);
    render_value_json(root, "", F_(pumpMod), pumpMod_, F_(percent));
    render_value_json(root, "", F_(pumpMod2), pumpMod2_, F_(percent));
    render_value_json(root, "", F_(sysPress), sysPress_, F_(bar), 10);

    // TODO add remaining values to web
    /*
    Mqtt::register_mqtt_ha_sensor(F_(heatingActive), this->device_type(), "heatingActive", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(tapwaterActive), this->device_type(), "tapwaterActive", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(serviceCode), this->device_type(), "serviceCode", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(serviceCodeNumber), this->device_type(), "serviceCodeNumber", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWSelTemp), this->device_type(), "wWSelTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWSetTemp), this->device_type(), "wWSetTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWDisinfectionTemp), this->device_type(), "wWDisinfectionTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(selFlowTemp), this->device_type(), "selFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(selBurnPow), this->device_type(), "selBurnPow", F_(percent), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(curBurnPow), this->device_type(), "curBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpMod), this->device_type(), "pumpMod", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpMod2), this->device_type(), "pumpMod2", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(wWType), this->device_type(), "wWType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWChargeType), this->device_type(), "wWChargeType", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWCircPump), this->device_type(), "wWCircPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWCiPuMode), this->device_type(), "wWCiPuMode", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWCirc), this->device_type(), "wWCirc", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(outdoorTemp), this->device_type(), "outdoorTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWCurTmp), this->device_type(), "wWCurTmp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWCurTmp2), this->device_type(), "wWCurTmp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWCurFlow), this->device_type(), "wWCurFlow", F("l/min"), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(curFlowTemp), this->device_type(), "curFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(retTemp), this->device_type(), "retTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(switchTemp), this->device_type(), "switchTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(sysPress), this->device_type(), "sysPress", F_(bar), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(boilTemp), this->device_type(), "boilTemp", F_(degrees), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wwStorageTemp1), this->device_type(), "wwStorageTemp1", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wwStorageTemp2), this->device_type(), "wwStorageTemp2", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(exhaustTemp), this->device_type(), "exhaustTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWActivated), this->device_type(), "wWActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWOnetime), this->device_type(), "wWOnetime", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWDisinfecting), this->device_type(), "wWDisinfecting", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWCharge), this->device_type(), "wWCharge", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWRecharge), this->device_type(), "wWRecharge", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWTempOK), this->device_type(), "wWTempOK", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWActive), this->device_type(), "wWActive", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(burnGas), this->device_type(), "burnGas", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(flameCurr), this->device_type(), "flameCurr", F_(uA), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(heatPump), this->device_type(), "heatPump", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(fanWork), this->device_type(), "fanWork", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(ignWork), this->device_type(), "ignWork", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWHeat), this->device_type(), "wWHeat", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(heatingActivated), this->device_type(), "heatingActivated", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(heatingTemp), this->device_type(), "heatingTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(pumpModMax), this->device_type(), "pumpModMax", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpModMin), this->device_type(), "pumpModMin", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(pumpDelay), this->device_type(), "pumpDelay", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(burnMinPeriod), this->device_type(), "burnMinPeriod", F_(min), nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(burnMinPower), this->device_type(), "burnMinPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(burnMaxPower), this->device_type(), "burnMaxPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(boilHystOn), this->device_type(), "boilHystOn", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(boilHystOff), this->device_type(), "boilHystOff", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(setFlowTemp), this->device_type(), "setFlowTemp", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWSetPumpPower), this->device_type(), "wWSetPumpPower", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(wwMixTemperature), this->device_type(), "wwMixTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wwBufferBoilerTemperature), this->device_type(), "wwBufferBoilerTemperature", F_(degrees), F_(icontemperature));
    Mqtt::register_mqtt_ha_sensor(F_(wWStarts), this->device_type(), "wWStarts", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(wWWorkM), this->device_type(), "wWWorkM", nullptr, nullptr);
    Mqtt::register_mqtt_ha_sensor(F_(setBurnPow), this->device_type(), "setBurnPow", F_(percent), F_(iconpercent));
    Mqtt::register_mqtt_ha_sensor(F_(burnStarts), this->device_type(), "burnStarts", nullptr, nullptr);
*/
}

bool Boiler::command_info(const char * value, const int8_t id, JsonObject & output) {
    return (export_values(output));
}

// creates JSON doc from values
// returns false if empty
bool Boiler::export_values(JsonObject & output) {
    char s[10]; // for formatting strings

    // Hot tap water bool
    if (Helpers::hasValue(heating_active_, EMS_VALUE_BOOL)) {
        output["heatingActive"] = Helpers::render_value(s, heating_active_, EMS_VALUE_BOOL);
    }

    // Central heating bool
    if (Helpers::hasValue(tap_water_active_, EMS_VALUE_BOOL)) {
        output["tapwaterActive"] = Helpers::render_value(s, tap_water_active_, EMS_VALUE_BOOL);
    }

    // Warm Water comfort setting
    if (Helpers::hasValue(wWComfort_)) {
        if (wWComfort_ == 0x00) {
            output["wWComfort"] = "Hot";
        } else if (wWComfort_ == 0xD8) {
            output["wWComfort"] = "Eco";
        } else if (wWComfort_ == 0xEC) {
            output["wWComfort"] = "Intelligent";
        }
    }

    // Warm Water selected temperature
    if (Helpers::hasValue(wWSelTemp_)) {
        output["wWSelTemp"] = wWSelTemp_;
    }

    // Warm Water set temperature
    if (Helpers::hasValue(wWSetTmp_)) {
        output["wWSetTemp"] = wWSetTmp_;
    }

    // Warm Water disinfection temperature
    if (Helpers::hasValue(wWDisinfectTemp_)) {
        output["wWDisinfectionTemp"] = wWDisinfectTemp_;
    }

    // Selected flow temperature deg
    if (Helpers::hasValue(selFlowTemp_)) {
        output["selFlowTemp"] = selFlowTemp_;
    }

    // Burner selected max power %
    if (Helpers::hasValue(selBurnPow_)) {
        output["selBurnPow"] = selBurnPow_;
    }

    // Burner current power %
    if (Helpers::hasValue(curBurnPow_)) {
        output["curBurnPow"] = curBurnPow_;
    }

    // Pump modulation %
    if (Helpers::hasValue(pumpMod_)) {
        output["pumpMod"] = pumpMod_;
    }

    // Heat Pump modulation %
    if (Helpers::hasValue(pumpMod2_)) {
        output["pumpMod2"] = pumpMod2_;
    }

    // Warm Water type
    if (wWType_ == 0) { // no output if not set
        output["wWType"] = F("off");
    } else if (wWType_ == 1) {
        output["wWType"] = F("flow");
    } else if (wWType_ == 2) {
        output["wWType"] = F("buffered flow");
    } else if (wWType_ == 3) {
        output["wWType"] = F("buffer");
    } else if (wWType_ == 4) {
        output["wWType"] = F("layered buffer");
    }

    // Warm Water charging type
    if (Helpers::hasValue(wWChargeType_, EMS_VALUE_BOOL)) {
        output["wWChargeType"] = wWChargeType_ ? "3-way valve" : "charge pump";
    }

    // Warm Water circulation pump available bool
    if (Helpers::hasValue(wWCircPump_, EMS_VALUE_BOOL)) {
        output["wWCircPump"] = Helpers::render_value(s, wWCircPump_, EMS_VALUE_BOOL);
    }

    // Warm Water circulation pump freq
    if (Helpers::hasValue(wWCircPumpMode_)) {
        if (wWCircPumpMode_ == 7) {
            output["wWCiPuMode"] = F("continuous");
        } else {
            char s[7];
            char buffer[2];
            buffer[0] = (wWCircPumpMode_ % 10) + '0';
            buffer[1] = '\0';
            strlcpy(s, buffer, 7);
            strlcat(s, "x3min", 7);
            output["wWCiPuMode"] = s;
        }
    }

    // Warm Water circulation active bool
    if (Helpers::hasValue(wWCirc_, EMS_VALUE_BOOL)) {
        output["wWCirc"] = Helpers::render_value(s, wWCirc_, EMS_VALUE_BOOL);
    }

    // Outside temperature
    if (Helpers::hasValue(extTemp_)) {
        output["outdoorTemp"] = (float)extTemp_ / 10;
    }

    // Warm Water current temperature (intern)
    if (Helpers::hasValue(wWCurTmp_)) {
        output["wWCurTmp"] = (float)wWCurTmp_ / 10;
    }

    // Warm Water current temperature (extern)
    if (Helpers::hasValue(wWCurTmp2_)) {
        output["wWCurTmp2"] = (float)wWCurTmp2_ / 10;
    }

    // Warm Water current tap water flow l/min
    if (Helpers::hasValue(wWCurFlow_)) {
        output["wWCurFlow"] = (float)wWCurFlow_ / 10;
    }

    // Current flow temperature
    if (Helpers::hasValue(curFlowTemp_)) {
        output["curFlowTemp"] = (float)curFlowTemp_ / 10;
    }

    // Return temperature
    if (Helpers::hasValue(retTemp_)) {
        output["retTemp"] = (float)retTemp_ / 10;
    }

    // Mixing switch temperature
    if (Helpers::hasValue(switchTemp_)) {
        output["switchTemp"] = (float)switchTemp_ / 10;
    }

    // System pressure
    if (Helpers::hasValue(sysPress_)) {
        output["sysPress"] = (float)sysPress_ / 10;
    }

    // Max boiler temperature
    if (Helpers::hasValue(boilTemp_)) {
        output["boilTemp"] = (float)boilTemp_ / 10;
    }

    // Warm water storage temperature (intern)
    if (Helpers::hasValue(wwStorageTemp1_)) {
        output["wwStorageTemp1"] = (float)wwStorageTemp1_ / 10;
    }

    // Warm water storage temperature (extern)
    if (Helpers::hasValue(wwStorageTemp2_)) {
        output["wwStorageTemp2"] = (float)wwStorageTemp2_ / 10;
    }

    // Exhaust temperature
    if (Helpers::hasValue(exhaustTemp_)) {
        output["exhaustTemp"] = (float)exhaustTemp_ / 10;
    }

    // Warm Water activated bool
    if (Helpers::hasValue(wWActivated_, EMS_VALUE_BOOL)) {
        output["wWActivated"] = Helpers::render_value(s, wWActivated_, EMS_VALUE_BOOL);
    }

    // Warm Water one time charging bool
    if (Helpers::hasValue(wWOneTime_, EMS_VALUE_BOOL)) {
        output["wWOnetime"] = Helpers::render_value(s, wWOneTime_, EMS_VALUE_BOOL);
    }

    // Warm Water disinfecting bool
    if (Helpers::hasValue(wWDisinfecting_, EMS_VALUE_BOOL)) {
        output["wWDisinfecting"] = Helpers::render_value(s, wWDisinfecting_, EMS_VALUE_BOOL);
    }

    // Warm water charging bool
    if (Helpers::hasValue(wWCharging_, EMS_VALUE_BOOL)) {
        output["wWCharge"] = Helpers::render_value(s, wWCharging_, EMS_VALUE_BOOL);
    }

    // Warm water recharge bool
    if (Helpers::hasValue(wWRecharging_, EMS_VALUE_BOOL)) {
        output["wWRecharge"] = Helpers::render_value(s, wWRecharging_, EMS_VALUE_BOOL);
    }

    // Warm water temperature ok bool
    if (Helpers::hasValue(wWTemperatureOK_, EMS_VALUE_BOOL)) {
        output["wWTempOK"] = Helpers::render_value(s, wWTemperatureOK_, EMS_VALUE_BOOL);
    }

    // Warm water active bool
    if (Helpers::hasValue(wWActive_, EMS_VALUE_BOOL)) {
        output["wWActive"] = Helpers::render_value(s, wWActive_, EMS_VALUE_BOOL);
    }

    // Gas bool
    if (Helpers::hasValue(burnGas_, EMS_VALUE_BOOL)) {
        output["burnGas"] = Helpers::render_value(s, burnGas_, EMS_VALUE_BOOL);
    }

    // Flame current uA
    if (Helpers::hasValue(flameCurr_)) {
        output["flameCurr"] = (float)(int16_t)flameCurr_ / 10;
    }

    // Boiler pump bool
    if (Helpers::hasValue(heatPmp_, EMS_VALUE_BOOL)) {
        output["heatPump"] = Helpers::render_value(s, heatPmp_, EMS_VALUE_BOOL);
    }

    // Fan bool
    if (Helpers::hasValue(fanWork_, EMS_VALUE_BOOL)) {
        output["fanWork"] = Helpers::render_value(s, fanWork_, EMS_VALUE_BOOL);
    }

    // Ignition bool
    if (Helpers::hasValue(ignWork_, EMS_VALUE_BOOL)) {
        output["ignWork"] = Helpers::render_value(s, ignWork_, EMS_VALUE_BOOL);
    }

    // Warm Water charging bool
    if (Helpers::hasValue(wWHeat_, EMS_VALUE_BOOL)) {
        output["wWHeat"] = Helpers::render_value(s, wWHeat_, EMS_VALUE_BOOL);
    }

    // heating activated bool
    if (Helpers::hasValue(heating_activated_, EMS_VALUE_BOOL)) {
        output["heatingActivated"] = Helpers::render_value(s, heating_activated_, EMS_VALUE_BOOL);
    }

    // Heating temperature setting on the boiler
    if (Helpers::hasValue(heating_temp_)) {
        output["heatingTemp"] = heating_temp_;
    }

    // Boiler circuit pump modulation max power %
    if (Helpers::hasValue(pump_mod_max_)) {
        output["pumpModMax"] = pump_mod_max_;
    }

    // Boiler circuit pump modulation min power %
    if (Helpers::hasValue(pump_mod_min_)) {
        output["pumpModMin"] = pump_mod_min_;
    }

    // Boiler circuit pump delay time min
    if (Helpers::hasValue(pumpDelay_)) {
        output["pumpDelay"] = pumpDelay_;
    }

    // Boiler burner min period min
    if (Helpers::hasValue(burnPeriod_)) {
        output["burnMinPeriod"] = burnPeriod_;
    }

    // Boiler burner min power %
    if (Helpers::hasValue(burnPowermin_)) {
        output["burnMinPower"] = burnPowermin_;
    }

    // Boiler burner max power %
    if (Helpers::hasValue(burnPowermax_)) {
        output["burnMaxPower"] = burnPowermax_;
    }

    // Boiler temp hysteresis on degrees
    if (Helpers::hasValue(boilTemp_on_)) {
        output["boilHystOn"] = boilTemp_on_;
    }

    // Boiler temp hysteresis off degrees
    if (Helpers::hasValue(boilTemp_off_)) {
        output["boilHystOff"] = boilTemp_off_;
    }

    // Set Flow temperature
    if (Helpers::hasValue(setFlowTemp_)) {
        output["setFlowTemp"] = setFlowTemp_;
    }

    // Warm Water pump set power %
    if (Helpers::hasValue(setWWPumpPow_)) {
        output["wWSetPumpPower"] = setWWPumpPow_;
    }

    // Warm water mix temperature
    if (Helpers::hasValue(wwMixTemperature_)) {
        output["wwMixTemperature"] = wwMixTemperature_;
    }

    // Warm water buffer boiler temperature
    if (Helpers::hasValue(wwBufferBoilerTemperature_)) {
        output["wwBufferBoilerTemperature"] = wwBufferBoilerTemperature_;
    }

    // Warm Water # starts
    if (Helpers::hasValue(wWStarts_)) {
        output["wWStarts"] = wWStarts_;
    }

    // Warm Water active time
    if (Helpers::hasValue(wWWorkM_)) {
        output["wWWorkM"] = wWWorkM_;
    }

    // Total UBA working time
    if (Helpers::hasValue(UBAuptime_)) {
        output["UBAuptime"] = UBAuptime_;
    }

    // Boiler burner set power %
    if (Helpers::hasValue(setBurnPow_)) {
        output["setBurnPow_"] = setBurnPow_;
    }

    // Burner # starts
    if (Helpers::hasValue(burnStarts_)) {
        output["burnStarts"] = burnStarts_;
    }

    // Total burner operating time
    if (Helpers::hasValue(burnWorkMin_)) {
        output["burnWorkMin"] = burnWorkMin_;
    }

    // Total heat operating time
    if (Helpers::hasValue(heatWorkMin_)) {
        output["heatWorkMin"] = heatWorkMin_;
    }

    // Service Code
    // Service Code Number
    if (Helpers::hasValue(serviceCode_)) {
        output["serviceCode"]       = serviceCodeChar_;
        output["serviceCodeNumber"] = serviceCode_;
    }

    return (output.size());
}

// publish values via MQTT
void Boiler::publish_values() {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_LARGE> doc;
    JsonObject                                     output = doc.to<JsonObject>();
    if (export_values(output)) {
        Mqtt::publish(F("boiler_data"), doc.as<JsonObject>());
    }

    // send out heating and tapwater status - even if there is no change (force = true)
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
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_LARGE> doc;
    JsonObject                                     output = doc.to<JsonObject>();
    if (!export_values(output)) {
        return; // empty
    }

    print_value_json(shell, F("heatingActive"), F_(heatingActive), nullptr, output);
    print_value_json(shell, F("tapwaterActive"), F_(tapwaterActive), nullptr, output);
    print_value_json(shell, F("serviceCode"), F_(serviceCode), nullptr, output);
    print_value_json(shell, F("serviceCodeNumber"), F_(serviceCodeNumber), nullptr, output);
    print_value_json(shell, F("wWSelTemp"), F_(wWSelTemp), F_(degrees), output);
    print_value_json(shell, F("wWSetTemp"), F_(wWSetTemp), F_(degrees), output);
    print_value_json(shell, F("wWDisinfectionTemp"), F_(wWDisinfectionTemp), F_(degrees), output);
    print_value_json(shell, F("selFlowTemp"), F_(selFlowTemp), F_(degrees), output);
    print_value_json(shell, F("selBurnPow"), F_(selBurnPow), F_(percent), output);
    print_value_json(shell, F("curBurnPow"), F_(curBurnPow), F_(percent), output);
    print_value_json(shell, F("pumpMod"), F_(pumpMod), F_(percent), output);
    print_value_json(shell, F("pumpMod2"), F_(pumpMod2), F_(percent), output);
    print_value_json(shell, F("wWType"), F_(wWType), nullptr, output);
    print_value_json(shell, F("wWChargeType"), F_(wWChargeType), nullptr, output);
    print_value_json(shell, F("wWCircPump"), F_(wWCircPump), nullptr, output);
    print_value_json(shell, F("wWCiPuMode"), F_(wWCiPuMode), nullptr, output);
    print_value_json(shell, F("wWCirc"), F_(wWCirc), nullptr, output);
    print_value_json(shell, F("outdoorTemp"), F_(outdoorTemp), F_(degrees), output);
    print_value_json(shell, F("wWCurTmp"), F_(wWCurTmp), F_(degrees), output);
    print_value_json(shell, F("wWCurTmp2"), F_(wWCurTmp2), F_(degrees), output);
    print_value_json(shell, F("wWCurFlow"), F_(wWCurFlow), F("l/min"), output);
    print_value_json(shell, F("curFlowTemp"), F_(curFlowTemp), F_(degrees), output);
    print_value_json(shell, F("retTemp"), F_(retTemp), F_(degrees), output);
    print_value_json(shell, F("switchTemp"), F_(switchTemp), F_(degrees), output);
    print_value_json(shell, F("sysPress"), F_(sysPress), nullptr, output);
    print_value_json(shell, F("boilTemp"), F_(boilTemp), F_(degrees), output);
    print_value_json(shell, F("wwStorageTemp1"), F_(wwStorageTemp1), F_(degrees), output);
    print_value_json(shell, F("wwStorageTemp2"), F_(wwStorageTemp2), F_(degrees), output);
    print_value_json(shell, F("exhaustTemp"), F_(exhaustTemp), F_(degrees), output);
    print_value_json(shell, F("wWActivated"), F_(wWActivated), nullptr, output);
    print_value_json(shell, F("wWOnetime"), F_(wWOnetime), nullptr, output);
    print_value_json(shell, F("wWDisinfecting"), F_(wWDisinfecting), nullptr, output);
    print_value_json(shell, F("wWCharge"), F_(wWCharge), nullptr, output);
    print_value_json(shell, F("wWRecharge"), F_(wWRecharge), nullptr, output);
    print_value_json(shell, F("wWTempOK"), F_(wWTempOK), nullptr, output);
    print_value_json(shell, F("wWActive"), F_(wWActive), nullptr, output);
    print_value_json(shell, F("burnGas"), F_(burnGas), nullptr, output);
    print_value_json(shell, F("flameCurr"), F_(flameCurr), F_(uA), output);
    print_value_json(shell, F("heatPump"), F_(heatPump), nullptr, output);
    print_value_json(shell, F("fanWork"), F_(fanWork), nullptr, output);
    print_value_json(shell, F("ignWork"), F_(ignWork), nullptr, output);
    print_value_json(shell, F("wWHeat"), F_(wWHeat), nullptr, output);
    print_value_json(shell, F("heatingActivated"), F_(heatingActivated), nullptr, output);
    print_value_json(shell, F("heatingTemp"), F_(heatingTemp), F_(degrees), output);
    print_value_json(shell, F("pumpModMax"), F_(pumpModMax), F_(percent), output);
    print_value_json(shell, F("pumpModMin"), F_(pumpModMin), F_(percent), output);
    print_value_json(shell, F("pumpDelay"), F_(pumpDelay), F_(min), output);
    print_value_json(shell, F("burnMinPeriod"), F_(burnMinPeriod), F_(min), output);
    print_value_json(shell, F("burnMinPower"), F_(burnMinPower), F_(percent), output);
    print_value_json(shell, F("burnMaxPower"), F_(burnMaxPower), F_(percent), output);
    print_value_json(shell, F("boilHystOn"), F_(boilHystOn), F_(degrees), output);
    print_value_json(shell, F("boilHystOff"), F_(boilHystOff), F_(degrees), output);
    print_value_json(shell, F("setFlowTemp"), F_(setFlowTemp), F_(degrees), output);
    print_value_json(shell, F("wWSetPumpPower"), F_(wWSetPumpPower), F_(percent), output);
    print_value_json(shell, F("wwMixTemperature"), F_(wwMixTemperature), F_(degrees), output);
    print_value_json(shell, F("wwBufferBoilerTemperature"), F_(wwBufferBoilerTemperature), F_(degrees), output);
    print_value_json(shell, F("wWStarts"), F_(wWStarts), nullptr, output);
    print_value_json(shell, F("wWWorkM"), F_(wWWorkM), nullptr, output);
    print_value_json(shell, F("setBurnPow"), F_(setBurnPow), F_(percent), output);
    print_value_json(shell, F("burnStarts"), F_(burnStarts), nullptr, output);

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
 */
void Boiler::check_active() {
    if ((boilerState_ & 0x09) != (last_boilerState & 0x09)) {
        char s[7];
        bool b = ((boilerState_ & 0x09) == 0x09);
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s, b));
        heating_active_ = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    }

    if ((boilerState_ & 0x0A) != (last_boilerState & 0x0A)) {
        char s[7];
        bool b = ((boilerState_ & 0x0A) == 0x0A);
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, b));
        tap_water_active_ = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
        EMSESP::tap_water_active(b); // let EMS-ESP know, used in the Shower class
    }

    last_boilerState = boilerState_;

    /*
    // hot tap water, using flow to check instead of the burner power
    // send these values back to the main EMSESP, so other classes (e.g. Shower) can use it
    if (Helpers::hasValue(wWCurFlow_) && Helpers::hasValue(burnGas_) && (wWType_ > 0) && (wWType_ < 3)) {
        tap_water_active_ = ((wWCurFlow_ != 0) && (burnGas_ != EMS_VALUE_BOOL_OFF));
        EMSESP::tap_water_active(tap_water_active_);
    }

    // heating
    // using a quick hack for checking the heating by looking at the Selected Flow Temp, but doesn't work for all boilers apparently
    if (Helpers::hasValue(selFlowTemp_) && Helpers::hasValue(burnGas_)) {
        heating_active_ = (!tap_water_active_ && ((selFlowTemp_ >= EMS_BOILER_SELFLOWTEMP_HEATING) && (burnGas_ != EMS_VALUE_BOOL_OFF)));
    }

    // see if the heating or hot tap water has changed, if so send
    // last_boilerActive stores heating in bit 1 and tap water in bit 2
    uint8_t latest_boilerState = (tap_water_active_ << 1) + heating_active_;
    if (latest_boilerState != last_boilerState) {
        last_boilerState = latest_boilerState;
        static char s[10];
        Mqtt::publish(F("tapwater_active"), Helpers::render_boolean(s, tap_water_active_));
        static char s2[10];
        Mqtt::publish(F("heating_active"), Helpers::render_boolean(s2, heating_active_));
    }
    */
}

// 0x33
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWActivated_, 1);    // 0xFF means on
    changed_ |= telegram->read_value(wWCircPump_, 6);     // 0xFF means on
    changed_ |= telegram->read_value(wWCircPumpMode_, 7); // 1=1x3min... 6=6x3min, 7=continuous
    changed_ |= telegram->read_value(wWChargeType_, 10);  // 0 = charge pump, 0xff = 3-way valve
    changed_ |= telegram->read_value(wWSelTemp_, 2);
    changed_ |= telegram->read_value(wWDisinfectTemp_, 8);
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
    changed_ |= telegram->read_bitvalue(heatPmp_, 7, 5);
    changed_ |= telegram->read_bitvalue(wWHeat_, 7, 6);
    changed_ |= telegram->read_bitvalue(wWCirc_, 7, 7);

    // warm water storage sensors (if present)
    // wwStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/proddy/EMS-ESP/issues/206
    changed_ |= telegram->read_value(wwStorageTemp1_, 9);  // 0x8300 if not available
    changed_ |= telegram->read_value(wwStorageTemp2_, 11); // 0x8000 if not available - this is boiler temp

    changed_ |= telegram->read_value(retTemp_, 13);
    changed_ |= telegram->read_value(flameCurr_, 15);
    changed_ |= telegram->read_value(serviceCode_, 20);

    // system pressure. FF means missing
    changed_ |= telegram->read_value(sysPress_, 17); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        changed_ |= telegram->read_value(serviceCodeChar_[0], 18);
        changed_ |= telegram->read_value(serviceCodeChar_[1], 19);
        serviceCodeChar_[2] = '\0'; // null terminate string
    }

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
    changed_ |= telegram->read_value(heating_activated_, 0);
    changed_ |= telegram->read_value(heating_temp_, 1);
    changed_ |= telegram->read_value(burnPowermax_, 2);
    changed_ |= telegram->read_value(burnPowermin_, 3);
    changed_ |= telegram->read_value(boilTemp_off_, 4);
    changed_ |= telegram->read_value(boilTemp_on_, 5);
    changed_ |= telegram->read_value(burnPeriod_, 6);
    changed_ |= telegram->read_value(pumpDelay_, 8);
    changed_ |= telegram->read_value(pump_mod_max_, 9);
    changed_ |= telegram->read_value(pump_mod_min_, 10);
}

/*
 * UBAMonitorWW - type 0x34 - warm water monitor. 19 bytes long
 * received every 10 seconds
 */
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wWSetTmp_, 0);
    changed_ |= telegram->read_value(wWCurTmp_, 1);
    changed_ |= telegram->read_value(wWCurTmp2_, 3);
    changed_ |= telegram->read_value(wWCurFlow_, 9);
    changed_ |= telegram->read_value(wWType_, 8);

    changed_ |= telegram->read_value(wWWorkM_, 10, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 13, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 5, 1);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 5, 2);
    changed_ |= telegram->read_bitvalue(wWCharging_, 5, 3);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 5, 4);
    changed_ |= telegram->read_bitvalue(wWTemperatureOK_, 5, 5);
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
        changed_ |= telegram->read_value(serviceCodeChar_[0], 4);
        changed_ |= telegram->read_value(serviceCodeChar_[1], 5);
        serviceCodeChar_[2] = '\0';
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
    changed_ |= telegram->read_value(extTemp_, 0);
    changed_ |= telegram->read_value(boilTemp_, 2);
    changed_ |= telegram->read_value(exhaustTemp_, 4);
    changed_ |= telegram->read_value(switchTemp_, 25); // only if there is a mixing module present
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
    changed_ |= telegram->read_bitvalue(heatPmp_, 2, 5);
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
    changed_ |= telegram->read_value(heating_activated_, 0);
    changed_ |= telegram->read_value(heating_temp_, 1);
    changed_ |= telegram->read_value(burnPowermax_, 6);
    changed_ |= telegram->read_value(burnPowermin_, 7);
    changed_ |= telegram->read_value(boilTemp_off_, 8);
    changed_ |= telegram->read_value(boilTemp_on_, 9);
    changed_ |= telegram->read_value(burnPeriod_, 10);
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
    changed_ |= telegram->read_value(wWSetTmp_, 0);
    changed_ |= telegram->read_value(wWCurTmp_, 1);
    changed_ |= telegram->read_value(wWCurTmp2_, 3);

    changed_ |= telegram->read_value(wWWorkM_, 17, 3);  // force to 3 bytes
    changed_ |= telegram->read_value(wWStarts_, 14, 3); // force to 3 bytes

    changed_ |= telegram->read_bitvalue(wWOneTime_, 12, 2);
    changed_ |= telegram->read_bitvalue(wWDisinfecting_, 12, 3);
    changed_ |= telegram->read_bitvalue(wWCharging_, 12, 4);
    changed_ |= telegram->read_bitvalue(wWRecharging_, 13, 4);
    changed_ |= telegram->read_bitvalue(wWTemperatureOK_, 13, 5);
    changed_ |= telegram->read_bitvalue(wWCircPump_, 13, 2);

    // changed_ |= telegram->read_value(wWActivated_, 20); // Activated is in 0xEA, this is something other 0/100%
    changed_ |= telegram->read_value(wWSelTemp_, 10);
    changed_ |= telegram->read_value(wWDisinfectTemp_, 9);
}

// 0x2A - MC10Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/proddy/EMS-ESP/issues/397
void Boiler::process_MC10Status(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(wwMixTemperature_, 14);
    changed_ |= telegram->read_value(wwBufferBoilerTemperature_, 18);
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(extTemp_, 0);
}

// UBASetPoint 0x1A
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    changed_ |= telegram->read_value(setFlowTemp_, 0);  // boiler set temp from thermostat
    changed_ |= telegram->read_value(setBurnPow_, 1);   // max output power in %
    changed_ |= telegram->read_value(setWWPumpPow_, 2); // ww pump speed/power?
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
        return false;
    }

    LOG_INFO(F("Setting boiler warm water temperature to %d C"), v);
    if (get_toggle_fetch(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 6, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 2, v, EMS_TYPE_UBAParameterWW);
        write_command(EMS_TYPE_UBAFlags, 3, v, EMS_TYPE_UBAParameterWW); // for i9000, see #397
    }

    return true;
}

// flow temp
bool Boiler::set_flow_temp(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
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
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameters)) {
        LOG_INFO(F("Setting boiler pump delay to %d min"), v);
        write_command(EMS_TYPE_UBAParameters, 8, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// note some boilers do not have this setting, than it's done by thermostat
// on a RC35 it's by EMSESP::send_write_request(0x37, 0x10, 2, &set, 1, 0); (set is 1,2,3) 1=hot, 2=eco, 3=intelligent
bool Boiler::set_warmwater_mode(const char * value, const int8_t id) {
    if (value == nullptr) {
        return false;
    }

    if (!get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        return false;
    }

    uint8_t set;
    if (strcmp(value, "hot") == 0) {
        LOG_INFO(F("Setting boiler warm water to Hot"));
        set = 0x00;
    } else if (strcmp(value, "eco") == 0) {
        LOG_INFO(F("Setting boiler warm water to Eco"));
        set = 0xD8;
    } else if (strcmp(value, "intelligent") == 0) {
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
        return false;
    }

    LOG_INFO(F("Setting boiler warm water %s"), v ? "on" : "off");

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
        return false;
    }

    LOG_INFO(F("Setting tap warm tap water %s"), v ? "on" : "off");
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
        return false;
    }

    LOG_INFO(F("Setting boiler warm water OneTime loading %s"), v ? "on" : "off");
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
        return false;
    }

    LOG_INFO(F("Setting boiler warm water circulation %s"), v ? "on" : "off");
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
        return false;
    }

    LOG_INFO(F("Setting boiler warm water circulation %s"), v ? "on" : "off");

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 10, v ? 0x01 : 0x00, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 6, v ? 0xFF : 0x00, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// Set the mode of circulation, 1x3min, ... 6x3min, continous
// true = on, false = off
bool Boiler::set_warmwater_circulation_mode(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (get_toggle_fetch(EMS_TYPE_UBAParameterWW)) {
        if (v < 7) {
            LOG_INFO(F("Setting circulation mode %dx3min"), v);
        } else {
            LOG_INFO(F("Setting circulation mode continous"));
        }
        write_command(EMS_TYPE_UBAParameterWW, 6, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

} // namespace emsesp
