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

#include "emsesp.h"
#include "WebSchedulerService.h"
#include <HTTPClient.h>

namespace emsesp {

WebSchedulerService::WebSchedulerService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebScheduler::read, WebScheduler::update, this, server, EMSESP_SCHEDULER_SERVICE_PATH, securityManager, AuthenticationPredicates::IS_AUTHENTICATED)
    , _fsPersistence(WebScheduler::read, WebScheduler::update, this, fs, EMSESP_SCHEDULER_FILE) {
}

// load the settings when the service starts
void WebSchedulerService::begin() {
    _fsPersistence.readFromFS();

    // save a local pointer to the scheduler item list
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems_ = &webScheduler.scheduleItems; });

    EMSESP::logger().info("Starting Scheduler service");
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s/#", F_(scheduler));
    Mqtt::subscribe(EMSdevice::DeviceType::SCHEDULER, topic, nullptr); // use empty function callback
#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.PSram()) {
        xTaskCreate((TaskFunction_t)scheduler_task, "scheduler_task", 5120, NULL, 1, NULL);
    }
#endif
}

// this creates the scheduler file, saving it to the FS
// and also calls when the Scheduler web page is refreshed
void WebScheduler::read(WebScheduler & webScheduler, JsonObject root) {
    JsonArray schedule = root["schedule"].to<JsonArray>();
    uint8_t   counter  = 1;
    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        JsonObject si = schedule.add<JsonObject>();
        si["id"]      = counter++; // id is only used to render the table and must be unique. 0 is for Dashboard
        si["flags"]   = scheduleItem.flags;
        si["active"]  = scheduleItem.flags != SCHEDULEFLAG_SCHEDULE_IMMEDIATE ? scheduleItem.active : false;
        si["time"]    = scheduleItem.flags != SCHEDULEFLAG_SCHEDULE_IMMEDIATE ? scheduleItem.time : "";
        si["cmd"]     = scheduleItem.cmd;
        si["value"]   = scheduleItem.value;
        si["name"]    = scheduleItem.name;
    }
}

// call on initialization and also when the Schedule web page is saved
// this loads the data into the internal class
StateUpdateResult WebScheduler::update(JsonObject root, WebScheduler & webScheduler) {
    // reset the list
    Command::erase_device_commands(EMSdevice::DeviceType::SCHEDULER);
    webScheduler.scheduleItems.clear();
    EMSESP::webSchedulerService.ha_reset();

    // build up the list of schedule items
    auto scheduleItems = root["schedule"].as<JsonArray>();
    for (const JsonObject schedule : scheduleItems) {
        // create each schedule item, overwriting any previous settings
        // ignore the id (as this is only used in the web for table rendering)
        auto si   = ScheduleItem();
        si.active = schedule["active"];
        si.flags  = schedule["flags"];
        si.time   = si.flags == SCHEDULEFLAG_SCHEDULE_IMMEDIATE ? "" : schedule["time"].as<std::string>();
        si.cmd    = schedule["cmd"].as<std::string>();
        si.value  = schedule["value"].as<std::string>();
        si.name   = schedule["name"].as<std::string>();

        // calculated elapsed minutes
        si.elapsed_min = Helpers::string2minutes(si.time);
        si.retry_cnt   = 0xFF; // no startup retries

        webScheduler.scheduleItems.push_back(si); // add to list
        if (!webScheduler.scheduleItems.back().name.empty()) {
            Command::add(
                EMSdevice::DeviceType::SCHEDULER,
                webScheduler.scheduleItems.back().name.c_str(),
                [webScheduler](const char * value, const int8_t id) {
                    return EMSESP::webSchedulerService.command_setvalue(value, id, webScheduler.scheduleItems.back().name.c_str());
                },
                FL_(schedule_cmd),
                CommandFlag::ADMIN_ONLY);
        }
    }

    EMSESP::webSchedulerService.publish(true);

    return StateUpdateResult::CHANGED;
}

// set active by api command
bool WebSchedulerService::command_setvalue(const char * value, const int8_t id, const char * name) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    for (ScheduleItem & scheduleItem : *scheduleItems_) {
        if (Helpers::toLower(scheduleItem.name) == Helpers::toLower(name)) {
            if (scheduleItem.active == v) {
                return true;
            }

            scheduleItem.active = v;
            publish_single(name, v);

            if (EMSESP::mqtt_.get_publish_onchange(0)) {
                publish();
            }

            return true;
        }
    }
    return false;
}

