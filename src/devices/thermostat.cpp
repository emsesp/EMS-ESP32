/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#include "thermostat.h"

namespace emsesp {

REGISTER_FACTORY(Thermostat, EMSdevice::DeviceType::THERMOSTAT);

uuid::log::Logger Thermostat::logger_{F_(thermostat), uuid::log::Facility::CONSOLE};

Thermostat::Thermostat(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // RF remote sensor seen at 0x40, maybe this is also for different hc with id 0x40 - 0x47? emsesp.cpp maps only 0x40
    if (device_id >= 0x40 && device_id <= 0x47) {
        register_telegram_type(0x0435, "RFTemp", false, MAKE_PF_CB(process_RemoteTemp));
        return;
    }
    // remote thermostats with humidity: RC100H remote, each thermostat is for one hc
    if (device_id >= 0x38 && device_id <= 0x3F) {
        register_telegram_type(0x042B + device_id - 0x38, "RemoteTemp", false, MAKE_PF_CB(process_RemoteTemp));
        if (product_id != Roomctrl::RC200) {
            register_telegram_type(0x047B + device_id - 0x38, "RemoteHumidity", false, MAKE_PF_CB(process_RemoteHumidity));
            register_telegram_type(0x0273 + device_id - 0x38, "RemoteCorrection", true, MAKE_PF_CB(process_RemoteCorrection));
            register_telegram_type(0x0A6A + device_id - 0x38, "RemoteBattery", true, MAKE_PF_CB(process_RemoteBattery));
            // maybe fixed type for these telegrams?
            // register_telegram_type(0x0273, "RemoteCorrection", true, MAKE_PF_CB(process_RemoteCorrection));
            // register_telegram_type(0x0A6B, "RemoteBattery", true, MAKE_PF_CB(process_RemoteBattery));
        }
        register_device_values(); // register device values for common values (not heating circuit)
        return;                   // no values to add
    }

    // common telegram handlers
    register_telegram_type(EMS_TYPE_RCOutdoorTemp, "RCOutdoorTemp", false, MAKE_PF_CB(process_RCOutdoorTemp));
    register_telegram_type(EMS_TYPE_RCTime, "RCTime", false, MAKE_PF_CB(process_RCTime));
    register_telegram_type(0xA2, "RCError", false, MAKE_PF_CB(process_RCError));
    register_telegram_type(0x12, "RCErrorMessage", false, MAKE_PF_CB(process_RCErrorMessage));
    register_telegram_type(0x13, "RCErrorMessage2", false, MAKE_PF_CB(process_RCErrorMessage));

    uint8_t model = this->model();

    // RC10
    if (model == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        monitor_typeids = {0xB1};
        set_typeids     = {0xB0};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "RC10Monitor", false, MAKE_PF_CB(process_RC10Monitor));
            register_telegram_type(set_typeids[i], "RC10Set", false, MAKE_PF_CB(process_RC10Set));
        }

        // RC35
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC35) || (model == EMSdevice::EMS_DEVICE_FLAG_RC30_N)) {
        monitor_typeids = {0x3E, 0x48, 0x52, 0x5C};
        set_typeids     = {0x3D, 0x47, 0x51, 0x5B};
        timer_typeids   = {0x3F, 0x49, 0x53, 0x5D};
        timer2_typeids  = {0x42, 0x4C, 0x56, 0x60};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "RC35Monitor", false, MAKE_PF_CB(process_RC35Monitor));
            register_telegram_type(set_typeids[i], "RC35Set", false, MAKE_PF_CB(process_RC35Set));
            register_telegram_type(timer_typeids[i], "RC35Timer", false, MAKE_PF_CB(process_RC35Timer));
            register_telegram_type(timer2_typeids[i], "RC35Timer2", false, MAKE_PF_CB(process_RC35Timer));
        }
        register_telegram_type(EMS_TYPE_IBASettings, "IBASettings", true, MAKE_PF_CB(process_IBASettings));
        register_telegram_type(EMS_TYPE_wwSettings, "WWSettings", true, MAKE_PF_CB(process_RC35wwSettings));
        register_telegram_type(0x38, "WWTimer", true, MAKE_PF_CB(process_RC35wwTimer));
        register_telegram_type(0x39, "WWCircTimer", true, MAKE_PF_CB(process_RC35wwTimer));

        // RC20
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        if (device_id == 0x17) { // master
            monitor_typeids = {0x91};
            set_typeids     = {0xA8};
            curve_typeids   = {0x90};
            timer_typeids   = {0x8F};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(monitor_typeids[i], "RC20Monitor", false, MAKE_PF_CB(process_RC20Monitor));
                register_telegram_type(set_typeids[i], "RC20Set", false, MAKE_PF_CB(process_RC20Set));
                register_telegram_type(curve_typeids[i], "RC20Temp", false, MAKE_PF_CB(process_RC20Temp));
                register_telegram_type(timer_typeids[i], "RC20Timer", false, MAKE_PF_CB(process_RC20Timer));
            }
        } else {
            // remote thermostat uses only 0xAF
            register_telegram_type(0xAF, "RC20Remote", false, MAKE_PF_CB(process_RC20Remote));
        }
        // RC20 newer
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC20_N) || (model == EMSdevice::EMS_DEVICE_FLAG_RC25)) {
        if (device_id == 0x17) { // master
            monitor_typeids = {0xAE};
            set_typeids     = {0xAD};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(monitor_typeids[i], "RC20Monitor", false, MAKE_PF_CB(process_RC20Monitor_2));
                register_telegram_type(set_typeids[i], "RC20Set", false, MAKE_PF_CB(process_RC20Set_2));
            }
        } else {
            // remote thermostat uses only 0xAF
            register_telegram_type(0xAF, "RC20Remote", false, MAKE_PF_CB(process_RC20Remote));
        }
        // RC30
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        monitor_typeids = {0x41};
        set_typeids     = {0xA7};
        curve_typeids   = {0x40};
        timer_typeids   = {0x3F};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "RC30Monitor", false, MAKE_PF_CB(process_RC30Monitor));
            register_telegram_type(set_typeids[i], "RC30Set", false, MAKE_PF_CB(process_RC30Set));
            register_telegram_type(curve_typeids[i], "RC30Temp", false, MAKE_PF_CB(process_RC30Temp));
            register_telegram_type(timer_typeids[i], "RC30Timer", false, MAKE_PF_CB(process_RC35Timer));
        }
        register_telegram_type(0xA9, "RC30Vacation", true, MAKE_PF_CB(process_RC30Vacation));
        register_telegram_type(EMS_TYPE_RC30wwSettings, "RC30WWSettings", true, MAKE_PF_CB(process_RC30wwSettings));
        register_telegram_type(0x38, "WWTimer", true, MAKE_PF_CB(process_RC35wwTimer));
        register_telegram_type(0x39, "WWCircTimer", true, MAKE_PF_CB(process_RC35wwTimer));

        // EASY
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_EASY) {
        monitor_typeids = {0x0A};
        set_typeids     = {};
        register_telegram_type(monitor_typeids[0], "EasyMonitor", true, MAKE_PF_CB(process_EasyMonitor));
        register_telegram_type(0x02A5, "EasyMonitor", false, MAKE_PF_CB(process_EasyMonitor));

        // CRF
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CRF) {
        monitor_typeids = {0x02A5, 0x02A6, 0x02A7, 0x02A8};
        set_typeids     = {};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "CRFMonitor", false, MAKE_PF_CB(process_CRFMonitor));
        }

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CR11) {
        monitor_typeids = {0x02A5};
        set_typeids     = {0x02B9};
        curve_typeids   = {0x029B};
        register_telegram_type(monitor_typeids[0], "RC300Monitor", true, MAKE_PF_CB(process_CR11Monitor));
        register_telegram_type(set_typeids[0], "RC300Set", false, MAKE_PF_CB(process_RC300Set));
        register_telegram_type(curve_typeids[0], "RC300Curves", true, MAKE_PF_CB(process_RC300Curve));

        // RC300/RC100 variants
    } else if (isRC300() || (model == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        monitor_typeids = {0x02A5, 0x02A6, 0x02A7, 0x02A8, 0x02A9, 0x02AA, 0x02AB, 0x02AC};
        set_typeids     = {0x02B9, 0x02BA, 0x02BB, 0x02BC, 0x02BD, 0x02BE, 0x02BF, 0x02C0};
        set2_typeids    = {0x02CC, 0x02CE, 0x02D0, 0x02D2}; // max. 4 heating circuits supported ny RC310
        summer_typeids  = {0x02AF, 0x02B0, 0x02B1, 0x02B2, 0x02B3, 0x02B4, 0x02B5, 0x02B6};
        curve_typeids   = {0x029B, 0x029C, 0x029D, 0x029E, 0x029F, 0x02A0, 0x02A1, 0x02A2};
        summer2_typeids = {0x0471, 0x0472, 0x0473, 0x0474, 0x0475, 0x0476, 0x0477, 0x0478};
        hp_typeids      = {0x0467, 0x0468, 0x0469, 0x046A};
        hpmode_typeids  = {0x0291, 0x0292, 0x0293, 0x0294};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "RC300Monitor", false, MAKE_PF_CB(process_RC300Monitor));
            register_telegram_type(set_typeids[i], "RC300Set", false, MAKE_PF_CB(process_RC300Set));
            register_telegram_type(summer_typeids[i], "RC300Summer", false, MAKE_PF_CB(process_RC300Summer));
            register_telegram_type(curve_typeids[i], "RC300Curves", false, MAKE_PF_CB(process_RC300Curve));
            register_telegram_type(summer2_typeids[i], "RC300Summer2", false, MAKE_PF_CB(process_RC300Summer2));
        }
        for (uint8_t i = 0; i < set2_typeids.size(); i++) {
            register_telegram_type(set2_typeids[i], "RC300Set2", false, MAKE_PF_CB(process_RC300Set2));
            register_telegram_type(hp_typeids[i], "HPSet", false, MAKE_PF_CB(process_HPSet));
            register_telegram_type(hpmode_typeids[i], "HPMode", true, MAKE_PF_CB(process_HPMode));
        }
        register_telegram_type(0x2F5, "RC300WWmode", true, MAKE_PF_CB(process_RC300WWmode));
        register_telegram_type(0x2F6, "RC300WW2mode", true, MAKE_PF_CB(process_RC300WWmode));
        register_telegram_type(0x31B, "RC300WWtemp", true, MAKE_PF_CB(process_RC300WWtemp));
        register_telegram_type(0x31D, "RC300WWmode2", false, MAKE_PF_CB(process_RC300WWmode2));
        register_telegram_type(0x31E, "RC300WWmode2", false, MAKE_PF_CB(process_RC300WWmode2));
        register_telegram_type(0x23A, "RC300OutdoorTemp", true, MAKE_PF_CB(process_RC300OutdoorTemp));
        register_telegram_type(0x267, "RC300Floordry", false, MAKE_PF_CB(process_RC300Floordry));
        if (model == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            register_telegram_type(0x241, "RC300Settings", true, MAKE_PF_CB(process_RC300Settings));
        } else {
            register_telegram_type(0x240, "RC300Settings", true, MAKE_PF_CB(process_RC300Settings));
        }

        register_telegram_type(0xBB, "HybridSettings", true, MAKE_PF_CB(process_HybridSettings));
        register_telegram_type(0x23E, "PVSettings", true, MAKE_PF_CB(process_PVSettings));
        if (model == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            register_telegram_type(0x43F, "CRHolidays", true, MAKE_PF_CB(process_RC300Holiday));
        } else {
            register_telegram_type(0x269, "RC300Holiday", true, MAKE_PF_CB(process_RC300Holiday));
        }
        register_telegram_type(0x16E, "Absent", true, MAKE_PF_CB(process_Absent));

        // JUNKERS/HT3
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (device_id >= 0x18 && device_id <= 0x1B) { // remote hc1-hc4
            register_telegram_type(0x123, "JunkersRemote", false, MAKE_PF_CB(process_JunkersRemoteMonitor));
            register_device_values(); // register device values for common values (not heating circuit)
            return;                   // no values to add
        }

        monitor_typeids = {0x016F, 0x0170, 0x0171, 0x0172};
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            register_telegram_type(monitor_typeids[i], "JunkersMonitor", false, MAKE_PF_CB(process_JunkersMonitor));
        }

        if (has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
            // FR120, FR100
            set_typeids = {0x0179, 0x017A, 0x017B, 0x017C};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(set_typeids[i], "JunkersSet", false, MAKE_PF_CB(process_JunkersSet2));
            }
        } else {
            set_typeids = {0x0165, 0x0166, 0x0167, 0x0168};
            for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
                register_telegram_type(set_typeids[i], "JunkersSet", false, MAKE_PF_CB(process_JunkersSet));
            }
        }
        register_telegram_type(0xBB, "HybridSettings", true, MAKE_PF_CB(process_HybridSettings));
        register_telegram_type(0x23, "JunkersSetMixer", true, MAKE_PF_CB(process_JunkersSetMixer));
        register_telegram_type(0x1D3, "JunkersDhw", true, MAKE_PF_CB(process_JunkersWW));
    }

    // register device values for common values (not heating circuit)
    register_device_values();

    // query all the heating circuits. This is only done once.
    // The automatic fetch will from now on only update the active heating circuits
    for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
        EMSESP::send_read_request(monitor_typeids[i], device_id);
    }
    for (uint8_t i = 0; i < set_typeids.size(); i++) {
        EMSESP::send_read_request(set_typeids[i], device_id);
    }

    EMSESP::send_read_request(EMS_TYPE_RCTime, device_id);
    EMSESP::send_read_request(0x12, device_id); // read last error (only published on errors)
    EMSESP::send_read_request(0xA2, device_id); // read errorCode (only published on errors)
}

// returns the heating circuit object based on the hc number
// of nullptr if it doesn't exist yet
std::shared_ptr<Thermostat::HeatingCircuit> Thermostat::heating_circuit(const int8_t id) {
    // if hc_num is 0 then return the first existing hc in the list
    if (id == AUTO_HEATING_CIRCUIT) {
        for (const auto & heating_circuit : heating_circuits_) {
            if (heating_circuit->is_active()) {
                return heating_circuit;
            }
        }
    }

    // otherwise find a match
    uint8_t hc_num = (uint8_t)id;
    for (const auto & heating_circuit : heating_circuits_) {
        if ((heating_circuit->hc_num() == hc_num) && heating_circuit->is_active()) {
            return heating_circuit;
        }
    }

    LOG_DEBUG("Heating circuit not found on device 0x%02X", device_id());
    return nullptr; // not found
}

// determine which heating circuit the type ID is referring too
// returns pointer to the HeatingCircuit or nullptr if it can't be found
// if its a new one, the heating circuit object will be created and also the fetch flags set
std::shared_ptr<Thermostat::HeatingCircuit> Thermostat::heating_circuit(std::shared_ptr<const Telegram> telegram) {
    // look through the Monitor and Set arrays to see if there is a match
    uint8_t hc_num  = 0; // 0 means we haven't found it yet
    bool    toggle_ = false;

    // search device-id types for remote thermostats first, they have only a single typeid for all hcs
    if (telegram->src >= 0x18 && telegram->src <= 0x1F) {
        hc_num  = telegram->src - 0x17;
        toggle_ = true;
    }

    // not found, search monitor message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < monitor_typeids.size(); i++) {
            if (monitor_typeids[i] == telegram->type_id) {
                hc_num  = i + 1;
                toggle_ = true;
                break;
            }
        }
    }

    // not found, search status message/set types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < set_typeids.size(); i++) {
            if (set_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search set2 types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < set2_typeids.size(); i++) {
            if (set2_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search summer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < summer_typeids.size(); i++) {
            if (summer_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search summer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < summer2_typeids.size(); i++) {
            if (summer2_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search heating_curve message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < curve_typeids.size(); i++) {
            if (curve_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search timer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < timer_typeids.size(); i++) {
            if (timer_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search timer message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < timer2_typeids.size(); i++) {
            if (timer2_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search heatpump message types
    if (hc_num == 0) {
        for (uint8_t i = 0; i < hp_typeids.size(); i++) {
            if (hp_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    if (hc_num == 0) {
        for (uint8_t i = 0; i < hpmode_typeids.size(); i++) {
            if (hpmode_typeids[i] == telegram->type_id) {
                hc_num = i + 1;
                break;
            }
        }
    }

    // not found, search device-id types for remote thermostats
    if (hc_num == 0 && telegram->dest >= 0x20 && telegram->dest <= 0x27) {
        hc_num = telegram->dest - 0x20;
    }

    // still didn't recognize it, ignore it
    if (hc_num == 0) {
        return nullptr;
    }

    // if we have the heating circuit already present, returns its object reference
    // otherwise create a new object and add it
    for (const auto & heating_circuit : heating_circuits_) {
        if (heating_circuit->hc_num() == hc_num) {
            return heating_circuit;
        }
    }

    // register new heatingcircuits only on active monitor telegrams
    if (!toggle_) {
        return nullptr;
    }

    /*
     * at this point we have discovered a new heating circuit
     */

    // if it's the first set the status flag
    if (heating_circuits_.empty()) {
        strlcpy(status_, "online", sizeof(status_));
    }

    // create a new heating circuit object and add to the list
    auto new_hc = std::make_shared<Thermostat::HeatingCircuit>(hc_num, model());
    heating_circuits_.push_back(new_hc);

    // sort based on hc number so there's a nice order when displaying
    // NOTE temporarily commented out the HC sorting until I'm 100% sure the return object still references the newly created object
    // not sure if new_hc and heating_circuits_.back() will still reference the new HC after its sorted - to check!
    // std::sort(heating_circuits_.begin(), heating_circuits_.end());

    // register the device values
    register_device_values_hc(new_hc);

    // set the flag saying we want its data during the next auto fetch
    // monitor is broadcasted, but not frequently in some thermostats (IVT, #356)

    if (monitor_typeids.size()) {
        toggle_fetch(monitor_typeids[hc_num - 1], toggle_);
    }
    if (set_typeids.size()) {
        toggle_fetch(set_typeids[hc_num - 1], toggle_);
    }
    if (hc_num <= set2_typeids.size()) {
        toggle_fetch(set2_typeids[hc_num - 1], toggle_);
    }
    if (summer_typeids.size()) {
        toggle_fetch(summer_typeids[hc_num - 1], toggle_);
    }
    if (curve_typeids.size()) {
        toggle_fetch(curve_typeids[hc_num - 1], toggle_);
    }
    if (timer_typeids.size()) {
        toggle_fetch(timer_typeids[hc_num - 1], toggle_);
    }
    if (timer2_typeids.size()) {
        toggle_fetch(timer2_typeids[hc_num - 1], toggle_);
    }
    if (summer2_typeids.size()) {
        toggle_fetch(summer2_typeids[hc_num - 1], toggle_);
    }
    if (hp_typeids.size()) {
        toggle_fetch(hp_typeids[hc_num - 1], toggle_);
    }

    return new_hc; // return back point to new HC object
}

// add the HVAC/Climate HA component for the HC
void Thermostat::add_ha_climate(std::shared_ptr<HeatingCircuit> hc) const {
    if (!Mqtt::ha_enabled()) {
        hc->climate = EMS_VALUE_UINT8_NOTSET;
        return;
    }

    if (Helpers::hasValue(hc->selTemp) && is_readable(&hc->selTemp)) {
        if (Helpers::hasValue(hc->roomTemp) && is_readable(&hc->roomTemp)) {
            hc->climate = 1; // use roomTemp as we have a sensor
        } else {
            hc->climate = 0; // use selTemp, as there is no sensor present in the thermostat
        }
    } else {
        hc->climate = EMS_VALUE_UINT8_NOTSET;
    }
}

// decodes the thermostat mode for the heating circuit based on the thermostat type
// modes are off, manual, auto, day, night and holiday
uint8_t Thermostat::HeatingCircuit::get_mode() const {
    uint8_t model = get_model();

    if (model == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        if (mode == 2) {
            return HeatingCircuit::Mode::DAY;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (mode == 0) {
            return HeatingCircuit::Mode::OFF;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC20 || model == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (mode == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CRF) {
        if (mode == 0) {
            return HeatingCircuit::Mode::AUTO;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::OFF;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CR11) {
        return HeatingCircuit::Mode::MANUAL;
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_BC400 || model == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        if (mode_new == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (mode_new == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode_new == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC300) || (model == EMSdevice::EMS_DEVICE_FLAG_R3000) || (model == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        if (mode == 0) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::AUTO;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (mode == 1) {
            return HeatingCircuit::Mode::MANUAL;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        } else if (mode == 3) {
            return HeatingCircuit::Mode::HOLIDAY;
        }
    } else { // default for all other thermostats
        if (mode == 0) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (mode == 1) {
            return HeatingCircuit::Mode::DAY;
        } else if (mode == 2) {
            return HeatingCircuit::Mode::AUTO;
        }
    }

    return HeatingCircuit::Mode::UNKNOWN;
}

// figures out the thermostat day/night mode depending on the thermostat type
// mode types are day, night, eco, comfort
uint8_t Thermostat::HeatingCircuit::get_mode_type() const {
    uint8_t model = get_model();

    if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (modetype == 2) {
            return HeatingCircuit::Mode::HEAT;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::ECO;
        } else if (modetype == 0) {
            return HeatingCircuit::Mode::NOFROST;
        }
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC35) || (model == EMSdevice::EMS_DEVICE_FLAG_RC30_N)) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::NIGHT;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::DAY;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CRF) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::OFF;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::ON;
        }
    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC300) || (model == EMSdevice::EMS_DEVICE_FLAG_R3000) || (model == EMSdevice::EMS_DEVICE_FLAG_BC400)) {
        if (modetype == 0) {
            return HeatingCircuit::Mode::ECO;
        } else if (modetype == 1) {
            return HeatingCircuit::Mode::COMFORT;
        }
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC100 || model == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        return HeatingCircuit::Mode::DAY; // no other modes on these devices
    }

    return HeatingCircuit::Mode::DAY;
}

std::shared_ptr<Thermostat::DhwCircuit> Thermostat::dhw_circuit(const uint8_t offset, const bool create) {
    // check for existing circuit
    for (const auto & dhw_circuit : dhw_circuits_) {
        if (dhw_circuit->offset() == offset) {
            return dhw_circuit;
        }
    }
    if (!create) {
        return nullptr;
    }
    // create a new circuit object and add to the list
    auto new_dhw = std::make_shared<Thermostat::DhwCircuit>(offset);
    dhw_circuits_.push_back(new_dhw);
    // register the device values
    register_device_values_dhw(new_dhw);

    return new_dhw; // return back point to new HC object
}

// type 0xB1 - data from the RC10 thermostat (0x17)
// Data: 04 23 00 BA 00 00 00 BA
void Thermostat::process_RC10Monitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    uint8_t mode = 1 << hc->mode;
    telegram->read_value(mode, 0);           // 1: nofrost, 2: night, 4: day
    has_update(hc->mode, mode >> 1);         // store as enum 0, 1, 2
    has_update(telegram, hc->selTemp, 1, 1); // is * 2, force as single byte
    has_update(telegram, hc->roomTemp, 2);   // is * 10
    has_update(telegram, hc->reduceminutes, 5);

    add_ha_climate(hc);
}

// type 0xB0 - for reading the mode from the RC10 thermostat (0x17)
// Data: 00 FF 00 1C 20 08 01
void Thermostat::process_RC10Set(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    auto dhw = dhw_circuit(0, true);
    has_update(telegram, ibaCalIntTemperature_, 0);
    has_update(telegram, backlight_, 1);
    has_update(telegram, dhw->wwMode_, 2);
    has_update(telegram, hc->nighttemp, 3);
    has_update(telegram, hc->daytemp, 4);
    has_update(telegram, hc->reducehours, 5);
    has_update(telegram, heatingpid_, 6);
}

// type 0xB2, mode setting Data: 04 00
// not used, we read mode from monitor 0xB1
void Thermostat::process_RC10Set_2(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    // uint8_t mode = 1 << hc->mode;
    // telegram->read_value(mode, 0);           // 1: nofrost, 2: night, 4: day
    // has_update(hc->mode, mode >> 1);          // store as enum 0, 1, 2
}

// 0xA8 - for reading the mode from the RC20 thermostat (0x17)
// RC20Set(0xA8), data: 01 00 FF F6 01 06 00 01 0D 01 00 FF FF 01 02 02 02 00 00 05 1E 05 1E 02 1C 00 FF 00 00 26 02
void Thermostat::process_RC20Set(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->mode, 23);        // 0:off, 1:manual, 2:auto
    has_update(telegram, hc->nofrosttemp, 24); // for mode off
    // has_update(telegram, hc->tempautotemp, 28); // no need to read this
    has_update(telegram, hc->manualtemp, 29);
}

// 0x90 - for reading curve temperature from the RC20 thermostat (0x17)
//
void Thermostat::process_RC20Temp(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->nighttemp, 3);
    has_update(telegram, hc->daylowtemp, 4);
    has_update(telegram, hc->daymidtemp, 5);
    has_update(telegram, hc->daytemp, 6);
}

// 0x8F - for reading timer from the RC20 thermostat (0x17)
// data: 04 5D 01 78 24 5D 21 6E 43 5D 41 78 64 5D 61 78 84 5D 81 78 E7 90 E7 90 E7 90 E7
// data: 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 (offset 27)
// data: E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 90 E7 (offset 54)
// data: 90 E7 90 01 00 00 01 01 00 01 01 00 01 01 00 01 01 00 00 (offset 81)
void Thermostat::process_RC20Timer(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    if ((telegram->message_length == 2 && telegram->offset < 83 && !(telegram->offset & 1))
        || (!telegram->offset && telegram->message_length > 1 && !strlen(hc->switchtime1))) {
        char    data[sizeof(hc->switchtime1)];
        uint8_t no   = telegram->offset / 2;
        uint8_t day  = telegram->message_data[0] >> 5;
        uint8_t temp = telegram->message_data[0] & 7;
        uint8_t time = telegram->message_data[1];

        // we use EN settings for the day abbreviation
        auto sday = FL_(enum_dayOfWeek)[day][0];

        if (day == 7) {
            snprintf(data, sizeof(data), "%02d not_set", no);
        } else {
            snprintf(data, sizeof(data), "%02d %s %02d:%02d T%d", no, sday, time / 6, 10 * (time % 6), temp);
        }
        has_update(hc->switchtime1, data, sizeof(hc->switchtime1));
    }
}

// type 0xAE - data from the RC20 thermostat (0x17) - not for RC20's
// 17 00 AE 00 80 12 2E 00 D0 00 00 64 (#data=8)
// https://github.com/emsesp/EMS-ESP/issues/361
void Thermostat::process_RC20Monitor_2(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    // modes byte 0,1: day: 8002, night: 0000, auto-day:0402, auto-night:0400
    has_bitupdate(telegram, hc->modetype, 1, 1); // day/night
    has_update(telegram, hc->selTemp, 2, 1);     // is * 2, force as single byte
    has_update(telegram, hc->roomTemp, 3);       // is * 10
    has_bitupdate(telegram, hc->summermode, 1, 0);

    add_ha_climate(hc);
}

// 0xAD - for reading the mode from the RC20/ES72 thermostat (0x17)
// see https://github.com/emsesp/EMS-ESP/issues/334#issuecomment-611698259
// offset: 01-nighttemp, 02-daytemp, 03-mode, 0B-program(1-9), 0D-setpoint_roomtemp(temporary)
// 17 00 AD 00 01 27 29 01 4B 05 01 FF 28 19 0A 02 00 00
// RC25(0x17) -> All(0x00), ?(0xAD), data: 01 27 2D 00 44 05 01 FF 28 19 0A 07 00 00 F6 12 5A 11 00 28 05 05 00
void Thermostat::process_RC20Set_2(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->heatingtype, 0);
    has_update(telegram, hc->nighttemp, 1); // is * 2,
    has_update(telegram, hc->daytemp, 2);   // is * 2,
    has_update(telegram, hc->mode, 3);
    has_enumupdate(telegram, hc->program, 11, 1); // 1 .. 9 predefined programs
    has_update(telegram, hc->tempautotemp, 13);

    // RC25 extension:
    has_update(telegram, ibaMinExtTemperature_, 14);
    has_update(telegram, hc->minflowtemp, 15);
    has_update(telegram, hc->maxflowtemp, 16);
    has_update(telegram, hc->summertemp, 17);
}

// 0xAF - for reading the roomtemperature from the RC20/ES72 thermostat (0x18, 0x19, ..)
void Thermostat::process_RC20Remote(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, tempsensor1_, 0);
}

// 0x42B - for reading the roomtemperature from the RC100H remote thermostat (0x38, 0x39, ..)
// e.g. "38 10 FF 00 03 2B 00 D1 08 2A 01"
void Thermostat::process_RemoteTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, tempsensor1_, 0);
    uint8_t hc = telegram->type_id - 0x42B;
    if (Roomctrl::is_remote(hc)) {
        toggle_fetch(0x273 + hc, false);
        toggle_fetch(0xA6A + hc, false);
    }
}

// 0x47B, ff - for reading humidity from the RC100H remote thermostat (0x38, 0x39, ..)
// e.g. "38 10 FF 00 03 7B 08 24 00 4B"
void Thermostat::process_RemoteHumidity(std::shared_ptr<const Telegram> telegram) {
    // has_update(telegram, dewtemperature_, 0); // this is int8
    has_update(telegram, humidity_, 1);
    has_update(telegram, dewtemperature_, 2); // this is int16
    // some thermostats use short telegram with int8 dewpoint, https://github.com/emsesp/EMS-ESP32/issues/1491
    if (telegram->offset == 0 && telegram->message_length < 4) {
        int8_t dew = dewtemperature_ / 10;
        telegram->read_value(dew, 0);
        if (dew != EMS_VALUE_INT8_NOTSET && dewtemperature_ != dew * 10) {
            dewtemperature_ = dew * 10;
            has_update(dewtemperature_);
        }
    }
}

// 0x273 - for reading temperaturcorrection from the RC100H remote thermostat (0x38, 0x39, ..)
// Thermostat(0x38) -> Me(0x0B), RemoteCorrection(0x0273), data: 0A 00
void Thermostat::process_RemoteCorrection(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, ibaCalIntTemperature_, 0);
}

// 0xA6A - for reading battery from the RC100H remote thermostat (0x38, 0x39, ..)
void Thermostat::process_RemoteBattery(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, battery_, 1);
}

