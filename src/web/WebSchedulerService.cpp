/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

#include "emsesp.h"

namespace emsesp {

using namespace std::placeholders; // for `_1` etc

WebSchedulerService::WebSchedulerService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebScheduler::read, WebScheduler::update, this, server, EMSESP_SCHEDULER_SERVICE_PATH, securityManager, AuthenticationPredicates::IS_AUTHENTICATED)
    , _fsPersistence(WebScheduler::read, WebScheduler::update, this, fs, EMSESP_SCHEDULER_FILE) {
}

// load the settings when the service starts
void WebSchedulerService::begin() {
    _fsPersistence.readFromFS();
    EMSESP::logger().info("Starting Scheduler");
}

// this creates the scheduler file, saving it to the FS
void WebScheduler::read(WebScheduler & webScheduler, JsonObject & root) {
    JsonArray schedule = root.createNestedArray("schedule");
    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        JsonObject si     = schedule.createNestedObject();
        si["active"]      = scheduleItem.active;
        si["flags"]       = scheduleItem.flags;
        si["time"]        = scheduleItem.time;
        si["e_min"]       = scheduleItem.elapsed_min;
        si["cmd"]         = scheduleItem.cmd;
        si["value"]       = scheduleItem.value;
        si["description"] = scheduleItem.description;
    }
}

// call on initialization and also when the page is saved via web UI
// this loads the data into the internal class
StateUpdateResult WebScheduler::update(JsonObject & root, WebScheduler & webScheduler) {
#ifdef EMSESP_STANDALONE
    // invoke some fake data for testing
    const char * json =
        "{[{\"active\":true,\"flags\":31,\"time\": \"07:30\",\"cmd\": \"hc1/mode\",\"value\": \"day\",\"description\": \"turn on central heating\"}]}";
    StaticJsonDocument<500> doc;
    deserializeJson(doc, json);
    root = doc.as<JsonObject>();
    Serial.println(COLOR_BRIGHT_MAGENTA);
    Serial.print("Using custom file: ");
    serializeJson(root, Serial);
    Serial.println(COLOR_RESET);
#endif

    webScheduler.scheduleItems.clear();

    if (root["schedule"].is<JsonArray>()) {
        for (const JsonObject schedule : root["schedule"].as<JsonArray>()) {
            // create each schedule item, overwriting any previous settings
            // ignore the id (as this is only used in the web for table rendering)
            auto si        = ScheduleItem();
            si.active      = schedule["active"];
            si.flags       = schedule["flags"];
            si.time        = schedule["time"].as<std::string>();
            si.cmd         = schedule["cmd"].as<std::string>();
            si.value       = schedule["value"].as<std::string>();
            si.description = schedule["description"].as<std::string>();

            // calculated elapsed minutes
            si.elapsed_min = Helpers::string2minutes(si.time);

            webScheduler.scheduleItems.push_back(si); // add to list
        }
    }
    return StateUpdateResult::CHANGED;
}

// process any scheduled jobs
// checks on the minute
void WebSchedulerService::loop() {
    // get list of scheduler events and exit if it's empty
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    if (scheduleItems->size() == 0) {
        return;
    }

    time_t          now       = time(nullptr);
    tm *            tm        = localtime(&now);
    static int      last_min  = 0;
    static uint32_t starttime = now / 60;

    // check if we're on the minute
    if (tm->tm_min == last_min || tm->tm_year < 122) {
        return;
    }

    last_min            = tm->tm_min;
    bool     has_NTP    = tm->tm_year > 120;
    uint8_t  dow        = 1 << tm->tm_wday; // 1 is Sunday
    uint16_t min        = tm->tm_hour * 60 + tm->tm_min;
    uint32_t check_time = now / 60 - starttime;

#ifdef EMSESP_DEBUG
    EMSESP::logger().debug("On the minute. dow=%d, min=%d", dow, min);
#endif

    for (const ScheduleItem & scheduleItem : *scheduleItems) {
#ifdef EMSESP_DEBUG
        EMSESP::logger().debug("Checking active=%d, flags=%d and elapsed_min=%d", scheduleItem.active, scheduleItem.flags, scheduleItem.elapsed_min);
#endif

        if (scheduleItem.active
            && ((has_NTP && (dow & scheduleItem.flags) && min == scheduleItem.elapsed_min)                                 // day of week scheduling - Weekly
                || (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0 && check_time == 0) // only on boot
                || (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min > 0
                    && (check_time % scheduleItem.elapsed_min == 0)))) { // periodic - Timer
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> docin;
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> docout; // only for commands without output
            JsonObject                                 in  = docin.to<JsonObject>();
            JsonObject                                 out = docout.to<JsonObject>();
            in["data"]                                     = scheduleItem.value;

#ifdef EMSESP_DEBUG
            EMSESP::logger().debug("Calling scheduled command %s with data %s", scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
#endif

            if (CommandRet::OK != Command::process(scheduleItem.cmd.c_str(), true, in, out)) {
                EMSESP::logger().warning("Scheduled command %s with data %s failed", scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            } else {
                EMSESP::logger().debug("Scheduled command %s with data %s executed", scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            }
        }
    }
}


} // namespace emsesp