// process json output for info/commands and value_info
bool WebSchedulerService::get_value_info(JsonObject output, const char * cmd) {
    if (scheduleItems_->empty()) {
        return true;
    }

    if (!strlen(cmd) || !strcmp(cmd, F_(values)) || !strcmp(cmd, F_(info))) {
        // list all names
        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            if (!scheduleItem.name.empty()) {
                if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                    output[scheduleItem.name] = scheduleItem.active;
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                    output[scheduleItem.name] = scheduleItem.active ? 1 : 0;
                } else {
                    char result[12];
                    output[scheduleItem.name] = Helpers::render_boolean(result, scheduleItem.active);
                }
            }
        }
        return true;
    }

    const char * attribute_s = Command::get_attribute(cmd);

    if (!strcmp(cmd, F_(entities))) {
        uint8_t i = 0;
        char    name[30];
        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            strlcpy(name, scheduleItem.name == "" ? Helpers::smallitoa(name, i++) : scheduleItem.name.c_str(), sizeof(name));
            get_value_json(output[name].to<JsonObject>(), scheduleItem);
        }
        return true;
    }

    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (Helpers::toLower(scheduleItem.name) == cmd) {
            get_value_json(output, scheduleItem);
            return Command::set_attribute(output, cmd, attribute_s);
        }
    }

    return false; // not found
}

// build the json for specific entity
void WebSchedulerService::get_value_json(JsonObject output, const ScheduleItem & scheduleItem) {
    output["name"]     = scheduleItem.name;
    output["fullname"] = scheduleItem.name;
    output["type"]     = "boolean";
    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
        output["value"] = scheduleItem.active;
    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
        output["value"] = scheduleItem.active ? 1 : 0;
    } else {
        char result[12];
        output["value"] = Helpers::render_boolean(result, scheduleItem.active);
    }
    if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_CONDITION) {
        output["condition"] = scheduleItem.time;
    } else if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_ONCHANGE) {
        output["onchange"] = scheduleItem.time;
    } else if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER) {
        output["timer"] = scheduleItem.time;
    } else if (scheduleItem.flags != SCHEDULEFLAG_SCHEDULE_IMMEDIATE) {
        output["time"] = scheduleItem.time;
    }
    output["command"]   = scheduleItem.cmd;
    output["cmd_data"]  = scheduleItem.value;
    bool hasName        = scheduleItem.name != "";
    output["readable"]  = hasName;
    output["writeable"] = hasName;
    output["visible"]   = hasName;
}

// publish single value
void WebSchedulerService::publish_single(const char * name, const bool state) {
    if (!Mqtt::enabled() || !Mqtt::publish_single() || name == nullptr || name[0] == '\0') {
        return;
    }

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    if (Mqtt::publish_single2cmd()) {
        snprintf(topic, sizeof(topic), "%s/%s", F_(scheduler), name);
    } else {
        snprintf(topic, sizeof(topic), "%s%s/%s", F_(scheduler), "_data", name);
    }

    char payload[12];
    Mqtt::queue_publish(topic, Helpers::render_boolean(payload, state));
}

// publish to Mqtt
void WebSchedulerService::publish(const bool force) {
    if (force) {
        ha_registered_ = false;
    }

    if (!Mqtt::enabled()) {
        return;
    }

    if (scheduleItems_->empty()) {
        return;
    }

    if (Mqtt::publish_single() && force) {
        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            publish_single(scheduleItem.name.c_str(), scheduleItem.active);
        }
    }

    JsonDocument doc;
    bool         ha_created = ha_registered_;
    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (!scheduleItem.name.empty() && !doc[scheduleItem.name].is<JsonVariantConst>()) {
            if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                doc[scheduleItem.name] = scheduleItem.active;
            } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                doc[scheduleItem.name] = scheduleItem.active ? 1 : 0;
            } else {
                char result[12];
                doc[scheduleItem.name] = Helpers::render_boolean(result, scheduleItem.active);
            }

            // create HA config
            if (Mqtt::ha_enabled() && !ha_registered_) {
                JsonDocument config;
                char         stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/%s_data", Mqtt::base().c_str(), F_(scheduler));
                config["stat_t"] = stat_t;

                char val_obj[50];
                char val_cond[65];
                snprintf(val_obj, sizeof(val_obj), "value_json['%s']", scheduleItem.name.c_str());
                snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
                config["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + "}}";

                char uniq_s[70];
                snprintf(uniq_s, sizeof(uniq_s), "%s_%s", F_(scheduler), scheduleItem.name.c_str());

                config["obj_id"]  = uniq_s;
                config["uniq_id"] = uniq_s; // same as object_id
                config["name"]    = scheduleItem.name.c_str();

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                char command_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

                snprintf(topic, sizeof(topic), "switch/%s/%s_%s/config", Mqtt::basename().c_str(), F_(scheduler), scheduleItem.name.c_str());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(scheduler), scheduleItem.name.c_str());
                config["cmd_t"] = command_topic;

                Mqtt::add_ha_bool(config);
                Mqtt::add_ha_sections_to_doc(F_(scheduler), stat_t, config, !ha_created, val_cond);

                ha_created |= Mqtt::queue_ha(topic, config.as<JsonObject>());
            }
        }
    }

    ha_registered_ = ha_created;

    if (!doc.isNull()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s_data", F_(scheduler));
        Mqtt::queue_publish(topic, doc.as<JsonObject>());
    }
}

