/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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

#include "shower.h"

namespace emsesp {

uuid::log::Logger Shower::logger_{F_(shower), uuid::log::Facility::CONSOLE};

static bool force_coldshot = false;

void Shower::start() {
    EMSESP::webSettingsService.read([&](WebSettings const & settings) {
        shower_timer_          = settings.shower_timer;
        shower_alert_          = settings.shower_alert;
        shower_alert_trigger_  = settings.shower_alert_trigger * 60; // convert from minutes to seconds
        shower_alert_coldshot_ = settings.shower_alert_coldshot;     // in seconds
        shower_min_duration_   = settings.shower_min_duration;       // in seconds
    });

    Command::add(
        EMSdevice::DeviceType::BOILER,
        F_(coldshot),
        [&](const char * value, const int8_t id, JsonObject output) {
            LOG_INFO("Forcing coldshot...");
            if (shower_state_) {
                output["message"] = "OK";
                force_coldshot    = true;
            } else {
                output["message"] = "Coldshot failed. Shower not active";
                LOG_WARNING("Coldshot failed. Shower not active");
                force_coldshot = false;
            }
            return true;
        },
        FL_(coldshot_cmd),
        CommandFlag::ADMIN_ONLY);
}

void Shower::loop() {
    if (!shower_timer_) {
        return;
    }

    // if haven't done already send the MQTT topic shower_active with the current state
    // which creates the HA Discovery topic
    static bool mqtt_sent_ = false;
    if (!mqtt_sent_) {
        if (Mqtt::connected()) {
            create_ha_discovery();
            set_shower_state(shower_state_, true); // force publish initial state
            mqtt_sent_ = true;
        }
    }

    auto time_now = uuid::get_uptime_sec(); // in sec

    // if already in cold mode, ignore all this logic until we're out of the cold blast
    if (!doing_cold_shot_) {
        // is the hot water running?
        if (EMSESP::tap_water_active()) {
            // if heater was previously off, start the timer
            if (timer_start_ == 0) {
                // hot water just started...
                timer_start_     = time_now;
                timer_pause_     = 0; // remove any last pauses
                doing_cold_shot_ = false;
                duration_        = 0;
                shower_state_    = false;
                next_alert_      = shower_alert_trigger_;
            } else {
                // hot water has been  on for a while
                // first check to see if hot water has been on long enough to be recognized as a Shower/Bath
                if (!shower_state_ && (time_now - timer_start_) > shower_min_duration_) {
                    set_shower_state(true);
                    LOG_DEBUG("hot water still running, starting shower timer");
                }
                // check if the shower has been on too long
                else if ((shower_alert_ && ((time_now - timer_start_) > next_alert_)) || force_coldshot) {
                    shower_alert_start();
                }
            }
        } else { // hot water is off
            // if it just turned off, record the time as it could be a short pause
            if (timer_start_ && (timer_pause_ == 0)) {
                timer_pause_ = time_now;
            }

            // if shower has been off for longer than the wait time
            if (timer_pause_ && ((time_now - timer_pause_) > SHOWER_PAUSE_TIME)) {
                // it is over the wait period, so assume that the shower has finished and calculate the total time and publish
                // because its unsigned long, can't have negative so check if length is less than OFFSET_TIME
                if ((timer_pause_ - timer_start_) > SHOWER_OFFSET_TIME) {
                    duration_ = (timer_pause_ - timer_start_ - SHOWER_OFFSET_TIME); // duration in seconds
                    if (duration_ > shower_min_duration_) {
                        JsonDocument doc;

                        // duration in seconds
                        doc["duration"] = duration_; // seconds
                        // time_t now      = time(nullptr);
                        // // if NTP enabled, publish timestamp
                        // if (now > 1576800000) { // year 2020
                        //     // doc["timestamp_s"] = now; // if needed, in seconds
                        //     tm * tm_ = localtime(&now);
                        //     char dt[25];
                        //     strftime(dt, sizeof(dt), "%FT%T%z", tm_);
                        //     doc["timestamp"] = dt;
                        //     LOG_INFO("Shower finished %s (duration %lus)", dt, duration_);
                        // } else {
                        //     LOG_INFO("Shower finished (duration %lus)", duration_);
                        // }
                        LOG_INFO("Shower finished (duration %lus)", duration_);
                        Mqtt::queue_publish("shower_data", doc.as<JsonObject>());
                    }
                }

                // reset everything
                timer_start_       = 0;
                timer_pause_       = 0;
                doing_cold_shot_   = false;
                alert_timer_start_ = 0;

                set_shower_state(false);
            }
        }
        return;
    }

    // at this point we're in the shower cold shot (doing_cold_shot_ == true)
    // keep repeating until the time is up
    if ((time_now - alert_timer_start_) > shower_alert_coldshot_) {
        shower_alert_stop();
    }
}

// turn off hot water to send a shot of cold
void Shower::shower_alert_start() {
    LOG_DEBUG("Shower Alert started");
    (void)Command::call(EMSdevice::DeviceType::BOILER, "tapactivated", "false", DeviceValueTAG::TAG_DHW1);
    doing_cold_shot_   = true;
    force_coldshot     = false;
    alert_timer_start_ = uuid::get_uptime_sec(); // timer starts now
}

// turn back on the hot water for the shower
void Shower::shower_alert_stop() {
    if (doing_cold_shot_) {
        LOG_DEBUG("Shower Alert stopped");
        (void)Command::call(EMSdevice::DeviceType::BOILER, "tapactivated", "true", DeviceValueTAG::TAG_DHW1);
        doing_cold_shot_ = false;
        force_coldshot   = false;
        next_alert_ += shower_alert_trigger_;
    }
}

// sets the state and publishes the state to the MQTT topic shower_active
void Shower::set_shower_state(bool state, bool force) {
    // sets the state
    shower_state_ = state;

    // only publish if that state has changed
    static bool old_shower_state_ = false;
    if ((shower_state_ == old_shower_state_) && !force) {
        return;
    }
    old_shower_state_ = shower_state_;

    // always publish as a string - see https://github.com/emsesp/EMS-ESP/issues/369
    // and with retain flag set so HA will pick it up when EMS-ESP reboots
    char s[12];
    Mqtt::queue_publish_retain("shower_active", Helpers::render_boolean(s, shower_state_));
}

// send status of shower to MQTT topic called shower_active - which is determined by the state parameter
// and creates the HA config topic if HA enabled
void Shower::create_ha_discovery() {
    // first create the HA MQTT Discovery config topic
    // this has to be done before the state is published
    if (Mqtt::ha_enabled() && !ha_configdone_) {
        JsonDocument doc;
        char         topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        char         str[70];

        // shower active
        doc["~"]    = Mqtt::base();
        doc["name"] = "Shower Active";

        if (Mqtt::entity_format() == Mqtt::entityFormat::MULTI_SHORT) {
            snprintf(str, sizeof(str), "%s_shower_active", Mqtt::basename().c_str());
        } else {
            snprintf(str, sizeof(str), "shower_active"); // v3.4 compatible
        }
        doc["uniq_id"]    = str;
        doc["def_ent_id"] = (std::string) "binary_sensor." + str;
        doc["stat_t"]     = "~/shower_active";

        Mqtt::add_ha_bool(doc.as<JsonObject>());
        Mqtt::add_ha_dev_section(doc.as<JsonObject>(), "Shower Sensors", nullptr, nullptr, nullptr, false);
        Mqtt::add_ha_avty_section(doc.as<JsonObject>()); // no conditions

        snprintf(topic, sizeof(topic), "binary_sensor/%s/shower_active/config", Mqtt::basename().c_str());
        ha_configdone_ = Mqtt::queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

        // shower duration
        doc.clear();
        doc["~"]    = Mqtt::base();
        doc["name"] = "Shower Duration";

        snprintf(str, sizeof(str), "%s_shower_duration", Mqtt::basename().c_str());

        doc["uniq_id"]    = str;
        doc["def_ent_id"] = (std::string) "sensor." + str;
        doc["stat_t"]     = "~/shower_data";

        // don't bother with value template conditions if using Domoticz which doesn't fully support MQTT Discovery
        if (Mqtt::discovery_type() == Mqtt::discoveryType::HOMEASSISTANT) {
            doc["val_tpl"] = "{{value_json.duration if value_json.duration is defined else 0}}";
        } else {
            doc["val_tpl"] = "{{value_json.duration}}";
        }

        doc["unit_of_meas"] = "s";
        doc["stat_cla"]     = "measurement";
        doc["dev_cla"]      = "duration";
        // doc["ent_cat"]      = "diagnostic";

        Mqtt::add_ha_dev_section(doc.as<JsonObject>(), "Shower Sensors", nullptr, "EMS-ESP", EMSESP_APP_VERSION, true);
        Mqtt::add_ha_avty_section(doc.as<JsonObject>(), "~/shower_data", "value_json.duration is defined");

        snprintf(topic, sizeof(topic), "sensor/%s/shower_duration/config", Mqtt::basename().c_str());
        Mqtt::queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag
    }
}

} // namespace emsesp
