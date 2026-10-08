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

#include "emsesp.h"
#include "WebSchedulerService.h"

#include "shuntingYard.h"

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

#if defined(EMSESP_TEST)
    load_test_data();
#endif
}

// this creates the scheduler file, saving it to the FS
// and also calls when the Scheduler web page is refreshed
void WebScheduler::read(WebScheduler & webScheduler, JsonObject root) {
    JsonArray schedule = root["schedule"].to<JsonArray>();
    uint8_t   counter  = 1;
    for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        JsonObject si  = schedule.add<JsonObject>();
        si["id"]       = counter++;
        si["flags"]    = scheduleItem.flags;
        si["active"]   = scheduleItem.active;
        si["time"]     = scheduleItem.time;
        si["cmd_name"] = scheduleItem.cmd_name;
        si["name"]     = (const char *)scheduleItem.name;
    }
}

// call on initialization and also when the Schedule web page is saved
StateUpdateResult WebScheduler::update(JsonObject root, WebScheduler & webScheduler) {
    Command::erase_device_commands(EMSdevice::DeviceType::SCHEDULER);
    for (ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
        char key[sizeof(scheduleItem.name) + 2];
        snprintf(key, sizeof(key), "s:%s", scheduleItem.name);
        if (EMSESP::nvs_.isKey(key)) {
            EMSESP::nvs_.remove(key);
        }
    }
    webScheduler.scheduleItems.clear();
    EMSESP::webSchedulerService.ha_reset();

    auto scheduleItems = root["schedule"].as<JsonArray>();
    for (const JsonObject schedule : scheduleItems) {
        // create each schedule item, overwriting any previous settings
        // ignore the id (as this is only used in the web for table rendering)
        auto si     = ScheduleItem();
        si.active   = schedule["active"];
        si.flags    = schedule["flags"];
        si.time     = schedule["time"].as<std::string>();
        si.cmd_name = schedule["cmd_name"].as<std::string>();
        strlcpy(si.name, schedule["name"].as<const char *>(), sizeof(si.name));

        // calculated elapsed minutes
        si.elapsed_min = Helpers::string2minutes(si.time.c_str());
        si.retry_cnt   = 0xFF;

        webScheduler.scheduleItems.push_back(si);
        char key[sizeof(webScheduler.scheduleItems.back().name) + 2];
        snprintf(key, sizeof(key), "s:%s", webScheduler.scheduleItems.back().name);
        if (EMSESP::nvs_.isKey(key)) {
            webScheduler.scheduleItems.back().active = EMSESP::nvs_.getBool(key);
        }
        Command::add(
            EMSdevice::DeviceType::SCHEDULER,
            webScheduler.scheduleItems.back().name,
            [name = std::string(webScheduler.scheduleItems.back().name)](const char * value, const int8_t id, JsonObject) {
                return EMSESP::webSchedulerService.command_setvalue(value, id, name.c_str());
            },
            FL_(schedule_cmd),
            CommandFlag::ADMIN_ONLY);
    }
    return StateUpdateResult::CHANGED;
}

// set active by api command
// value is a boolean to enable/disable the schedule item
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

            if (EMSESP::mqtt_.get_publish_onchange(EMSdevice::DeviceType::SYSTEM)) {
                publish();
            }

            char key[sizeof(scheduleItem.name) + 2];
            snprintf(key, sizeof(key), "s:%s", scheduleItem.name);
            EMSESP::nvs_.putBool(key, scheduleItem.active);
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
            if (scheduleItem.name[0] != '\0') {
                Mqtt::add_value_bool(output, scheduleItem.name, scheduleItem.active);
            }
        }
        return true;
    }

    if (!strcmp(cmd, F_(entities))) {
        uint8_t i = 0;
        char    name[20];
        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            strlcpy(name, scheduleItem.name[0] == '\0' ? Helpers::smallitoa(name, i++) : scheduleItem.name, sizeof(name));
            get_value_json(output[name].to<JsonObject>(), scheduleItem);
        }
        return true;
    }

    if (!strcmp(cmd, F_(metrics))) {
        std::string metrics = get_metrics_prometheus();
        if (!metrics.empty()) {
            output["api_data"] = metrics;
            return true;
        }
        return false;
    }

    const char * attribute_s = Command::get_attribute(cmd);
    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (Helpers::toLower(scheduleItem.name) == cmd) {
            get_value_json(output, scheduleItem);
            return Command::get_attribute(output, cmd, attribute_s);
        }
    }

    return false; // not found
}

// generate Prometheus metrics format from scheduler values
std::string WebSchedulerService::get_metrics_prometheus() {
    std::string result;
    result.reserve(scheduleItems_->size() * 140);
    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.name[0] == '\0') {
            continue;
        }
        result += (std::string) "# HELP emsesp_" + scheduleItem.name + " " + scheduleItem.name + ", boolean, readable, visible, writable\n";
        result += (std::string) "# TYPE emsesp_" + scheduleItem.name + " gauge\n";
        result += (std::string) "emsesp_" + scheduleItem.name + " " + (scheduleItem.active ? "1\n" : "0\n");
    }
    return result;
}