// count number of entries, default: only named items
uint8_t WebSchedulerService::count_entities(bool cmd_only) {
    uint8_t count = 0;
    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (!scheduleItem.name.empty() || !cmd_only) {
            count++;
        }
    }
    return count;
}

#include "shuntingYard.hpp"

// execute scheduled command
bool WebSchedulerService::command(const char * name, const std::string & command, const std::string & data) {
    std::string cmd = Helpers::toLower(command);

    // check http commands. e.g.
    // tasmota(get): http://<tasmotsIP>/cm?cmnd=power%20ON
    // shelly(get): http://<shellyIP>/relais/0?turn=on
    // parse json
    JsonDocument doc;
    if (deserializeJson(doc, cmd) == DeserializationError::Ok) {
        HTTPClient  http;
        std::string url = doc["url"] | "";
        // for a GET with parameters replace commands with values
        // don't search the complete url, it may contain a devicename in path
        auto q = url.find_first_of('?');
        if (q != std::string::npos) {
            auto s = url.substr(q + 1); // copy only parameters
            auto l = s.length();
            commands(s, false);
            url.replace(q + 1, l, s);
        }
        if (http.begin(url.c_str())) {
            // add any given headers
            for (JsonPair p : doc["header"].as<JsonObject>()) {
                http.addHeader(p.key().c_str(), p.value().as<String>().c_str());
            }
            std::string value  = doc["value"] | data.c_str(); // extract value if its in the command, or take the data
            std::string method = doc["method"] | "GET";       // default GET

            // if there is data, force a POST
            int httpResult = 0;
            if (value.length() || method == "post") { // we have all lowercase
                if (value.find_first_of('{') != std::string::npos) {
                    http.addHeader("Content-Type", "application/json"); // auto-set to JSON
                }
                httpResult = http.POST(value.c_str());
            } else {
                httpResult = http.GET(); // normal GET
            }

            http.end();

            // check HTTP return code
            if (httpResult != 200) {
                char error[100];
                snprintf(error, sizeof(error), "Schedule %s: URL command failed with http code %d", name, httpResult);
                emsesp::EMSESP::logger().warning(error);
                return false;
            }
#if defined(EMSESP_DEBUG)
            char msg[100];
            snprintf(msg, sizeof(msg), "Schedule %s: URL command successful with http code %d", name, httpResult);
            emsesp::EMSESP::logger().debug(msg);
#endif
            return true;
        }
        // we can add other json tests here
    }

    doc.clear();
    JsonObject input = doc.to<JsonObject>();
    if (!data.empty()) { // empty data queries a value
        input["data"] = data;
    }

    JsonDocument doc_output; // only for commands without output
    JsonObject   output = doc_output.to<JsonObject>();

    // prefix "api/" to command string
    char command_str[COMMAND_MAX_LENGTH];
    snprintf(command_str, sizeof(command_str), "/api/%s", cmd.c_str());

    uint8_t return_code = Command::process(command_str, true, input, output); // admin set
    if (return_code == CommandRet::OK) {
#if defined(EMSESP_DEBUG)
        EMSESP::logger().debug("Schedule command '%s' with data '%s' was successful", cmd.c_str(), data.c_str());
#endif
        if (data.empty() && output.size()) {
            Mqtt::queue_publish("response", output);
        }
        return true;
    }

    char error[100];
    if (output.size()) {
        // check for empty name
        snprintf(error, sizeof(error), "Schedule %s: %s", name ? name : "", (const char *)output["message"]); // use error message if we have it
    } else {
        snprintf(error, sizeof(error), "Schedule %s: command %s failed with error %s", name, cmd.c_str(), Command::return_code_string(return_code));
    }

    emsesp::EMSESP::logger().warning(error);
    return false;
}