// type 0x0165, ff
void Thermostat::process_JunkersSet(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->controlmode, 0);       // 0-off, 1-unmixed-no-IPM, 2-unmixed-IPM, 3-mixed
    has_update(telegram, hc->control, 1);           // remote: 0-off, 1-FB10, 2-FB100
    has_update(telegram, hc->heatingtype, 2);       // 0-off, 1-curve, 2-radiator, 3-convector, 4-floor
    has_update(telegram, hc->minflowtemp, 3);       // degrees
    has_update(telegram, hc->designtemp, 4);        // degrees
    has_update(telegram, hc->maxflowtemp, 5);       // degrees
    has_update(telegram, hc->roominfl_factor, 6);   // %
    has_update(telegram, hc->roominfluence, 7);     // 0-off, 1-ext, 2-int, 3-auto
    has_update(telegram, hc->offsettemp, 8);        // 0-off, 0xFF-on
    has_enumupdate(telegram, hc->roomsensor, 9, 1); // 1-intern, 2-extern, 3-autoselect the lower value
    has_update(telegram, hc->nofrostmode, 10);      // 0-off, 0xFF-on
    has_update(telegram, hc->summertemp, 11);       // degrees
    // 12 Frostgrenztemperatur für Heizkeis (*2)?
    has_enumupdate(telegram, hc->program, 13, 1);     // 1-6: 1 = A, 2 = B,...
    has_enumupdate(telegram, hc->mode, 14, 1);        // 0 = nofrost, 1 = eco, 2 = heat, 3 = auto
    has_update(telegram, hc->nofrosttemp, 15);        // is * 2
    has_update(telegram, hc->nighttemp, 16);          // is * 2
    has_update(telegram, hc->daytemp, 17);            // is * 2
    has_enumupdate(telegram, hc->fastHeatup, 18, 1);  // slow, normal, fast
    has_enumupdate(telegram, hc->holidaymode, 19, 1); // as mode: nofrost, eco, heat, auto
}

// type 0x0179, ff for Junkers_OLD
void Thermostat::process_JunkersSet2(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->controlmode, 0); // 0-off, 1-unmixed-no-IPM, 2-unmixed-IPM, 3-mixed
    // 1 curve factor roomtemp
    // 2 curve factor heating
    has_update(telegram, hc->maxflowtemp, 3);          // degrees
    has_enumupdate(telegram, hc->mode, 4, 1);          // 0 = nofrost, 1 = eco, 2 = heat, 3 = auto
    has_update(telegram, hc->nofrosttemp, 5);          // is * 2
    has_update(telegram, hc->nighttemp, 6);            // is * 2
    has_update(telegram, hc->daytemp, 7);              // is * 2
    has_enumupdate(telegram, hc->holidaymode, 8, 1);   // as mode: 1-nofrost, 2-eco, 3-heat, 4-auto
    has_update(telegram, hc->switchonoptimization, 9); // 0-off, 0xFF-on
    has_enumupdate(telegram, hc->program, 10, 1);      // 1-6: 1 = A, 2 = B,...
}

// type 0x123 - FB10 Junkers remote
void Thermostat::process_JunkersRemoteMonitor(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, tempsensor1_, 0); // roomTemp from remote
}

// type 0xA3 - for external temp settings from the the RC* thermostats (e.g. RC35)
void Thermostat::process_RCOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, dampedoutdoortemp_, 0);
    has_update(telegram, tempsensor1_, 3); // sensor 1 - is * 10
    has_update(telegram, tempsensor2_, 5); // sensor 2 - is * 10
}

// 0x91 - data from the RC20 thermostat (0x17) - 15 bytes long
// RC20Monitor(0x91), data: 90 2A 00 D5 1A 00 00 05 00 5A 04 00 D6 00
// offset 8: setburnpower to boiler, offset 9: setflowtemp to boiler (thermostat: targetflowtemp) send via 0x1A
void Thermostat::process_RC20Monitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->selTemp, 1, 1); // is * 2, force as single byte
    has_update(telegram, hc->roomTemp, 2);   // is * 10

    add_ha_climate(hc);
}

// type 0x0A - data from the Nefit Easy/TC100 thermostat (0x18) - 31 bytes long
void Thermostat::process_EasyMonitor(std::shared_ptr<const Telegram> telegram) {
    monitor_typeids[0] = telegram->type_id;
    auto hc            = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    if (telegram->type_id == 0x0A) {
        int16_t temp = hc->roomTemp;
        if (telegram->read_value(temp, 8) && temp != 0) {
            has_update(telegram, hc->roomTemp, 8); // is * 100
            has_update(telegram, hc->selTemp, 10); // is * 100
            toggle_fetch(0x0A, true);
        }
    } else if (telegram->type_id == 0x02A5) { // see #2277
        int16_t temp = hc->roomTemp / 10;
        if (telegram->read_value(temp, 0)) {     // is * 10
            has_update(hc->roomTemp, temp * 10); // * 100
            toggle_fetch(0x0A, false);
        }
        int16_t sel = hc->selTemp / 50;
        if (telegram->read_value(sel, 6, 1)) { // is * 2
            has_update(hc->selTemp, sel * 50); // * 100
        }
    }

    add_ha_climate(hc);
}

// Settings Parameters - 0xA5 - RC30_1
void Thermostat::process_IBASettings(std::shared_ptr<const Telegram> telegram) {
    // 22 - display line on RC35

    // display on Thermostat: 0 int. temp, 1 int. setpoint, 2 ext. temp., 3 burner temp., 4 ww temp, 5 functioning mode, 6 time, 7 data, 8 smoke temp
    has_update(telegram, ibaMainDisplay_, 0);
    has_update(telegram, ibaLanguage_, 1);          // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    has_update(telegram, ibaCalIntTemperature_, 2); // offset int. temperature sensor, by * 0.1 Kelvin
    has_update(telegram, ibaBuildingType_, 6);      // building type: 0 = light, 1 = medium, 2 = heavy
    has_update(telegram, ibaMinExtTemperature_, 5); // min ext temp for heating curve, in deg., 0xF6=-10, 0x0 = 0, 0xFF=-1
    has_update(telegram, ibaClockOffset_, 12);      // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
    has_update(telegram, ibaDamping_, 21);          // damping 0-off, 0xff-on
}

// Settings WW 0x37 - RC35
void Thermostat::process_RC35wwSettings(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(0, true);
    has_update(telegram, dhw->wwProgMode_, 0);     // 0-like hc, 0xFF own prog
    has_update(telegram, dhw->wwCircProg_, 1);     // 0-like hc, 0xFF own prog
    has_update(telegram, dhw->wwMode_, 2);         // 0-off, 1-on, 2-auto
    has_update(telegram, dhw->wwCircMode_, 3);     // 0-off, 1-on, 2-auto
    has_update(telegram, dhw->wwDisinfecting_, 4); // 0-off, 0xFF on
    has_update(telegram, dhw->wwDisinfectDay_, 5); // 0-6 Day of week, 7 every day
    has_update(telegram, dhw->wwDisinfectHour_, 6);
    has_update(telegram, dhw->wwMaxTemp_, 8);    // Limiter 60 degrees
    has_update(telegram, dhw->wwOneTimeKey_, 9); // 0-off, 0xFF on
}

// Settings WW 0x3A - RC30
void Thermostat::process_RC30wwSettings(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(0, true);
    has_update(telegram, dhw->wwMode_, 0);         // 0-on, 1-off, 2-auto
    has_update(telegram, dhw->wwWhenModeOff_, 1);  // 0-off, 0xFF on
    has_update(telegram, dhw->wwDisinfecting_, 2); // 0-off, 0xFF on
    has_update(telegram, dhw->wwDisinfectDay_, 3); // 0-6 Day of week, 7 every day
    has_update(telegram, dhw->wwDisinfectHour_, 4);
}

// type 0x38 (ww) and 0x39 (circ)
void Thermostat::process_RC35wwTimer(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(0, true);
    if ((telegram->message_length == 2 && telegram->offset < 83 && !(telegram->offset & 1))
        || (!telegram->offset && telegram->type_id == 0x38 && !strlen(dhw->wwSwitchTime_) && telegram->message_length > 1)
        || (!telegram->offset && telegram->type_id == 0x39 && !strlen(dhw->wwCircSwitchTime_) && telegram->message_length > 1)) {
        uint8_t no   = telegram->offset / 2;
        uint8_t day  = telegram->message_data[0] >> 5;
        uint8_t on   = telegram->message_data[0] & 1;
        uint8_t time = telegram->message_data[1];

        char data[sizeof(dhw->wwSwitchTime_)];
        // we use EN settings for the day abbreviation
        auto sday = (FL_(enum_dayOfWeek)[day][0]);
        if (day == 7) {
            snprintf(data, sizeof(data), "%02d not_set", no);
        } else {
            snprintf(data, sizeof(data), "%02d %s %02d:%02d %s", no, sday, time / 6, 10 * (time % 6), on ? "on" : "off");
        }
        if (telegram->type_id == 0x38) {
            has_update(dhw->wwSwitchTime_, data, sizeof(dhw->wwSwitchTime_));
        } else {
            has_update(dhw->wwCircSwitchTime_, data, sizeof(dhw->wwCircSwitchTime_));
        }
        if (is_fetch(telegram->type_id)) {
            toggle_fetch(telegram->type_id, false); // dont fetch again
        }
    }

    // vacation/holiday only from dhw timer
    if (telegram->type_id != 0x38) {
        return;
    }

    if (telegram->message_length + telegram->offset >= 92 && telegram->offset <= 87) {
        char data[sizeof(dhw->wwVacation_) + 4]; // avoid compiler warning
        snprintf(data,
                 sizeof(data),
                 "%02d.%02d.%04d-%02d.%02d.%04d",
                 telegram->message_data[87 - telegram->offset],
                 telegram->message_data[88 - telegram->offset],
                 telegram->message_data[89 - telegram->offset] + 2000,
                 telegram->message_data[90 - telegram->offset],
                 telegram->message_data[91 - telegram->offset],
                 telegram->message_data[92 - telegram->offset] + 2000);
        has_update(dhw->wwVacation_, data, sizeof(dhw->wwVacation_));
    }

    if (telegram->message_length + telegram->offset >= 98 && telegram->offset <= 93) {
        char data[sizeof(dhw->wwHoliday_) + 4]; // avoid compiler warning
        snprintf(data,
                 sizeof(data),
                 "%02d.%02d.%04d-%02d.%02d.%04d",
                 telegram->message_data[93 - telegram->offset],
                 telegram->message_data[94 - telegram->offset],
                 telegram->message_data[95 - telegram->offset] + 2000,
                 telegram->message_data[96 - telegram->offset],
                 telegram->message_data[97 - telegram->offset],
                 telegram->message_data[98 - telegram->offset] + 2000);
        has_update(dhw->wwHoliday_, data, sizeof(dhw->wwHoliday_));
    }
}

// type 0x6F - FR10/FR50/FR100/FR110/FR120 Junkers
void Thermostat::process_JunkersMonitor(std::shared_ptr<const Telegram> telegram) {
    // ignore single byte telegram messages
    if (telegram->message_length <= 1) {
        return;
    }

    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_enumupdate(telegram, hc->modetype, 0, 1); // 1 = nofrost, 2 = eco, 3 = heat
    has_update(telegram, hc->selTemp, 2);         // value is * 10

    if ((hc->control == 1) || (hc->control == 2)) {
        has_update(telegram, hc->roomTemp, 6); // roomTemp from remote
    } else {
        has_update(telegram, hc->roomTemp, 4); // value is * 10
    }

    add_ha_climate(hc);
}

// 0xBB Heatpump optimization
// ?(0xBB), data: 00 00 00 00 00 00 00 00 00 00 00 FF 02 0F 1E 0B 1A 00 14 03
void Thermostat::process_HybridSettings(std::shared_ptr<const Telegram> telegram) {
    has_enumupdate(telegram, hybridStrategy_, 12, 1); // cost = 2, temperature = 3, mix = 4
    has_update(telegram, switchOverTemp_, 13);        // full degrees
    has_update(telegram, energyCostRatio_, 14);       // is *10
    has_update(telegram, fossileFactor_, 15);         // is * 10
    has_update(telegram, electricFactor_, 16);        // is * 10
    has_update(telegram, delayBoiler_, 18);           // minutes
    has_update(telegram, tempDiffBoiler_, 19);        // relative degrees
}

// 0x23E PV settings
void Thermostat::process_PVSettings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, pvRaiseHeat_, 0);
    has_update(telegram, pvEnableWw_, 3);
    has_update(telegram, pvLowerCool_, 5);
}

// 0x16E Absent settings - hc or dhw or device_data? #1957
void Thermostat::process_Absent(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, absent_, 0);
}

void Thermostat::process_JunkersSetMixer(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->targetflowtemp, 0);
}

void Thermostat::process_JunkersWW(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(0, true);
    has_bitupdate(telegram, dhw->wwCharge_, 0, 3);
}

// type 0x02A5 - data from Worchester CRF200
void Thermostat::process_CRFMonitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->roomTemp, 0);       // is * 10
    has_bitupdate(telegram, hc->modetype, 2, 0); // bit 0, modetype (off=0 , on=1)
    has_bitupdate(telegram, hc->mode, 2, 4);     // bit 4, mode (auto=0, off=1)
    has_update(telegram, hc->selTemp, 6, 1);     // is * 2, force as single byte
    has_update(telegram, hc->targetflowtemp, 4);

    add_ha_climate(hc);
}

// type 0x02A5 - data from CR11
void Thermostat::process_CR11Monitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->roomTemp, 0);   // is * 10
    has_update(hc->mode, 0);                 // set manual for CR11
    has_update(telegram, hc->selTemp, 6, 1); // is * 2, force as single byte
    has_update(telegram, hc->targetflowtemp, 4);

    add_ha_climate(hc);
}

// type 0x02A5 - data from the Nefit RC1010/3000 thermostat (0x18) and RC300/310s on 0x10
// Rx: 10 0B FF 00 01 A5 80 00 01 30 23 00 30 28 01 E7 03 03 01 01 E7 02 33 00 00 11 01 03 FF FF 00 04
void Thermostat::process_RC300Monitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->roomTemp, 0); // is * 10
    has_bitupdate(telegram, hc->modetype, 10, 1);
    // auto status, read mode in settings
    // has_bitupdate(telegram, hc->mode, 10, 0); // bit 0, mode (auto=1 or manual=0)

    // if manual, take the current setpoint temp at pos 6
    // if auto, take the next setpoint temp at pos 7
    // pos 3 is the current target temp and sometimes can be 0
    // see https://github.com/emsesp/EMS-ESP/issues/256#issuecomment-585171426
    // pos 3 actual setpoint (optimized), i.e. changes with temporary change, summer/holiday-modes
    // pos 6 actual setpoint according to programmed changes eco/comfort
    // pos 7 next setpoint in the future, time to next setpoint in pos 8/9
    has_update(telegram, hc->selTemp, 3, 1); // is * 2, force as single byte
    // has_bitupdate(telegram, hc->summermode, 2, 4);
    // summermode is bit 4 for boilers and bit 6 for heatpumps: 0:winter, 1:summer
    telegram->read_value(hc->statusbyte, 2);
    // use summertemp or hpoperatingstate, https://github.com/emsesp/EMS-ESP32/issues/747, #550, #503
    if ((hc->statusbyte & 1) || !is_received(summer2_typeids[hc->hc()])) {
        has_update(hc->summermode, hc->statusbyte & 0x50 ? 1 : 0);
        has_update(hc->hpoperatingstate, EMS_VALUE_UINT8_NOTSET);
    } else {
        has_enumupdate(telegram, hc->hpoperatingstate, 20, 1); // 1:heating, 2:off, 3:cooling
        has_update(hc->summermode, EMS_VALUE_UINT8_NOTSET);
    }
    has_update(telegram, hc->targetflowtemp, 4);
    has_update(telegram, hc->curroominfl, 27);
    has_update(telegram, hc->currSolarInfl, 29);
    has_update(telegram, hc->coolingon, 32);
    has_update(telegram, hc->vacationmode, 18);

    add_ha_climate(hc);
}