// build the json for specific entity
void WebSchedulerService::get_value_json(JsonObject output, const ScheduleItem & scheduleItem) {
    output["name"]     = (const char *)scheduleItem.name;
    output["fullname"] = (const char *)scheduleItem.name;
    output["type"]     = "boolean";
    Mqtt::add_value_bool(output, "value", scheduleItem.active);
    if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_CONDITION) {
        output["condition"] = scheduleItem.time;
    } else if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_ONCHANGE) {
        output["onchange"] = scheduleItem.time;
    } else if (scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER) {
        output["timer"] = scheduleItem.time;
    } else {
        output["time"] = scheduleItem.time;
    }
    output["cmd_name"]  = scheduleItem.cmd_name;
    output["readable"]  = true;
    output["writeable"] = true;
    output["visible"]   = true;
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
        snprintf(topic, sizeof(topic), "%s_data/%s", F_(scheduler), name);
    }

    char payload[12];
    Mqtt::queue_publish(topic, Helpers::render_boolean(payload, state));
}

// publish to Mqtt
void WebSchedulerService::publish(const bool force) {
    if (!Mqtt::enabled() || scheduleItems_->empty()) {
        return;
    }
    if (force) {
        if (Mqtt::publish_single()) {
            for (const ScheduleItem & scheduleItem : *scheduleItems_) {
                publish_single(scheduleItem.name, scheduleItem.active);
            }
            return;
        } else if (!EMSESP::mqtt_.get_publish_onchange(EMSdevice::DeviceType::SYSTEM)) {
            return; // wait for first time period
        }
    }

    JsonDocument doc(PSRAM_DOC);
    JsonObject   output     = doc.to<JsonObject>();
    bool         ha_created = ha_configdone_;
    for (const ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.name[0] != '\0' && !output[scheduleItem.name].is<JsonVariantConst>()) {
            Mqtt::add_value_bool(output, (const char *)scheduleItem.name, scheduleItem.active);

            // create HA config
            if (Mqtt::ha_enabled() && !ha_configdone_) {
                JsonDocument config(PSRAM_DOC);
                config["~"] = Mqtt::base();

                char stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "~/%s_data", F_(scheduler));
                config["stat_t"] = stat_t;

                char val_obj[50];
                char val_cond[65];
                snprintf(val_obj, sizeof(val_obj), "value_json['%s']", scheduleItem.name);
                snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);

                char val_tpl[150];
                if (Mqtt::discovery_type() == Mqtt::discoveryType::HOMEASSISTANT) {
                    snprintf(val_tpl, sizeof(val_tpl), "{{%s if %s else 'None'}}", val_obj, val_cond);
                } else {
                    snprintf(val_tpl, sizeof(val_tpl), "{{%s}}", val_obj); // omit value conditional Jinja2 template code
                }
                config["val_tpl"] = val_tpl;

                char uniq_s[70];
                snprintf(uniq_s, sizeof(uniq_s), "%s_%s", F_(scheduler), scheduleItem.name);

                config["uniq_id"] = uniq_s;
                config["name"]    = (const char *)scheduleItem.name;

                char def_ent_id[80];
                snprintf(def_ent_id, sizeof(def_ent_id), "switch.%s", uniq_s);
                config["def_ent_id"] = def_ent_id;

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                char command_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

                snprintf(topic, sizeof(topic), "switch/%s/%s_%s/config", Mqtt::basename().c_str(), F_(scheduler), scheduleItem.name);
                snprintf(command_topic, sizeof(command_topic), "~/%s/%s", F_(scheduler), scheduleItem.name);
                config["cmd_t"] = command_topic;

                Mqtt::add_ha_bool(config.as<JsonObject>());
                Mqtt::add_ha_dev_section(config.as<JsonObject>(), F_(scheduler), !ha_created);
                Mqtt::add_ha_avty_section(config.as<JsonObject>(), stat_t, val_cond);

                ha_created |= Mqtt::queue_ha(topic, config.as<JsonObject>());
            }
        }
    }

    ha_configdone_ = ha_created;

    if (!doc.isNull()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s_data", F_(scheduler));
        Mqtt::queue_publish(topic, output);
    }
}

// count number of scheduler entries
uint8_t WebSchedulerService::count_entities() {
    return static_cast<uint8_t>(scheduleItems_ ? scheduleItems_->size() : 0);
}

// execute the command associated with a schedule item
// looks up the named command in WebCommandService and runs it
bool WebSchedulerService::runScheduleCommand(const ScheduleItem & si) {
    if (si.cmd_name.empty()) {
        EMSESP::logger().warning("Schedule '%s': no command assigned", si.name);
        return false;
    }
    return EMSESP::webCommandService.dispatchCommand(si.cmd_name.c_str());
}

