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

#include "solar.h"

namespace emsesp {

REGISTER_FACTORY(Solar, EMSdevice::DeviceType::SOLAR);

uuid::log::Logger Solar::logger_{F_(solar), uuid::log::Facility::CONSOLE};

Solar::Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    LOG_DEBUG(F("Adding new Solar module with device ID 0x%02X"), device_id);

    // telegram handlers
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_telegram_type(0x97, F("SM10Monitor"), false, MAKE_PF_CB(process_SM10Monitor));
        register_telegram_type(0x96, F("SM10Config"), true, MAKE_PF_CB(process_SM10Config));
        EMSESP::send_read_request(0x97, device_id);
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        if (device_id == 0x2A) {
            register_telegram_type(0x07D6, F("SM100wwTemperature"), false, MAKE_PF_CB(process_SM100wwTemperature));
            register_telegram_type(0x07AA, F("SM100wwStatus"), false, MAKE_PF_CB(process_SM100wwStatus));
            register_telegram_type(0x07AB, F("SM100wwCommand"), false, MAKE_PF_CB(process_SM100wwCommand));
        } else {
            register_telegram_type(0xF9, F("ParamCfg"), false, MAKE_PF_CB(process_SM100ParamCfg));
            register_telegram_type(0x0358, F("SM100SystemConfig"), true, MAKE_PF_CB(process_SM100SystemConfig));
            register_telegram_type(0x035A, F("SM100SolarCircuitConfig"), true, MAKE_PF_CB(process_SM100SolarCircuitConfig));
            register_telegram_type(0x0362, F("SM100Monitor"), true, MAKE_PF_CB(process_SM100Monitor));
            register_telegram_type(0x0363, F("SM100Monitor2"), true, MAKE_PF_CB(process_SM100Monitor2));
            register_telegram_type(0x0366, F("SM100Config"), true, MAKE_PF_CB(process_SM100Config));
            register_telegram_type(0x0364, F("SM100Status"), false, MAKE_PF_CB(process_SM100Status));
            register_telegram_type(0x036A, F("SM100Status2"), false, MAKE_PF_CB(process_SM100Status2));
            register_telegram_type(0x0380, F("SM100CollectorConfig"), true, MAKE_PF_CB(process_SM100CollectorConfig));
            register_telegram_type(0x038E, F("SM100Energy"), true, MAKE_PF_CB(process_SM100Energy));
            register_telegram_type(0x0391, F("SM100Time"), true, MAKE_PF_CB(process_SM100Time));
        }
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_telegram_type(0x0103, F("ISM1StatusMessage"), true, MAKE_PF_CB(process_ISM1StatusMessage));
        register_telegram_type(0x0101, F("ISM1Set"), true, MAKE_PF_CB(process_ISM1Set));
    }

    // device values...

    // special case for a device_id with 0x2A where it's not actual a solar module
    if (device_id == 0x2A) {
        register_device_value(TAG_NONE, &type_, DeviceValueType::TEXT, nullptr, FL_(type), DeviceValueUOM::NONE);
        strlcpy(type_, "warm water circuit", sizeof(type_));
        register_device_value(TAG_NONE, &wwTemp_1_, DeviceValueType::UINT, nullptr, FL_(wwTemp1), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &wwTemp_3_, DeviceValueType::UINT, nullptr, FL_(wwTemp3), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &wwTemp_4_, DeviceValueType::UINT, nullptr, FL_(wwTemp4), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &wwTemp_5_, DeviceValueType::UINT, nullptr, FL_(wwTemp5), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &wwTemp_7_, DeviceValueType::UINT, nullptr, FL_(wwTemp7), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &wwPump_, DeviceValueType::UINT, nullptr, FL_(wwPump), DeviceValueUOM::DEGREES);
        return;
    }

    register_device_value(TAG_NONE, &id_, DeviceValueType::UINT, nullptr, FL_(ID), DeviceValueUOM::NONE);
    id_ = product_id;

    register_device_value(TAG_NONE, &collectorTemp_, DeviceValueType::SHORT, FL_(div10), FL_(collectorTemp), DeviceValueUOM::DEGREES);
    register_device_value(TAG_NONE, &tankBottomTemp_, DeviceValueType::SHORT, FL_(div10), FL_(tankBottomTemp), DeviceValueUOM::DEGREES);
    register_device_value(TAG_NONE, &solarPump_, DeviceValueType::BOOL, nullptr, FL_(solarPump), DeviceValueUOM::BOOLEAN);
    register_device_value(TAG_NONE, &pumpWorkTime_, DeviceValueType::TIME, nullptr, FL_(pumpWorkTime), DeviceValueUOM::MINUTES);
    register_device_value(TAG_NONE, &tankMaxTemp_, DeviceValueType::UINT, nullptr, FL_(tankMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_TankMaxTemp));

    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_device_value(TAG_NONE, &solarPumpModulation_, DeviceValueType::UINT, nullptr, FL_(solarPumpModulation), DeviceValueUOM::PERCENT);
        register_device_value(TAG_NONE, &solarPumpMinMod_, DeviceValueType::UINT, nullptr, FL_(pumpMinMod), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_PumpMinMod));
        register_device_value(
            TAG_NONE, &solarPumpTurnonDiff_, DeviceValueType::UINT, nullptr, FL_(solarPumpTurnonDiff), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_TurnonDiff));
        register_device_value(
            TAG_NONE, &solarPumpTurnoffDiff_, DeviceValueType::UINT, nullptr, FL_(solarPumpTurnoffDiff), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_TurnoffDiff));
        register_device_value(TAG_NONE, &collectorShutdown_, DeviceValueType::BOOL, nullptr, FL_(collectorShutdown), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &tankHeated_, DeviceValueType::BOOL, nullptr, FL_(tankHeated), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &solarPower_, DeviceValueType::ULONG, nullptr, FL_(solarPower), DeviceValueUOM::W);
        register_device_value(TAG_NONE, &energyLastHour_, DeviceValueType::ULONG, FL_(div10), FL_(energyLastHour), DeviceValueUOM::WH);
        register_device_value(TAG_NONE, &maxFlow_, DeviceValueType::UINT, FL_(div10), FL_(maxFlow), DeviceValueUOM::LMIN, MAKE_CF_CB(set_SM10MaxFlow));
        register_device_value(TAG_NONE, &wwMinTemp_, DeviceValueType::UINT, nullptr, FL_(wwMinTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMinTemp));
        register_device_value(TAG_NONE, &solarIsEnabled_, DeviceValueType::BOOL, nullptr, FL_(activated), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_solarEnabled));
    }
    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_device_value(TAG_NONE, &collectorShutdown_, DeviceValueType::BOOL, nullptr, FL_(collectorShutdown), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &tankHeated_, DeviceValueType::BOOL, nullptr, FL_(tankHeated), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &energyLastHour_, DeviceValueType::ULONG, FL_(div10), FL_(energyLastHour), DeviceValueUOM::WH);
    }
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        register_device_value(TAG_NONE, &solarPumpModulation_, DeviceValueType::UINT, nullptr, FL_(solarPumpModulation), DeviceValueUOM::PERCENT);
        register_device_value(TAG_NONE, &solarPumpMinMod_, DeviceValueType::UINT, nullptr, FL_(pumpMinMod), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_PumpMinMod));
        register_device_value(
            TAG_NONE, &solarPumpTurnonDiff_, DeviceValueType::UINT, FL_(div10), FL_(solarPumpTurnonDiff), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_TurnonDiff));
        register_device_value(TAG_NONE,
                              &solarPumpTurnoffDiff_,
                              DeviceValueType::UINT,
                              FL_(div10),
                              FL_(solarPumpTurnoffDiff),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_TurnoffDiff));
        register_device_value(TAG_NONE, &tankBottomTemp2_, DeviceValueType::SHORT, FL_(div10), FL_(tank2BottomTemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &heatExchangerTemp_, DeviceValueType::SHORT, FL_(div10), FL_(heatExchangerTemp), DeviceValueUOM::DEGREES);
        register_device_value(TAG_NONE, &cylinderPumpModulation_, DeviceValueType::UINT, nullptr, FL_(cylinderPumpModulation), DeviceValueUOM::PERCENT);
        register_device_value(TAG_NONE, &valveStatus_, DeviceValueType::BOOL, nullptr, FL_(valveStatus), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &tankHeated_, DeviceValueType::BOOL, nullptr, FL_(tankHeated), DeviceValueUOM::BOOLEAN);
        register_device_value(TAG_NONE, &collectorShutdown_, DeviceValueType::BOOL, nullptr, FL_(collectorShutdown), DeviceValueUOM::BOOLEAN);
        register_device_value(
            TAG_NONE, &collectorMaxTemp_, DeviceValueType::UINT, nullptr, FL_(collectorMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_CollectorMaxTemp));
        register_device_value(
            TAG_NONE, &collectorMinTemp_, DeviceValueType::UINT, nullptr, FL_(collectorMinTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_CollectorMinTemp));
        register_device_value(TAG_NONE, &energyLastHour_, DeviceValueType::ULONG, FL_(div10), FL_(energyLastHour), DeviceValueUOM::WH);
        register_device_value(TAG_NONE, &energyToday_, DeviceValueType::ULONG, nullptr, FL_(energyToday), DeviceValueUOM::WH);
        register_device_value(TAG_NONE, &energyTotal_, DeviceValueType::ULONG, FL_(div10), FL_(energyTotal), DeviceValueUOM::KWH);

        register_device_value(TAG_NONE,
                              &heatTransferSystem_,
                              DeviceValueType::BOOL,
                              nullptr,
                              FL_(heatTransferSystem),
                              DeviceValueUOM::BOOLEAN,
                              MAKE_CF_CB(set_heatTransferSystem));
        register_device_value(TAG_NONE, &externalTank_, DeviceValueType::BOOL, nullptr, FL_(externalTank), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_externalTank));
        register_device_value(
            TAG_NONE, &thermalDisinfect_, DeviceValueType::BOOL, nullptr, FL_(thermalDisinfect), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_thermalDisinfect));
        register_device_value(TAG_NONE, &heatMetering_, DeviceValueType::BOOL, nullptr, FL_(heatMetering), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_heatMetering));
        register_device_value(TAG_NONE, &solarIsEnabled_, DeviceValueType::BOOL, nullptr, FL_(activated), DeviceValueUOM::BOOLEAN, MAKE_CF_CB(set_solarEnabled));

        // telegram 0x035A
        register_device_value(
            TAG_NONE, &solarPumpMode_, DeviceValueType::ENUM, FL_(enum_solarmode), FL_(solarPumpMode), DeviceValueUOM::LIST, MAKE_CF_CB(set_solarMode));
        register_device_value(TAG_NONE,
                              &solarPumpKick_,
                              DeviceValueType::BOOL,
                              nullptr,
                              FL_(solarPumpKick),
                              DeviceValueUOM::BOOLEAN,
                              MAKE_CF_CB(set_solarPumpKick)); // pump kick for vacuum collector, 00=off
        register_device_value(TAG_NONE,
                              &plainWaterMode_,
                              DeviceValueType::BOOL,
                              nullptr,
                              FL_(plainWaterMode),
                              DeviceValueUOM::BOOLEAN,
                              MAKE_CF_CB(set_plainWaterMode)); // system does not use antifreeze, 00=off
        register_device_value(TAG_NONE,
                              &doubleMatchFlow_,
                              DeviceValueType::BOOL,
                              nullptr,
                              FL_(doubleMatchFlow),
                              DeviceValueUOM::BOOLEAN,
                              MAKE_CF_CB(set_doubleMatchFlow)); // double Match Flow, 00=off

        // telegram 0x380
        register_device_value(TAG_NONE, &climateZone_, DeviceValueType::UINT, nullptr, FL_(climateZone), DeviceValueUOM::NONE, MAKE_CF_CB(set_climateZone)); // climate zone identifier
        register_device_value(TAG_NONE,
                              &collector1Area_,
                              DeviceValueType::USHORT,
                              FL_(div10),
                              FL_(collector1Area),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_collector1Area)); // Area of collector field 1
        register_device_value(TAG_NONE,
                              &collector1Type_,
                              DeviceValueType::ENUM,
                              FL_(enum_collectortype),
                              FL_(collector1Type),
                              DeviceValueUOM::LIST,
                              MAKE_CF_CB(set_collector1Type)); // Type of collector field 1, 01=flat, 02=vacuum
    }
}

