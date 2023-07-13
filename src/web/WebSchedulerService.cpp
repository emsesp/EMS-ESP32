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
// and also calls when the Scheduler web page is refreshed
void WebScheduler::read(WebScheduler & webScheduler, JsonObject & root) {
    JsonArray schedule = root.createNestedArray("schedule");
    uint8_t   counter  = 0;
    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        JsonObject si = schedule.createNestedObject();
        si["id"]      = counter++; // id is only used to render the table and must be unique
        si["active"]  = scheduleItem.active;
        si["flags"]   = scheduleItem.flags;
        si["time"]    = scheduleItem.time;
        si["cmd"]     = scheduleItem.cmd;
        si["value"]   = scheduleItem.value;
        si["name"]    = scheduleItem.name;
    }
}

// call on initialization and also when the Scheduile web page is saved
// this loads the data into the internal class
StateUpdateResult WebScheduler::update(JsonObject & root, WebScheduler & webScheduler) {
#ifdef EMSESP_STANDALONE
    // invoke some fake data for testing
    const char * json =
        "{[{\"id\":1,\"active\":true,\"flags\":31,\"time\": \"07:30\",\"cmd\": \"hc1/mode\",\"value\": \"day\",\"name\": \"turn on central heating\"}]}";
    StaticJsonDocument<500> doc;
    deserializeJson(doc, json);
    root = doc.as<JsonObject>();
    Serial.println(COLOR_BRIGHT_MAGENTA);
    Serial.print("Using custom file: ");
    serializeJson(root, Serial);
    Serial.println(COLOR_RESET);
#endif

    for (ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        Command::erase_command(EMSdevice::DeviceType::SCHEDULER, scheduleItem.name.c_str());
    }
    webScheduler.scheduleItems.clear();

    if (root["schedule"].is<JsonArray>()) {
        for (const JsonObject schedule : root["schedule"].as<JsonArray>()) {
            // create each schedule item, overwriting any previous settings
            // ignore the id (as this is only used in the web for table rendering)
            auto si   = ScheduleItem();
            si.active = schedule["active"];
            si.flags  = schedule["flags"];
            si.time   = schedule["time"].as<std::string>();
            si.cmd    = schedule["cmd"].as<std::string>();
            si.value  = schedule["value"].as<std::string>();
            si.name   = schedule["name"].as<std::string>();

            // calculated elapsed minutes
            si.elapsed_min = Helpers::string2minutes(si.time);
            si.retry_cnt   = 0xFF;                    // no startup retries

            webScheduler.scheduleItems.push_back(si); // add to list
            if (!webScheduler.scheduleItems.back().name.empty()) {
                Command::add(
                    EMSdevice::DeviceType::SCHEDULER,
                    webScheduler.scheduleItems.back().name.c_str(),
                    [webScheduler](const char * value, const int8_t id) {
                        return EMSESP::webSchedulerService.command_setvalue(value, webScheduler.scheduleItems.back().name);
                    },
                    FL_(schedule_cmd),
                    CommandFlag::ADMIN_ONLY);
            }
        }
    }
    EMSESP::webSchedulerService.publish(true);
    return StateUpdateResult::CHANGED;
}

// set active by api command
bool WebSchedulerService::command_setvalue(const char * value, const std::string name) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    for (ScheduleItem & scheduleItem : *scheduleItems) {
        if (scheduleItem.name == name) {
            if (scheduleItem.active == v) {
                return true;
            }
            scheduleItem.active = v;
            publish_single(name.c_str(), v);
            if (EMSESP::mqtt_.get_publish_onchange(0)) {
                publish();
            }
            return true;
        }
    }
    return false;
}