// queue schedules to be handled executed in WebSchedulerService::loop() called from emsesp.cpp
bool WebSchedulerService::onChange(const char * cmd) {
    for (ScheduleItem & scheduleItem : *scheduleItems_) {
        if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_ONCHANGE && Helpers::toLower(scheduleItem.time.c_str()) == Helpers::toLower(cmd)) {
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
            auto match = compute(scheduleItem.time.c_str());
            if (match.length() == 1 && match[0] == '1' && scheduleItem.retry_cnt == 0xFF) {
                scheduleItem.retry_cnt = runScheduleCommand(scheduleItem) ? 1 : 0xFF;
            } else if (match.length() == 1 && match[0] == '0' && scheduleItem.retry_cnt == 1) {
                scheduleItem.retry_cnt = 0xFF;
            } else if (match.length() != 1) {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("condition result: %s", match.c_str());
#endif
            }
        }
    }
}

// process any scheduled jobs
void WebSchedulerService::loop() {
    static int8_t   last_tm_min     = -2; // invalid value also used for startup commands
    static uint32_t last_uptime_min = 0;
    static uint32_t last_uptime_sec = 0;

    if (scheduleItems_->empty()) {
        return;
    }
    // do not execute any command in the first 60 secondes
    if (uuid::get_uptime_sec() < 60) {
        return;
    }

    // check if we have onChange events
    while (!cmd_changed_.empty()) {
        ScheduleItem si = *cmd_changed_.front();
        runScheduleCommand(si);
        cmd_changed_.pop_front();
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
                scheduleItem.retry_cnt = runScheduleCommand(scheduleItem) ? 0xFF : 0;
            }
        }
        last_tm_min = -1;
    }

    // check timer every minute, sync to EMS-ESP clock
    uint32_t uptime_min = uuid::get_uptime_sec() / 60;
    if (last_uptime_min != uptime_min) {
        for (ScheduleItem & scheduleItem : *scheduleItems_) {
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min == 0
                && scheduleItem.retry_cnt < MAX_STARTUP_RETRIES) {
                scheduleItem.retry_cnt = runScheduleCommand(scheduleItem) ? 0xFF : scheduleItem.retry_cnt + 1;
            }
            if (scheduleItem.active && scheduleItem.flags == SCHEDULEFLAG_SCHEDULE_TIMER && scheduleItem.elapsed_min > 0
                && (uptime_min % scheduleItem.elapsed_min == 0)) {
                runScheduleCommand(scheduleItem);
            }
        }
        last_uptime_min = uptime_min;
    }

    // check calendar, sync to RTC, only execute if year is valid
    time_t now = time(nullptr);
    tm *   tm  = localtime(&now);
    if (tm->tm_min != last_tm_min && tm->tm_year > 120) {
        uint8_t  real_dow = 1 << tm->tm_wday;
        uint16_t real_min = tm->tm_hour * 60 + tm->tm_min;

        for (const ScheduleItem & scheduleItem : *scheduleItems_) {
            uint8_t dow = scheduleItem.flags & SCHEDULEFLAG_SCHEDULE_TIMER ? 0 : scheduleItem.flags;
            if (scheduleItem.active && (real_dow & dow) && real_min == scheduleItem.elapsed_min) {
                runScheduleCommand(scheduleItem);
            }
        }
        last_tm_min = tm->tm_min;
    }
}

#if defined(EMSESP_TEST)
void WebSchedulerService::load_test_data() {
    Command::erase_device_commands(EMSdevice::DeviceType::SCHEDULER);
    update([&](WebScheduler & webScheduler) {
        webScheduler.scheduleItems.clear();

        auto si     = ScheduleItem();
        si.active   = true;
        si.flags    = 1; // day schedule
        si.time     = "12:00";
        si.cmd_name = "fetch_values";
        strcpy(si.name, "test_scheduler1");
        si.elapsed_min = 0;
        si.retry_cnt   = 0xFF;

        webScheduler.scheduleItems.push_back(si);

        si          = ScheduleItem();
        si.active   = true;
        si.flags    = SCHEDULEFLAG_SCHEDULE_TIMER;
        si.time     = "01:00";
        si.cmd_name = "send_message";
        strcpy(si.name, "test_scheduler2");
        si.elapsed_min = 60;
        si.retry_cnt   = 0xFF;

        webScheduler.scheduleItems.push_back(si);

        for (const auto & item : webScheduler.scheduleItems) {
            if (item.name[0] != '\0') {
                Command::add(
                    EMSdevice::DeviceType::SCHEDULER,
                    item.name,
                    [name = std::string(item.name)](const char * value, const int8_t id, JsonObject) {
                        return EMSESP::webSchedulerService.command_setvalue(value, id, name.c_str());
                    },
                    FL_(schedule_cmd),
                    CommandFlag::ADMIN_ONLY);
            }
        }

        return StateUpdateResult::CHANGED;
    });
}
#endif

} // namespace emsesp