// publish HA config
bool Solar::publish_ha_config() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    doc["uniq_id"] = F_(solar);
    doc["ic"]      = F_(icondevice);

    char stat_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), Mqtt::tag_to_topic(device_type(), DeviceValueTAG::TAG_NONE).c_str());
    doc["stat_t"] = stat_t;

    char name_s[40];
    snprintf(name_s, sizeof(name_s), FSTR_(productid_fmt), device_type_name().c_str());
    doc["name"] = name_s;

    doc["val_tpl"] = FJSON("{{value_json.id}}");
    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = FJSON("EMS-ESP Solar");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = brand_to_string();
    dev["mdl"]     = name();
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp-solar");

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), "sensor/%s/solar/config", Mqtt::base().c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    return true;
}

// SM10Monitor - type 0x96
// Solar(0x30) -> All(0x00), (0x96), data: FF 18 19 0A 02 5A 27 0A 05 2D 1E 0F 64 28 0A
void Solar::process_SM10Config(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(solarIsEnabled_, 0)); // FF on
    has_update(telegram->read_value(solarPumpMinMod_, 2));
    has_update(telegram->read_value(solarPumpTurnonDiff_, 7));
    has_update(telegram->read_value(solarPumpTurnoffDiff_, 8));
    has_update(telegram->read_value(tankMaxTemp_, 5));
    has_update(telegram->read_value(wwMinTemp_, 6));
}