// called from emsesp.cpp on every entity-change
// queue schedules to be handled executed in scheduler-loop
bool WebSchedulerService::onChange(const char * cmd) {
    for (ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_ONCHANGE
            && Helpers::toLower(scheduleItem.time).find(Helpers::toLower(cmd)) != std::string::npos) {
            cmd_changed_.push_back(&scheduleItem);
            return true;
        }
    }
    return false;
}

// handle condition schedules, parse string stored in schedule.time field
void WebSchedulerService::condition() {
    for (ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_CONDITION) {
            auto match = compute(scheduleItem.time);
#ifdef EMESESP_DEBUG
            // emsesp::EMSESP::logger().debug("condition match: %s", match.c_str());
#endif
            if (match.length() == 1 && match[0] == '1' && scheduleItem.retry_cnt == 0xFF) {
                scheduleItem.retry_cnt = command(scheduleItem.name.c_str(), scheduleItem.cmd, compute(scheduleItem.value)) ? 1 : 0xFF;
            } else if (match.length() == 1 && match[0] == '0' && scheduleItem.retry_cnt == 1) {
                scheduleItem.retry_cnt = 0xFF;
            } else if (match.length() != 1) { // the match is not boolean
                emsesp::EMSESP::logger().debug("condition result: %s", match.c_str());
            }
        }
    }
}

// process any scheduled jobs
void WebSchedulerService::loop() {
    // initialize static value on startup
    static int8_t   last_tm_min     = -2; // invalid value also used for startup commands
    static uint32_t last_uptime_min = 0;
    static uint32_t last_uptime_sec = 0;

    // get list of scheduler events and exit if it's empty
    if (scheduleItems_->empty()) {
        return;
    }

    // check if we have onChange events
    while (!cmd_changed_.empty()) {
        ScheduleItem si = *cmd_changed_.front();
        command(si.name.c_str(), si.cmd, compute(si.value));
        cmd_changed_.pop_front();
    }

    for (ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_IMMEDIATE) {
            command(scheduleItem.name.c_str(), scheduleItem.cmd, compute(scheduleItem.value));
            scheduleItem.active = false;
        }
    }

    // check conditions every 10 seconds, start after one minute
    uint32_t uptime_sec = uuid::get_uptime_sec() / 10;
    if (last_uptime_sec != uptime_sec && uptime_sec > 5) {
        condition();
        last_uptime_sec = uptime_sec;
    }

    // check startup commands
    if (last_tm_min == -2) {
        for (ScheduleItem & scheduleItem : *scheduleItems_) {
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0) {
                scheduleItem.retry_cnt = command(scheduleItem.name.c_str(), scheduleItem.cmd, compute(scheduleItem.value)) ? 0xFF : 0;
            }
        }
        last_tm_min = -1; // startup done, now use for RTC
    }

    // check timer every minute, sync to EMS-ESP clock
    uint32_t uptime_min = uuid::get_uptime_sec() / 60;
    if (last_uptime_min != uptime_min) {
        for (ScheduleItem & scheduleItem : *scheduleItems_) {
            // retry startup commands not yet executed
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0
                && scheduleItem.retry_cnt < MAX_STARTUP_RETRIES) {
                scheduleItem.retry_cnt = command(scheduleItem.name.c_str(), scheduleItem.cmd, scheduleItem.value) ? 0xFF : scheduleItem.retry_cnt + 1;
            }
            // scheduled timer commands
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min > 0
                && (uptime_min % scheduleItem.elapsed_min == 0)) {
                command(scheduleItem.name.c_str(), scheduleItem.cmd, compute(scheduleItem.value));
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

        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            uint8_t dow = scheduleItem.flags & SCHEDULEFLAG_SCHEDULE_TIMER ? 0 : scheduleItem.flags;
            if (scheduleItem.active && (real_dow & dow) && real_min == scheduleItem.elapsed_min) {
                command(scheduleItem.name.c_str(), scheduleItem.cmd, compute(scheduleItem.value));
            }
        }
        last_tm_min = tm->tm_min;
    }
}

// process schedules async
void WebSchedulerService::scheduler_task(void * pvParameters) {
    while (1) {
        delay(10); // no need to hurry
        if (EMSESP::system_.systemStatus() == SYSTEM_STATUS::SYSTEM_STATUS_NORMAL) {
            EMSESP::webSchedulerService.loop();
        }
    }
#ifndef EMSESP_STANDALONE
    vTaskDelete(NULL);
#endif
}

