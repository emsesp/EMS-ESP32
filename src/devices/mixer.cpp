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

#include "mixer.h"

namespace emsesp {

REGISTER_FACTORY(Mixer, EMSdevice::DeviceType::MIXER);

uuid::log::Logger Mixer::logger_{F_(mixer), uuid::log::Facility::CONSOLE};

Mixer::Mixer(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Mixer with device ID 0x%02X"), device_id);

    if (flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        if (device_id <= 0x27) {
            // telegram handlers 0x20 - 0x27 for HC
            register_telegram_type(device_id - 0x20 + 0x02D7, F("MMPLUSStatusMessage_HC"), true, [&](std::shared_ptr<const Telegram> t) { process_MMPLUSStatusMessage_HC(t); });
        } else {
            // telegram handlers for warm water/DHW 0x28, 0x29
            register_telegram_type(device_id - 0x28 + 0x0331, F("MMPLUSStatusMessage_WWC"), true, [&](std::shared_ptr<const Telegram> t) { process_MMPLUSStatusMessage_WWC(t); });
        }
    }

    // EMS 1.0
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        register_telegram_type(0x00AA, F("MMConfigMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMConfigMessage(t); });
        register_telegram_type(0x00AB, F("MMStatusMessage"), true, [&](std::shared_ptr<const Telegram> t) { process_MMStatusMessage(t); });
        register_telegram_type(0x00AC, F("MMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_MMSetMessage(t); });
    }

    // HT3
    if (flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        register_telegram_type(0x010C, F("IPMSetMessage"), false, [&](std::shared_ptr<const Telegram> t) { process_IPMStatusMessage(t); });
    }

    // register the device values and set hc_ and type_
    if (device_id <= 0x27) {
        type_       = Type::HC;
        hc_         = device_id - 0x20 + 1;
        uint8_t tag = TAG_HC1 + hc_ - 1;
        register_device_value(tag, &id_, DeviceValueType::UINT, nullptr, F("id"), nullptr); // empty full name to prevent being shown in web or console
        register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT, nullptr, F("flowSetTemp"), F("setpoint flow temperature"), DeviceValueUOM::DEGREES);
        register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, FL_(div10), F("flowTempHc"), F("flow temperature in assigned hc (TC1)"), DeviceValueUOM::DEGREES);
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, nullptr, F("pumpStatus"), F("pump status in assigned hc (PC1)"), DeviceValueUOM::PUMP);
        register_device_value(tag, &status_, DeviceValueType::INT, nullptr, F("valveStatus"), F("mixing valve actuator in assigned hc (VC1)"), DeviceValueUOM::PERCENT);
    } else {
        type_       = Type::WWC;
        hc_         = device_id - 0x28 + 1;
        uint8_t tag = TAG_WWC1 + hc_ - 1;
        register_device_value(tag, &id_, DeviceValueType::UINT, nullptr, F("id"), nullptr); // empty full name to prevent being shown in web or console
        register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, FL_(div10), F("wwTemp"), F("current warm water temperature"), DeviceValueUOM::DEGREES);
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, nullptr, F("pumpStatus"), F("pump status in assigned hc (PC1)"), DeviceValueUOM::PUMP);
        register_device_value(tag, &status_, DeviceValueType::INT, nullptr, F("tempStatus"), F("temperature switch in assigned hc (MC1)"));
    }

    id_ = product_id;
}