// SM10Monitor - type 0x97
void Solar::process_SM10Monitor(std::shared_ptr<const Telegram> telegram) {
    uint8_t solarpumpmod = solarPumpModulation_;

    has_update(telegram->read_bitvalue(collectorShutdown_, 0, 3));
    has_update(telegram->read_bitvalue(tankHeated_, 0, 2));    // tankMaxTemp reached
    has_update(telegram->read_value(collectorTemp_, 2));       // collector temp from SM10, is *10
    has_update(telegram->read_value(tankBottomTemp_, 5));      // tank bottom temp from SM10, is *10
    has_update(telegram->read_value(solarPumpModulation_, 4)); // modulation solar pump
    has_update(telegram->read_bitvalue(solarPump_, 7, 1));
    has_update(telegram->read_value(pumpWorkTime_, 8, 3));

    // mask out pump-boosts
    if (solarpumpmod == 0 && solarPumpModulation_ == 100) {
        solarPumpModulation_ = solarPumpMinMod_; // set to minimum
    }

    if (!Helpers::hasValue(maxFlow_)) {
        EMSESP::webSettingsService.read([&](WebSettings & settings) { maxFlow_ = settings.solar_maxflow; });
    }

    // solar publishes every minute, do not count reads by other devices
    if (telegram->dest == 0) {
        // water 4.184 J/gK, glycol ~2.6-2.8 J/gK, no aceotrope
        // solarPower_ = (collectorTemp_ - tankBottomTemp_) * solarPumpModulation_ * maxFlow_ * 10 / 1434; // water
        solarPower_ = (collectorTemp_ - tankBottomTemp_) * solarPumpModulation_ * maxFlow_ * 10 / 1665; //40% glycol@40°C
        if (energy.size() >= 60) {
            energy.pop_front();
        }
        energy.push_back(solarPower_);
        uint32_t sum = 0;
        for (auto e : energy) {
            sum += e;
        }
        energyLastHour_ = sum / 6; // counts in 0.1 Wh
    }
}

