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

using namespace std::placeholders; // for `_1` etc

namespace emsesp {

uint32_t WebAPIService::api_count_ = 0;
uint16_t WebAPIService::api_fails_ = 0;

WebAPIService::WebAPIService(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void WebAPIService::registerURI() {
    // POST /api/{device}[/{hc|wwc|id}][/{name}]
    // note: must explicity use 'Content-Type: application/json' in header
    _server->on(EMSESP_API_SERVICE_PATH, HTTP_POST, [this](PsychicRequest * request, JsonVariant & json) {
        // if no json body then treat it as a secure GET
        if (!json.is<JsonObject>()) {
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
            JsonObject                                 input = input_doc.to<JsonObject>();
            return parse(request, input);
        }

        // extract values from the json. these will be used as default values
        auto && input = json.as<JsonObject>();
        return parse(request, input);
    });

    // GET /{device}/{entity}
    _server->on(EMSESP_API_SERVICE_PATH, HTTP_GET, [this](PsychicRequest * request) {
        StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
        JsonObject                                 input = input_doc.to<JsonObject>(); // empty input json
        return parse(request, input);
    });


    // GET - for when using GET query parameters, the old style from v2
    _server->on("/api", HTTP_GET, [this](PsychicRequest * request) {
        StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
        JsonObject                                 input = input_doc.to<JsonObject>(); // empty input json
        return parse(request, input);
    });

    // for settings
    _server->on(GET_SETTINGS_PATH, HTTP_GET, _securityManager->wrapRequest(std::bind(&WebAPIService::getSettings, this, _1), AuthenticationPredicates::IS_ADMIN));
    _server->on(GET_CUSTOMIZATIONS_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WebAPIService::getCustomizations, this, _1), AuthenticationPredicates::IS_ADMIN));
    _server->on(GET_SCHEDULE_PATH, HTTP_GET, _securityManager->wrapRequest(std::bind(&WebAPIService::getSchedule, this, _1), AuthenticationPredicates::IS_ADMIN));
    _server->on(GET_ENTITIES_PATH, HTTP_GET, _securityManager->wrapRequest(std::bind(&WebAPIService::getEntities, this, _1), AuthenticationPredicates::IS_ADMIN));
}

// parse the URL looking for query or path parameters
// reporting back any errors
esp_err_t WebAPIService::parse(PsychicRequest * request, JsonObject & input) {
    // check if the user has admin privileges (token is included and authorized)
    bool is_admin = false;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        is_admin                      = settings.notoken_api || AuthenticationPredicates::IS_ADMIN(authentication);
    });

    // check for query parameters first, the old style from v2
    // api?device={device}&cmd={name}&data={value}&id={hc}
    // TODO check if this works, because we're using wildcard now for api/*
    EMSESP::logger().info("API URL: %s", request->url().c_str()); // TODO remove
    if (request->hasParam(F_(device))) {
        input["device"] = request->getParam(F_(device))->value().c_str();
        EMSESP::logger().info("API: have device"); // TODO remove
    }

    // if (request->url() == "/api") {
    // get the device
    if (request->hasParam(F_(device))) {
        input["device"] = request->getParam(F_(device))->value().c_str();
    }
    if (request->hasParam(F_(cmd))) {
        input["cmd"] = request->getParam(F_(cmd))->value().c_str();
    }
    if (request->hasParam(F_(data))) {
        input["data"] = request->getParam(F_(data))->value().c_str();
    }
    if (request->hasParam(F_(value))) {
        input["value"] = request->getParam(F_(value))->value().c_str();
    }
    if (request->hasParam(F_(id))) {
        input["id"] = Helpers::atoint(request->getParam(F_(id))->value().c_str());
    }
    if (request->hasParam(F_(hc))) {
        input["hc"] = Helpers::atoint(request->getParam(F_(hc))->value().c_str());
    }
    if (request->hasParam(F_(wwc))) {
        input["wwc"] = Helpers::atoint(request->getParam(F_(wwc))->value().c_str());
    }
    serializeJson(input, Serial); // TODO remove
    // }

    // capture current heap memory before allocating the large return buffer
    EMSESP::system_.refreshHeapMem();

    // output json buffer
    size_t buffer = EMSESP_JSON_SIZE_XXXLARGE;

    PsychicJsonResponse response = PsychicJsonResponse(request, false, buffer);

    // TODO add back memory managegement. Be careful we do need to free()/delete() any object we extend with new()
    // while (!response->getSize()) {
    //     delete response;
    //     buffer -= 1024;
    //     response = new PrettyAsyncJsonResponse(false, buffer);
    // response = PsychicJsonResponse(request, false, buffer);
    // }

    JsonObject output = response.getRoot();

    // call command
    uint8_t return_code = Command::process(request->url().c_str(), is_admin, input, output);

    // handle failure
    if (return_code != CommandRet::OK) {
        char error[100];
        if (output.size()) {
            snprintf(error, sizeof(error), "API failed with error: %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
        } else {
            snprintf(error, sizeof(error), "API failed with error code (%s)", Command::return_code_string(return_code).c_str());
        }
        EMSESP::logger().err(error);
        api_fails_++;

        // FAIL, OK, NOT_FOUND, ERROR, NOT_ALLOWED = 400 (bad request), 200 (OK), 400 (not found), 400 (bad request), 401 (unauthorized)
        int ret_codes[6] = {400, 200, 400, 400, 401, 400};
        request->reply(ret_codes[return_code]); // exit with error code
    }

    api_count_++; // another succesful api call

    // if we're returning single values, just sent as plain text and not json
    // https://github.com/emsesp/EMS-ESP32/issues/462#issuecomment-1093877210
    if (output.containsKey("api_data")) {
        JsonVariant data = output["api_data"];
        return request->reply(200, "text/plain; charset=utf-8", data.as<const char *>());
    }

    // normal return
    // as setContentType("application/json; charset=utf-8")
    return response.send();
}

esp_err_t WebAPIService::getSettings(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, this, FS_BUFFER_SIZE);
    JsonObject          root     = response.getRoot();

    root["type"] = "settings";

    JsonObject node = root.createNestedObject("System");
    node["version"] = EMSESP_APP_VERSION;

    System::extractSettings(NETWORK_SETTINGS_FILE, "Network", root);
    System::extractSettings(AP_SETTINGS_FILE, "AP", root);
    System::extractSettings(MQTT_SETTINGS_FILE, "MQTT", root);
    System::extractSettings(NTP_SETTINGS_FILE, "NTP", root);
    System::extractSettings(OTA_SETTINGS_FILE, "OTA", root);
    System::extractSettings(SECURITY_SETTINGS_FILE, "Security", root);
    System::extractSettings(EMSESP_SETTINGS_FILE, "Settings", root);

    return response.send();
}

esp_err_t WebAPIService::getCustomizations(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, this, FS_BUFFER_SIZE);
    JsonObject          root     = response.getRoot();

    root["type"] = "customizations";
    System::extractSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", root);

    return response.send();
}

esp_err_t WebAPIService::getSchedule(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, this, FS_BUFFER_SIZE);
    JsonObject          root     = response.getRoot();

    root["type"] = "schedule";

    System::extractSettings(EMSESP_SCHEDULER_FILE, "Schedule", root);

    return response.send();
}

esp_err_t WebAPIService::getEntities(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, this, FS_BUFFER_SIZE);
    JsonObject          root     = response.getRoot();

    root["type"] = "entities";

    System::extractSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", root);

    return response.send();
}

} // namespace emsesp