// hard coded tests
#if defined(EMSESP_TEST)
void WebSchedulerService::test() {
    static bool already_added = false;
    if (!already_added) {
        update([&](WebScheduler & webScheduler) {
            // webScheduler.scheduleItems.clear();
            // test 1
            auto si        = ScheduleItem();
            si.active      = true;
            si.flags       = 1;
            si.time        = "12:00";
            si.cmd         = "system/fetch";
            si.value       = "10";
            si.name        = "test_scheduler";
            si.elapsed_min = 0;
            si.retry_cnt   = 0xFF; // no startup retries

            webScheduler.scheduleItems.push_back(si);

            // test 2
            si             = ScheduleItem();
            si.active      = false;
            si.flags       = 1;
            si.time        = "13:00";
            si.cmd         = "system/message";
            si.value       = "20";
            si.name        = ""; // to make sure its excluded from Dashboard
            si.elapsed_min = 0;
            si.retry_cnt   = 0xFF; // no startup retries

            webScheduler.scheduleItems.push_back(si);
            already_added = true;

            return StateUpdateResult::CHANGED; // persist the changes
        });
    }

    // test shunting yard
    std::string test_cmd = "system/message";
    std::string test_value;

    // should output 'locale is en'
    test_value = "\"locale is \"system/settings/locale";
    command("test", test_cmd.c_str(), compute(test_value).c_str());

    // test with negative value
    // should output 'rssi is -23'
    test_value = "\"rssi is \"0+system/network/rssi";
    command("test1", test_cmd.c_str(), compute(test_value).c_str());

    // should output 'rssi is -23 dbm'
    test_value = "\"rssi is \"(system/network/rssi)\" dBm\"";
    command("test2", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "(custom/seltemp/value)";
    command("test3", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "\"seltemp=\"(custom/seltemp/value)";
    command("test4", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "(custom/seltemp)";
    command("test5", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "boiler/flowtempoffset";
    command("test7", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "(boiler/flowtempoffset/value)";
    command("test8", test_cmd.c_str(), compute(test_value).c_str());

    test_value = "(boiler/storagetemp1/value)";
    command("test9", test_cmd.c_str(), compute(test_value).c_str());

    // (14 - 40) * 2.8 + 5 = -67.8
    test_value = "(custom/seltemp - boiler/flowtempoffset) * 2.8 + 5";
    command("test10", test_cmd.c_str(), compute(test_value).c_str());

    // test case conversion
    test_value = "(thermostat/hc1/modetype == \"comfort\")";
    command("test11a", test_cmd.c_str(), compute(test_value).c_str()); // should be 1 true
    test_value = "(thermostat/hc1/modetype == \"Comfort\")";
    command("test11b", test_cmd.c_str(), compute(test_value).c_str()); // should be 1 true
    test_value = "(thermostat/hc1/modetype == \"unknown\")";
    command("test11c", test_cmd.c_str(), compute(test_value).c_str()); // should be 0 false

    // can't find entity, should fail
    test_value = "(boiler/storagetemp/value1)";
    command("test12", test_cmd.c_str(), compute(test_value).c_str());

    // can't find attribute, should fail
    test_value = "(boiler/storagetemp1/value1)";
    command("test13", test_cmd.c_str(), compute(test_value).c_str());

    // check when entity has no value, should pass
    test_value = "(boiler/storagetemp2/value)";
    command("test14", test_cmd.c_str(), compute(test_value).c_str());

    // should pass
    test_value = "(boiler/storagetemp1/value)";
    command("test15", test_cmd.c_str(), compute(test_value).c_str());

    // test HTTP POST to call HA script
    // test_cmd = "{\"method\":\"POST\",\"url\":\"http://192.168.1.42:8123/api/services/script/test_notify2\", \"header\":{\"authorization\":\"Bearer "
    //            "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJhMmNlYWI5NDgzMmI0ODE2YWQ2NzU4MjkzZDE2YWMxZSIsImlhdCI6MTcyMTM5MTI0NCwiZXhwIjoyMDM2NzUxMjQ0fQ."
    //            "S5sago1tEI6lNhrDCO0dM_WsVQHkD_laAjcks8tWAqo\"}}";
    // command("test99", test_cmd.c_str(), "");
}
#endif

} // namespace emsesp