/*
 * process_SM100SystemConfig - type 0x0358 EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 58 FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 00 FF 01 00 00
 */
void Solar::process_SM100SystemConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(heatTransferSystem_, 5, 1));
    has_update(telegram->read_value(externalTank_, 9, 1));
    has_update(telegram->read_value(thermalDisinfect_, 10, 1));
    has_update(telegram->read_value(heatMetering_, 14, 1));
    has_update(telegram->read_value(solarIsEnabled_, 19, 1));
}

/*
 * process_SM100SolarCircuitConfig - type 0x035A EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 5A 64 05 00 58 14 01 01 32 64 00 00 00 5A 0C
 */
void Solar::process_SM100SolarCircuitConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(collectorMaxTemp_, 0, 1));
    has_update(telegram->read_value(tankMaxTemp_, 3, 1));
    has_update(telegram->read_value(collectorMinTemp_, 4, 1));
    has_update(telegram->read_value(solarPumpMode_, 5, 1));
    has_update(telegram->read_value(solarPumpMinMod_, 6, 1));
    has_update(telegram->read_value(solarPumpTurnoffDiff_, 7, 1));
    has_update(telegram->read_value(solarPumpTurnonDiff_, 8, 1));
    has_update(telegram->read_value(solarPumpKick_, 9, 1));
    has_update(telegram->read_value(plainWaterMode_, 10, 1));
    has_update(telegram->read_value(doubleMatchFlow_, 11, 1));
}