// type 0x02B9 EMS+ for reading from RC300/RC310 thermostat
// Thermostat(0x10) -> Me(0x0B), RC300Set(0x2B9), data: FF 2E 2A 26 1E 02 4E FF FF 00 1C 01 E1 20 01 0F 05 00 00 02 1F
void Thermostat::process_RC300Set(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr || model() == EMSdevice::EMS_DEVICE_FLAG_CR11) {
        return;
    }

    // NOTE when setting the room temp we pick from two values, hopefully one is correct!
    // manual is position 10
    // comfort is position 2, there are 3 levels in pos 3, 2, 1
    // eco is position 4
    // auto is position 8, temporary until next switch
    // actual setpoint taken from RC300Monitor (MichaelDvP 12.06.2020)
    // has_update(telegram, hc->selTemp, 8, 1);  // single byte conversion, value is * 2 - auto?
    // has_update(telegram, hc->selTemp, 10, 1); // single byte conversion, value is * 2 - manual

    // set mode for CR120, BC400, https://github.com/emsesp/EMS-ESP32/discussions/1779
    has_update(telegram, hc->mode_new, 21);  // for BC400, CR120
    has_bitupdate(telegram, hc->mode, 0, 0); // RC300, RC100
    has_update(telegram, hc->daytemp, 2);    // is * 2
    has_update(telegram, hc->nighttemp, 4);  // is * 2

    // has_update(telegram, hc->tempautotemp, 8); // is * 2
    // manipulate tempautotemp to show -1°C (with scale 0.5°C) if value is 0xFF
    // see https://github.com/emsesp/EMS-ESP32/issues/321
    int8_t tat = hc->tempautotemp;
    telegram->read_value(tat, 8);
    if ((uint8_t)tat == 0xFF) {
        tat = -2;
    }
    has_update(hc->tempautotemp, tat);

    if (model() == EMS_DEVICE_FLAG_CR120) {
        has_update(telegram, hc->manualtemp, 22); // is * 2
    } else {
        has_update(telegram, hc->manualtemp, 10); // is * 2
    }
    has_enumupdate(telegram, hc->program, 11, 1); // timer program 1 or 2

    has_enumupdate(telegram, hc->reducemode, 5, 1); // 1-outdoor temp threshold, 2-room temp threshold, 3-reduced mode
    has_update(telegram, hc->reducetemp, 9);
    has_update(telegram, hc->noreducetemp, 12);
    has_update(telegram, hc->cooltemp, 17);              // see https://github.com/emsesp/EMS-ESP32/issues/590 and 2456
    has_enumupdate(telegram, hc->switchProgMode, 19, 1); // 1-level, 2-absolute
    has_update(telegram, hc->redThreshold, 20);
    has_update(telegram, hc->boost, 23);
    has_update(telegram, hc->boosttime, 24);
    has_update(telegram, hc->cooling, 28);
}

// types 0x2AF ff
// RC300Summer(0x02AF), data: 00 28 00 00 3C 26 00 00 19 0F 00 (from a heatpump)
void Thermostat::process_RC300Summer(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->roominfluence, 0);
    has_update(telegram, hc->roominfl_factor, 1); // is * 10
    has_update(telegram, hc->offsettemp, 2);
    has_update(telegram, hc->solarInfl, 3);
    if (!is_received(summer2_typeids[hc->hc()])) {
        has_update(telegram, hc->summertemp, 6);
        has_update(telegram, hc->summersetmode, 7);
    }

    if (hc->heatingtype < 3) {
        has_update(telegram, hc->designtemp, 4);
    } else {
        has_update(telegram, hc->designtemp, 5);
    }

    has_update(telegram, hc->minflowtemp, 8);
    has_update(telegram, hc->fastHeatup, 10);
}

// types 0x471 ff summer2_typeids
// (0x473), data: 00 11 04 01 01 1C 08 04
void Thermostat::process_RC300Summer2(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    if (hc->statusbyte & 1) {
        has_update(telegram, hc->summersetmode, 0);
        has_update(hc->hpoperatingmode, EMS_VALUE_UINT8_NOTSET);
    } else {
        has_update(telegram, hc->hpoperatingmode, 0);
        has_update(hc->summersetmode, EMS_VALUE_UINT8_NOTSET);
    }
    has_update(telegram, hc->summertemp, 1);
    has_update(telegram, hc->heatondelay, 2);
    has_update(telegram, hc->heatoffdelay, 3);
    has_update(telegram, hc->instantstart, 4);
    has_update(telegram, hc->coolstart, 5);
    has_update(telegram, hc->coolondelay, 6);
    has_update(telegram, hc->cooloffdelay, 7);
}

// types 0x29B ff
// Thermostat(0x10) -> Me(0x0B), RC300Curves(0x29B), data: 01 01 00 FF FF 01 05 30 52
void Thermostat::process_RC300Curve(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_enumupdate(telegram, hc->controlmode, 0, 1); // 1-weather_compensated, 2-outside_footpoint,3-n/a, 4-room --  RC310
    has_update(telegram, hc->heatingtype, 1);        // 1=radiator, 2=convector, 3=floor
    has_update(telegram, hc->switchonoptimization, 4);
    has_enumupdate(telegram, hc->nofrostmode, 5, 1); // 1-room, 2-outdoor, 3- room & outdoor
    has_update(telegram, hc->nofrosttemp, 6);

    if (hc->heatingtype < 2) {
        has_update(telegram, hc->maxflowtemp, 8);
    } else {
        has_update(telegram, hc->maxflowtemp, 7);
    }
}

// types 0x31B
void Thermostat::process_RC300WWtemp(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(0, true);
    has_update(telegram, dhw->wwSetTemp_, 0);
    has_update(telegram, dhw->wwSetTempLow_, 1);
}

// type 02F5
// RC300WWmode(0x2F5), data: 01 FF 04 00 00 00 08 05 00 08 04 00 00 00 00 00 00 00 00 00 01
// RC300WWmode(0x2F6), data: 02 FF 04 00 00 00 08 05 00 08 04 00 00 00 00 00 00 00 00 00 01
void Thermostat::process_RC300WWmode(std::shared_ptr<const Telegram> telegram) {
    uint8_t circuit = 0;
    telegram->read_value(circuit, 0); // 00-no circuit, 01-boiler, 02-mixer
    auto dhw = dhw_circuit(telegram->type_id - 0x2F5, circuit != 0);
    if (dhw == nullptr) {
        return;
    }
    // circulation pump see: https://github.com/Th3M3/buderus_ems-wiki/blob/master/Einstellungen%20der%20Bedieneinheit%20RC310.md
    has_update(telegram, dhw->wwCircPump_, 1); // FF=off, 0=on ?

    if (model() == EMSdevice::EMS_DEVICE_FLAG_BC400) {
        const uint8_t modes[] = {0, 5, 1, 2, 4}; // off, eco+, eco, comfort, auto
        uint8_t       wwmode  = dhw->wwMode_ < sizeof(modes) ? modes[dhw->wwMode_] : EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(wwmode, 2);
        const uint8_t modes1[] = {0, 2, 3, 0, 4, 1};
        has_update(dhw->wwMode_, wwmode < sizeof(modes1) ? modes1[wwmode] : EMS_VALUE_UINT8_NOTSET);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000) {
        // https://github.com/emsesp/EMS-ESP32/pull/1722#discussion_r1582823521
        const uint8_t modes[] = {1, 2, 5}; // normal, comfort, eco+
        uint8_t       wwmode  = dhw->wwMode_ < sizeof(modes) ? modes[dhw->wwMode_] : EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(wwmode, 2);
        const uint8_t modes1[] = {0, 0, 1, 0, 0, 2}; // 0=normal (1), 1=comfort(2), 2=eco+(5)
        has_update(dhw->wwMode_, wwmode < sizeof(modes1) ? modes1[wwmode] : EMS_VALUE_UINT8_NOTSET);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        const uint8_t modes[] = {1, 2, 4}; //// 0=normal (1), 1=comfort(2), 2=auto(4)
        uint8_t       wwmode  = dhw->wwMode_ < sizeof(modes) ? modes[dhw->wwMode_] : EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(wwmode, 2);
        const uint8_t modes1[] = {0, 0, 1, 0, 2, 0};
        has_update(dhw->wwMode_, wwmode < sizeof(modes1) ? modes1[wwmode] : EMS_VALUE_UINT8_NOTSET);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        const uint8_t modes[] = {0, 2, 3}; //// 0=off(0), 1=on(2), 2=auto(3)
        uint8_t       wwmode  = dhw->wwMode_ < sizeof(modes) ? modes[dhw->wwMode_] : EMS_VALUE_UINT8_NOTSET;
        telegram->read_value(wwmode, 2);
        const uint8_t modes1[] = {0, 0, 1, 2, 0, 0};
        has_update(dhw->wwMode_, wwmode < sizeof(modes1) ? modes1[wwmode] : EMS_VALUE_UINT8_NOTSET);
    } else {
        has_update(telegram, dhw->wwMode_, 2); // 0=off, 1=low, 2=high, 3=auto, 4=own prog
    }
    has_update(telegram, dhw->wwCircMode_, 3);        // 0=off, 1=on, 2=auto, 4=own?
    has_update(telegram, dhw->wwChargeDuration_, 10); // value in steps of 15 min
    has_update(telegram, dhw->wwCharge_, 11);         // 0=off, 0xFF=on
    has_update(telegram, dhw->wwDisinfecting_, 5);    // 0=off, 0xFF=on
    has_update(telegram, dhw->wwDisinfectHour_, 6);   // value in steps of 15 min
    has_update(telegram, dhw->wwDisinfectDay_, 7);    // 0-6 Day of week, 7 every day
    has_update(telegram, dhw->wwDailyHeating_, 8);    // 0=off, 0xFF=on
    has_update(telegram, dhw->wwDailyHeatTime_, 9);   // value in steps of 15 min
}

// types 0x31D and 0x31E
// RC300WWmode2(0x31D), data: 00 00 09 07
void Thermostat::process_RC300WWmode2(std::shared_ptr<const Telegram> telegram) {
    auto dhw = dhw_circuit(telegram->type_id - 0x31D);
    if (dhw == nullptr) {
        return;
    }
    // 0x31D for WW system 1, 0x31E for WW system 2
    // pos 1 = holiday mode
    // pos 2 = current status of DHW setpoint
    // pos 3 = current status of DHW circulation pump
    has_update(telegram, dhw->wwExtra_, 0); // 0=no, 1=yes
}

// 0x23A damped outdoor temp
void Thermostat::process_RC300OutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, dampedoutdoortemp2_, 0); // is *10
}

// 0x240 RC300 parameter, 0x0241 for CW100, see https://github.com/emsesp/EMS-ESP32/issues/2290
// RC300Settings(0x240), data: 26 00 03 00 00 00 00 00 FF 01 F6 06 FF 00 00 00 00 00 00 00 00 00 00
void Thermostat::process_RC300Settings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, ibaCalIntTemperature_, 7);
    has_update(telegram, ibaDamping_, 8);
    has_enumupdate(telegram, ibaBuildingType_, 9, 1); // 1=light, 2=medium, 3=heavy
    has_update(telegram, ibaMinExtTemperature_, 10);
    has_update(telegram, hasSolar_, 13);
}

// 0x2CC - e.g. wwprio for  RC310 hcx parameter
void Thermostat::process_RC300Set2(std::shared_ptr<const Telegram> telegram) {
    // typeids are not in a row.  hc:0x2CC, hc2: 0x2CE  for RC310
    // telegram is either offset 3 with data length of 1 and values 0/1 (radiators) - 10 0B FF 03 01 CC 01 F6
    // or offset 0 with data length of 6 bytes - offset 3 values are 0x00 or 0xFF - 10 0B FF 00 01 CE FF 13 0A FF 1E 00 20

    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->wwprio, 3);
}

// 0x267 RC300 floordrying
void Thermostat::process_RC300Floordry(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, floordrystatus_, 0);
    has_update(telegram, floordrytemp_, 1);
}

// 0x269 - 0x26D  RC300 EMS+ holidaymodes 1 to 5
// special case R3000 only date in 0x269, CR50 only 0x043F
void Thermostat::process_RC300Holiday(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset || telegram->message_length < 6) {
        return;
    }
    char data[sizeof(vacation[0]) + 4];
    snprintf(data,
             sizeof(data),
             "%02d.%02d.%04d-%02d.%02d.%04d",
             telegram->message_data[2],
             telegram->message_data[1],
             telegram->message_data[0] + 2000,
             telegram->message_data[5],
             telegram->message_data[4],
             telegram->message_data[3] + 2000);
    has_update(vacation[0], data, sizeof(vacation[0]));
}

// 0x291 ff.  HP mode
// thermostat(0x10) -W-> Me(0x0B), HPMode(0x0291), data: 01 00 00 03 FF 00
void Thermostat::process_HPMode(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->control, 3);
    has_update(telegram, hc->hpmode, 5);
}

// 0x467 ff HP settings
void Thermostat::process_HPSet(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, hc->dewoffset, 4);     // 7-35°C
    has_update(telegram, hc->roomtempdiff, 3);  // 1-10K
    has_update(telegram, hc->hpminflowtemp, 0); // 2-10K
}

// type 0x41 - data from the RC30 thermostat(0x10) - 14 bytes long
// RC30Monitor(0x41), data: 80 20 00 AC 00 00 00 02 00 05 09 00 AC 00
void Thermostat::process_RC30Monitor(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->selTemp, 1, 1); // is * 2, force as single byte
    has_update(telegram, hc->roomTemp, 2);

    add_ha_climate(hc);
}

// type 0xA7 - for reading the mode from the RC30 thermostat (0x10) and all the installation settings
// RC30Set(0xA7), data: 01 00 FF F6 01 06 00 01 0D 00 00 FF FF 01 02 02 02 00 00 05 1F 05 1F 01 0E 00 FF
// RC30Set(0xA7), data: 00 00 20 02 (offset 27)
void Thermostat::process_RC30Set(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }
    has_update(telegram, ibaLanguage_, 0);          // language on Thermostat: 0 german, 1 dutch, 2 french, 3 italian
    has_update(telegram, ibaCalIntTemperature_, 1); // offset int. temperature sensor, by * 0.1 Kelvin
    has_update(telegram, autodst_, 2);              // Automatic change Daylight Saving time: (0x00 = off, 0xFF = on)
    has_update(telegram, ibaBuildingType_, 4);      // building type: 0 = light, 1 = medium, 2 = heavy
    has_update(telegram, ibaClockOffset_, 10);      // offset (in sec) to clock, 0xff = -1 s, 0x02 = 2 s
    has_update(telegram, backlight_, 12);           // Keyboard lighting: (0x00 = off, 0xFF = on)
    has_update(telegram, mixingvalves_, 17);        // Number of Mixing Valves: (0x00=0, 0x01=1, 0x02=2)
    has_update(telegram, brightness_, 18);          // Screen brightness 0F=dark F1=light
    has_update(telegram, hc->mode, 23);
    has_update(telegram, hc->nofrosttemp, 24);  // Set Temperature when mode is Off / 10 (e.g.: 0x0F = 7.5 degrees Celsius)
    has_update(telegram, heatingpid_, 25);      // PID setting 00=1 01=2 02=3
    has_update(telegram, preheating_, 26);      // Preheating in the clock program: (0x00 = off, 0xFF = on)
    has_update(telegram, hc->tempautotemp, 28); // is * 2
    has_update(telegram, hc->manualtemp, 29);   // manualtemp is * 2
}

// type 0x40 (HC1) - for reading the operating mode from the RC30 thermostat (0x10)
// RC30Temp(0x40), data: 01 01 02 20 24 28 2A 1E 0E 00 01 5A 32 05 4B 2D 00 28 00 3C FF 11 00 05 00
void Thermostat::process_RC30Temp(std::shared_ptr<const Telegram> telegram) {
    // check to see we have a valid type. heating: 1 radiator, 2 convectors, 3 floors
    if (telegram->offset == 0 && telegram->message_data[0] == 0x00) {
        return;
    }

    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->heatingtype, 0); // 0- off, 1-radiator, 2-convector, 3-floor
    has_update(telegram, hc->controlmode, 1); // 0-outdoortemp, 1-roomtemp
    has_update(telegram, hc->nighttemp, 3);   // is * 2 (T1)
    has_update(telegram, hc->daylowtemp, 4);  // is * 2 (T2)
    has_update(telegram, hc->daymidtemp, 5);  // is * 2 (T3)
    has_update(telegram, hc->daytemp, 6);     // is * 2 (T4)
    has_update(telegram, hc->holidaytemp, 7); // is * 2
}

// type 0x3E (HC1), 0x48 (HC2), 0x52 (HC3), 0x5C (HC4) - data from the RC35 thermostat (0x10) - 16 bytes
void Thermostat::process_RC35Monitor(std::shared_ptr<const Telegram> telegram) {
    // Check if heatingciruit is active, see https://github.com/emsesp/EMS-ESP32/issues/786
    // roomtemp is measured value or 7D00 on active hc's, zero on inactive
    uint16_t active = 0;
    if (!telegram->read_value(active, 3)) {
        return;
    }

    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->selTemp, 2, 1); // is * 2, force to single byte, is 0 in summermode
    has_update(telegram, hc->roomTemp, 3);   // is * 10 - or 0x7D00 if thermostat is mounted on boiler

    has_bitupdate(telegram, hc->modetype, 1, 1);
    has_bitupdate(telegram, hc->summermode, 1, 0);
    has_bitupdate(telegram, hc->holidaymode, 0, 5);

    has_update(telegram, hc->targetflowtemp, 14);

    add_ha_climate(hc);
}

// type 0x3D (HC1), 0x47 (HC2), 0x51 (HC3), 0x5B (HC4) - Working Mode Heating - for reading the mode from the RC35 thermostat (0x10)
void Thermostat::process_RC35Set(std::shared_ptr<const Telegram> telegram) {
    // check to see we have a valid type. heating: 1 radiator, 2 convectors, 3 floors, 4 room supply
    if (telegram->offset == 0 && telegram->message_data[0] == 0x00) {
        return;
    }

    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    has_update(telegram, hc->heatingtype, 0);   // 0- off, 1-radiator, 2-convector, 3-floor
    has_update(telegram, hc->nighttemp, 1);     // is * 2
    has_update(telegram, hc->daytemp, 2);       // is * 2
    has_update(telegram, hc->holidaytemp, 3);   // is * 2
    has_update(telegram, hc->roominfluence, 4); // is * 1
    has_update(telegram, hc->offsettemp, 6);    // is * 2
    has_update(telegram, hc->mode, 7);          // night, day, auto

    has_update(telegram, hc->switchonoptimization, 19); // 0xFF for on

    has_update(telegram, hc->wwprio, 21);         // 0xFF for on
    has_update(telegram, hc->summertemp, 22);     // is * 1
    has_update(telegram, hc->nofrosttemp, 23);    // is * 1
    has_update(telegram, hc->nofrostmode, 28);    // 0-off, 1-outdoor, 2-roomtemp 5°C
    has_update(telegram, hc->flowtempoffset, 24); // is * 1, only in mixed circuits
    has_update(telegram, hc->reducemode, 25);     // 0-nofrost, 1-reduce, 2-roomhold, 3-outdoorhold
    has_update(telegram, hc->control, 26);        // 0-off, 1-RC20 (remote), 2-RC35
    has_update(telegram, hc->controlmode, 33);    // 0-outdoortemp, 1-roomtemp
    has_update(telegram, hc->tempautotemp, 37);
    has_update(telegram, hc->noreducetemp, 38);  // outdoor temperature for no reduce
    has_update(telegram, hc->reducetemp, 39);    // temperature for off/reduce
    has_update(telegram, hc->vacreducetemp, 40); // temperature for off/reduce in vacations
    has_update(telegram, hc->vacreducemode, 41); // vacations reduce mode
    has_update(telegram, hc->minflowtemp, 16);

    // RC35 stores values for floorheating in different position
    if (hc->heatingtype == 3 && model() == EMSdevice::EMS_DEVICE_FLAG_RC35) {
        has_update(telegram, hc->designtemp, 36);  // is * 1
        has_update(telegram, hc->maxflowtemp, 35); // is * 1
    } else {                                       // radiator/convector
        has_update(telegram, hc->designtemp, 17);  // is * 1
        has_update(telegram, hc->maxflowtemp, 15); // is * 1
    }
    setValueEnum(&hc->heatingtype, hc->control == 1 ? FL_(enum_heatingtype2) : FL_(enum_heatingtype));
}

// type 0x3F (HC1), 0x49 (HC2), 0x53 (HC3), 0x5D (HC4) - timer setting
void Thermostat::process_RC35Timer(std::shared_ptr<const Telegram> telegram) {
    auto hc = heating_circuit(telegram);
    if (hc == nullptr) {
        return;
    }

    uint8_t prog = telegram->type_id == timer_typeids[hc->hc()] ? 0 : 1;
    if ((telegram->message_length == 2 && telegram->offset < 83 && !(telegram->offset & 1))
        || (!telegram->offset && telegram->message_length > 1 && !prog && !strlen(hc->switchtime1))
        || (!telegram->offset && telegram->message_length > 1 && prog && !strlen(hc->switchtime2))) {
        char    data[sizeof(hc->switchtime1)];
        uint8_t no   = telegram->offset / 2;
        uint8_t day  = telegram->message_data[0] >> 5;
        uint8_t on   = model() == EMSdevice::EMS_DEVICE_FLAG_RC30 ? telegram->message_data[0] & 7 : telegram->message_data[0] & 1;
        uint8_t time = telegram->message_data[1];

        // we use EN settings for the day abbreviation
        auto sday = (FL_(enum_dayOfWeek)[day][0]);
        if (day == 7) {
            snprintf(data, sizeof(data), "%02d not_set", no);
        } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
            snprintf(data, sizeof(data), "%02d %s %02d:%02d T%d", no, sday, time / 6, 10 * (time % 6), on);
        } else {
            snprintf(data, sizeof(data), "%02d %s %02d:%02d %s", no, sday, time / 6, 10 * (time % 6), on ? "on" : "off");
        }
        if (!prog) {
            strlcpy(hc->switchtime1, data, sizeof(hc->switchtime1));
            has_update(hc->switchtime1); // always publish
        } else {
            strlcpy(hc->switchtime2, data, sizeof(hc->switchtime2));
            has_update(hc->switchtime2);
            if (is_fetch(telegram->type_id)) {
                toggle_fetch(telegram->type_id, false);
            }
        }
    }

    has_update(telegram, hc->program, 84); // 0 .. 10, 0-userprogram 1, 10-userprogram 2
    has_update(telegram, hc->pause, 85);   // time in hours
    has_update(telegram, hc->party, 86);   // time in hours

    if (telegram->message_length + telegram->offset >= 92 && telegram->offset <= 87) {
        char data[sizeof(hc->vacation) + 4]; // avoid compiler warning
        snprintf(data,
                 sizeof(data),
                 "%02d.%02d.%04d-%02d.%02d.%04d",
                 telegram->message_data[87 - telegram->offset],
                 telegram->message_data[88 - telegram->offset],
                 telegram->message_data[89 - telegram->offset] + 2000,
                 telegram->message_data[90 - telegram->offset],
                 telegram->message_data[91 - telegram->offset],
                 telegram->message_data[92 - telegram->offset] + 2000);
        has_update(hc->vacation, data, sizeof(hc->vacation));
    }

    if (telegram->message_length + telegram->offset >= 98 && telegram->offset <= 93) {
        char data[sizeof(hc->holiday) + 4]; // avoid compiler warning
        snprintf(data,
                 sizeof(data),
                 "%02d.%02d.%04d-%02d.%02d.%04d",
                 telegram->message_data[93 - telegram->offset],
                 telegram->message_data[94 - telegram->offset],
                 telegram->message_data[95 - telegram->offset] + 2000,
                 telegram->message_data[96 - telegram->offset],
                 telegram->message_data[97 - telegram->offset],
                 telegram->message_data[98 - telegram->offset] + 2000);
        has_update(hc->holiday, data, sizeof(hc->holiday));
    }
}