// process json output for info/commands and value_info
bool WebSchedulerService::get_value_info(JsonObject & output, const char * cmd) {
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    if (scheduleItems->size() == 0) {
        return false;
    }
    if (Helpers::toLower(cmd) == "commands") {
        output["info"]     = "lists all values";
        output["commands"] = "lists all commands";
        for (const ScheduleItem & scheduleItem : *scheduleItems) {
            if (!scheduleItem.name.empty()) {
                output[scheduleItem.name] = "activate schedule";
            }
        }
        return true;
    }
    if (strlen(cmd) == 0 || Helpers::toLower(cmd) == "values" || Helpers::toLower(cmd) == "info") {
        // list all names
        for (const ScheduleItem & scheduleItem : *scheduleItems) {
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
        return (output.size() > 0);
    }
    char command_s[30];
    strlcpy(command_s, cmd, sizeof(command_s));
    char * attribute_s = nullptr;

    // check specific attribute to fetch instead of the complete record
    char * breakp = strchr(command_s, '/');
    if (breakp) {
        *breakp     = '\0';
        attribute_s = breakp + 1;
    }
    JsonVariant data;
    for (const ScheduleItem & scheduleItem : *scheduleItems) {
        if (Helpers::toLower(scheduleItem.name) == Helpers::toLower(command_s)) {
            if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                output[scheduleItem.name] = scheduleItem.active;
            } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                output[scheduleItem.name] = scheduleItem.active ? 1 : 0;
            } else {
                char result[12];
                output[scheduleItem.name] = Helpers::render_boolean(result, scheduleItem.active);
            }
            data = output[scheduleItem.name];
        }
    }
    if (attribute_s && !strcmp(attribute_s, "value")) {
        output.clear();
        output["api_data"] = data;
    }
    if (output.size()) {
        return true;
    }
    output["message"] = "unknown command";
    return false;
}

// publish single value
void WebSchedulerService::publish_single(const char * name, const bool state) {
    if (!Mqtt::publish_single() || name == nullptr || name[0] == '\0') {
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
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    if (scheduleItems->size() == 0) {
        return;
    }
    if (Mqtt::publish_single() && force) {
        for (const ScheduleItem & scheduleItem : *scheduleItems) {
            publish_single(scheduleItem.name.c_str(), scheduleItem.active);
        }
    }

    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE);
    for (const ScheduleItem & scheduleItem : *scheduleItems) {
        if (!scheduleItem.name.empty() && !doc.containsKey(scheduleItem.name)) {
            if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                doc[scheduleItem.name] = scheduleItem.active;
            } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                doc[scheduleItem.name] = scheduleItem.active ? 1 : 0;
            } else {
                char result[12];
                doc[scheduleItem.name] = Helpers::render_boolean(result, scheduleItem.active);
            }

            // create HA config
            if (Mqtt::ha_enabled() && force) {
                StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> config;
                char                                        stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/scheduler_data", Mqtt::basename().c_str());
                config["stat_t"] = stat_t;

                char val_obj[50];
                char val_cond[65];
                snprintf(val_obj, sizeof(val_obj), "value_json['%s']", scheduleItem.name.c_str());
                snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
                config["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + "}}";

                char uniq_s[70];
                snprintf(uniq_s, sizeof(uniq_s), "scheduler_%s", scheduleItem.name.c_str());

                config["obj_id"]  = uniq_s;
                config["uniq_id"] = uniq_s; // same as object_id
                config["name"]    = scheduleItem.name.c_str();

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                char command_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                snprintf(topic, sizeof(topic), "switch/%s/scheduler_%s/config", Mqtt::basename().c_str(), scheduleItem.name.c_str());
                snprintf(command_topic, sizeof(command_topic), "%s/scheduler/%s", Mqtt::basename().c_str(), scheduleItem.name.c_str());
                config["cmd_t"] = command_topic;
                if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                    config["pl_on"]  = true;
                    config["pl_off"] = false;
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                    config["pl_on"]  = 1;
                    config["pl_off"] = 0;
                } else {
                    char result[12];
                    config["pl_on"]  = Helpers::render_boolean(result, true);
                    config["pl_off"] = Helpers::render_boolean(result, false);
                }

                JsonObject dev = config.createNestedObject("dev");
                JsonArray  ids = dev.createNestedArray("ids");
                ids.add(Mqtt::basename());

                // add "availability" section
                Mqtt::add_avty_to_doc(stat_t, config.as<JsonObject>(), val_cond);
                Mqtt::queue_ha(topic, config.as<JsonObject>());
            }
        }
    }
    if (doc.size() > 0) {
        Mqtt::queue_publish("scheduler_data", doc.as<JsonObject>());
    }
}

bool WebSchedulerService::has_commands() {
    EMSESP::webSchedulerService.read([&](WebScheduler & webScheduler) { scheduleItems = &webScheduler.scheduleItems; });
    if (scheduleItems->size() == 0) {
        return false;
    }
    for (const ScheduleItem & scheduleItem : *scheduleItems) {
        if (!scheduleItem.name.empty()) {
            return true;
        }
    }
    return false;
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
        if (strlen(data) == 0 && output.size()) {
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
// checks on the minute and at startup
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