/* process_SM100ParamCfg - type 0xF9 EMS 1.0
 * This telegram is used to inquire the min, default, max, and current values of a value that is usually read and written with another telegram ID
 * The CS200 uses this method extensively to find out which values may be set in the SM100
 * e.g. B0 10 F9 00 FF 02 5A 03 17 00 00 00 14 00 00 00 3C 00 00 00 5A 00 00 00 59 29 - requested with 90 B0 F9 00 11 FF 02 5A 03 AF
 * byte 0 = 0xFF
 * byte 1-2 = telegram ID used to write this value
 * byte 3 = offset in telegram used to write this value
 * byte 4 = unknown
 * bytes 5..8 = minimum value
 * bytes 9..12 = default value
 * bytes 13..16 = maximum value
 * bytes 17..20 = current value
 *
 * e.g. B0 0B F9 00 00 02 5A 00 00 6E
 */
void Solar::process_SM100ParamCfg(std::shared_ptr<const Telegram> telegram) {
    uint16_t t_id = EMS_VALUE_USHORT_NOTSET;
    uint8_t  of   = EMS_VALUE_UINT_NOTSET;
    int32_t  min  = EMS_VALUE_USHORT_NOTSET;
    int32_t  def  = EMS_VALUE_USHORT_NOTSET;
    int32_t  max  = EMS_VALUE_USHORT_NOTSET;
    int32_t  cur  = EMS_VALUE_USHORT_NOTSET;
    has_update(telegram->read_value(t_id, 1));
    has_update(telegram->read_value(of, 3));
    has_update(telegram->read_value(min, 5));
    has_update(telegram->read_value(def, 9));
    has_update(telegram->read_value(max, 13));
    has_update(telegram->read_value(cur, 17));

    // LOG_DEBUG(F("SM100ParamCfg param=0x%04X, offset=%d, min=%d, default=%d, max=%d, current=%d"), t_id, of, min, def, max, cur));
}

/*
 * SM100Monitor - type 0x0362 EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 62 00 77 01 D4 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 00 F9 80 00 80 9E - for heat exchanger temp
 * e.g, 30 00 FF 00 02 62 01 AC
 *      30 00 FF 18 02 62 80 00
 *      30 00 FF 00 02 62 01 A1 - for tank bottom temps
 * bytes 0+1 = TS1 Temperature sensor for collector
 * bytes 2+3 = TS2 Temperature sensor 1 cylinder, bottom
 * bytes 16+17 = TS5 Temperature sensor 2 cylinder, bottom, or swimming pool
 * bytes 20+21 = TS6 Temperature sensor external heat exchanger
 */
void Solar::process_SM100Monitor(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(collectorTemp_, 0));      // is *10 - TS1: Temperature sensor for collector array 1
    has_update(telegram->read_value(tankBottomTemp_, 2));     // is *10 - TS2: Temperature sensor 1 cylinder, bottom
    has_update(telegram->read_value(tankBottomTemp2_, 16));   // is *10 - TS5: Temperature sensor 2 cylinder, bottom, or swimming pool
    has_update(telegram->read_value(heatExchangerTemp_, 20)); // is *10 - TS6: Heat exchanger temperature sensor
}

// SM100wwTemperature - 0x07D6
// Solar Module(0x2A) -> (0x00), (0x7D6), data: 01 C1 00 00 02 5B 01 AF 01 AD 80 00 01 90
void Solar::process_SM100wwTemperature(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wwTemp_1_, 0));
    has_update(telegram->read_value(wwTemp_3_, 4));
    has_update(telegram->read_value(wwTemp_4_, 6));
    has_update(telegram->read_value(wwTemp_5_, 8));
    has_update(telegram->read_value(wwTemp_7_, 12));
}

// SM100wwStatus - 0x07AA
// Solar Module(0x2A) -> (0x00), (0x7AA), data: 64 00 04 00 03 00 28 01 0F
void Solar::process_SM100wwStatus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(wwPump_, 0));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// SM100Monitor2 - 0x0363
// e.g. B0 00 FF 00 02 63 80 00 80 00 00 00 80 00 80 00 80 00 00 80 00 5A
void Solar::process_SM100Monitor2(std::shared_ptr<const Telegram> telegram) {
    // not implemented yet
}