// type 0x9A (HC1)
void Thermostat::process_RC30Vacation(std::shared_ptr<const Telegram> telegram) {
    if ((telegram->offset + telegram->message_length) > 57) {
        return;
    }
    static uint8_t vacation_telegram[57] = {0}; // make a copy of the whole telegram to access blocks
    memcpy(&vacation_telegram[telegram->offset], telegram->message_data, telegram->message_length);
    for (uint8_t index = 0, pos = 0; index < 8; index++, pos += 7) {
        char data[sizeof(vacation[0]) + 4]; // avoid compiler warning
        snprintf(data,
                 sizeof(data),
                 "%02d.%02d.%04d-%02d.%02d.%04d",
                 vacation_telegram[1 + pos],
                 vacation_telegram[2 + pos],
                 vacation_telegram[3 + pos] + 2000,
                 vacation_telegram[4 + pos],
                 vacation_telegram[5 + pos],
                 vacation_telegram[6 + pos] + 2000);
        if (vacation_telegram[1 + pos]) { // data is set (day >  0)
            has_update(vacation[index], data, sizeof(vacation[0]));
        }
    }
}

// process_RCTime - type 0x06 - date and time from a thermostat - 12 or 15 bytes long
void Thermostat::process_RCTime(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 8) {
        return;
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_EASY) {
        return; // not supported
    }

    static uint8_t setTimeRetry = 0;
    uint8_t        dst          = 0xFE;
    bool           use_dst      = !telegram->read_value(dst, 9) || dst == 0xFF;
    if ((telegram->message_data[7] & 0x0C) && has_command(&dateTime_)) { // date and time not valid
        if (setTimeRetry < 3) {
            if (!use_dst) {
                set_datetime("ntp", 0); // set from NTP without dst
            } else {
                set_datetime("ntp", -1); // set from NTP
            }
            setTimeRetry++;
        }
        return;
    }

    // check clock
    time_t now   = time(nullptr);
    tm *   tm_   = localtime(&now);
    bool   tset_ = tm_->tm_year > 110;                       // year 2010 and up, time is valid
    tm_->tm_year = (telegram->message_data[0] & 0x7F) + 100; // IVT
    tm_->tm_mon  = telegram->message_data[1] - 1;
    tm_->tm_mday = telegram->message_data[3];
    tm_->tm_hour = telegram->message_data[2];
    tm_->tm_min  = telegram->message_data[4];
    tm_->tm_sec  = telegram->message_data[5];
    if (use_dst) {
        tm_->tm_isdst = telegram->message_data[7] & 0x01;
    }

    // render date to DD.MM.YYYY HH:MM and publish
    char newdatetime[sizeof(dateTime_)];
    strftime(newdatetime, sizeof(dateTime_), "%d.%m.%Y %H:%M", tm_);
    has_update(dateTime_, newdatetime, sizeof(dateTime_));

    bool   ivtclock     = (telegram->message_data[0] & 0x80) == 0x80; // dont sync ivt-clock, #439
    bool   junkersclock = model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS;
    time_t ttime        = mktime(tm_); // thermostat time
    // correct thermostat clock if we have valid ntp time, and could write the command
    if (!ivtclock && !junkersclock && tset_ && EMSESP::system_.ntp_connected() && !EMSESP::system_.readonly_mode() && has_command(&dateTime_)) {
        double difference = difftime(now, ttime);
        if (difference > 15 || difference < -15) {
            if (setTimeRetry < 3) {
                if (!use_dst) {
                    set_datetime("ntp", 0); // set from NTP without dst
                    LOG_INFO("thermostat time correction from ntp, ignoring dst");
                } else {
                    set_datetime("ntp", -1); // set from NTP
                    LOG_INFO("thermostat time correction from ntp");
                }
                setTimeRetry++;
            }
        } else {
            setTimeRetry = 0;
        }
    }
#ifndef EMSESP_STANDALONE
    if (!tset_ && tm_->tm_year > 110) { // emsesp clock not set, but thermostat clock
        if (ivtclock) {
            tm_->tm_isdst = -1;          // determine dst
            ttime         = mktime(tm_); // thermostat time
        }
        struct timeval newnow = {.tv_sec = ttime, .tv_usec = 0};
        settimeofday(&newnow, nullptr);
        LOG_INFO("ems-esp time set from thermostat");
    }
#endif
}

// process_RCError - type 0xA2 - error message - 14 bytes long
// 10 00 A2 00 41 32 32 03 30 00 02 00 00 00 00 00 00 02 CRC
//              A  2  2  816
void Thermostat::process_RCError(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }

    telegram->read_value(errorNumber_, 3);
    char code[sizeof(errorCode_)];
    code[0] = telegram->message_data[0];
    code[1] = telegram->message_data[1];
    code[2] = telegram->message_data[2];
    snprintf(&code[3], sizeof(code) - 3, "(%d)", errorNumber_);
    has_update(errorCode_, code, sizeof(errorCode_));
}

// 0x12 and 0x13 error log
// RCErrorMessage(0x12), data: 32 32 03 30 95 0A 0A 15 18 00 01 19 32 32 03 30 95 0A 09 05 18 00 01 19 31 38 03
// RCErrorMessage(0x12), data: 39 95 08 09 0F 19 00 01 17 64 31 03 34 95 07 10 08 00 00 01 70 (offset 27)
void Thermostat::process_RCErrorMessage(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 11) {
        return;
    }

    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
    if (telegram->message_data[4] & 0x80) {          // valid date
        static uint32_t lastCodeDate_           = 0; // last code date
        char            code[sizeof(lastCode_)] = {0};
        uint16_t        codeNo                  = EMS_VALUE_UINT16_NOTSET;
        code[0]                                 = telegram->message_data[0];
        code[1]                                 = telegram->message_data[1];
        code[2]                                 = 0;
        telegram->read_value(codeNo, 2);
        uint16_t year     = (telegram->message_data[4] & 0x7F) + 2000;
        uint8_t  month    = telegram->message_data[5];
        uint8_t  day      = telegram->message_data[7];
        uint8_t  hour     = telegram->message_data[6];
        uint8_t  min      = telegram->message_data[8];
        uint16_t duration = EMS_VALUE_INT16_NOTSET;
        uint32_t date     = (year - 2000) * 535680UL + month * 44640UL + day * 1440UL + hour * 60 + min;
        telegram->read_value(duration, 9);
        // store only the newest code from telegrams 12 and 13
        if (date > lastCodeDate_) {
            lastCodeDate_ = date;
            snprintf(&code[2], sizeof(code) - 2, "(%d) %02d.%02d.%d %02d:%02d (%d min)", codeNo, day, month, year, hour, min, duration);
            has_update(lastCode_, code, sizeof(lastCode_));
        }
    }
}

/*
 *
 * *** settings ***
 *
 */

// hp mode RC300
bool Thermostat::set_roomtempdiff(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(hp_typeids[hc->hc()], 3, v, hp_typeids[hc->hc()]);
        return true;
    }
    return false;
}
bool Thermostat::set_dewoffset(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(hp_typeids[hc->hc()], 4, v, hp_typeids[hc->hc()]);
        return true;
    }
    return false;
}

bool Thermostat::set_hpminflowtemp(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (Helpers::value2temperature(value, v)) {
        write_command(hp_typeids[hc->hc()], 0, v, hp_typeids[hc->hc()]);
        return true;
    }
    return false;
}

bool Thermostat::set_hpmode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_hpmode))) {
        return false;
    }
    write_command(hpmode_typeids[hc->hc()], 5, v, hpmode_typeids[hc->hc()]);
    return true;
}

// 0xBB Hybrid pump
bool Thermostat::set_hybridStrategy(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_hybridStrategy))) {
        return false;
    }
    write_command(0xBB, 12, v + 1, 0xBB);
    return true;
}

bool Thermostat::set_switchOverTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0xBB, 13, v, 0xBB);
    return true;
}

bool Thermostat::set_energyCostRatio(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 14, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Thermostat::set_fossileFactor(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 15, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Thermostat::set_electricFactor(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 16, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Thermostat::set_delayBoiler(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0xBB, 18, v, 0xBB);
    return true;
}

bool Thermostat::set_tempDiffBoiler(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }
    write_command(0xBB, 19, v, 0xBB);
    return true;
}

bool Thermostat::set_pvEnableWw(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x23E, 3, v ? 0xFF : 0, 0x23E);
        return true;
    }
    return false;
}

bool Thermostat::set_pvRaiseHeat(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x23E, 0, v, 0x23E);
        return true;
    }
    return false;
}

bool Thermostat::set_pvLowerCool(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x23E, 5, v, 0x23E);
        return true;
    }
    return false;
}

// 0xA5 - Set minimum external temperature
bool Thermostat::set_minexttemp(const char * value, const int8_t id) {
    int mt;
    if (!Helpers::value2temperature(value, mt)) {
        return false;
    }

    if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC20_N) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC25)) {
        write_command(0xAD, 14, mt, 0xAD);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        write_command(0x241, 10, mt, 0x241);
    } else if (isRC300()) {
        write_command(0x240, 10, mt, 0x240);
    } else {
        write_command(EMS_TYPE_IBASettings, 5, mt, EMS_TYPE_IBASettings);
    }

    return true;
}

// 0xA5/0xA7 - Clock offset
bool Thermostat::set_clockoffset(const char * value, const int8_t id) {
    int co;
    if (!Helpers::value2number(value, co)) {
        return false;
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30Settings, 10, co, EMS_TYPE_RC30Settings);
    } else {
        write_command(EMS_TYPE_IBASettings, 12, co, EMS_TYPE_IBASettings);
    }

    return true;
}

// 0xA5/0xA7 - Calibrate internal temperature
bool Thermostat::set_calinttemp(const char * value, const int8_t id) {
    float ct;
    if (!Helpers::value2temperature(value, ct, true)) {
        return false;
    }

    auto t = (int8_t)(ct * 10);
    LOG_DEBUG("Calibrating internal temperature to %d.%d C", t / 10, t < 0 ? -t % 10 : t % 10);

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        write_command(0xB0, 0, t, 0xB0);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30Settings, 1, t, EMS_TYPE_RC30Settings);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100H) {
        write_command(0x273, 0, t, 0x273);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        write_command(0x241, 7, t, 0x241);
    } else if (isRC300()) {
        write_command(0x240, 7, t, 0x240);
    } else {
        write_command(EMS_TYPE_IBASettings, 2, t, EMS_TYPE_IBASettings);
    }

    return true;
}

// 0xA5 - Set the display settings, RC30_N
bool Thermostat::set_display(const char * value, const int8_t id) {
    uint8_t ds;
    if (!Helpers::value2enum(value, ds, FL_(enum_ibaMainDisplay))) {
        return false;
    }

    write_command(EMS_TYPE_IBASettings, 0, ds, EMS_TYPE_IBASettings);

    return true;
}

// 0xA7 - Set Screen brightness
bool Thermostat::set_brightness(const char * value, const int8_t id) {
    int bo;
    if (!Helpers::value2number(value, bo, -15, 15)) {
        return false;
    }

    write_command(EMS_TYPE_RC30Settings, 18, bo, EMS_TYPE_RC30Settings);

    return true;
}

// thermostat/hc<x>/remotetemp
// value of -1 to turn off
bool Thermostat::set_remotetemp(const char * value, const int8_t id) {
    float f;
    if (!Helpers::value2temperature(value, f)) {
        return false;
    }

    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    if (f > 100 || f < 0) {
        hc->remotetemp = EMS_VALUE_INT16_NOTSET;
    } else {
        hc->remotetemp = (int16_t)(f * 10);
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        Roomctrl::set_remotetemp(Roomctrl::FB10, hc->hc(), hc->remotetemp); // FB10
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N) {
        Roomctrl::set_remotetemp(Roomctrl::RC20, hc->hc(), hc->remotetemp); // RC20
    } else if (isRC300()) {
        if (hc->control == 1 || hc->control == 0) {
            Roomctrl::set_remotetemp(Roomctrl::RC200, hc->hc(), hc->remotetemp);  // RC200
        } else if (hc->control == 3) {                                            // RC100H(3)
            Roomctrl::set_remotetemp(Roomctrl::RC100H, hc->hc(), hc->remotetemp); // RC100H
        } else if (hc->control == 2) {                                            // RC100(2)
            Roomctrl::set_remotetemp(Roomctrl::RC100, hc->hc(), hc->remotetemp);  // RC100
        } else if (hc->control == 6) {
            Roomctrl::set_remotetemp(Roomctrl::RT800, hc->hc(), hc->remotetemp);
        } else {
            hc->remotetemp = EMS_VALUE_INT16_NOTSET;
            Roomctrl::set_remotetemp(0, hc->hc(), EMS_VALUE_INT16_NOTSET); // unknown remote set, switch off
            return false;
        }
    }

    return true;
}

bool Thermostat::set_remotehum(const char * value, const int8_t id) {
    int h;
    if (!Helpers::value2number(value, h)) {
        return false;
    }

    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    if (h > 100 || h < 0) {
        hc->remotehum = EMS_VALUE_UINT8_NOTSET;
    } else {
        hc->remotehum = h;
    }

    if (hc->control == 3) {
        Roomctrl::set_remotehum(Roomctrl::RC100H, hc->hc(), hc->remotehum); // RC100H
        return true;
    } else if (hc->control == 6) {
        Roomctrl::set_remotehum(Roomctrl::RT800, hc->hc(), hc->remotehum);
        return true;
    }
    Roomctrl::set_remotehum(Roomctrl::RC100H, hc->hc(), EMS_VALUE_UINT8_NOTSET); // RC100H
    return false;
}

// 0xA5/0xA7 - Set the building settings
bool Thermostat::set_building(const char * value, const int8_t id) {
    uint8_t bd;
    if (!Helpers::value2enum(value, bd, FL_(enum_ibaBuildingType))) {
        return false;
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        write_command(0x241, 9, bd + 1, 0x241);
    } else if (isRC300()) {
        write_command(0x240, 9, bd + 1, 0x240);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30Settings, 4, bd, EMS_TYPE_RC30Settings);
    } else {
        write_command(EMS_TYPE_IBASettings, 6, bd, EMS_TYPE_IBASettings);
    }

    return true;
}

// 0xB0/0xA7 - Set RC10 heating pid
bool Thermostat::set_heatingpid(const char * value, const int8_t id) {
    uint8_t pid;
    if (!Helpers::value2enum(value, pid, FL_(enum_PID))) {
        return false;
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        write_command(0xB0, 6, pid, 0xB0);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30Settings, 25, pid, EMS_TYPE_RC30Settings);
    }

    return true;
}

// 0xA5 and 0x0240- Set the damping settings
bool Thermostat::set_damping(const char * value, const int8_t id) {
    bool dmp;
    if (Helpers::value2bool(value, dmp)) {
        if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            write_command(0x241, 8, dmp ? 0xFF : 0, 0x241);
        } else if (isRC300()) {
            write_command(0x240, 8, dmp ? 0xFF : 0, 0x240);
        } else {
            write_command(EMS_TYPE_IBASettings, 21, dmp ? 0xFF : 0, EMS_TYPE_IBASettings);
        }
        return true;
    }
    return false;
}

// 0x0241- Set solar
bool Thermostat::set_solar(const char * value, const int8_t id) {
    bool b;
    if (Helpers::value2bool(value, b)) {
        if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            write_command(0x241, 13, b ? 0xFF : 0, 0x241);
        } else {
            write_command(0x240, 13, b ? 0xFF : 0, 0x240);
        }
        return true;
    }
    return false;
}

// 0xA5/0xA7 Set the language settings
bool Thermostat::set_language(const char * value, const int8_t id) {
    uint8_t lg;

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (!Helpers::value2enum(value, lg, FL_(enum_ibaLanguage_RC30))) {
            return false;
        }
        write_command(EMS_TYPE_RC30Settings, 0, lg, EMS_TYPE_RC30Settings);
    } else {
        if (!Helpers::value2enum(value, lg, FL_(enum_ibaLanguage))) {
            return false;
        }
        write_command(EMS_TYPE_IBASettings, 1, lg, EMS_TYPE_IBASettings);
    }

    return true;
}

// Set the control-mode for hc 0-off, 1-RC20, 2-RC3x
bool Thermostat::set_control(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t ctrl;
    // Junkers
    // 1-FB10, 2-FB100
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS && !has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
        if (Helpers::value2enum(value, ctrl, FL_(enum_j_control))) {
            write_command(set_typeids[hc->hc()], 1, ctrl);
            return true;
        }
        // BC400
        // 1-RC100, 2-RC100H, 3-RC200
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_BC400) {
        if (Helpers::value2enum(value, ctrl, FL_(enum_control2))) {
            write_command(hpmode_typeids[hc->hc()], 3, ctrl);
            hc->control = ctrl; // set in advance, dont wait for verify
            if (hc->remotetemp != EMS_VALUE_INT16_NOTSET && ctrl > 0) {
                if (ctrl == 2) {
                    Roomctrl::set_remotetemp(Roomctrl::RC100, hc->hc(), hc->remotetemp);
                } else if (ctrl == 3) {
                    Roomctrl::set_remotetemp(Roomctrl::RC100H, hc->hc(), hc->remotetemp);
                } else if (ctrl == 6) {
                    Roomctrl::set_remotetemp(Roomctrl::RT800, hc->hc(), hc->remotetemp);
                } else {
                    hc->remotetemp = EMS_VALUE_INT16_NOTSET;
                    Roomctrl::set_remotetemp(0, hc->hc(), hc->remotetemp);
                }
            }
            return true;
        }
        // RC100 or RC300
        // 1-RC200, 2-RC100, 3-RC100H
    } else if (isRC300() || model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        if (Helpers::value2enum(value, ctrl, FL_(enum_control1))) {
            write_command(hpmode_typeids[hc->hc()], 3, ctrl);
            hc->control = ctrl; // set in advance, dont wait for verify
            if (hc->remotetemp != EMS_VALUE_INT16_NOTSET && ctrl > 0) {
                if (ctrl == 1) {
                    Roomctrl::set_remotetemp(Roomctrl::RC200, hc->hc(), hc->remotetemp);
                } else if (ctrl == 2) {
                    Roomctrl::set_remotetemp(Roomctrl::RC100, hc->hc(), hc->remotetemp);
                } else if (ctrl == 3) {
                    Roomctrl::set_remotetemp(Roomctrl::RC100H, hc->hc(), hc->remotetemp);
                } else {
                    hc->remotetemp = EMS_VALUE_INT16_NOTSET;
                    Roomctrl::set_remotetemp(0, hc->hc(), hc->remotetemp);
                }
                // Roomctrl::set_remotetemp(ctrl == 1 ? Roomctrl::RC200 : ctrl == 3 ? Roomctrl::RC100H : Roomctrl::RC100, hc->hc(), hc->remotetemp);
            }
            return true;
        }
    } else if (Helpers::value2enum(value, ctrl, FL_(enum_control))) {
        write_command(set_typeids[hc->hc()], EMS_OFFSET_RC35Set_control, ctrl);
        return true;
    }

    return false;
}

// Set sensor for Junkers, 1-external (from remote), 2-internal, 3-minimum value from int/ext
bool Thermostat::set_roomsensor(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t ctrl;
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS && !has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
        if (Helpers::value2enum(value, ctrl, FL_(enum_roomsensor))) {
            write_command(set_typeids[hc->hc()], 9, ctrl + 1);
            return true;
        }
    }
    return false;
}

// sets the thermostat ww working mode, where mode is a string, ems and ems+
bool Thermostat::set_wwmode(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    uint8_t set;

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode3))) {
            return false;
        }
        write_command(0xB0, 2, set, 0xB0);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_BC400) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode4))) { // off, eco+, eco, comfort, auto
            return false;
        }
        const uint8_t modes[] = {0, 5, 1, 2, 4};
        write_command(0x02F5 + dhw->offset(), 2, modes[set], 0x02F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode6))) { // normal, comfort, auto
            return false;
        }
        const uint8_t modes[] = {0, 2, 4};
        write_command(0x02F5 + dhw->offset(), 2, modes[set], 0x02F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode2))) { // off, on, auto
            return false;
        }
        const uint8_t modes[] = {0, 2, 3};
        write_command(0x02F5 + dhw->offset(), 2, modes[set], 0x02F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000) { // Rego3000 - https://github.com/emsesp/EMS-ESP32/issues/1692
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode5))) {
            return false;
        }
        const uint8_t modes[] = {1, 2, 5};
        write_command(0x02F5 + dhw->offset(), 2, modes[set], 0x02F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC300) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode))) {
            return false;
        }
        write_command(0x02F5 + dhw->offset(), 2, set, 0x02F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode3))) {
            return false;
        }
        write_command(EMS_TYPE_RC30wwSettings, 0, set, EMS_TYPE_RC30wwSettings);
    } else {
        if (!Helpers::value2enum(value, set, FL_(enum_wwMode2))) {
            return false;
        }
        write_command(EMS_TYPE_wwSettings, 2, set, EMS_TYPE_wwSettings);
    }

    return true;
}

//Set ww when thermostat mode is off (RC30)
bool Thermostat::set_wwwhenmodeoff(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    write_command(EMS_TYPE_RC30wwSettings, 1, b ? 0xFF : 0x00, EMS_TYPE_RC30wwSettings);

    return true;
}

// Set ww temperature, ems+
bool Thermostat::set_wwtemp(const char * value, const int8_t id) {
    int t = 0;
    if (!Helpers::value2temperature(value, t, false, 0, 90)) {
        return false;
    }

    write_command(0x031B, 0, t, 0x031B);

    return true;
}

// Set ww low temperature, ems+
bool Thermostat::set_wwtemplow(const char * value, const int8_t id) {
    int t;
    if (!Helpers::value2temperature(value, t)) {
        return false;
    }

    write_command(0x031B, 1, t, 0x031B);

    return true;
}

