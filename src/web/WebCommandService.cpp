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
#include "WebCommandService.h"

#include "shuntingYard.h"
#include "httpClient.h"

namespace emsesp {

#ifndef EMSESP_STANDALONE
QueueHandle_t WebCommandService::commandQueue_ = nullptr;
#endif

WebCommandService::WebCommandService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebCommands::read, WebCommands::update, this, server, EMSESP_COMMANDS_SERVICE_PATH, securityManager, AuthenticationPredicates::IS_AUTHENTICATED)
    , _fsPersistence(WebCommands::read, WebCommands::update, this, fs, EMSESP_COMMANDS_FILE) {
}

void WebCommandService::begin() {
    _fsPersistence.readFromFS();

    EMSESP::webCommandService.read([&](WebCommands & webCommands) { commandItems_ = &webCommands.commandItems; });

    EMSESP::logger().info("Starting Commands service");
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s/#", F_(commands));
    Mqtt::subscribe(EMSdevice::DeviceType::COMMAND, topic, nullptr);

#ifndef EMSESP_STANDALONE
    // when PSRAM is available, run command execution (potentially blocking, e.g. HTTP) in its own task
    // so it never stalls the main loop. Without PSRAM we execute inline (see queueCommand()).
    if (EMSESP::system_.PSram()) {
        commandQueue_ = xQueueCreate(EMSESP_COMMAND_QUEUE_SIZE, sizeof(CommandJob *));
        if (commandQueue_ != nullptr) {
#if defined(CONFIG_FREERTOS_UNICORE) || (EMSESP_COMMAND_RUNNING_CORE < 0)
            xTaskCreate((TaskFunction_t)command_task, "command_task", EMSESP_COMMAND_STACKSIZE, NULL, EMSESP_COMMAND_PRIORITY, NULL);
#else
            xTaskCreatePinnedToCore(
                (TaskFunction_t)command_task, "command_task", EMSESP_COMMAND_STACKSIZE, NULL, EMSESP_COMMAND_PRIORITY, NULL, EMSESP_COMMAND_RUNNING_CORE);
#endif
        }
    }
#endif

#if defined(EMSESP_TEST)
    load_test_data();
#endif
}

// enqueue a command for the worker task. Fire-and-forget: returns true when the job was queued
// (or executed inline when no worker exists). It does NOT report the command's success/failure.
bool WebCommandService::queueCommand(const char * name, const char * value) {
#ifndef EMSESP_STANDALONE
    if (commandQueue_ != nullptr) {
        CommandJob * job = new CommandJob();
        if (job == nullptr) {
            return false;
        }
        job->name      = name ? name : "";
        job->has_value = (value != nullptr);
        job->value     = value ? value : "";
        if (xQueueSend(commandQueue_, &job, 0) != pdPASS) {
            EMSESP::logger().warning("Command queue full, dropping '%s'", job->name.c_str());
            delete job;
            return false;
        }
        return true;
    }
#endif
    // no worker task available (no PSRAM or standalone build) - run synchronously
    return executeCommand(name, value);
}

// true if the command definition is a HTTP/URL command (JSON with a http(s):// url),
// i.e. one that will do a blocking TCP/TLS request when executed
bool WebCommandService::isUrlCommand(const std::string & command) {
    JsonDocument doc;
    if (deserializeJson(doc, command) != DeserializationError::Ok) {
        return false;
    }
    std::string url       = doc["url"] | "";
    auto        lower_url = Helpers::toLower(url.c_str());
    return lower_url.starts_with("http://") || lower_url.starts_with("https://");
}

// true if the command runs the shunting-yard on its own argument. Those values must be passed
// through raw: a first pass strips the quotes from literal text, so the command's own pass then
// re-parses that text as an expression and chokes on characters like '!' or drops the spaces.
bool WebCommandService::computesOwnValue(const std::string & cmd) {
    return cmd == "system/message" || cmd == "system/sendmail";
}