// SM100wwCommand - 0x07AB
// Thermostat(0x10) -> Solar Module(0x2A), (0x7AB), data: 01 00 01
void Solar::process_SM100wwCommand(std::shared_ptr<const Telegram> telegram) {
    // not implemented yet
}

#pragma GCC diagnostic pop

// SM100Config - 0x0366
// e.g. B0 00 FF 00 02 66     01 62 00 13 40 14
void Solar::process_SM100Config(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(availabilityFlag_, 0));
    has_update(telegram->read_value(configFlag_, 1));
    has_update(telegram->read_value(userFlag_, 2));
}

/*
 * SM100Status - type 0x0364 EMS+ for pump modulations - for MS/SM100 and MS/SM200
 - PS1: Solar circuit pump for collector array 1
 - PS5: Cylinder primary pump when using an external heat exchanger
 * e.g. 30 00 FF 09 02 64 64 = 100%
 */
void Solar::process_SM100Status(std::shared_ptr<const Telegram> telegram) {
    uint8_t solarpumpmod    = solarPumpModulation_;
    uint8_t cylinderpumpmod = cylinderPumpModulation_;
    has_update(telegram->read_value(cylinderPumpModulation_, 8));
    has_update(telegram->read_value(solarPumpModulation_, 9));

    if (solarpumpmod == 0 && solarPumpModulation_ == 100) { // mask out boosts
        solarPumpModulation_ = solarPumpMinMod_;            // set to minimum
    }

    if (cylinderpumpmod == 0 && cylinderPumpModulation_ == 100) { // mask out boosts
        cylinderPumpModulation_ = solarPumpMinMod_;               // set to minimum
    }
    has_update(telegram->read_bitvalue(tankHeated_, 3, 1));        // issue #422
    has_update(telegram->read_bitvalue(collectorShutdown_, 3, 0)); // collector shutdown
}

/*
 * SM100Status2 - type 0x036A EMS+ for pump on/off at offset 0x0A - for SM100 and SM200
 * e.g. B0 00 FF 00 02 6A 03 03 03 03 01 03 03 03 03 03 01 03
 * byte 4 = VS2 3-way valve for cylinder 2 : test=01, on=04 and off=03
 * byte 10 = PS1 Solar circuit pump for collector array 1: test=b0001(1), on=b0100(4) and off=b0011(3)
 */
void Solar::process_SM100Status2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_bitvalue(valveStatus_, 4, 2)); // on if bit 2 set
    has_update(telegram->read_bitvalue(solarPump_, 10, 2));  // on if bit 2 set
}

/*
 * SM100CollectorConfig - type 0x0380 EMS+  - for SM100 and SM200
 * e.g. B0 0B FF 00 02 80 50 64 00 00 29 01 00 00 01
 */
void Solar::process_SM100CollectorConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(climateZone_, 0));
    has_update(telegram->read_value(collector1Area_, 3));
    has_update(telegram->read_enumvalue(collector1Type_, 5, 1));
}

/*
 * SM100Energy - type 0x038E EMS+ for energy readings
 * e.g. 30 00 FF 00 02 8E 00 00 00 00 00 00 06 C5 00 00 76 35
 */
void Solar::process_SM100Energy(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(energyLastHour_, 0)); // last hour / 10 in Wh
    has_update(telegram->read_value(energyToday_, 4));    // todays in Wh
    has_update(telegram->read_value(energyTotal_, 8));    // total / 10 in kWh
}

/*
 * SM100Time - type 0x0391 EMS+ for pump working time
 */
void Solar::process_SM100Time(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(pumpWorkTime_, 1, 3));
}

/*
 * Junkers ISM1 Solar Module - type 0x0103 EMS+ for energy readings
 *  e.g. B0 00 FF 00 00 03 32 00 00 00 00 13 00 D6 00 00 00 FB D0 F0
 */
void Solar::process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(collectorTemp_, 4));  // Collector Temperature
    has_update(telegram->read_value(tankBottomTemp_, 6)); // Temperature Bottom of Solar Boiler tank
    uint16_t Wh = energyLastHour_ / 10;
    has_update(telegram->read_value(Wh, 2)); // Solar Energy produced in last hour only ushort, is not * 10
    energyLastHour_ = Wh * 10;               // set to *10

    has_update(telegram->read_bitvalue(solarPump_, 8, 0));         // PS1 Solar pump on (1) or off (0)
    has_update(telegram->read_value(pumpWorkTime_, 10, 3));        // force to 3 bytes
    has_update(telegram->read_bitvalue(collectorShutdown_, 9, 0)); // collector shutdown on/off
    has_update(telegram->read_bitvalue(tankHeated_, 9, 2));        // tank full
}