// Set ww charge RC300, ems+
bool Thermostat::set_wwcharge(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    if ((model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS)) {
        write_command(0x0115, 0, b ? 0xFF : 0x00, 0x01D3);
    } else {
        write_command(0x02F5 + dhw->offset(), 11, b ? 0xFF : 0x00, 0x02F5 + dhw->offset());
    }

    return true;
}

// Set ww charge duration in steps of 15 min, ems+
bool Thermostat::set_wwchargeduration(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    int t;
    if (!Helpers::value2number(value, t)) {
        return false;
    }
    t = (t + 8) / 15;

    write_command(0x2F5 + dhw->offset(), 10, t, 0x02F5 + dhw->offset());
    return true;
}

// set ww prio
bool Thermostat::set_wwprio(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (isRC300()) {
        write_command(set2_typeids[hc->hc()], 3, b ? 0xFF : 0x00, set2_typeids[hc->hc()]);
    } else {
        write_command(set_typeids[hc->hc()], 21, b ? 0xFF : 0x00, set_typeids[hc->hc()]);
    }

    return true;
}

// set cooling
bool Thermostat::set_cooling(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 28, b ? 0x01 : 0x00, set_typeids[hc->hc()]);
    return true;
}

// set cooling delays
bool Thermostat::set_coolondelay(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(summer2_typeids[hc->hc()], 6, v, summer2_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_cooloffdelay(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(summer2_typeids[hc->hc()], 7, v, summer2_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_switchProgMode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    uint8_t set;
    if (!Helpers::value2enum(value, set, FL_(enum_switchProgMode))) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 19, set + 1, set_typeids[hc->hc()]);
    return true;
}

// sets the thermostat ww circulation working mode, where mode is a string
bool Thermostat::set_wwcircmode(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    uint8_t set;

    if (isRC300() || (model() == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        if (!Helpers::value2enum(value, set, FL_(enum_wwCircMode))) {
            return false;
        }
        write_command(0x02F5 + dhw->offset(), 3, set, 0x02F5 + dhw->offset());
        return true;
    }
    if (!Helpers::value2enum(value, set, FL_(enum_wwMode2))) {
        return false;
    }

    write_command(EMS_TYPE_wwSettings, 3, set, EMS_TYPE_wwSettings);

    return true;
}

bool Thermostat::set_wwDailyHeating(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    write_command(0x2F5 + dhw->offset(), 8, b ? 0xFF : 0x00, 0x2F5 + dhw->offset());
    return true;
}

bool Thermostat::set_wwDailyHeatTime(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    int set;
    if (!Helpers::value2number(value, set)) {
        return false;
    }

    if (isRC300() || (model() == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        uint8_t t = (set + 8) / 15;
        if (t > 95) {
            return false;
        }

        write_command(0x2F5 + dhw->offset(), 9, t, 0x2F5 + dhw->offset());
    }
    return true;
}

bool Thermostat::set_wwDisinfect(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    if (isRC300() || (model() == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        write_command(0x2F5 + dhw->offset(), 5, b ? 0xFF : 0x00, 0x2F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30wwSettings, 2, b ? 0xFF : 0x00, EMS_TYPE_RC30wwSettings);
    } else {
        write_command(0x37, 4, b ? 0xFF : 0x00, 0x37);
    }

    return true;
}

bool Thermostat::set_wwDisinfectDay(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    uint8_t set;
    if (!Helpers::value2enum(value, set, FL_(enum_dayOfWeek))) {
        return false;
    }

    if (isRC300() || (model() == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        write_command(0x2F5 + dhw->offset(), 7, set, 0x2F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30wwSettings, 3, set, EMS_TYPE_RC30wwSettings);
    } else {
        write_command(0x37, 5, set, 0x37);
    }

    return true;
}

bool Thermostat::set_wwDisinfectHour(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    int set;
    if (isRC300() || (model() == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        if (!Helpers::value2number(value, set, 0, 1431)) {
            return false;
        }
        write_command(0x2F5 + dhw->offset(), 6, (set + 8) / 15, 0x2F5 + dhw->offset());
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (!Helpers::value2number(value, set, 0, 23)) {
            return false;
        }
        write_command(EMS_TYPE_RC30wwSettings, 4, set, EMS_TYPE_RC30wwSettings);
    } else {
        if (!Helpers::value2number(value, set, 0, 23)) {
            return false;
        }
        write_command(0x37, 6, set, 0x37);
    }

    return true;
}

bool Thermostat::set_wwMaxTemp(const char * value, const int8_t id) {
    int t;
    if (!Helpers::value2temperature(value, t, false, 0, 90)) {
        return false;
    }

    write_command(0x37, 8, t, 0x37);

    return true;
}

bool Thermostat::set_wwOneTimeKey(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    write_command(0x37, 9, b ? 0xFF : 0x00, 0x37);

    return true;
}

// for RC10, 0xB0 or RC30, 0xA7
bool Thermostat::set_backlight(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        write_command(EMS_TYPE_RC30Settings, 12, b ? 0xFF : 0x00, EMS_TYPE_RC30Settings);
    } else {
        write_command(0xB0, 1, b ? 0xFF : 0x00, 0xB0);
    }

    return true;
}

bool Thermostat::set_autodst(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    write_command(EMS_TYPE_RC30Settings, 2, b ? 0xFF : 0x00, EMS_TYPE_RC30Settings);

    return true;
}

bool Thermostat::set_preheating(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }

    write_command(EMS_TYPE_RC30Settings, 26, b ? 0xFF : 0x00, EMS_TYPE_RC30Settings);

    return true;
}

bool Thermostat::set_mixingvalves(const char * value, const int8_t id) {
    int m;
    if (!Helpers::value2number(value, m, 0, 2)) {
        return false;
    }

    write_command(EMS_TYPE_RC30Settings, 17, m, EMS_TYPE_RC30Settings);

    return true;
}

bool Thermostat::set_wwProgMode(const char * value, const int8_t id) {
    uint8_t set;
    if (!Helpers::value2enum(value, set, FL_(enum_wwProgMode))) {
        return false;
    }

    write_command(0x37, 0, set ? 0xFF : 0x00, 0x37);

    return true;
}

bool Thermostat::set_wwCircProg(const char * value, const int8_t id) {
    uint8_t set;
    if (!Helpers::value2enum(value, set, FL_(enum_wwProgMode))) {
        return false;
    }

    write_command(0x37, 1, set ? 0xFF : 0x00, 0x37);

    return true;
}

// set the holiday or vacations as string dd.mm.yyyy-dd.mm.yyyy
bool Thermostat::set_holiday(const char * value, const int8_t id, const bool vacation) {
    auto hc = heating_circuit(id);

    if (value == nullptr) {
        return false;
    }

    if (strlen(value) != 21) {
        return false;
    }

    uint8_t data[6];
    data[0] = (value[0] - '0') * 10 + (value[1] - '0');
    data[1] = (value[3] - '0') * 10 + (value[4] - '0');
    data[2] = (value[7] - '0') * 100 + (value[8] - '0') * 10 + (value[9] - '0');
    data[3] = (value[11] - '0') * 10 + (value[12] - '0');
    data[4] = (value[14] - '0') * 10 + (value[15] - '0');
    data[5] = (value[18] - '0') * 100 + (value[19] - '0') * 10 + (value[20] - '0');

    if (!data[0] || data[0] > 31 || !data[1] || data[1] > 12 || !data[3] || data[3] > 31 || !data[4] || data[4] > 12) {
        return false;
    }
    if (!vacation || value[10] == '+') { // + for compatibility
        if (hc) {
            write_command(timer_typeids[hc->hc()], 93, data, 6, timer_typeids[hc->hc()]);

        } else {
            write_command(0x38, 93, data, 6, 0x38); // dhw
            write_command(0x39, 93, data, 6, 0);    // circ, no validate
        }
    } else {
        if (hc) {
            write_command(timer_typeids[hc->hc()], 87, data, 6, timer_typeids[hc->hc()]);
        } else {
            write_command(0x38, 87, data, 6, 0x38); // dhw
            write_command(0x39, 87, data, 6, 0);    // circ
        }
    }

    return true;
}

// set vacations as string dd.mm.yyyy-dd.mm.yyyy
// RC30 and rego 3000
bool Thermostat::set_RC30Vacation(const char * value, const int8_t id) {
    if (strlen(value) != 21) {
        return false;
    }

    uint8_t data[6];
    data[0] = (value[0] - '0') * 10 + (value[1] - '0');
    data[1] = (value[3] - '0') * 10 + (value[4] - '0');
    data[2] = (value[7] - '0') * 100 + (value[8] - '0') * 10 + (value[9] - '0');
    data[3] = (value[11] - '0') * 10 + (value[12] - '0');
    data[4] = (value[14] - '0') * 10 + (value[15] - '0');
    data[5] = (value[18] - '0') * 100 + (value[19] - '0') * 10 + (value[20] - '0');

    if (!data[0] || data[0] > 31 || !data[1] || data[1] > 12 || !data[3] || data[3] > 31 || !data[4] || data[4] > 12) {
        return false;
    }
    if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000) {
        write_command(0x269, 0, data, 6, 0x269);
        return true;
    }
    // RC30
    write_command(0xA9, 1 + 7 * (id - 1), data, 6, 0x9A);
    return true;
}

// set R3000 holiday as string dd.mm.yyyy-dd.mm.yyyy
// RC30 and rego 3000 and CR50
bool Thermostat::set_R3000Holiday(const char * value, const int8_t id) {
    if (strlen(value) != 21) {
        return false;
    }

    uint8_t data[6];
    data[2] = (value[0] - '0') * 10 + (value[1] - '0');
    data[1] = (value[3] - '0') * 10 + (value[4] - '0');
    data[0] = (value[7] - '0') * 100 + (value[8] - '0') * 10 + (value[9] - '0');
    data[5] = (value[11] - '0') * 10 + (value[12] - '0');
    data[4] = (value[14] - '0') * 10 + (value[15] - '0');
    data[3] = (value[18] - '0') * 100 + (value[19] - '0') * 10 + (value[20] - '0');

    if (!data[2] || data[2] > 31 || !data[1] || data[1] > 12 || !data[5] || data[5] > 31 || !data[4] || data[4] > 12) {
        return false;
    }
    if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000) {
        write_command(0x269, 0, data, 6, 0x269);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        write_command(0x43F, 0, data, 6, 0x43F);
    }
    return true;
}


// set pause in hours
bool Thermostat::set_pause(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    if (value == nullptr) {
        read_command(timer_typeids[hc->hc()], 85, 1);
        return true;
    }
    int hrs = 0;
    if (!Helpers::value2number(value, hrs)) {
        return false;
    }

    write_command(timer_typeids[hc->hc()], 85, hrs);

    return true;
}

// set partymode in hours
bool Thermostat::set_party(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);

    if (hc == nullptr) {
        return false;
    }

    if (value == nullptr) {
        read_command(timer_typeids[hc->hc()], 86, 1);
        return true;
    }

    int hrs = 0;
    if (!Helpers::value2number(value, hrs)) {
        return false;
    }

    write_command(timer_typeids[hc->hc()], 86, hrs);

    return true;
}

bool Thermostat::set_absent(const char * value, const int8_t id) {
    bool b;
    if (Helpers::value2bool(value, b)) {
        write_command(0x16E, 0, b ? 0xFF : 0, 0x16E);
        return true;
    }
    return false;
}

bool Thermostat::set_redthreshold(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    float t;
    if (Helpers::value2temperature(value, t)) {
        write_command(set_typeids[hc->hc()], 20, (int8_t)(t * 2), set_typeids[hc->hc()]);
        return true;
    }
    return false;
}

bool Thermostat::set_solarinfl(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    float t;
    if (Helpers::value2temperature(value, t)) {
        write_command(summer_typeids[hc->hc()], 3, (int8_t)t, summer_typeids[hc->hc()]);
        return true;
    }
    return false;
}

// set date&time as string dd.mm.yyyy-hh:mm:ss-dw-dst or "NTP" for setting to internet-time
// dw - day of week (0..6), dst- summertime (0/1)
// id is ignored
bool Thermostat::set_datetime(const char * value, const int8_t id) {
    std::string dt;
    if (!Helpers::value2string(value, dt)) {
        return false;
    }

    uint8_t data[9];
    if (dt == "ntp") {
        time_t now = time(nullptr);
        tm *   tm_ = localtime(&now);
        if (tm_->tm_year < 110) { // no valid time
            return false;
        }
        if (!EMSESP::system_.ntp_connected()) {
            LOG_WARNING("Set date: no valid NTP data, setting from ESP Clock");
        }

        data[0] = tm_->tm_year - 100; // Bosch counts from 2000
        data[1] = tm_->tm_mon + 1;
        data[2] = tm_->tm_hour;
        data[3] = tm_->tm_mday;
        data[4] = tm_->tm_min;
        data[5] = tm_->tm_sec;
        data[6] = (tm_->tm_wday + 6) % 7;            // Bosch counts from Mo, time from Su
        data[7] = (id == 0) ? 2 : tm_->tm_isdst + 2; // set DST and flag for ext. clock
        if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
            data[6]++; // Junkers use 1-7;
            data[7] = 0;
        }
    } else if (dt.length() == 23) {
        data[0] = (dt[7] - '0') * 100 + (dt[8] - '0') * 10 + (dt[9] - '0'); // year
        data[1] = (dt[3] - '0') * 10 + (dt[4] - '0');                       // month
        data[2] = (dt[11] - '0') * 10 + (dt[12] - '0');                     // hour
        data[3] = (dt[0] - '0') * 10 + (dt[1] - '0');                       // day
        data[4] = (dt[14] - '0') * 10 + (dt[15] - '0');                     // min
        data[5] = (dt[17] - '0') * 10 + (dt[18] - '0');                     // sec
        data[6] = (dt[20] - '0');                                           // day of week, Mo:0
        data[7] = (dt[22] - '0') + 2;                                       // DST and flag
        if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
            data[7] = 0;
        }
    } else {
        LOG_WARNING("Set date: invalid data, wrong length");
        return false;
    }
    if (data[1] == 0 || data[1] > 12 || data[2] > 23 || data[3] == 0 || data[3] > 31 || data[4] > 59 || data[5] > 59 || data[6] > 6 || data[7] > 3) {
        LOG_WARNING("Invalid date/time: %02d.%02d.2%03d-%02d:%02d:%02d-%d-%d", data[3], data[1], data[0], data[2], data[4], data[5], data[6], data[7]);
        return false;
    }

    // LOG_INFO("Setting date and time: %02d.%02d.2%03d-%02d:%02d:%02d-%d-%d", data[3], data[1], data[0], data[2], data[4], data[5], data[6], data[7]);
    write_command(EMS_TYPE_time, 0, data, 8, EMS_TYPE_time);

    return true;
}

// set RC300 roominfluence factor
bool Thermostat::set_roominfl_factor(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    float val;
    if (!Helpers::value2float(value, val)) {
        return false;
    }
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        write_command(set_typeids[hc->hc()], 6, (uint8_t)((val + 5) / 10));

    } else {
        write_command(summer_typeids[hc->hc()], 1, (uint8_t)(val * 10));
    }

    return true;
}

// set Junkers roominfluence mode
bool Thermostat::set_roominfl_mode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    uint8_t val;
    if (!Helpers::value2enum(value, val, FL_(enum_roominfluence))) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 7, (uint8_t)val);

    return true;
}

// sets the thermostat working mode
bool Thermostat::set_mode(const char * value, const int8_t id) {
    if ((value == nullptr) || (strlen(value) >= 20)) {
        return false;
    }

    // first determine which enum we are using
    const char * const ** mode_list = nullptr; // points to a translated list of modes
    switch (model()) {
    case EMSdevice::EMS_DEVICE_FLAG_RC10:
        mode_list = FL_(enum_mode6);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        mode_list = FL_(enum_mode2);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N:
    case EMSdevice::EMS_DEVICE_FLAG_RC25:
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
        mode_list = FL_(enum_mode3);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_BC400:
    case EMSdevice::EMS_DEVICE_FLAG_CR120:
        mode_list = FL_(enum_mode2);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
    case EMSdevice::EMS_DEVICE_FLAG_R3000:
        mode_list = FL_(enum_mode);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        mode_list = FL_(enum_mode4);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_CRF:
        mode_list = FL_(enum_mode5);
        break;
    default:
        return false;
    }

    uint8_t enum_index = 0;

    // check for the mode being a full string name or single digit
    if (!Helpers::value2enum(value, enum_index, mode_list)) {
        mode_list = FL_(enum_mode_ha);
        if (!Mqtt::ha_enabled() || !Helpers::value2enum(value, enum_index, mode_list)) {
            // We have logging on fail in command.cpp
            // LOG_WARNING("wrong mode: %s", value);
            return false; // not found
        }
    }

    // compare the english string
    auto mode = mode_list[enum_index][0];
    if (!strcmp(mode, FL_(auto)[0])) {
        return set_mode_n(HeatingCircuit::Mode::AUTO, id);
    }
    if (!strcmp(mode, FL_(off)[0])) {
        return set_mode_n(HeatingCircuit::Mode::OFF, id);
    }
    if (!strcmp(mode, FL_(manual)[0])) {
        return set_mode_n(HeatingCircuit::Mode::MANUAL, id);
    }
    if (!strcmp(mode, FL_(day)[0])) {
        return set_mode_n(HeatingCircuit::Mode::DAY, id);
    }
    if (!strcmp(mode, FL_(night)[0])) {
        return set_mode_n(HeatingCircuit::Mode::NIGHT, id);
    }
    if (!strcmp(mode, FL_(heat)[0])) {
        return set_mode_n(HeatingCircuit::Mode::HEAT, id);
    }
    if (!strcmp(mode, FL_(nofrost)[0])) {
        return set_mode_n(HeatingCircuit::Mode::NOFROST, id);
    }
    if (!strcmp(mode, FL_(eco)[0])) {
        return set_mode_n(HeatingCircuit::Mode::ECO, id);
    }
    if (!strcmp(mode, FL_(holiday)[0])) {
        return set_mode_n(HeatingCircuit::Mode::HOLIDAY, id);
    }
    if (!strcmp(mode, FL_(comfort)[0])) {
        return set_mode_n(HeatingCircuit::Mode::COMFORT, id);
    }

    return false; // not found
}

// Set the thermostat working mode
// mode is HeatingCircuit::Mode
bool Thermostat::set_mode_n(const uint8_t mode, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t  set_mode_value, offset;
    uint8_t  hc_p            = hc->hc();
    uint16_t set_typeid      = set_typeids[hc_p];
    uint16_t validate_typeid = set_typeid;

    // set the value to send via EMS depending on the mode type
    switch (mode) {
    case HeatingCircuit::Mode::NIGHT:
    case HeatingCircuit::Mode::OFF:
        set_mode_value = 0;
        break;
    case HeatingCircuit::Mode::DAY:
    case HeatingCircuit::Mode::HEAT:
    case HeatingCircuit::Mode::MANUAL:
    case HeatingCircuit::Mode::NOFROST:
        set_mode_value = 1;
        break;
    default: // AUTO & ECO
        set_mode_value = 2;
        break;
    }

    uint8_t model_ = model();
    switch (model_) {
    case EMSdevice::EMS_DEVICE_FLAG_RC10:
        offset          = 0;
        validate_typeid = 0xB1;
        set_typeid      = 0xB2;
        if (mode == HeatingCircuit::Mode::NOFROST || mode == HeatingCircuit::Mode::OFF) {
            set_mode_value = 1;
        } else if (mode == HeatingCircuit::Mode::NIGHT) {
            set_mode_value = 2;
        } else { // DAY
            set_mode_value = 4;
        }
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        offset = EMS_OFFSET_RC20Set_mode;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N: // ES72
    case EMSdevice::EMS_DEVICE_FLAG_RC25:
        offset = EMS_OFFSET_RC20_2_Set_mode;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        offset = EMS_OFFSET_RC30Set_mode;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
        offset = EMS_OFFSET_RC35Set_mode;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_BC400:
    case EMSdevice::EMS_DEVICE_FLAG_CR120:
        offset = EMS_OFFSET_RCPLUSSet_mode_new;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
    case EMSdevice::EMS_DEVICE_FLAG_R3000:
        offset         = EMS_OFFSET_RCPLUSSet_mode;
        set_mode_value = set_mode_value == 2 ? 0xFF : 0;
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        if (has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
            offset = EMS_OFFSET_JunkersSetMessage2_set_mode;
        } else {
            offset = EMS_OFFSET_JunkersSetMessage_set_mode;
        }
        // validate_typeid = monitor_typeids[hc_p];
        if (mode == HeatingCircuit::Mode::NOFROST) {
            set_mode_value = 0x01;
        } else if (mode == HeatingCircuit::Mode::ECO || (mode == HeatingCircuit::Mode::NIGHT)) {
            set_mode_value = 0x02;
        } else if ((mode == HeatingCircuit::Mode::HEAT) || (mode == HeatingCircuit::Mode::DAY)) {
            set_mode_value = 0x03; // comfort
        } else if (mode == HeatingCircuit::Mode::AUTO) {
            set_mode_value = 0x04;
        }
        break;
    default:
        offset = 0;
        break;
    }

    // add the write command to the Tx queue
    // post validate is the corresponding monitor or set type IDs as they can differ per model
    write_command(set_typeid, offset, set_mode_value, validate_typeid);

    // set hc->mode temporary until validate is received
    if (model_ == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        hc->mode = set_mode_value >> 1;
    } else if (model_ == EMSdevice::EMS_DEVICE_FLAG_BC400 || model_ == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        hc->mode_new = set_mode_value;
    } else if (model_ == EMSdevice::EMS_DEVICE_FLAG_RC300 || model_ == EMSdevice::EMS_DEVICE_FLAG_R3000 || model_ == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        hc->mode = set_mode_value == 0xFF ? 1 : 0;
    } else if (model_ == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        hc->mode = set_mode_value - 1;
    } else {
        hc->mode = set_mode_value;
    }
    has_update(&hc->mode);

    return true;
}

// sets the thermostat summermode for RC300
bool Thermostat::set_summermode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;

    if (is_received(summer2_typeids[hc->hc()])) {
        if ((hc->statusbyte & 1) && Helpers::value2enum(value, set, FL_(enum_summermode))) {
            write_command(summer2_typeids[hc->hc()], 0, set, summer2_typeids[hc->hc()]);
            return true;
        } else if (Helpers::value2enum(value, set, FL_(enum_hpoperatingmode))) {
            write_command(summer2_typeids[hc->hc()], 0, set, summer2_typeids[hc->hc()]);
            return true;
        }
    } else if (Helpers::value2enum(value, set, FL_(enum_summermode))) {
        write_command(summer_typeids[hc->hc()], 7, set, summer_typeids[hc->hc()]);
        return true;
    }

    return false;
}

// Set fastheatupfactor, ems+
bool Thermostat::set_fastheatup(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    int set;

    if (!Helpers::value2number(value, set)) {
        return false;
    }

    write_command(summer_typeids[hc->hc()], 10, set, summer_typeids[hc->hc()]);
    return true;
}

