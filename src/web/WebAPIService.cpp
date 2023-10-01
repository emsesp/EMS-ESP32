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

WebAPIService::WebAPIService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _apiHandler("/api", std::bind(&WebAPIService::webAPIService_post, this, _1, _2), 256) { // for POSTS, must use 'Content-Type: application/json' in header
    server->on("/api", HTTP_GET, std::bind(&WebAPIService::webAPIService_get, this, _1));     // for GETS
    server->addHandler(&_apiHandler);

    // for settings
    server->on(GET_SETTINGS_PATH, HTTP_GET, securityManager->wrapRequest(std::bind(&WebAPIService::getSettings, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(GET_CUSTOMIZATIONS_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebAPIService::getCustomizations, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(GET_SCHEDULE_PATH, HTTP_GET, securityManager->wrapRequest(std::bind(&WebAPIService::getSchedule, this, _1), AuthenticationPredicates::IS_ADMIN));
    server->on(GET_ENTITIES_PATH, HTTP_GET, securityManager->wrapRequest(std::bind(&WebAPIService::getEntities, this, _1), AuthenticationPredicates::IS_ADMIN));
}

// HTTP GET
// GET /{device}
// GET /{device}/{entity}
void WebAPIService::webAPIService_get(AsyncWebServerRequest * request) {
    // has no body JSON so create dummy as empty input object
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
    JsonObject                                 input = input_doc.to<JsonObject>();
    parse(request, input);
}

// For HTTP POSTS with an optional JSON body
// HTTP_POST | HTTP_PUT | HTTP_PATCH
// POST /{device}[/{hc|id}][/{name}]
void WebAPIService::webAPIService_post(AsyncWebServerRequest * request, JsonVariant & json) {
    // if no body then treat it as a secure GET
    if (!json.is<JsonObject>()) {
        webAPIService_get(request);
        return;
    }

    // extract values from the json. these will be used as default values
    auto && input = json.as<JsonObject>();
    parse(request, input);
}

// parse the URL looking for query or path parameters
// reporting back any errors
void WebAPIService::parse(AsyncWebServerRequest * request, JsonObject & input) {
    // check if the user has admin privileges (token is included and authorized)
    bool is_admin = false;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        is_admin                      = settings.notoken_api || AuthenticationPredicates::IS_ADMIN(authentication);
    });

    // check for query parameters first, the old style from v2
    // api?device={device}&cmd={name}&data={value}&id={hc}
    if (request->url() == "/api") {
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
    }

    // capture current heap memory before allocating the large return buffer
    emsesp::EMSESP::system_.refreshHeapMem();

    // output json buffer
    size_t buffer   = EMSESP_JSON_SIZE_XXXLARGE;
    auto * response = new PrettyAsyncJsonResponse(false, buffer);
    while (!response->getSize()) {
        delete response;
        buffer -= 1024;
        response = new PrettyAsyncJsonResponse(false, buffer);
    }
    JsonObject output = response->getRoot();

    // call command
    uint8_t return_code = Command::process(request->url().c_str(), is_admin, input, output);

    if (return_code != CommandRet::OK) {
        char error[100];
        if (output.size()) {
            snprintf(error, sizeof(error), "API failed with error: %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
        } else {
            snprintf(error, sizeof(error), "API failed with error code (%s)", Command::return_code_string(return_code).c_str());
        }
        emsesp::EMSESP::logger().err(error);
        api_fails_++;
    }

    // if we're returning single values, just sent as plain text
    // https://github.com/emsesp/EMS-ESP32/issues/462#issuecomment-1093877210
    if (output.containsKey("api_data")) {
        JsonVariant data = output["api_data"];
        request->send(200, "text/plain; charset=utf-8", data.as<String>());
        api_count_++;
        delete response;
        return;
    }

    // send the json that came back from the command call
    // FAIL, OK, NOT_FOUND, ERROR, NOT_ALLOWED = 400 (bad request), 200 (OK), 400 (not found), 400 (bad request), 401 (unauthorized)
    int ret_codes[6] = {400, 200, 400, 400, 401, 400};
    response->setCode(ret_codes[return_code]);
    response->setLength();
    response->setContentType("application/json; charset=utf-8");
    request->send(response);
    api_count_++;

#if defined(EMSESP_STANDALONE)
    Serial.print(COLOR_YELLOW);
    Serial.print("web response code: ");
    Serial.println(ret_codes[return_code]);
    if (output.size()) {
        serializeJsonPretty(output, Serial);
    }
    Serial.println();
    Serial.print(COLOR_RESET);
#endif
}

void WebAPIService::getSettings(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, FS_BUFFER_SIZE);
    JsonObject root     = response->getRoot();

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

    response->setLength();
    request->send(response);
}

void WebAPIService::getCustomizations(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, FS_BUFFER_SIZE);
    JsonObject root     = response->getRoot();

    root["type"] = "customizations";

    System::extractSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", root);

    response->setLength();
    request->send(response);
}

void WebAPIService::getSchedule(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, FS_BUFFER_SIZE);
    JsonObject root     = response->getRoot();

    root["type"] = "schedule";

    System::extractSettings(EMSESP_SCHEDULER_FILE, "Schedule", root);

    response->setLength();
    request->send(response);
}

void WebAPIService::getEntities(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, FS_BUFFER_SIZE);
    JsonObject root     = response->getRoot();

    root["type"] = "entities";

    System::extractSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", root);

    response->setLength();
    request->send(response);
}

} // namespace emsesp
