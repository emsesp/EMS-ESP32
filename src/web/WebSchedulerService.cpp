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
    EMSESP::logger().info("Starting Scheduler service");
}

// this creates the scheduler file, saving it to the FS
void WebScheduler::read(WebScheduler & webScheduler, JsonObject & root) {
    JsonArray schedule = root.createNestedArray("schedule");
    uint8_t   count    = 0;
    char      s[3];
    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        JsonObject si     = schedule.createNestedObject();
        si["id"]          = Helpers::smallitoa(s, count++); // create unique ID as a string
        si["active"]      = scheduleItem.active;
        si["flags"]       = scheduleItem.flags;
        si["time"]        = scheduleItem.time;
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
            si.id          = schedule["id"].as<std::string>();
            si.active      = schedule["active"];
            si.flags       = schedule["flags"];
            si.time        = schedule["time"].as<std::string>();
            si.cmd         = schedule["cmd"].as<std::string>();
            si.value       = schedule["value"].as<std::string>();
            si.description = schedule["description"].as<std::string>();

            // calculated elapsed minutes
            si.elapsed_min = Helpers::string2minutes(si.time);
            si.retry_cnt   = 0xFF; // no starup retries

            webScheduler.scheduleItems.push_back(si); // add to list
        }
    }
    return StateUpdateResult::CHANGED;
}

// execute scheduled command
bool WebSchedulerService::command(const char * cmd, const char * data) {
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc_input;
    JsonObject                                 input = doc_input.to<JsonObject>();
    if (strlen(data)) { // empty data queries a value
        input["data"] = data;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc_output; // only for commands without output
    JsonObject                                 output = doc_output.to<JsonObject>();

    // prefix "api/" to command string
    char command_str[100];
    snprintf(command_str, sizeof(command_str), "/api/%s", cmd);
    uint8_t return_code = Command::process(command_str, true, input, output); // admin set

    if (return_code == CommandRet::OK) {
        EMSESP::logger().debug("Scheduled command %s with data %s successfully", cmd, data);
        if (strlen(data) == 0 && Mqtt::enabled() && Mqtt::send_response() && output.size()) {
            Mqtt::queue_publish("response", output);
        }
        return true;
    }

    char error[100];
    if (output.size()) {
        snprintf(error,
                 sizeof(error),
                 "Scheduled command %s failed with error: %s (%s)",
                 cmd,
                 (const char *)output["message"],
                 Command::return_code_string(return_code).c_str());
    } else {
        snprintf(error, sizeof(error), "Scheduled command %s failed with error code (%s)", cmd, Command::return_code_string(return_code).c_str());
    }
    emsesp::EMSESP::logger().err(error);
    return false;
}

// process any scheduled jobs
// checks on the minute
void WebSchedulerService::loop() {
    // initialize static value on startup
    static int8_t   last_tm_min     = -1; // invalid value also used for startup commands
    static uint32_t last_uptime_min = 0;

    // get list of scheduler events and exit if it's empty
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    if (scheduleItems->size() == 0) {
        return;
    }

    // check startup commands
    if (last_tm_min == -1) {
        for (ScheduleItem & scheduleItem : *scheduleItems) {
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0) {
                scheduleItem.retry_cnt = command(scheduleItem.cmd.c_str(), scheduleItem.value.c_str()) ? 0xFF : 0;
            }
        }
        last_tm_min = 0; // startup done, now use for RTC
    }

    // check timer every minute, sync to EMS-ESP clock
    uint32_t uptime_min = uuid::get_uptime_sec() / 60;
    if (last_uptime_min != uptime_min) {
        for (ScheduleItem & scheduleItem : *scheduleItems) {
            // retry startup commands not yet executed
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0
                && scheduleItem.retry_cnt < MAX_STARTUP_RETRIES) {
                scheduleItem.retry_cnt = command(scheduleItem.cmd.c_str(), scheduleItem.value.c_str()) ? 0xFF : scheduleItem.retry_cnt + 1;
            }
            // scheduled timer commands
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min > 0
                && (uptime_min % scheduleItem.elapsed_min == 0)) {
                command(scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            }
        }
        last_uptime_min = uptime_min;
    }

    // check calender, sync to RTC, only execute if year is valid
    time_t now = time(nullptr);
    tm *   tm  = localtime(&now);
    if (tm->tm_min != last_tm_min && tm->tm_year > 120) {
        // find the real dow and minute from RTC
        uint8_t  real_dow = 1 << tm->tm_wday; // 1 is Sunday
        uint16_t real_min = tm->tm_hour * 60 + tm->tm_min;

        for (const ScheduleItem & scheduleItem : *scheduleItems) {
            if (scheduleItem.active && (real_dow & scheduleItem.flags) && real_min == scheduleItem.elapsed_min) {
                command(scheduleItem.cmd.c_str(), scheduleItem.value.c_str());
            }
        }
        last_tm_min = tm->tm_min;
    }
}

} // namespace emsesp
