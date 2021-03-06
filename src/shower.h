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

#ifndef EMSESP_SHOWER_H
#define EMSESP_SHOWER_H

#include "emsesp.h"

namespace emsesp {

class Shower {
  public:
    void start();
    void loop();

    void send_mqtt_stat(bool state, bool force = false);

    bool shower_alert() const {
        return shower_alert_;
    }

    void shower_alert(const bool shower_alert) {
        shower_alert_ = shower_alert;
    }

    bool shower_timer() const {
        return shower_timer_;
    }

    void shower_timer(const bool shower_timer) {
        shower_timer_ = shower_timer;
    }

  private:
    static uuid::log::Logger logger_;

    static constexpr uint32_t SHOWER_PAUSE_TIME        = 15000;  // in ms. 15 seconds, max time if water is switched off & on during a shower
    static constexpr uint32_t SHOWER_MIN_DURATION      = 120000; // in ms. 2 minutes, before recognizing its a shower
    static constexpr uint32_t SHOWER_OFFSET_TIME       = 5000;   // in ms. 5 seconds grace time, to calibrate actual time under the shower
    static constexpr uint32_t SHOWER_COLDSHOT_DURATION = 10;     // in seconds. 10 seconds for cold water before turning back hot water
    static constexpr uint32_t SHOWER_MAX_DURATION      = 420000; // in ms. 7 minutes, before trigger a shot of cold water

    void publish_values();
    void shower_alert_start();
    void shower_alert_stop();

    bool     shower_timer_;          // true if we want to report back on shower times
    bool     shower_alert_;          // true if we want the alert of cold water
    bool     ha_configdone_ = false; // for HA MQTT Discovery
    bool     shower_on_;
    uint32_t timer_start_;     // ms
    uint32_t timer_pause_;     // ms
    uint32_t duration_;        // ms
    bool     doing_cold_shot_; // true if we've just sent a jolt of cold water
};

} // namespace emsesp

#endif
