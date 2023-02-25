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
void WebSchedulerService::loop() {
    /*
    
    // quit if nothing in the webScheduler.scheduleItems list
    if (!webScheduler.scheduleItems.count()) {
        return;
    }

    // trigger first on elapsed time and process any Timer events
    // elapsed time is scheduleItems.elapsed_time and already calculated

    // next trigger on the minute and process all the Weekly events

    time_t     now      = time(nullptr);
    tm *       tm       = localtime(&now);
    static int last_min = 0;

    if (tm->tm_min == last_min || tm->tm_year < 122) {
        return;
    }

    last_min     = tm->tm_min;
    uint8_t  dow = 1 << tm->tm_wday; // I used 0 for Monday but can change
    uint16_t min = tm->tm_hour * 60 + tm->tm_min;

    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        if ((dow & scheduleItem.flags) && min == scheduleItem.elapsed_min) {
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> docin;
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> docout; // only for commands without output
            JsonObject                                 in  = docin.to<JsonObject>();
            JsonObject                                 out = docout.to<JsonObject>();
            in["data"]                                     = scheduleItem.value;
            if (CommandRet::OK != Command::process(scheduleItem.cmd.c_str(), true, in, out)) {
                EMSESP::logger().warning("Scheduled command %s with data %s failed", scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            } else {
                EMSESP::logger().debug("Scheduled command %s with data %s executed", scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            }
        }
    }

    */
}


} // namespace emsesp