// true if a value expression contains an embedded {"url":...} JSON snippet, which compute()
// will resolve with a blocking HTTP request. Mirrors the scan compute() does in shuntingYard.cpp
bool WebCommandService::valueContainsUrl(const std::string & value) {
    auto f = value.find_first_of('{');
    while (f != std::string::npos) {
        // find the matching closing brace, like compute() does
        auto e = f + 1;
        for (uint8_t i = 1; i > 0; e++) {
            if (e >= value.length()) {
                return false; // unbalanced braces, compute() will give up too
            } else if (value[e] == '}') {
                i--;
            } else if (value[e] == '{') {
                i++;
            }
        }
        JsonDocument doc;
        if (deserializeJson(doc, value.substr(f, e - f)) == DeserializationError::Ok) {
            for (JsonPairConst p : doc.as<JsonObjectConst>()) {
                if (Helpers::toLower(p.key().c_str()) == "url") {
                    return true;
                }
            }
        }
        f = value.find_first_of('{', e);
    }
    return false;
}

// smart dispatch: commands that will do a blocking HTTP/TLS request - either a URL command or an
// internal command whose value embeds a {url} fetch - are offloaded to the worker task so they
// can't stall the caller (main loop for MQTT, async_tcp task for the web API). Everything else
// runs synchronously, so the caller still gets the command's real success/failure.
// for queued commands the return value only means "dispatched".
bool WebCommandService::dispatchCommand(const char * name, const char * value) {
#ifndef EMSESP_STANDALONE
    if (commandQueue_ != nullptr) {
        const CommandItem * ci = find(name);
        if (ci != nullptr) {
            if (isUrlCommand(ci->cmd.c_str())) {
                return queueCommand(name, value);
            }
            // internal command whose value embeds a {url} fetch (e.g. system/message) - the value is
            // resolved by compute() at execution time and would block, so offload it to the worker task
            // the effective value is the override if given, else the command's stored default
            const std::string effective_value = value ? value : std::string(ci->value.c_str());
            if (valueContainsUrl(effective_value)) {
                return queueCommand(name, value);
            }
        }
    }
#endif
    return executeCommand(name, value);
}

#ifndef EMSESP_STANDALONE
// worker task: blocks on the queue and executes each command in turn, off the main loop
void WebCommandService::command_task(void * pvParameters) {
    CommandJob * job = nullptr;
    while (1) {
        if (xQueueReceive(commandQueue_, &job, portMAX_DELAY) == pdPASS && job != nullptr) {
            EMSESP::webCommandService.executeCommand(job->name.c_str(), job->has_value ? job->value.c_str() : nullptr);
            delete job;
            job = nullptr;
        }
    }
}
#endif

void WebCommands::read(WebCommands & webCommands, JsonObject root) {
    JsonArray items   = root["commands"].to<JsonArray>();
    uint8_t   counter = 1;
    for (const CommandItem & ci : webCommands.commandItems) {
        JsonObject obj = items.add<JsonObject>();
        obj["id"]      = counter++;
        obj["cmd"]     = ci.cmd;
        obj["value"]   = ci.value;
        obj["name"]    = (const char *)ci.name;
    }
}

StateUpdateResult WebCommands::update(JsonObject root, WebCommands & webCommands) {
    Command::erase_device_commands(EMSdevice::DeviceType::COMMAND);
    webCommands.commandItems.clear();

    auto items = root["commands"].as<JsonArray>();
    for (const JsonObject item : items) {
        auto ci  = CommandItem();
        ci.cmd   = item["cmd"].as<std::string>();
        ci.value = item["value"].as<std::string>();
        strlcpy(ci.name, item["name"].as<const char *>(), sizeof(ci.name));

        webCommands.commandItems.push_back(ci);
        Command::add(
            EMSdevice::DeviceType::COMMAND,
            webCommands.commandItems.back().name,
            [name = std::string(webCommands.commandItems.back().name)](const char * value, const int8_t, JsonObject) {
                return EMSESP::webCommandService.dispatchCommand(name.c_str(), value); // value is optional
            },
            FL_(command_cmd),
            CommandFlag::ADMIN_ONLY);
    }
    return StateUpdateResult::CHANGED;
}

// find a command item by name (case-insensitive)
const CommandItem * WebCommandService::find(const char * name) {
    if (name == nullptr || name[0] == '\0') {
        return nullptr;
    }
    auto lower_name = Helpers::toLower(name);
    for (const CommandItem & ci : *commandItems_) {
        if (ci.name[0] != '\0' && Helpers::toLower(ci.name) == lower_name) {
            return &ci;
        }
    }
    return nullptr;
}