// publish HA config
bool Mixer::publish_ha_config() {
    // if we don't have valid values for this HC don't add it ever again
    if (!Helpers::hasValue(pumpStatus_)) {
        return false;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;

    char uniq_id[20];
    snprintf_P(uniq_id, sizeof(uniq_id), PSTR("Mixer%02X"), device_id() - 0x20 + 1);
    doc["uniq_id"] = uniq_id;

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/mixer_data"), Mqtt::base().c_str());
    doc["stat_t"] = stat_t;

    char name[20];
    snprintf_P(name, sizeof(name), PSTR("Mixer %02X"), device_id() - 0x20 + 1);
    doc["name"] = name;

    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Mixer");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = this->name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-mixer");

    // determine the topic, if its HC and WWC. This is determined by the incoming telegram types.
    std::string topic(Mqtt::MQTT_TOPIC_MAX_SIZE, '\0');
    if (type_ == Type::HC) {
        snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/sensor/%s/mixer_hc%d/config"), Mqtt::base().c_str(), hc_);
    } else {
        snprintf_P(&topic[0], topic.capacity() + 1, PSTR("homeassistant/sensor/%s/mixer_wwc%d/config"), Mqtt::base().c_str(), hc_); // WWC
    }

    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// heating circuits 0x02D7, 0x02D8 etc...
// e.g.  A0 00 FF 00 01 D7 00 00 00 80 00 00 00 00 03 C5
//       A0 0B FF 00 01 D7 00 00 00 80 00 00 00 00 03 80
void Mixer::process_MMPLUSStatusMessage_HC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(flowTempHc_, 3)); // is * 10
    has_update(telegram->read_value(flowSetTemp_, 5));
    has_update(telegram->read_bitvalue(pumpStatus_, 0, 0));
    has_update(telegram->read_value(status_, 2)); // valve status
}

// Mixer warm water loading/DHW - 0x0331, 0x0332
// e.g. A9 00 FF 00 02 32 02 6C 00 3C 00 3C 3C 46 02 03 03 00 3C // on 0x28
//      A8 00 FF 00 02 31 02 35 00 3C 00 3C 3C 46 02 03 03 00 3C // in 0x29
void Mixer::process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(flowTempHc_, 0)); // is * 10
    has_update(telegram->read_bitvalue(pumpStatus_, 2, 0));
    has_update(telegram->read_value(status_, 11)); // temp status
}

// Mixer IMP - 0x010C
// e.g.  A0 00 FF 00 00 0C 01 00 00 00 00 00 54
//       A1 00 FF 00 00 0C 02 04 00 01 1D 00 82
void Mixer::process_IPMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    // check if circuit is active, 0-off, 1-unmixed, 2-mixed
    uint8_t ismixed = 0;
    telegram->read_value(ismixed, 0);
    if (ismixed == 0) {
        return;
    }

    // do we have a mixed circuit
    if (ismixed == 2) {
        has_update(telegram->read_value(flowTempHc_, 3)); // is * 10
        has_update(telegram->read_value(status_, 2));     // valve status
    }

    has_update(telegram->read_bitvalue(pumpStatus_, 1, 0)); // pump is also in unmixed circuits
    has_update(telegram->read_value(flowSetTemp_, 5));      // flowSettemp is also in unmixed circuits, see #711
}

// Mixer on a MM10 - 0xAB
// e.g. Mixer Module -> All, type 0xAB, telegram: 21 00 AB 00 2D 01 BE 64 04 01 00 (CRC=15) #data=7
// see also https://github.com/proddy/EMS-ESP/issues/386
void Mixer::process_MMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    // the heating circuit is determine by which device_id it is, 0x20 - 0x23
    // 0x21 is position 2. 0x20 is typically reserved for the WM10 switch module
    // see https://github.com/proddy/EMS-ESP/issues/270 and https://github.com/proddy/EMS-ESP/issues/386#issuecomment-629610918

    has_update(telegram->read_value(flowTempHc_, 1));       // is * 10
    has_update(telegram->read_bitvalue(pumpStatus_, 3, 2)); // is 0 or 0x64 (100%), check only bit 2
    has_update(telegram->read_value(flowSetTemp_, 0));
    has_update(telegram->read_value(status_, 4)); // valve status -100 to 100
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Mixer on a MM10 - 0xAA
// e.g. Thermostat -> Mixer Module, type 0xAA, telegram: 10 21 AA 00 FF 0C 0A 11 0A 32 xx
void Mixer::process_MMConfigMessage(std::shared_ptr<const Telegram> telegram) {
    // pos 0: active FF = on
    // pos 1: valve runtime 0C = 120 sec in units of 10 sec
}

// Mixer on a MM10 - 0xAC
// e.g. Thermostat -> Mixer Module, type 0xAC, telegram: 10 21 AC 00 1E 64 01 AB
void Mixer::process_MMSetMessage(std::shared_ptr<const Telegram> telegram) {
    // pos 0: flowtemp setpoint 1E = 30°C
    // pos 1: position in %
}

#pragma GCC diagnostic pop

} // namespace emsesp
