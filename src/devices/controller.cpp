/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#include "controller.h"

namespace emsesp {

REGISTER_FACTORY(Controller, EMSdevice::DeviceType::CONTROLLER);

Controller::Controller(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // IVT broadcasts Thermostat time  from controller (0x09) if display is off.
    if ((flags & 0x0F) == EMSdevice::EMS_DEVICE_FLAG_IVT) {
        register_telegram_type(0x06, "RCTime", false, MAKE_PF_CB(process_dateTime));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(dateTime), DeviceValueUOM::NONE);
    }

    // the CV200 ventilation controller (productID 233, deviceID 0x10) is the clock master of the ventilation
    // system, so sync its clock from NTP like it's done for thermostats (see Thermostat::process_RCTime)
    if (product_id == 233) {
        register_telegram_type(0x06, "RCTime", false, MAKE_PF_CB(process_CV200DateTime));
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &dateTime_, DeviceValueType::STRING, FL_(tpl_datetime), FL_(dateTime), DeviceValueUOM::NONE, MAKE_CF_CB(set_datetime));
    }
}

// process_dateTime - type 0x06 - date and time from a thermostat - 14 bytes long, IVT only
void Controller::process_dateTime(const std::shared_ptr<const Telegram> & telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }

    char newdatetime[sizeof(dateTime_)];
    // publish as dd.mm.yyyy hh:mm
    snprintf(newdatetime,
             sizeof(dateTime_),
             "%02d.%02d.%04d %02d:%02d",
             telegram->message_data[3],
             telegram->message_data[1] - 1,
             (telegram->message_data[0] & 0x7F) + 2000,
             telegram->message_data[2],
             telegram->message_data[4]);
    has_update(dateTime_, newdatetime, sizeof(dateTime_));
}

// process the clock broadcast (0x06) from the CV200 and correct it from NTP when it drifts,
// same logic as Thermostat::process_RCTime
void Controller::process_CV200DateTime(const std::shared_ptr<const Telegram> & telegram) {
    if (telegram->offset > 0 || telegram->message_length < 8) {
        return;
    }

    static uint8_t setTimeRetry = 0;
    uint8_t        dst          = 0xFE;
    bool           use_dst      = !telegram->read_value(dst, 9) || dst == 0xFF;

    if ((telegram->message_data[7] & 0x0C) && has_command(&dateTime_)) { // date and time not valid
        if (setTimeRetry < 3) {
            set_datetime("ntp", use_dst ? -1 : 0); // set from NTP, id 0 ignores dst
            setTimeRetry++;
        }

        return;
    }

    // check clock
    time_t now  = time(nullptr);
    bool   sync = false;
    // change to thermostat timezone if different
    EMSESP::esp32React.getNTPSettingsService()->read([&](const NTPSettings & settings) {
        sync = settings.thermostat_sync != 0;
        if (settings.thermostat_sync == 2) {
            setenv("TZ", settings.tzFormatT.c_str(), 1);
            tzset();
        }
    });
    tm * tm_     = localtime(&now);
    bool tset_   = tm_->tm_year > 110; // year 2010 and up, ESP time is valid
    tm_->tm_year = (telegram->message_data[0] & 0x7F) + 100;
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

    tm_->tm_isdst = -1;          // determine dst
    time_t ttime  = mktime(tm_); // make controller time
    // change back emsesp timezone
    EMSESP::esp32React.getNTPSettingsService()->read([&](const NTPSettings & settings) {
        if (settings.thermostat_sync == 2) {
            setenv("TZ", settings.tzFormat.c_str(), 1);
            tzset();
        }
    });

    // correct the controller clock if we have valid ntp time, and could write the command
    if (sync && tset_ && EMSESP::system_.ntp_connected() && !EMSESP::system_.readonly_mode() && has_command(&dateTime_)) {
        int difference = difftime(now, ttime);
        if (difference > 15 || difference < -15) {
            if (setTimeRetry < 3) {
                set_datetime("ntp", use_dst ? -1 : 0);
                EMSESP::logger().info("CV200 controller time correction %d seconds from ntp", difference);
                setTimeRetry++;
            }
        } else {
            setTimeRetry = 0;
        }
    }
}

// set date&time as string dd.mm.yyyy-hh:mm:ss-dw-dst or "ntp" for setting to internet-time
// dw - day of week (0..6), dst - summertime (0/1), id 0 ignores dst
// same format as Thermostat::set_datetime
bool Controller::set_datetime(const char * value, const int8_t id) {
    std::string dt;
    if (!Helpers::value2string(value, dt)) {
        return false;
    }

    uint8_t data[8];
    if (dt == "ntp") {
        time_t now = time(nullptr);
        tm *   tm_ = localtime(&now);
        EMSESP::esp32React.getNTPSettingsService()->read([&](const NTPSettings & settings) {
            if (settings.thermostat_sync == 2) {
                setenv("TZ", settings.tzFormatT.c_str(), 1);
                tzset();
                tm_ = localtime(&now);
                setenv("TZ", settings.tzFormat.c_str(), 1);
                tzset();
            }
        });
        if (tm_->tm_year < 110) { // no valid time
            return false;
        }

        if (!EMSESP::system_.ntp_connected()) {
            EMSESP::logger().warning("Set date: no valid NTP data, setting from ESP Clock");
        }

        data[0] = tm_->tm_year - 100; // Bosch counts from 2000
        data[1] = tm_->tm_mon + 1;
        data[2] = tm_->tm_hour;
        data[3] = tm_->tm_mday;
        data[4] = tm_->tm_min;
        data[5] = tm_->tm_sec;
        data[6] = (tm_->tm_wday + 6) % 7;            // Bosch counts from Mo, time from Su
        data[7] = (id == 0) ? 2 : tm_->tm_isdst + 2; // set DST and flag for ext. clock
    } else if (dt.length() == 23 || dt.length() == 21) {
        data[0] = (dt[7] - '0') * 100 + (dt[8] - '0') * 10 + (dt[9] - '0'); // year
        data[1] = (dt[3] - '0') * 10 + (dt[4] - '0');                       // month
        data[2] = (dt[11] - '0') * 10 + (dt[12] - '0');                     // hour
        data[3] = (dt[0] - '0') * 10 + (dt[1] - '0');                       // day
        data[4] = (dt[14] - '0') * 10 + (dt[15] - '0');                     // min
        data[5] = (dt[17] - '0') * 10 + (dt[18] - '0');                     // sec
        data[6] = (dt[20] - '0');                                           // day of week, Mo:0
        data[7] = dt.length() == 21 ? 2 : (dt[22] - '0') + 2;               // DST and flag
    } else {
        EMSESP::logger().warning("Set date: invalid data, wrong length");
        return false;
    }

    if (data[1] == 0 || data[1] > 12 || data[2] > 23 || data[3] == 0 || data[3] > 31 || data[4] > 59 || data[5] > 59 || data[6] > 6 || data[7] > 3) {
        EMSESP::logger().warning("Invalid date/time: %02d.%02d.2%03d-%02d:%02d:%02d-%d-%d", data[3], data[1], data[0], data[2], data[4], data[5], data[6], data[7]);
        return false;
    }

    write_command(0x06, 0, data, 8, 0x06);

    return true;
}

} // namespace emsesp