// execute a named command — looks up by name and runs it
bool WebCommandService::executeCommand(const char * name, const char * value) {
    const CommandItem * ci = find(name);
    if (!ci) {
        EMSESP::logger().warning("Command '%s' not found", name ? name : "");
        return false;
    }
    // if there is a value use it, otherwise use the command's default value
    std::string cmd_value = value ? value : ci->value.c_str();
    return executeCommand(ci->name, std::string(ci->cmd.c_str()), cmd_value);
}

// execute a command with explicit cmd and value strings
// handles both HTTP URLs (JSON format) and internal API commands
bool WebCommandService::executeCommand(const char * name, const std::string & command, const std::string & data) {
    std::string cmd = Helpers::toLower(command);

    // run the value through the shunting-yard calculator so expressions like "custom/heatcnt + 1"
    // are resolved (entity references replaced by their values, then computed). Plain values pass
    // through unchanged. Applies to both URL and internal commands, like the old scheduler code
    // which computed the value before executing. Commands that compute their own argument are
    // skipped here so the value is only ever evaluated once.
    std::string computed_data = data;
    if (!data.empty() && !computesOwnValue(cmd)) {
        computed_data = compute(data);
        if (computed_data.empty()) {
            EMSESP::logger().warning("Command '%s': cannot compute value '%s'. Literal text must be quoted", name, data.c_str());
            return false;
        }
    }

    // handle HTTP commands (JSON with url/method/value)
    JsonDocument doc;
    if (deserializeJson(doc, cmd) == DeserializationError::Ok) {
        std::string url = doc["url"] | "";
        auto        q   = url.find_first_of('?');
        if (q != std::string::npos) {
            auto s = url.substr(q + 1);
            auto l = s.length();
            commands(s, false);
            url.replace(q + 1, l, s);
        }
        // the cmd's embedded value only gets entity substitution (commands), the passed value is fully computed
        std::string value  = doc["value"] | computed_data;
        std::string method = doc["method"] | "GET";
        commands(value, false);
        auto lower_url = Helpers::toLower(url.c_str());
        if (lower_url.starts_with("http://") || lower_url.starts_with("https://")) {
            std::string result;
            int         httpResult = HttpClient::request(url, method, value, doc["header"].as<JsonObjectConst>(), result);
            if (httpResult != 200) {
                EMSESP::logger().warning("Command '%s': URL command failed with http code %d", name, httpResult);
                return false;
            }
#if defined(EMSESP_DEBUG)
            EMSESP::logger().debug("Command '%s': URL '%s' successful with http code %d", name, url.c_str(), httpResult);
#endif
            return true;
        }
    }

    // handle internal API commands
    doc.clear();
    JsonObject input = doc.to<JsonObject>();
    if (!computed_data.empty()) {
        input["data"] = computed_data;
    }

    JsonDocument doc_output;
    JsonObject   output = doc_output.to<JsonObject>();

    char command_str[COMMAND_MAX_LENGTH];
    snprintf(command_str, sizeof(command_str), "/api/%s", cmd.c_str());

    uint8_t return_code = Command::process(command_str, true, input, output);
    if (return_code == CommandRet::OK) {
#if defined(EMSESP_DEBUG)
        EMSESP::logger().debug("Command '%s' (%s with data '%s') was successful", name, cmd.c_str(), data.c_str());
#endif
        if (data.empty() && output.size()) {
            Mqtt::queue_publish("response", output);
        }
        return true;
    }

    char error[100];
    if (output.size()) {
        snprintf(error, sizeof(error), "Command '%s': %s", name ? name : "", (const char *)output["message"]);
    } else {
        snprintf(error, sizeof(error), "Command '%s': %s failed with error %s", name, cmd.c_str(), Command::return_code_string(return_code));
    }
    EMSESP::logger().warning(error);
    return false;
}