// Set holidaymode Junkers
bool Thermostat::set_holidaymode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t v;

    if (!Helpers::value2enum(value, v, FL_(enum_mode4))) {
        return false;
    }
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS && has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
        write_command(set_typeids[hc->hc()], 8, v + 1, set_typeids[hc->hc()]);
    } else {
        write_command(set_typeids[hc->hc()], 19, v + 1, set_typeids[hc->hc()]);
    }
    return true;
}

// Set heatup mode Junkers
bool Thermostat::set_heatup(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t v;

    if (!Helpers::value2enum(value, v, FL_(enum_heatup))) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 18, v + 1, set_typeids[hc->hc()]);
    return true;
}

// Set switchonoptimization RC310
bool Thermostat::set_switchonoptimization(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    bool b;

    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35) {
        write_command(set_typeids[hc->hc()], 19, b ? 0xFF : 0x00, set_typeids[hc->hc()]);
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS && has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
        write_command(set_typeids[hc->hc()], 9, b ? 0xFF : 0x00, set_typeids[hc->hc()]);
    } else {
        write_command(curve_typeids[hc->hc()], 4, b ? 0xFF : 0x00, curve_typeids[hc->hc()]);
    }
    return true;
}

bool Thermostat::set_boost(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 23, b ? 0xFF : 0x00, set_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_boosttime(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(set_typeids[hc->hc()], 24, (uint8_t)v, set_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_heatondelay(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(summer2_typeids[hc->hc()], 2, (uint8_t)v, summer2_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_heatoffdelay(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(summer2_typeids[hc->hc()], 3, (uint8_t)v, summer2_typeids[hc->hc()]);
    return true;
}

bool Thermostat::set_instantstart(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(summer2_typeids[hc->hc()], 4, (uint8_t)v, summer2_typeids[hc->hc()]);
    return true;
}

// sets the thermostat reducemode for RC35 and RC310
bool Thermostat::set_reducemode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;
    if (isRC300() || model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        if (Helpers::value2enum(value, set, FL_(enum_reducemode1))) {
            write_command(set_typeids[hc->hc()], 5, set + 1, set_typeids[hc->hc()]);
            return true;
        }
    } else {
        if (Helpers::value2enum(value, set, FL_(enum_reducemode))) {
            write_command(set_typeids[hc->hc()], EMS_OFFSET_RC35Set_reducemode, set, set_typeids[hc->hc()]);
            return true;
        }
    }
    return false;
}


// sets the thermostat reducemode for RC35 vacations
bool Thermostat::set_vacreducemode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;
    if (!Helpers::value2enum(value, set, FL_(enum_reducemode))) {
        return false;
    }

    write_command(set_typeids[hc->hc()], EMS_OFFSET_RC35Set_vacreducemode, set, set_typeids[hc->hc()]);
    return true;
}

// sets the thermostat nofrost mode for RC35, RC300/RC310
bool Thermostat::set_nofrostmode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    uint8_t set;
    if (isRC300() || model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        if (Helpers::value2enum(value, set, FL_(enum_nofrostmode1))) {
            write_command(curve_typeids[hc->hc()], 5, set + 1, curve_typeids[hc->hc()]);
            return true;
        }
    } else {
        if (Helpers::value2enum(value, set, FL_(enum_nofrostmode))) {
            write_command(set_typeids[hc->hc()], EMS_OFFSET_RC35Set_nofrostmode, set, set_typeids[hc->hc()]);
            return true;
        }
    }
    return false;
}

// sets the thermostat heatingtype for RC35, RC300
bool Thermostat::set_heatingtype(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (Helpers::value2enum(value, set, FL_(enum_heatingtype1))) {
            write_command(set_typeids[hc->hc()], 0, set, set_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N) {
        if (Helpers::value2enum(value, set, hc->control == 1 ? FL_(enum_heatingtype2) : FL_(enum_heatingtype))) {
            write_command(set_typeids[hc->hc()], 0, set, set_typeids[hc->hc()]);
            return true;
        }
    } else if (Helpers::value2enum(value, set, FL_(enum_heatingtype))) {
        if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC20_N) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC25)) {
            write_command(set_typeids[hc->hc()], 0, set, set_typeids[hc->hc()]);
        } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
            write_command(curve_typeids[hc->hc()], 0, set, curve_typeids[hc->hc()]);
        } else {
            write_command(curve_typeids[hc->hc()], 1, set, curve_typeids[hc->hc()]);
        }
        return true;
    }

    return false;
}

// sets the thermostat controlmode for RC35, RC300
bool Thermostat::set_controlmode(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;
    if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode3))) {
            write_command(set_typeids[hc->hc()], 0, set, set_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100 || model() == EMSdevice::EMS_DEVICE_FLAG_CR120) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode))) {
            write_command(curve_typeids[hc->hc()], 0, set + 1, curve_typeids[hc->hc()]);
            return true;
        }
    } else if (isRC300()) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode1))) {
            write_command(curve_typeids[hc->hc()], 0, set + 1, curve_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode2))) {
            write_command(curve_typeids[hc->hc()], 1, set, curve_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N) {
        if (Helpers::value2enum(value, set, FL_(enum_controlmode2))) {
            write_command(set_typeids[hc->hc()], 33, set, set_typeids[hc->hc()]);
            return true;
        }
    }

    return false;
}

// sets the thermostat time for nightmode for RC10, telegram 0xB0
bool Thermostat::set_reducehours(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    int v;

    if (Helpers::value2number(value, v)) {
        write_command(set_typeids[hc->hc()], 5, v, set_typeids[hc->hc()]);
        return true;
    }

    return false;
}

// sets a single switchtime in the thermostat program for RC35, ww, circ, hc..
bool Thermostat::set_switchtime(const char * value, const uint16_t type_id, char * out, size_t len) {
    if (value == nullptr) {
        return false;
    }
    uint8_t no   = 44;   // invalid value
    uint8_t day  = 8;    // invalid
    uint8_t on   = 8;    // invalid
    uint8_t time = 0x91; // invalid

    if (value[0] == '{') {
        JsonDocument         doc;
        DeserializationError error = deserializeJson(doc, value);
        if (error) {
            return false;
        }
        if (!doc["no"].is<int>()) {
            return false;
        }
        no = doc["no"];
        if (!doc["day"].is<const char *>() || !doc["mode"].is<const char *>() || !doc["time"].is<const char *>()) {
            EMSESP::wait_validate(type_id);
            read_command(type_id, 2 * no, 2);
            return true;
        }
        const char * s_day = doc["day"];
        if (!Helpers::value2enum(s_day, day, FL_(enum_dayOfWeek))) {
            return false;
        }
        const char * s_mode = doc["mode"];
        const char * s_time = doc["time"];
        if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N) {
            bool b;
            if (Helpers::value2bool(s_mode, b)) {
                on = b ? 1 : 0;
            }
            if (strlen(s_time) == 5 && s_time[2] == ':') {
                time = 6 * ((s_time[0] - '0') * 10 + (s_time[1] - '0')) + (s_time[3] - '0');
            }
        } else if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC20) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC30)) {
            if (s_mode[0] == 'T') {
                on = s_mode[1] - '0';
            } else {
                on = s_mode[0] - '0';
            }
            if (strlen(s_time) == 5 && s_time[2] == ':') {
                time = 6 * ((s_time[0] - '0') * 10 + (s_time[1] - '0')) + (s_time[3] - '0');
            }
        } else { // RC300
            Helpers::value2enum(s_mode, on, FL_(enum_switchmode));
            if (strlen(s_time) == 5 && s_time[2] == ':') {
                time = 4 * ((s_time[0] - '0') * 10 + (s_time[1] - '0')) + ((s_time[3] - '0') * 10 + (s_time[4] - '0')) / 15;
            }
        }
        if (strncmp(s_mode, "not_set", 7) == 0) {
            day  = 7;
            on   = 7;
            time = 0x90;
            // LOG_INFO("switchtime %02d cleared", no);
        }
    } else {
        if (strlen(value) > 1) {
            no = (value[0] - '0') * 10 + (value[1] - '0');
        }
        if (strlen(value) == 2) {
            if (no < 42) {
                EMSESP::wait_validate(type_id);
                read_command(type_id, 2 * no, 2);
                return true;
            }
            return false;
        }
        if (strlen(value) > 4) {
            for (uint8_t i = 0; i < 7; i++) {
                // we use EN settings for the day abbreviation
                if (!strncmp(&value[3], (FL_(enum_dayOfWeek)[i][0]), 2)) {
                    day = i;
                }
            }
        }
        if (strlen(value) > 10) {
            time = 6 * ((value[6] - '0') * 10 + (value[7] - '0')) + (value[9] - '0');
        }
        if (strlen(value) > 13 && value[12] == 'o') {
            on = value[13] == 'n' ? 1 : 0;
        } else if (strlen(value) == 14 && value[12] == 'T') {
            on = value[13] - '0';
        } else if (strlen(value) == 13) {
            on = value[12] - '0';
        }
        if (strlen(value) >= 10 && strncmp(&value[3], "not_set", 7) == 0) {
            day  = 7;
            on   = 7;
            time = 0x90;
            // LOG_INFO("switchtime %02d cleared", no);
        }
    }
    uint8_t data[2] = {0xE7, 0x90}; // unset switchtime
    if (day != 7 && on != 7) {
        data[0] = (day << 5) + on;
        data[1] = time;
    }

    uint8_t min_on = 1;
    uint8_t max_on = 4;
    if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC35) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N)) {
        min_on = 0;
        max_on = 1;
    }
    if (no > 41 || time > 0x90 || day > 7 || ((on < min_on || on > max_on) && on != 7)) {
        // LOG_WARNING("Setting switchtime: Invalid data: %s", value);
        // LOG_WARNING("Setting switchtime: Invalid data: %02d.%1d.0x%02X.%1d", no, day, time, on);
        return false;
    }
    if (data[0] != 0xE7) {
        // we use EN settings for the day abbreviation
        auto sday = (FL_(enum_dayOfWeek)[day][0]);
        if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N) {
            snprintf(out, len, "%02d %s %02d:%02d %s", no, sday, time / 6, 10 * (time % 6), on ? "on" : "off");
        } else if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC20) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC30)) {
            snprintf(out, len, "%02d %s %02d:%02d T%d", no, sday, time / 6, 10 * (time % 6), on);
        } else {
            auto son = (FL_(enum_switchmode)[on][0]);
            snprintf(out, len, "%02d %s %02d:%02d %s", no, sday, time / 6, 10 * (time % 6), son);
        }
    } else {
        snprintf(out, len, "%02d not_set", no);
    }
    write_command(type_id, no * 2, &data[0], 2, 0);
    return true;
}

// set switchtime for own1 program
bool Thermostat::set_switchtime1(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    char out[sizeof(hc->switchtime1)] = {'\0'};

    if (set_switchtime(value, timer_typeids[hc->hc()], out, sizeof(out))) {
        if (strlen(out)) {
            has_update(hc->switchtime1, out, sizeof(hc->switchtime1));
        }
        return true;
    }
    return false;
}

// set switchtime for own2 program
bool Thermostat::set_switchtime2(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }
    char out[sizeof(hc->switchtime2)] = {'\0'};

    if (set_switchtime(value, timer_typeids[hc->hc()] + 3, out, sizeof(out))) {
        if (strlen(out)) {
            has_update(hc->switchtime2, out, sizeof(hc->switchtime2));
        }
        return true;
    }
    return false;
}
// sets a single switchtime in the thermostat dhw program for RC35
bool Thermostat::set_wwCircSwitchTime(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw == nullptr) {
        return false;
    }
    char out[sizeof(dhw->wwCircSwitchTime_)] = {'\0'};

    if (set_switchtime(value, 0x39, out, sizeof(out))) {
        if (strlen(out)) {
            has_update(dhw->wwCircSwitchTime_, out, sizeof(dhw->wwCircSwitchTime_));
        }
        return true;
    }
    return false;
}

// sets a single switchtime in the thermostat circulation program for RC35
bool Thermostat::set_wwSwitchTime(const char * value, const int8_t id) {
    auto dhw = dhw_circuit(id2dhw(id));
    if (dhw != nullptr) {
        return false;
    }
    char out[sizeof(dhw->wwSwitchTime_)] = {'\0'};


    if (set_switchtime(value, 0x38, out, sizeof(out))) {
        if (strlen(out)) {
            has_update(dhw->wwSwitchTime_, out, sizeof(dhw->wwSwitchTime_));
        }
        return true;
    }
    return false;
}

// sets the thermostat program for RC35 and RC20
bool Thermostat::set_program(const char * value, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t set;
    if ((model() == EMSdevice::EMS_DEVICE_FLAG_RC20_N) || (model() == EMSdevice::EMS_DEVICE_FLAG_RC25)) {
        if (Helpers::value2enum(value, set, FL_(enum_progMode3))) {
            write_command(set_typeids[hc->hc()], 11, set + 1, set_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC35 || model() == EMSdevice::EMS_DEVICE_FLAG_RC30_N || model() == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        if (Helpers::value2enum(value, set, FL_(enum_progMode2))) {
            write_command(timer_typeids[hc->hc()], 84, set, timer_typeids[hc->hc()]);
            return true;
        }
    } else if (isRC300() || model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
        if (Helpers::value2enum(value, set, FL_(enum_progMode))) {
            write_command(set_typeids[hc->hc()], 11, set + 1, set_typeids[hc->hc()]);
            return true;
        }
    } else if (model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        if (Helpers::value2enum(value, set, FL_(enum_progMode4))) {
            if (has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
                write_command(set_typeids[hc->hc()], 10, set + 1, set_typeids[hc->hc()]);
            } else {
                write_command(set_typeids[hc->hc()], 13, set + 1, set_typeids[hc->hc()]);
            }
            return true;
        }
    }
    return false;
}

// Set the temperature of the thermostat
// the id passed into this function is the heating circuit number
bool Thermostat::set_temperature(const float temperature, const uint8_t mode, const int8_t id) {
    auto hc = heating_circuit(id);
    if (hc == nullptr) {
        return false;
    }

    uint8_t  model           = this->model();
    int8_t   offset          = -1; // we use -1 to check if there is a value
    uint8_t  factor          = 2;  // some temperatures only use 1
    uint16_t validate_typeid = monitor_typeids[hc->hc()];
    uint16_t set_typeid      = set_typeids[hc->hc()];

    if (model == EMSdevice::EMS_DEVICE_FLAG_RC10) {
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT:
            offset = 3;
            break;
        case HeatingCircuit::Mode::DAY:
            offset = 4;
            break;
        case HeatingCircuit::Mode::AUTO:
            if (hc->get_mode() == HeatingCircuit::Mode::NIGHT) {
                offset = 3;
            } else {
                offset = 4;
            }
            break;
        default:
            break;
        }

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC20) {
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT:
            offset          = 3;
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAYLOW:
            offset          = 4;
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAYMID:
            offset          = 5;
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAY:
            offset          = 6;
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::MANUAL:
            offset          = EMS_OFFSET_RC20Set_temp_manual;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::OFF:
            offset          = EMS_OFFSET_RC20Set_temp_off;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::AUTO:
            if (hc->get_mode() == HeatingCircuit::Mode::OFF) {
                offset = EMS_OFFSET_RC20Set_temp_off;
            } else if (hc->get_mode() == HeatingCircuit::Mode::MANUAL) {
                offset = EMS_OFFSET_RC20Set_temp_manual;
            } else {
                offset = EMS_OFFSET_RC20Set_temp_auto;
            }
            break;
        default:
            break;
        }

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC30) {
        switch (mode) {
        case HeatingCircuit::Mode::OFF:
            offset          = EMS_OFFSET_RC30Set_temp_off;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::MANUAL:
            offset          = EMS_OFFSET_RC30Set_temp_manual;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset          = EMS_OFFSET_RC30Set_temp_auto;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::NIGHT:
            set_typeid      = curve_typeids[hc->hc()];
            offset          = EMS_OFFSET_RC30Temp_temp_night;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAYLOW:
            set_typeid      = curve_typeids[hc->hc()];
            offset          = EMS_OFFSET_RC30Temp_temp_daylow;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAYMID:
            set_typeid      = curve_typeids[hc->hc()];
            offset          = EMS_OFFSET_RC30Temp_temp_daymid;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::DAY:
            set_typeid      = curve_typeids[hc->hc()];
            offset          = EMS_OFFSET_RC30Temp_temp_day;
            validate_typeid = set_typeid;
            break;
        case HeatingCircuit::Mode::HOLIDAY:
            set_typeid      = curve_typeids[hc->hc()];
            offset          = EMS_OFFSET_RC30Temp_temp_holiday;
            validate_typeid = set_typeid;
            break;
        default:
            if (hc->get_mode() == HeatingCircuit::Mode::OFF) {
                offset = EMS_OFFSET_RC30Set_temp_off;
            } else if (hc->get_mode() == HeatingCircuit::Mode::MANUAL) {
                offset = EMS_OFFSET_RC30Set_temp_manual;
            } else {
                offset = EMS_OFFSET_RC30Set_temp_auto;
            }
            break;
        }

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_CR11) {
        offset = 10; // just seltemp write to manualtemp

    } else if (isRC300() || (model == EMSdevice::EMS_DEVICE_FLAG_RC100)) {
        validate_typeid = set_typeids[hc->hc()];
        switch (mode) {
        case HeatingCircuit::Mode::SUMMER:
            if (is_received(summer2_typeids[hc->hc()])) {
                offset     = 0x01;
                set_typeid = summer2_typeids[hc->hc()];
            } else {
                offset     = 0x06;
                set_typeid = summer_typeids[hc->hc()];
            }
            validate_typeid = set_typeid;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::COOLSTART:
            offset          = 5;
            set_typeid      = summer2_typeids[hc->hc()];
            validate_typeid = set_typeid;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::MANUAL:
            if (model == EMSdevice::EMS_DEVICE_FLAG_CR120) {
                offset = 22; // manual offset CR120
            } else {
                offset = 10; // manual offset
            }
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset = 0x08; // auto offset
            if (temperature == -1) {
                factor = 1; // to write 0xFF
            }
            break;
        case HeatingCircuit::Mode::COOLTEMP:
            offset = 0x11;
            break;
        case HeatingCircuit::Mode::COMFORT:
            offset = 0x02; // comfort offset level 2
            break;
        case HeatingCircuit::Mode::ECO:
            offset = 0x04; // eco offset
            break;
        case HeatingCircuit::Mode::OFFSET:
            offset          = 2;
            set_typeid      = summer_typeids[hc->hc()];
            validate_typeid = set_typeid;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::DESIGN:
            set_typeid      = summer_typeids[hc->hc()];
            validate_typeid = set_typeid;
            if (hc->heatingtype == 3) {
                offset = 5;
            } else {
                offset = 4;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::MINFLOW:
            set_typeid      = summer_typeids[hc->hc()];
            validate_typeid = set_typeid;
            offset          = 8;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::MAXFLOW:
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            if (hc->heatingtype == 3) {
                offset = 7;
            } else {
                offset = 8;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOFROST:
            set_typeid      = curve_typeids[hc->hc()];
            validate_typeid = set_typeid;
            offset          = 6;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::ROOMINFLUENCE:
            set_typeid      = summer_typeids[hc->hc()];
            validate_typeid = set_typeid;
            offset          = 0;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::NOREDUCE:
            validate_typeid = set_typeid;
            offset          = 12;
            factor          = 1;
            break;
        case HeatingCircuit::Mode::REDUCE:
            validate_typeid = set_typeid;
            offset          = 9;
            factor          = 1;
            break;
        default: // seltemp
            uint8_t mode_ = hc->get_mode();
            if (model == EMSdevice::EMS_DEVICE_FLAG_CR120 && mode_ == HeatingCircuit::Mode::MANUAL) {
                offset = 22; // manual offset CR120
            } else if (mode_ == HeatingCircuit::Mode::MANUAL) {
                offset = 10; // manual offset
            } else {
                offset = 8; // auto offset
                // special case to reactivate auto temperature, see #737, #746
                if (temperature == -1) {
                    factor = 1;
                }
            }
            validate_typeid = monitor_typeids[hc->hc()]; // get setpoint roomtemp back
            break;
        }

    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC20_N) || (model == EMSdevice::EMS_DEVICE_FLAG_RC25)) {
        switch (mode) {
        case HeatingCircuit::Mode::MINFLOW:
            offset = 15;
            factor = 1;
            break;
        case HeatingCircuit::Mode::MAXFLOW:
            offset = 16;
            factor = 1;
            break;
        case HeatingCircuit::Mode::SUMMER:
            offset = 17;
            factor = 1;
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset = 13;
            break;
        case HeatingCircuit::Mode::NIGHT: // change the night temp
            offset = EMS_OFFSET_RC20_2_Set_temp_night;
            break;
        case HeatingCircuit::Mode::DAY: // change the day temp
            offset = EMS_OFFSET_RC20_2_Set_temp_day;
            break;
        default:
            // automatic selection, if no type is defined, we use the standard code
            uint8_t mode_ = hc->get_mode();
            if (mode_ == HeatingCircuit::Mode::NIGHT) {
                offset = EMS_OFFSET_RC20_2_Set_temp_night;
            } else if (mode_ == HeatingCircuit::Mode::DAY) {
                offset = EMS_OFFSET_RC20_2_Set_temp_day;
            } else {
                offset = 13; // tempautotemp
            }
            break;
        }

    } else if ((model == EMSdevice::EMS_DEVICE_FLAG_RC35) || (model == EMSdevice::EMS_DEVICE_FLAG_RC30_N)) {
        validate_typeid = set_typeids[hc->hc()];
        switch (mode) {
        case HeatingCircuit::Mode::NIGHT: // change the night temp
            offset = EMS_OFFSET_RC35Set_temp_night;
            break;
        case HeatingCircuit::Mode::DAY: // change the day temp
            offset = EMS_OFFSET_RC35Set_temp_day;
            break;
        case HeatingCircuit::Mode::HOLIDAY: // change the holiday temp
            offset = EMS_OFFSET_RC35Set_temp_holiday;
            break;
        case HeatingCircuit::Mode::OFFSET: // change the offset temp
            offset = EMS_OFFSET_RC35Set_temp_offset;
            break;
        case HeatingCircuit::Mode::FLOWOFFSET: // change the offset of flowtemp
            offset = EMS_OFFSET_RC35Set_temp_flowoffset;
            factor = 1;
            break;
        case HeatingCircuit::Mode::DESIGN:
            if (hc->heatingtype == 3 && model == EMSdevice::EMS_DEVICE_FLAG_RC35) {
                offset = EMS_OFFSET_RC35Set_temp_design_floor;
            } else {
                offset = EMS_OFFSET_RC35Set_temp_design;
            }
            factor = 1;
            break;
        case HeatingCircuit::Mode::SUMMER:
            offset = EMS_OFFSET_RC35Set_temp_summer;
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOFROST:
            offset = EMS_OFFSET_RC35Set_temp_nofrost;
            factor = 1;
            break;
        case HeatingCircuit::Mode::ROOMINFLUENCE:
            offset = 4;
            factor = 1;
            break;
        case HeatingCircuit::Mode::NOREDUCE:
            offset = EMS_OFFSET_RC35Set_noreducetemp;
            factor = 1;
            break;
        case HeatingCircuit::Mode::REDUCE:
            offset = EMS_OFFSET_RC35Set_reducetemp;
            factor = 1;
            break;
        case HeatingCircuit::Mode::VACREDUCE:
            offset = EMS_OFFSET_RC35Set_vacreducetemp;
            factor = 1;
            break;
        case HeatingCircuit::Mode::TEMPAUTO:
            offset = EMS_OFFSET_RC35Set_seltemp;
            break;
        case HeatingCircuit::Mode::MINFLOW:
            offset = 16;
            factor = 1;
            break;
        case HeatingCircuit::Mode::MAXFLOW:
            if (hc->heatingtype == 3 && model == EMSdevice::EMS_DEVICE_FLAG_RC35) {
                offset = 35;
            } else {
                offset = 15;
            }
            factor = 1;
            break;
        default:
            // automatic selection, if no type is defined, we use the standard code
            validate_typeid = monitor_typeids[hc->hc()]; //get setpoint roomtemp back
            uint8_t mode_   = hc->get_mode();
            if (mode_ == HeatingCircuit::Mode::NIGHT) {
                offset = EMS_OFFSET_RC35Set_temp_night;
            } else if (mode_ == HeatingCircuit::Mode::DAY) {
                offset = EMS_OFFSET_RC35Set_temp_day;
            } else if (model == EMSdevice::EMS_DEVICE_FLAG_RC35) {
                offset = EMS_OFFSET_RC35Set_seltemp; // https://github.com/emsesp/EMS-ESP/issues/310
            } else {
                // RC30_N missing temporary auto temperature https://github.com/emsesp/EMS-ESP32/issues/395
                uint8_t modetype = hc->get_mode_type();
                offset           = (modetype == HeatingCircuit::Mode::NIGHT) ? EMS_OFFSET_RC35Set_temp_night : EMS_OFFSET_RC35Set_temp_day;
            }
            break;
        }

    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        // figure out if we have older or new thermostats, Heating Circuits on 0x65 or 0x79
        // see https://github.com/emsesp/EMS-ESP/issues/335#issuecomment-593324716)
        bool old_junkers = (has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD));
        if (!old_junkers) {
            switch (mode) {
            case HeatingCircuit::Mode::SUMMER:
                offset = 11;
                break;
            case HeatingCircuit::Mode::MINFLOW:
                offset = 3;
                factor = 1;
                break;
            case HeatingCircuit::Mode::MAXFLOW:
                offset = 5;
                factor = 1;
                break;
            case HeatingCircuit::Mode::DESIGN:
                offset = 4;
                factor = 1;
                break;
            case HeatingCircuit::Mode::NOFROST:
                offset = EMS_OFFSET_JunkersSetMessage_no_frost_temp;
                break;
            case HeatingCircuit::Mode::NIGHT:
            case HeatingCircuit::Mode::ECO:
                offset = EMS_OFFSET_JunkersSetMessage_night_temp;
                break;
            case HeatingCircuit::Mode::HEAT:
            case HeatingCircuit::Mode::DAY:
                offset = EMS_OFFSET_JunkersSetMessage_day_temp;
                break;
            default:
                // automatic selection, if no type is defined, we check mode and modetype
                uint8_t mode_ = hc->get_mode();
                if (mode_ == HeatingCircuit::Mode::NIGHT || mode_ == HeatingCircuit::Mode::ECO) {
                    offset = EMS_OFFSET_JunkersSetMessage_night_temp;
                } else if (mode_ == HeatingCircuit::Mode::DAY || mode_ == HeatingCircuit::Mode::HEAT) {
                    offset = EMS_OFFSET_JunkersSetMessage_day_temp;
                } else if (mode_ == HeatingCircuit::Mode::NOFROST) {
                    offset = EMS_OFFSET_JunkersSetMessage_no_frost_temp;
                } else {
                    // auto mode, missing temporary parameter, use modetype https://github.com/emsesp/EMS-ESP32/issues/400
                    uint8_t modetype = hc->get_mode_type();
                    if (modetype == HeatingCircuit::Mode::NIGHT || modetype == HeatingCircuit::Mode::ECO) {
                        offset = EMS_OFFSET_JunkersSetMessage_night_temp;
                    } else if (modetype == HeatingCircuit::Mode::DAY || modetype == HeatingCircuit::Mode::HEAT) {
                        offset = EMS_OFFSET_JunkersSetMessage_day_temp;
                    } else {
                        offset = EMS_OFFSET_JunkersSetMessage_no_frost_temp;
                    }
                }
                break;
            }

        } else {
            // older, like the FR100
            switch (mode) {
            case HeatingCircuit::Mode::MAXFLOW:
                offset = 3;
                factor = 1;
                break;
            case HeatingCircuit::Mode::NOFROST:
                offset = EMS_OFFSET_JunkersSetMessage2_no_frost_temp;
                break;
            case HeatingCircuit::Mode::ECO:
            case HeatingCircuit::Mode::NIGHT:
                offset = EMS_OFFSET_JunkersSetMessage2_eco_temp;
                break;
            case HeatingCircuit::Mode::HEAT:
            case HeatingCircuit::Mode::DAY:
                offset = EMS_OFFSET_JunkersSetMessage2_heat_temp;
                break;
            default:
                // automatic selection, if no type is defined, we check mode and modetype
                uint8_t mode_ = hc->get_mode();
                if (mode_ == HeatingCircuit::Mode::NIGHT || mode_ == HeatingCircuit::Mode::ECO) {
                    offset = EMS_OFFSET_JunkersSetMessage2_eco_temp;
                } else if (mode_ == HeatingCircuit::Mode::DAY || mode_ == HeatingCircuit::Mode::HEAT) {
                    offset = EMS_OFFSET_JunkersSetMessage2_heat_temp;
                } else if (mode_ == HeatingCircuit::Mode::NOFROST) {
                    offset = EMS_OFFSET_JunkersSetMessage2_no_frost_temp;
                } else {
                    // auto mode, missing temporary parameter, use modetype https://github.com/emsesp/EMS-ESP32/issues/400
                    uint8_t modetype = hc->get_mode_type();
                    if (modetype == HeatingCircuit::Mode::NIGHT || modetype == HeatingCircuit::Mode::ECO) {
                        offset = EMS_OFFSET_JunkersSetMessage2_eco_temp;
                    } else if (modetype == HeatingCircuit::Mode::DAY || modetype == HeatingCircuit::Mode::HEAT) {
                        offset = EMS_OFFSET_JunkersSetMessage2_heat_temp;
                    } else {
                        offset = EMS_OFFSET_JunkersSetMessage2_no_frost_temp;
                    }
                }
                break;
            }
        }
    }

    // if we know what to send and to where, go and do it
    if (offset != -1) {
        // add the write command to the Tx queue. value is *2
        // post validate is the corresponding monitor or set type IDs as they can differ per model
        write_command(set_typeid, offset, (uint8_t)(temperature * (float)factor), validate_typeid);
        return true;
    }
    LOG_DEBUG("temperature mode %d not found", mode);
    return false;
}