/*
 * Junkers ISM1 Solar Module - type 0x0101 EMS+ for setting values
 */
void Solar::process_ISM1Set(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram->read_value(tankMaxTemp_, 6));
}

/*
 * Settings
 */
// collector shutdown temperature
bool Solar::set_CollectorMaxTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2number(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 3, (uint8_t)temperature / 10, 0x96);
    } else {
        write_command(0x35A, 0, (uint8_t)temperature, 0x35A);
    }
    return true;
}

// collector shutdown temperature
bool Solar::set_CollectorMinTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2number(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 4, (uint8_t)temperature / 10, 0x96);
    } else {
        write_command(0x35A, 4, (uint8_t)temperature, 0x35A);
    }
    return true;
}

bool Solar::set_TankMaxTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2number(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 5, (uint8_t)temperature, 0x96);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        write_command(0x101, 6, (uint8_t)temperature, 0x101);
    } else {
        write_command(0x35A, 3, (uint8_t)temperature, 0x35A);
    }
    return true;
}
bool Solar::set_PumpMinMod(const char * value, const int8_t id) {
    int modulation;
    if (!Helpers::value2number(value, modulation)) {
        return false;
    }
    write_command(0x96, 2, (uint8_t)modulation, 0x96);
    return true;
}

bool Solar::set_wwMinTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2number(value, temperature)) {
        return false;
    }
    write_command(0x96, 6, (uint8_t)temperature, 0x96);
    return true;
}

bool Solar::set_TurnoffDiff(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2float(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 8, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 7, (uint8_t)(temperature * 10), 0x35A);
    }
    return true;
}

bool Solar::set_TurnonDiff(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2float(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 7, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 8, (uint8_t)(temperature * 10), 0x35A);
    }
    return true;
}

// external value to calculate energy
bool Solar::set_SM10MaxFlow(const char * value, const int8_t id) {
    float flow;
    if (!Helpers::value2float(value, flow)) {
        return false;
    }
    maxFlow_ = (flow * 10);
    EMSESP::webSettingsService.update(
        [&](WebSettings & settings) {
            settings.solar_maxflow = maxFlow_;
            return StateUpdateResult::CHANGED;
        },
        "local");
    return true;
}

bool Solar::set_heatTransferSystem(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x358, 5, v ? 0x01 : 0x00, 0x358);
    return true;
}

bool Solar::set_externalTank(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x358, 9, v ? 0x01 : 0x00, 0x358);
    return true;
}

bool Solar::set_thermalDisinfect(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x358, 10, v ? 0x01 : 0x00, 0x358);
    return true;
}

bool Solar::set_heatMetering(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x358, 14, v ? 0x01 : 0x00, 0x358);
    return true;
}

bool Solar::set_solarEnabled(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 0, v ? 0xFF : 0x00, 0x96);
    } else {
        write_command(0x358, 19, v ? 0x01 : 0x00, 0x358);
    }
    return true;
}

bool Solar::set_solarMode(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_solarmode))) {
        return false;
    }
    write_command(0x35A, 5, num, 0x35A);
    return true;
}

bool Solar::set_solarPumpKick(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x35A, 9, v ? 0x01 : 0x00, 0x35A);
    return true;
}

bool Solar::set_plainWaterMode(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x35A, 10, v ? 0x01 : 0x00, 0x35A);
    return true;
}

bool Solar::set_doubleMatchFlow(const char * value, const int8_t id) {
    bool v = false;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x35A, 11, v ? 0x01 : 0x00, 0x35A);
    return true;
}

bool Solar::set_climateZone(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x380, 0, v, 0x380);
    return true;
}

bool Solar::set_collector1Area(const char * value, const int8_t id) {
    float v = 0;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0x380, 3, (uint16_t)(v * 10), 0x380);
    return true;
}

bool Solar::set_collector1Type(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_collectortype))) {
        return false;
    }
    write_command(0x380, 5, num + 1, 0x380);
    return true;
}

} // namespace emsesp
