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

#ifndef EMSESP_SHOWER_H
#define EMSESP_SHOWER_H

#include "emsesp.h"

namespace emsesp {

class Shower {
  public:
    void start();
    void loop();

    void set_shower_state(bool state, bool force = false);

    // commands
    static bool command_coldshot(const char * value, const int8_t id);

    void shower_timer(bool enable) {
        shower_timer_ = enable;
    }

    void shower_alert(bool enable) {
        shower_alert_ = enable;
    }

  private:
    static uuid::log::Logger logger_;

    static constexpr uint32_t SHOWER_PAUSE_TIME  = 15; // 15 seconds, max time if water is switched off & on during a shower
    static constexpr uint32_t SHOWER_OFFSET_TIME = 5;  // 5 seconds grace time, to calibrate actual time under the shower

    void shower_alert_start();
    void shower_alert_stop();

    bool     shower_timer_;          // true if we want to report back on shower times
    bool     shower_alert_;          // true if we want the alert of cold water
    uint32_t shower_alert_trigger_;  // default 7 minutes, before trigger a shot of cold water
    uint32_t shower_alert_coldshot_; // default 10 seconds for cold water before turning back hot water
    uint32_t shower_min_duration_;   // default 3 minutes (180 seconds), before recognizing its a shower
    uint32_t next_alert_;
    bool     ha_configdone_ = false; // for HA MQTT Discovery
    bool     shower_state_;

    uint32_t timer_start_; // sec
    uint32_t timer_pause_; // sec
    uint32_t duration_;    // sec

    // cold shot
    uint32_t alert_timer_start_; // sec
    bool     doing_cold_shot_;   // true if we've just sent a jolt of cold water
};

} // namespace emsesp

#endif