bool Thermostat::set_temperature_value(const char * value, const int8_t id, const uint8_t mode, bool relative) {
    float f;
    if (Helpers::value2temperature(value, f, relative)) {
        return set_temperature(f, mode, id);
    }
    return false;
}

// register main device values, top level for all thermostats (excluding heating circuits)
// as these are done in void Thermostat::register_device_values_hc()
void Thermostat::register_device_values() {
    // RF remote sensor seen at 0x40, maybe this is also for different hc with id 0x40 - 0x47? emsesp.cpp maps only 0x40
    if (device_id() >= 0x40 && device_id() <= 0x47) {
        int8_t tag = DeviceValueTAG::TAG_HC1 + device_id() - 0x40;
        register_device_value(tag, &tempsensor1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(RFTemp), DeviceValueUOM::DEGREES);
        return;
    }
    // RC100H remote with humidity, this is also EMSdevice::EMS_DEVICE_FLAG_RC100 for set_calinttemp
    if (device_id() >= 0x38 && device_id() <= 0x3F) {
        // each device controls only one hc, so we tag the values
        int8_t tag = DeviceValueTAG::TAG_HC1 + device_id() - 0x38;
        register_device_value(tag, &tempsensor1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(RFTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &dewtemperature_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(dewTemperature), DeviceValueUOM::DEGREES);
        register_device_value(tag, &humidity_, DeviceValueType::UINT8, FL_(airHumidity), DeviceValueUOM::PERCENT);
        register_device_value(tag,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(tag, &battery_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(battery), DeviceValueUOM::PERCENT);
        return;
    }
    // Remotes RC20 and Junkers FB10, show only internal sensor
    if ((this->model() == EMSdevice::EMS_DEVICE_FLAG_RC20_N || this->model() == EMSdevice::EMS_DEVICE_FLAG_RC20
         || this->model() == EMSdevice::EMS_DEVICE_FLAG_JUNKERS)
        && device_id() >= 0x18 && device_id() <= 0x1B) {
        int8_t tag = DeviceValueTAG::TAG_HC1 + device_id() - 0x18;
        register_device_value(tag, &tempsensor1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(RFTemp), DeviceValueUOM::DEGREES);
        return;
    }
    // Common for all thermostats
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &errorCode_, DeviceValueType::STRING, FL_(errorCode), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &lastCode_, DeviceValueType::STRING, FL_(lastCode), DeviceValueUOM::NONE);

    switch (this->model()) {
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_R3000:
    case EMSdevice::EMS_DEVICE_FLAG_BC400:
    case EMSdevice::EMS_DEVICE_FLAG_CR120:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &dateTime_,
                              DeviceValueType::STRING,
                              FL_(tpl_datetime),
                              FL_(dateTime),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_datetime));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &floordrystatus_,
                              DeviceValueType::ENUM,
                              FL_(enum_floordrystatus),
                              FL_(floordrystatus),
                              DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &dampedoutdoortemp2_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(dampedoutdoortemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &floordrytemp_, DeviceValueType::UINT8, FL_(floordrytemp), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_building));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT8,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ibaDamping_, DeviceValueType::BOOL, FL_(damping), DeviceValueUOM::NONE, MAKE_CF_CB(set_damping));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hasSolar_, DeviceValueType::BOOL, FL_(hasSolar), DeviceValueUOM::NONE, MAKE_CF_CB(set_solar));
        if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000 || model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                                  &vacation[0],
                                  DeviceValueType::STRING,
                                  FL_(tpl_holidays),
                                  FL_(vacations),
                                  DeviceValueUOM::NONE,
                                  MAKE_CF_CB(set_R3000Holiday));
        }
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hybridStrategy_,
                              DeviceValueType::ENUM,
                              FL_(enum_hybridStrategy),
                              FL_(hybridStrategy),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_hybridStrategy));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &switchOverTemp_,
                              DeviceValueType::INT8,
                              FL_(switchOverTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_switchOverTemp),
                              -20,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyCostRatio_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyCostRatio),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_energyCostRatio),
                              0,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &fossileFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(fossileFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_fossileFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &electricFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(electricFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_electricFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &delayBoiler_,
                              DeviceValueType::UINT8,
                              FL_(delayBoiler),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_delayBoiler),
                              5,
                              120);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempDiffBoiler_,
                              DeviceValueType::UINT8,
                              FL_(tempDiffBoiler),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_tempDiffBoiler),
                              1,
                              99);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pvEnableWw_, DeviceValueType::BOOL, FL_(pvEnableWw), DeviceValueUOM::NONE, MAKE_CF_CB(set_pvEnableWw));
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &pvRaiseHeat_, DeviceValueType::INT8, FL_(pvRaiseHeat), DeviceValueUOM::K, MAKE_CF_CB(set_pvRaiseHeat), 0, 5);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &pvLowerCool_, DeviceValueType::INT8, FL_(pvLowerCool), DeviceValueUOM::K, MAKE_CF_CB(set_pvLowerCool), -5, 0);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &absent_, DeviceValueType::BOOL, FL_(absent), DeviceValueUOM::NONE, MAKE_CF_CB(set_absent));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC10:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatingpid_,
                              DeviceValueType::ENUM,
                              FL_(enum_PID),
                              FL_(heatingPID),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_heatingpid));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &backlight_, DeviceValueType::BOOL, FL_(backlight), DeviceValueUOM::NONE, MAKE_CF_CB(set_backlight));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N:
    case EMSdevice::EMS_DEVICE_FLAG_RC25:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT8,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaClockOffset_,
                              DeviceValueType::INT8,
                              FL_(ibaClockOffset),
                              DeviceValueUOM::SECONDS,
                              MAKE_CF_CB(set_clockoffset)); // offset (in sec) to clock, 0xff=-1s, 0x02=2s
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &autodst_, DeviceValueType::BOOL, FL_(autodst), DeviceValueUOM::NONE, MAKE_CF_CB(set_autodst));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaLanguage_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaLanguage_RC30),
                              FL_(ibaLanguage),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_language));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &backlight_, DeviceValueType::BOOL, FL_(backlight), DeviceValueUOM::NONE, MAKE_CF_CB(set_backlight));
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &brightness_, DeviceValueType::INT8, FL_(brightness), DeviceValueUOM::NONE, MAKE_CF_CB(set_brightness), -15, 15);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &mixingvalves_, DeviceValueType::UINT8, FL_(mixingvalves), DeviceValueUOM::NONE, MAKE_CF_CB(set_mixingvalves), 0, 2);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_building));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatingpid_,
                              DeviceValueType::ENUM,
                              FL_(enum_PID),
                              FL_(heatingPID),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_heatingpid));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &preheating_, DeviceValueType::BOOL, FL_(preheating), DeviceValueUOM::NONE, MAKE_CF_CB(set_preheating));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaMainDisplay_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaMainDisplay),
                              FL_(ibaMainDisplay),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_display));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ibaLanguage_, DeviceValueType::ENUM, FL_(enum_ibaLanguage), FL_(ibaLanguage), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaClockOffset_,
                              DeviceValueType::INT8,
                              FL_(ibaClockOffset),
                              DeviceValueUOM::SECONDS,
                              MAKE_CF_CB(set_clockoffset)); // offset (in sec) to clock, 0xff=-1s, 0x02=2s
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT8,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempsensor1_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempsensor1),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempsensor2_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempsensor2),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ibaDamping_, DeviceValueType::BOOL, FL_(damping), DeviceValueUOM::NONE, MAKE_CF_CB(set_damping));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dampedoutdoortemp_, DeviceValueType::INT8, FL_(dampedoutdoortemp), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_building));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &dateTime_,
                              DeviceValueType::STRING,
                              FL_(tpl_datetime),
                              FL_(dateTime),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_datetime));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaCalIntTemperature_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(ibaCalIntTemperature),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_calinttemp),
                              -5,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaMinExtTemperature_,
                              DeviceValueType::INT8,
                              FL_(ibaMinExtTemperature),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minexttemp),
                              -30,
                              0);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempsensor1_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempsensor1),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempsensor2_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempsensor2),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ibaDamping_, DeviceValueType::BOOL, FL_(damping), DeviceValueUOM::NONE, MAKE_CF_CB(set_damping));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dampedoutdoortemp_, DeviceValueType::INT8, FL_(dampedoutdoortemp), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &ibaBuildingType_,
                              DeviceValueType::ENUM,
                              FL_(enum_ibaBuildingType),
                              FL_(ibaBuildingType),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_building));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        // FR100 is not writable, see. https://github.com/emsesp/EMS-ESP32/issues/536
        // FW500 is not writable, see. https://github.com/emsesp/EMS-ESP32/issues/666
        if (has_flags(EMSdevice::EMS_DEVICE_FLAG_JUNKERS_OLD)) {
            register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(tpl_datetime), FL_(dateTime), DeviceValueUOM::NONE);
        } else {
            register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                                  &dateTime_,
                                  DeviceValueType::STRING,
                                  FL_(tpl_datetime),
                                  FL_(dateTime),
                                  DeviceValueUOM::NONE,
                                  MAKE_CF_CB(set_datetime));
        }
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hybridStrategy_,
                              DeviceValueType::ENUM,
                              FL_(enum_hybridStrategy),
                              FL_(hybridStrategy),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_hybridStrategy));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &switchOverTemp_,
                              DeviceValueType::INT8,
                              FL_(switchOverTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_switchOverTemp),
                              -20,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyCostRatio_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyCostRatio),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_energyCostRatio),
                              0,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &fossileFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(fossileFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_fossileFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &electricFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(electricFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_electricFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &delayBoiler_,
                              DeviceValueType::UINT8,
                              FL_(delayBoiler),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_delayBoiler),
                              5,
                              120);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempDiffBoiler_,
                              DeviceValueType::UINT8,
                              FL_(tempDiffBoiler),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_tempDiffBoiler),
                              1,
                              99);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_EASY:
        // Easy TC100 have no date/time, see issue #100, not sure about CT200, so leave it.
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    case EMSdevice::EMS_DEVICE_FLAG_CR11:
        break;
    case EMSdevice::EMS_DEVICE_FLAG_CRF:
    default:
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE); // can't set datetime
        break;
    }

#if defined(EMSESP_STANDALONE)
    // if we're just dumping out values, create a single dummy hc
    auto new_hc = std::make_shared<Thermostat::HeatingCircuit>(1, this->model()); // hc 1
    heating_circuits_.push_back(new_hc);
    register_device_values_hc(new_hc);

    // also a dhw circuit...
    auto new_dhw = std::make_shared<Thermostat::DhwCircuit>(0); // offset 0, dhw num 1
    dhw_circuits_.push_back(new_dhw);
    register_device_values_dhw(new_dhw);
#endif
}

