/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

uint32_t WebAPIService::api_count_ = 0;
uint16_t WebAPIService::api_fails_ = 0;

WebAPIService::WebAPIService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // API
    server->on(EMSESP_API_SERVICE_PATH, [this](AsyncWebServerRequest * request, JsonVariant json) { webAPIService(request, json); });

    // settings
    server->on(GET_SETTINGS_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { getSettings(request); }, AuthenticationPredicates::IS_ADMIN));

    server->on(GET_CUSTOMIZATIONS_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { getCustomizations(request); }, AuthenticationPredicates::IS_ADMIN));

    server->on(GET_SCHEDULE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { getSchedule(request); }, AuthenticationPredicates::IS_ADMIN));

    server->on(GET_ENTITIES_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { getEntities(request); }, AuthenticationPredicates::IS_ADMIN));
}

// POST|GET /{device}
// POST|GET /{device}/{entity}
void WebAPIService::webAPIService(AsyncWebServerRequest * request, JsonVariant json) {
    JsonObject input;
    // if no body then treat it as a secure GET
    if ((request->method() == HTTP_GET) || (!json.is<JsonObject>())) {
        // HTTP GET
        JsonDocument input_doc; // has no body JSON so create dummy as empty input object
        input = input_doc.to<JsonObject>();
    } else {
        // HTTP_POST | HTTP_PUT | HTTP_PATCH
        input = json.as<JsonObject>(); // extract values from the json. these will be used as default values
    }
    parse(request, input);
}

#ifdef EMSESP_TEST
// for test.cpp so we can invoke GETs to test the API
void WebAPIService::webAPIService(AsyncWebServerRequest * request) {
    JsonDocument input_doc;
    parse(request, input_doc.to<JsonObject>());
}
#endif

// parse the URL looking for query or path parameters
// reporting back any errors
void WebAPIService::parse(AsyncWebServerRequest * request, JsonObject input) {
    // check if the user has admin privileges (token is included and authorized)
    bool is_admin = false;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        is_admin                      = settings.notoken_api || AuthenticationPredicates::IS_ADMIN(authentication);
    });

    // check for query parameters first, the old style from v2
    // api?device={device}&cmd={name}&data={value}&id={hc}
    if (request->url() == EMSESP_API_SERVICE_PATH) {
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
    AsyncJsonResponse * response = new AsyncJsonResponse(false);

    // add more mem if needed - won't be needed in ArduinoJson 7
    // while (!response->getSize()) {
    //     delete response;
    //     buffer -= 1024;
    //     response = new AsyncJsonResponse(false, buffer);
    // }

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
        String data = output["api_data"].as<String>();
        request->send(200, "text/plain; charset=utf-8", data);
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
    Serial.print("data: ");
    if (output.size()) {
        serializeJson(output, Serial);
    }
    Serial.print("  (response code ");
    Serial.print(ret_codes[return_code]);
    Serial.println(")");
    Serial.print(COLOR_RESET);
#endif
}

void WebAPIService::getSettings(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["type"] = "settings";

    JsonObject node = root["System"].to<JsonObject>();
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
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["type"] = "customizations";

    System::extractSettings(EMSESP_CUSTOMIZATION_FILE, "Customizations", root);

    response->setLength();
    request->send(response);
}

void WebAPIService::getSchedule(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["type"] = "schedule";

    System::extractSettings(EMSESP_SCHEDULER_FILE, "Schedule", root);

    response->setLength();
    request->send(response);
}

void WebAPIService::getEntities(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    root["type"] = "entities";

    System::extractSettings(EMSESP_CUSTOMENTITY_FILE, "Entities", root);

    response->setLength();
    request->send(response);
}

} // namespace emsesp