bool WebCommandService::get_value_info(JsonObject output, const char * cmd) {
    if (commandItems_->empty()) {
        return true;
    }

    if (!strlen(cmd) || !strcmp(cmd, F_(values))) {
        for (const CommandItem & ci : *commandItems_) {
            if (ci.name[0] != '\0') {
                output[(const char *)ci.name] = ci.value.c_str();
            }
        }
        return true;
    }

    if (!strcmp(cmd, F_(info))) {
        for (const CommandItem & ci : *commandItems_) {
            if (ci.name[0] != '\0') {
                output[(const char *)ci.name] = std::string(ci.cmd + ": " + ci.value);
            }
        }
        return true;
    }

    if (!strcmp(cmd, F_(entities))) {
        for (const CommandItem & ci : *commandItems_) {
            if (ci.name[0] != '\0') {
                get_value_json(output[ci.name].to<JsonObject>(), ci);
            }
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

    // look up specific command by name
    const char * attribute_s = Command::get_attribute(cmd);
    for (const CommandItem & ci : *commandItems_) {
        if (Helpers::toLower(ci.name) == cmd) {
            get_value_json(output, ci);
            return Command::get_attribute(output, cmd, attribute_s);
        }
    }

    return false;
}

std::string WebCommandService::get_metrics_prometheus() {
    std::string result;
    result.reserve(commandItems_->size() * 100);
    for (const CommandItem & ci : *commandItems_) {
        if (ci.name[0] == '\0') {
            continue;
        }
        result += (std::string) "# HELP emsesp_cmd_" + ci.name + " " + ci.name + ", readable, writable, visible\n";
        result += (std::string) "# TYPE emsesp_cmd_" + ci.name + " gauge\n";
        result += (std::string) "emsesp_cmd_" + ci.name + " 1\n";
    }
    return result;
}

void WebCommandService::get_value_json(JsonObject output, const CommandItem & ci) {
    output["name"]      = (const char *)ci.name;
    output["fullname"]  = (const char *)ci.name;
    output["type"]      = "string";
    output["command"]   = ci.cmd;
    output["value"]     = ci.value;
    output["readable"]  = true;
    output["writeable"] = true;
    output["visible"]   = true;
}

void WebCommandService::publish(const bool force) {
    if (!Mqtt::enabled() || commandItems_->empty()) {
        return;
    }
    if (force && !EMSESP::mqtt_.get_publish_onchange(EMSdevice::DeviceType::SYSTEM)) {
        return;
    }

    JsonDocument doc(PSRAM_DOC);
    JsonObject   output = doc.to<JsonObject>();
    for (const CommandItem & ci : *commandItems_) {
        if (ci.name[0] != '\0' && !output[ci.name].is<JsonVariantConst>()) {
            output[(const char *)ci.name] = ci.value.c_str();
        }
    }

    if (!doc.isNull()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s_data", F_(commands));
        Mqtt::queue_publish(topic, output);
    }
}

uint8_t WebCommandService::count_entities() {
    return static_cast<uint8_t>(commandItems_ ? commandItems_->size() : 0);
}

#if defined(EMSESP_TEST)
void WebCommandService::load_test_data() {
    Command::erase_device_commands(EMSdevice::DeviceType::COMMAND);
    update([&](WebCommands & webCommands) {
        webCommands.commandItems.clear();

        auto ci  = CommandItem();
        ci.cmd   = "system/fetch";
        ci.value = "10";
        strcpy(ci.name, "fetch_values");
        webCommands.commandItems.push_back(ci);

        ci       = CommandItem();
        ci.cmd   = "system/message";
        ci.value = "hello";
        strcpy(ci.name, "send_message");
        webCommands.commandItems.push_back(ci);

        ci       = CommandItem();
        ci.cmd   = "system/message";
        ci.value = "{\"url\":\"http://emsesp.org/versions.json\"}";
        strcpy(ci.name, "get_versions");
        webCommands.commandItems.push_back(ci);

        // manually add the commands
        for (const auto & item : webCommands.commandItems) {
            if (item.name[0] != '\0') {
                Command::add(
                    EMSdevice::DeviceType::COMMAND,
                    item.name,
                    [name = std::string(item.name)](const char * value, const int8_t, JsonObject) {
                        return EMSESP::webCommandService.dispatchCommand(name.c_str(), value);
                    },
                    FL_(command_cmd),
                    CommandFlag::ADMIN_ONLY);
            }
        }

        return StateUpdateResult::CHANGED;
    });
}
#endif

} // namespace emsesp