// registers the values for a heating circuit
void Thermostat::register_device_values_hc(std::shared_ptr<Thermostat::HeatingCircuit> hc) {
    uint8_t model = hc->get_model();

    // heating circuit
    int8_t tag = DeviceValueTAG::TAG_HC1 + hc->hc();

    // different logic on how temperature values are stored, depending on model
    uint8_t seltemp_divider;
    uint8_t roomtemp_divider;

    if (model == EMSdevice::EMS_DEVICE_FLAG_EASY) {
        seltemp_divider  = DeviceValueNumOp::DV_NUMOP_DIV100;
        roomtemp_divider = DeviceValueNumOp::DV_NUMOP_DIV100;
    } else if (model == EMSdevice::EMS_DEVICE_FLAG_JUNKERS) {
        seltemp_divider  = DeviceValueNumOp::DV_NUMOP_DIV10;
        roomtemp_divider = DeviceValueNumOp::DV_NUMOP_DIV10;
    } else {
        seltemp_divider  = DeviceValueNumOp::DV_NUMOP_DIV2;
        roomtemp_divider = DeviceValueNumOp::DV_NUMOP_DIV10;
    }

    if (has_flags(EMSdevice::EMS_DEVICE_FLAG_NO_WRITE)) {
        register_device_value(tag, &hc->selTemp, DeviceValueType::INT16, seltemp_divider, FL_(selRoomTemp), DeviceValueUOM::DEGREES);
    } else {
        register_device_value(tag, &hc->selTemp, DeviceValueType::INT16, seltemp_divider, FL_(selRoomTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_temp), 0, 30);
    }
    register_device_value(tag, &hc->roomTemp, DeviceValueType::INT16, roomtemp_divider, FL_(roomTemp), DeviceValueUOM::DEGREES);
    register_device_value(tag, &hc->climate, DeviceValueType::ENUM, FL_(enum_climate), FL_(haclimate), DeviceValueUOM::NONE, nullptr, 5, 30);

    switch (model) {
    case EMSdevice::EMS_DEVICE_FLAG_RC10:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode6), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(nighttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag, &hc->reducehours, DeviceValueType::UINT8, FL_(reducehours), DeviceValueUOM::HOURS, MAKE_CF_CB(set_reducehours));
        register_device_value(tag, &hc->reduceminutes, DeviceValueType::UINT16, FL_(reduceminutes), DeviceValueUOM::MINUTES);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_R3000:
    case EMSdevice::EMS_DEVICE_FLAG_BC400:
    case EMSdevice::EMS_DEVICE_FLAG_CR120:
        if (model == EMSdevice::EMS_DEVICE_FLAG_BC400 || model == EMSdevice::EMS_DEVICE_FLAG_CR120) {
            register_device_value(tag, &hc->mode_new, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        } else {
            register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        }
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(ecotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ecotemp));
        register_device_value(tag,
                              &hc->manualtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(manualtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_manualtemp));
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(comforttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_comforttemp));
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT8, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp), 10, 30);
        register_device_value(tag, &hc->designtemp, DeviceValueType::UINT8, FL_(designtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_designtemp));
        register_device_value(tag, &hc->offsettemp, DeviceValueType::INT8, FL_(offsettemp), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_offsettemp));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT8, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT8, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag, &hc->roominfluence, DeviceValueType::UINT8, FL_(roominfluence), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_roominfluence));
        register_device_value(tag,
                              &hc->roominfl_factor,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(roominfl_factor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_roominfl_factor));
        register_device_value(tag, &hc->curroominfl, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(curroominfl), DeviceValueUOM::DEGREES_R);
        register_device_value(
            tag, &hc->nofrostmode, DeviceValueType::ENUM, FL_(enum_nofrostmode1), FL_(nofrostmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_nofrostmode));
        register_device_value(tag, &hc->nofrosttemp, DeviceValueType::INT8, FL_(nofrosttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nofrosttemp));
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT8, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(
            tag, &hc->summersetmode, DeviceValueType::ENUM, FL_(enum_summermode), FL_(summersetmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_summermode));
        register_device_value(tag,
                              &hc->hpoperatingmode,
                              DeviceValueType::ENUM,
                              FL_(enum_hpoperatingmode),
                              FL_(hpoperatingmode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_summermode));
        register_device_value(tag, &hc->summermode, DeviceValueType::ENUM, FL_(enum_summer), FL_(summermode), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->hpoperatingstate, DeviceValueType::ENUM, FL_(enum_operatingstate), FL_(hpoperatingstate), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->vacationmode, DeviceValueType::BOOL, FL_(vacationmode), DeviceValueUOM::NONE);
        if (model == EMSdevice::EMS_DEVICE_FLAG_RC100 || model == EMSdevice::EMS_DEVICE_FLAG_CR120) {
            register_device_value(
                tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode), FL_(controlmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_controlmode));
        } else {
            register_device_value(
                tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode1), FL_(controlmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_controlmode));
        }
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag,
                              &hc->tempautotemp,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(tempautotemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_tempautotemp),
                              -1,
                              30);
        register_device_value(
            tag, &hc->cooltemp, DeviceValueType::INT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(cooltemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_cooltemp), -1, 30);
        register_device_value(tag, &hc->fastHeatup, DeviceValueType::UINT8, FL_(fastheatup), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_fastheatup));
        register_device_value(tag,
                              &hc->switchonoptimization,
                              DeviceValueType::BOOL,
                              FL_(switchonoptimization),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_switchonoptimization));
        register_device_value(tag, &hc->reducemode, DeviceValueType::ENUM, FL_(enum_reducemode1), FL_(reducemode), DeviceValueUOM::NONE, MAKE_CF_CB(set_reducemode));
        register_device_value(tag, &hc->noreducetemp, DeviceValueType::INT8, FL_(noreducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_noreducetemp));
        register_device_value(tag, &hc->reducetemp, DeviceValueType::INT8, FL_(reducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_reducetemp));
        register_device_value(tag, &hc->wwprio, DeviceValueType::BOOL, FL_(wwprio), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwprio));
        register_device_value(tag, &hc->cooling, DeviceValueType::BOOL, FL_(hpcooling), DeviceValueUOM::NONE, MAKE_CF_CB(set_cooling));
        register_device_value(tag, &hc->coolingon, DeviceValueType::BOOL, FL_(coolingOn), DeviceValueUOM::NONE);

        register_device_value(tag, &hc->hpmode, DeviceValueType::ENUM, FL_(enum_hpmode), FL_(hpmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_hpmode));
        register_device_value(tag, &hc->dewoffset, DeviceValueType::UINT8, FL_(dewoffset), DeviceValueUOM::K, MAKE_CF_CB(set_dewoffset), 2, 10);
        register_device_value(tag, &hc->roomtempdiff, DeviceValueType::UINT8, FL_(roomtempdiff), DeviceValueUOM::K, MAKE_CF_CB(set_roomtempdiff));
        register_device_value(tag, &hc->hpminflowtemp, DeviceValueType::UINT8, FL_(hpminflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_hpminflowtemp));
        if (model == EMSdevice::EMS_DEVICE_FLAG_BC400) {
            register_device_value(tag, &hc->control, DeviceValueType::ENUM, FL_(enum_control2), FL_(control), DeviceValueUOM::NONE, MAKE_CF_CB(set_control));
        } else {
            register_device_value(tag, &hc->control, DeviceValueType::ENUM, FL_(enum_control1), FL_(control), DeviceValueUOM::NONE, MAKE_CF_CB(set_control));
        }
        if (model != EMSdevice::EMS_DEVICE_FLAG_RC100 && model != EMSdevice::EMS_DEVICE_FLAG_CR120) {
            register_device_value(tag,
                                  &hc->remotetemp,
                                  DeviceValueType::CMD,
                                  DeviceValueNumOp::DV_NUMOP_DIV10,
                                  FL_(remotetemp),
                                  DeviceValueUOM::DEGREES,
                                  MAKE_CF_CB(set_remotetemp),
                                  -1,
                                  101);
            register_device_value(tag, &hc->remotehum, DeviceValueType::CMD, FL_(remotehum), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_remotehum), -1, 101);
        }
        register_device_value(tag, &hc->heatondelay, DeviceValueType::UINT8, FL_(heatondelay), DeviceValueUOM::HOURS, MAKE_CF_CB(set_heatondelay), 1, 48);
        register_device_value(tag, &hc->heatoffdelay, DeviceValueType::UINT8, FL_(heatoffdelay), DeviceValueUOM::HOURS, MAKE_CF_CB(set_heatoffdelay), 1, 48);
        register_device_value(tag, &hc->instantstart, DeviceValueType::UINT8, FL_(instantstart), DeviceValueUOM::K, MAKE_CF_CB(set_instantstart), 1, 10);
        register_device_value(tag, &hc->boost, DeviceValueType::BOOL, FL_(boost), DeviceValueUOM::NONE, MAKE_CF_CB(set_boost));
        register_device_value(tag, &hc->boosttime, DeviceValueType::UINT8, FL_(boosttime), DeviceValueUOM::HOURS, MAKE_CF_CB(set_boosttime));
        register_device_value(tag, &hc->coolstart, DeviceValueType::UINT8, FL_(coolstart), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_coolstart), 20, 35);
        register_device_value(tag, &hc->coolondelay, DeviceValueType::UINT8, FL_(coolondelay), DeviceValueUOM::HOURS, MAKE_CF_CB(set_coolondelay), 1, 48);
        register_device_value(tag, &hc->cooloffdelay, DeviceValueType::UINT8, FL_(cooloffdelay), DeviceValueUOM::HOURS, MAKE_CF_CB(set_cooloffdelay), 1, 48);
        register_device_value(tag,
                              &hc->switchProgMode,
                              DeviceValueType::ENUM,
                              FL_(enum_switchProgMode),
                              FL_(switchProgMode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_switchProgMode));

        register_device_value(tag,
                              &hc->redThreshold,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(redthreshold),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_redthreshold),
                              12,
                              22);
        register_device_value(tag, &hc->solarInfl, DeviceValueType::UINT8, FL_(solarinfl), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_solarinfl), -5, -1);
        register_device_value(tag, &hc->currSolarInfl, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(currsolarinfl), DeviceValueUOM::DEGREES);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_CRF:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode5), FL_(mode), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype5), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT8, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_CR11:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode), FL_(mode), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT8, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(tag,
                              &hc->manualtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(manualtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_manualtemp));
        register_device_value(
            tag, &hc->nofrosttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(offtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_offtemp));
        register_device_value(tag,
                              &hc->daylowtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(daylowtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_daylowtemp));
        register_device_value(tag,
                              &hc->daymidtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(daymidtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_daymidtemp));
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(dayhightemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(nighttemp2), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(
            tag, &hc->switchtime1, DeviceValueType::STRING, FL_(tpl_switchtime1), FL_(switchtime), DeviceValueUOM::NONE, MAKE_CF_CB(set_switchtime1));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode3), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype3), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(nighttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode3), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT8, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT8, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag,
                              &hc->tempautotemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(tempautotemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_tempautotemp));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT8, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp), 10, 30);
        register_device_value(tag, &hc->summermode, DeviceValueType::ENUM, FL_(enum_summer), FL_(summermode), DeviceValueUOM::NONE);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC25:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode3), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype3), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(nighttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode3), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT8, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp));
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT8, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp));
        register_device_value(tag,
                              &hc->tempautotemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(tempautotemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_tempautotemp));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT8, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp), 9, 25);
        register_device_value(tag, &hc->summermode, DeviceValueType::ENUM, FL_(enum_summer), FL_(summermode), DeviceValueUOM::NONE);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode2), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        // register_device_value(tag, &hc->holiday, DeviceValueType::STRING, FL_(tpl_holidays), FL_(holidays), DeviceValueUOM::NONE, MAKE_CF_CB(set_holiday));
        // register_device_value(tag, &hc->vacation, DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations), DeviceValueUOM::NONE, MAKE_CF_CB(set_vacation));
        register_device_value(tag, &hc->pause, DeviceValueType::UINT8, FL_(pause), DeviceValueUOM::HOURS, MAKE_CF_CB(set_pause));
        register_device_value(tag, &hc->party, DeviceValueType::UINT8, FL_(party), DeviceValueUOM::HOURS, MAKE_CF_CB(set_party));
        register_device_value(tag, &vacation[0], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations1), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation1));
        register_device_value(tag, &vacation[1], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations2), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation2));
        register_device_value(tag, &vacation[2], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations3), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation3));
        register_device_value(tag, &vacation[3], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations4), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation4));
        register_device_value(tag, &vacation[4], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations5), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation5));
        register_device_value(tag, &vacation[5], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations6), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation6));
        register_device_value(tag, &vacation[6], DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations7), DeviceValueUOM::NONE, MAKE_CF_CB(set_RC30Vacation7));
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode2), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(
            tag, &hc->switchtime1, DeviceValueType::STRING, FL_(tpl_switchtime1), FL_(switchtime1), DeviceValueUOM::NONE, MAKE_CF_CB(set_switchtime1));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(
            tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode2), FL_(controlmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_controlmode));
        register_device_value(tag,
                              &hc->holidaytemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(holidaytemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_holidaytemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(nighttemp2), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nighttemp));
        register_device_value(tag,
                              &hc->daylowtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(daylowtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_daylowtemp));
        register_device_value(tag,
                              &hc->daymidtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(daymidtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_daymidtemp));
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(dayhightemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp));
        register_device_value(tag,
                              &hc->manualtemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(manualtemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_manualtemp));
        register_device_value(
            tag, &hc->nofrosttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(offtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_offtemp));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode3), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype3), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(daytemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_daytemp), 10, 30);
        register_device_value(tag,
                              &hc->nighttemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(nighttemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_nighttemp),
                              10,
                              30);
        register_device_value(tag, &hc->designtemp, DeviceValueType::UINT8, FL_(designtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_designtemp), 30, 90);
        register_device_value(tag,
                              &hc->offsettemp,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(offsettemp),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_offsettemp),
                              -5,
                              5);
        register_device_value(tag,
                              &hc->holidaytemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(holidaytemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_holidaytemp),
                              5,
                              30);
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT8, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &hc->summertemp, DeviceValueType::UINT8, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summertemp), 9, 25);
        register_device_value(tag, &hc->summermode, DeviceValueType::ENUM, FL_(enum_summer), FL_(summermode), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->holidaymode, DeviceValueType::BOOL, FL_(holidaymode), DeviceValueUOM::NONE);
        register_device_value(tag, &hc->nofrosttemp, DeviceValueType::INT8, FL_(nofrosttemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_nofrosttemp), -20, 10);
        register_device_value(
            tag, &hc->nofrostmode, DeviceValueType::ENUM, FL_(enum_nofrostmode), FL_(nofrostmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_nofrostmode));
        register_device_value(tag, &hc->roominfluence, DeviceValueType::UINT8, FL_(roominfluence), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_roominfluence), 0, 10);
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT8, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp), 5, 70);
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT8, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp), 30, 90);
        register_device_value(tag, &hc->flowtempoffset, DeviceValueType::UINT8, FL_(flowtempoffset), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_flowtempoffset), 0, 20);
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(tag, &hc->reducemode, DeviceValueType::ENUM, FL_(enum_reducemode), FL_(reducemode), DeviceValueUOM::NONE, MAKE_CF_CB(set_reducemode));
        register_device_value(
            tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode2), FL_(controlmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_controlmode));
        register_device_value(tag, &hc->control, DeviceValueType::ENUM, FL_(enum_control), FL_(control), DeviceValueUOM::NONE, MAKE_CF_CB(set_control));
        register_device_value(tag, &hc->holiday, DeviceValueType::STRING, FL_(tpl_holidays), FL_(holidays), DeviceValueUOM::NONE, MAKE_CF_CB(set_holiday));
        register_device_value(tag, &hc->vacation, DeviceValueType::STRING, FL_(tpl_holidays), FL_(vacations), DeviceValueUOM::NONE, MAKE_CF_CB(set_vacation));
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode2), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag, &hc->pause, DeviceValueType::UINT8, FL_(pause), DeviceValueUOM::HOURS, MAKE_CF_CB(set_pause), 0, 99);
        register_device_value(tag, &hc->party, DeviceValueType::UINT8, FL_(party), DeviceValueUOM::HOURS, MAKE_CF_CB(set_party), 0, 99);
        register_device_value(tag,
                              &hc->tempautotemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(tempautotemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_tempautotemp),
                              0,
                              30);
        register_device_value(tag, &hc->noreducetemp, DeviceValueType::INT8, FL_(noreducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_noreducetemp), -31, 10);
        register_device_value(tag, &hc->reducetemp, DeviceValueType::INT8, FL_(reducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_reducetemp), -20, 10);
        register_device_value(tag, &hc->vacreducetemp, DeviceValueType::INT8, FL_(vacreducetemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_vacreducetemp), -20, 10);
        register_device_value(
            tag, &hc->vacreducemode, DeviceValueType::ENUM, FL_(enum_reducemode), FL_(vacreducemode), DeviceValueUOM::NONE, MAKE_CF_CB(set_vacreducemode));
        register_device_value(tag,
                              &hc->remotetemp,
                              DeviceValueType::CMD,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(remotetemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_remotetemp),
                              -1,
                              101);
        register_device_value(tag, &hc->wwprio, DeviceValueType::BOOL, FL_(wwprio), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwprio));
        register_device_value(tag,
                              &hc->switchonoptimization,
                              DeviceValueType::BOOL,
                              FL_(switchonoptimization),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_switchonoptimization));
        register_device_value(
            tag, &hc->switchtime1, DeviceValueType::STRING, FL_(tpl_switchtime), FL_(switchtime1), DeviceValueUOM::NONE, MAKE_CF_CB(set_switchtime1));
        register_device_value(
            tag, &hc->switchtime2, DeviceValueType::STRING, FL_(tpl_switchtime), FL_(switchtime2), DeviceValueUOM::NONE, MAKE_CF_CB(set_switchtime2));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        register_device_value(tag, &hc->mode, DeviceValueType::ENUM, FL_(enum_mode4), FL_(mode), DeviceValueUOM::NONE, MAKE_CF_CB(set_mode));
        register_device_value(tag, &hc->modetype, DeviceValueType::ENUM, FL_(enum_modetype4), FL_(modetype), DeviceValueUOM::NONE);
        register_device_value(
            tag, &hc->daytemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(heattemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_heattemp));
        register_device_value(
            tag, &hc->nighttemp, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV2, FL_(ecotemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ecotemp));
        register_device_value(tag,
                              &hc->nofrosttemp,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(nofrosttemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_nofrosttemp));
        register_device_value(tag, &hc->control, DeviceValueType::ENUM, FL_(enum_j_control), FL_(control), DeviceValueUOM::NONE, MAKE_CF_CB(set_control));
        register_device_value(tag, &hc->program, DeviceValueType::ENUM, FL_(enum_progMode4), FL_(program), DeviceValueUOM::NONE, MAKE_CF_CB(set_program));
        register_device_value(tag,
                              &hc->remotetemp,
                              DeviceValueType::CMD,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(remotetemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_remotetemp),
                              -1,
                              101);
        register_device_value(tag, &hc->targetflowtemp, DeviceValueType::UINT8, FL_(targetflowtemp), DeviceValueUOM::DEGREES);
        register_device_value(tag,
                              &hc->summertemp,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(summertemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_summertemp),
                              9,
                              25);
        register_device_value(tag, &hc->roomsensor, DeviceValueType::ENUM, FL_(enum_roomsensor), FL_(roomsensor), DeviceValueUOM::NONE, MAKE_CF_CB(set_roomsensor));
        register_device_value(tag, &hc->holidaymode, DeviceValueType::ENUM, FL_(enum_mode4), FL_(holidaymode), DeviceValueUOM::NONE, MAKE_CF_CB(set_holidaymode));
        register_device_value(tag,
                              &hc->switchonoptimization,
                              DeviceValueType::BOOL,
                              FL_(switchonoptimization),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_switchonoptimization));
        register_device_value(tag, &hc->fastHeatup, DeviceValueType::ENUM, FL_(enum_heatup), FL_(heatup), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatup));
        register_device_value(tag, &hc->minflowtemp, DeviceValueType::UINT8, FL_(minflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_minflowtemp), 5, 70);
        register_device_value(tag, &hc->maxflowtemp, DeviceValueType::UINT8, FL_(maxflowtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_maxflowtemp), 30, 90);
        register_device_value(tag, &hc->designtemp, DeviceValueType::UINT8, FL_(designtemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_designtemp), 30, 90);
        register_device_value(
            tag, &hc->roominfluence, DeviceValueType::ENUM, FL_(enum_roominfluence), FL_(roominfluence), DeviceValueUOM::NONE, MAKE_CF_CB(set_roominfl_mode));
        register_device_value(tag,
                              &hc->roominfl_factor,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(roominfl_factor),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_roominfl_factor));
        register_device_value(
            tag, &hc->heatingtype, DeviceValueType::ENUM, FL_(enum_heatingtype1), FL_(heatingtype), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatingtype));
        register_device_value(
            tag, &hc->controlmode, DeviceValueType::ENUM, FL_(enum_controlmode3), FL_(controlmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_controlmode));
        break;
    default:
        break;
    }
}

// registers the values for a heating circuit
void Thermostat::register_device_values_dhw(std::shared_ptr<Thermostat::DhwCircuit> dhw) {
    int8_t tag = dhw->id();
    switch (this->model()) {
    case EMSdevice::EMS_DEVICE_FLAG_RC100:
    case EMSdevice::EMS_DEVICE_FLAG_RC300:
    case EMSdevice::EMS_DEVICE_FLAG_R3000:
    case EMSdevice::EMS_DEVICE_FLAG_BC400:
    case EMSdevice::EMS_DEVICE_FLAG_CR120:
        if (model() == EMSdevice::EMS_DEVICE_FLAG_BC400) {
            register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode4), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        } else if (model() == EMSdevice::EMS_DEVICE_FLAG_R3000) {
            register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode5), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        } else if (model() == EMSdevice::EMS_DEVICE_FLAG_CR120) {
            register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode6), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        } else if (model() == EMSdevice::EMS_DEVICE_FLAG_RC100) {
            register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        } else {
            register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        }
        register_device_value(tag, &dhw->wwSetTemp_, DeviceValueType::UINT8, FL_(wwSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwtemp));
        register_device_value(tag, &dhw->wwSetTempLow_, DeviceValueType::UINT8, FL_(wwSetTempLow), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwtemplow));
        register_device_value(tag, &dhw->wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwcircmode));
        register_device_value(tag,
                              &dhw->wwChargeDuration_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL15,
                              FL_(wwChargeDuration),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_wwchargeduration));
        register_device_value(tag, &dhw->wwCharge_, DeviceValueType::BOOL, FL_(wwCharge), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwcharge));
        register_device_value(tag, &dhw->wwExtra_, DeviceValueType::BOOL, FL_(wwExtra), DeviceValueUOM::NONE);
        register_device_value(tag, &dhw->wwDisinfecting_, DeviceValueType::BOOL, FL_(wwDisinfecting), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfect));
        register_device_value(
            tag, &dhw->wwDisinfectDay_, DeviceValueType::ENUM, FL_(enum_dayOfWeek), FL_(wwDisinfectDay), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectDay));
        register_device_value(tag,
                              &dhw->wwDisinfectHour_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL15,
                              FL_(wwDisinfectTime),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_wwDisinfectHour),
                              0,
                              1431);
        register_device_value(tag, &dhw->wwDailyHeating_, DeviceValueType::BOOL, FL_(wwDailyHeating), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDailyHeating));
        register_device_value(tag,
                              &dhw->wwDailyHeatTime_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL15,
                              FL_(wwDailyHeatTime),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_wwDailyHeatTime),
                              0,
                              1431);
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC10:
        register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode3), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20_N:
    case EMSdevice::EMS_DEVICE_FLAG_RC25:
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC20:
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30:
        register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode3), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        register_device_value(tag, &dhw->wwWhenModeOff_, DeviceValueType::BOOL, FL_(wwWhenModeOff), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwwhenmodeoff));
        register_device_value(tag, &dhw->wwDisinfecting_, DeviceValueType::BOOL, FL_(wwDisinfecting), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfect));
        register_device_value(
            tag, &dhw->wwDisinfectDay_, DeviceValueType::ENUM, FL_(enum_dayOfWeek), FL_(wwDisinfectDay), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectDay));
        register_device_value(tag, &dhw->wwDisinfectHour_, DeviceValueType::UINT8, FL_(wwDisinfectHour), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectHour), 0, 23);
        // register_device_value(tag, &dhw->wwHoliday_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwHolidays), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwHoliday));
        // register_device_value(tag, &dhw->wwVacation_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwVacations), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwVacation));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC30_N:
        register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        register_device_value(tag, &dhw->wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwcircmode));
        register_device_value(tag, &dhw->wwProgMode_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwProgMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwProgMode));
        register_device_value(tag, &dhw->wwCircProg_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwCircProg), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircProg));
        register_device_value(tag, &dhw->wwDisinfecting_, DeviceValueType::BOOL, FL_(wwDisinfecting), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfect));
        register_device_value(
            tag, &dhw->wwDisinfectDay_, DeviceValueType::ENUM, FL_(enum_dayOfWeek), FL_(wwDisinfectDay), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectDay));
        register_device_value(tag, &dhw->wwDisinfectHour_, DeviceValueType::UINT8, FL_(wwDisinfectHour), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectHour), 0, 23);
        register_device_value(tag, &dhw->wwMaxTemp_, DeviceValueType::UINT8, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp));
        register_device_value(tag, &dhw->wwOneTimeKey_, DeviceValueType::BOOL, FL_(wwOneTimeKey), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwOneTimeKey));
        register_device_value(
            tag, &dhw->wwSwitchTime_, DeviceValueType::STRING, FL_(tpl_switchtime), FL_(wwswitchtime), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwSwitchTime));
        register_device_value(tag,
                              &dhw->wwCircSwitchTime_,
                              DeviceValueType::STRING,
                              FL_(tpl_switchtime),
                              FL_(wwcircswitchtime),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_wwCircSwitchTime));
        register_device_value(tag, &dhw->wwHoliday_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwHolidays), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwHoliday));
        register_device_value(tag, &dhw->wwVacation_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwVacations), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwVacation));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_RC35:
        register_device_value(tag, &dhw->wwMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwmode));
        register_device_value(tag, &dhw->wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwMode2), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwcircmode));
        register_device_value(tag, &dhw->wwProgMode_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwProgMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwProgMode));
        register_device_value(tag, &dhw->wwCircProg_, DeviceValueType::ENUM, FL_(enum_wwProgMode), FL_(wwCircProg), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircProg));
        register_device_value(tag, &dhw->wwDisinfecting_, DeviceValueType::BOOL, FL_(wwDisinfecting), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfect));
        register_device_value(
            tag, &dhw->wwDisinfectDay_, DeviceValueType::ENUM, FL_(enum_dayOfWeek), FL_(wwDisinfectDay), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectDay));
        register_device_value(tag, &dhw->wwDisinfectHour_, DeviceValueType::UINT8, FL_(wwDisinfectHour), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwDisinfectHour), 0, 23);
        register_device_value(tag, &dhw->wwMaxTemp_, DeviceValueType::UINT8, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp), 60, 80);
        register_device_value(tag, &dhw->wwOneTimeKey_, DeviceValueType::BOOL, FL_(wwOneTimeKey), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwOneTimeKey));
        register_device_value(
            tag, &dhw->wwSwitchTime_, DeviceValueType::STRING, FL_(tpl_switchtime), FL_(wwswitchtime), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwSwitchTime));
        register_device_value(tag,
                              &dhw->wwCircSwitchTime_,
                              DeviceValueType::STRING,
                              FL_(tpl_switchtime),
                              FL_(wwcircswitchtime),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_wwCircSwitchTime));
        register_device_value(tag, &dhw->wwHoliday_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwHolidays), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwHoliday));
        register_device_value(tag, &dhw->wwVacation_, DeviceValueType::STRING, FL_(tpl_holidays), FL_(wwVacations), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwVacation));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_JUNKERS:
        register_device_value(tag, &dhw->wwCharge_, DeviceValueType::BOOL, FL_(wwCharge), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwcharge));
        break;
    case EMSdevice::EMS_DEVICE_FLAG_EASY:
    case EMSdevice::EMS_DEVICE_FLAG_CRF:
    default:
        break;
    }
}

} // namespace emsesp
