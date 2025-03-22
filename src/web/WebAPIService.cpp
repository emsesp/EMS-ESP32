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

namespace emsesp {

uint32_t WebAPIService::api_count_ = 0;
uint16_t WebAPIService::api_fails_ = 0;

WebAPIService::WebAPIService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    AsyncCallbackJsonWebHandler * jsonHandler = new AsyncCallbackJsonWebHandler(EMSESP_API_SERVICE_PATH);
    jsonHandler->setMethod(HTTP_POST | HTTP_GET);
    jsonHandler->onRequest([this](AsyncWebServerRequest * request, JsonVariant json) { webAPIService(request, json); });
    server->addHandler(jsonHandler);
}

// POST|GET api/
// POST|GET api/{device}
// POST|GET api/{device}/{entity}
void WebAPIService::webAPIService(AsyncWebServerRequest * request, JsonVariant json) {
    JsonDocument input_doc; // has no body JSON so create dummy as empty input object
    JsonObject   input;
    // if no body then treat it as a secure GET
    if ((request->method() == HTTP_GET) || (!json.is<JsonObject>())) {
        // HTTP GET
        input = input_doc.to<JsonObject>();
    } else {
        // HTTP_POST
        input = json.as<JsonObject>(); // extract values from the json. these will be used as default values
    }
    parse(request, input);
}

#ifdef EMSESP_TEST
// for test.cpp and unit tests so we can invoke GETs to test the API
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
    EMSESP::webSettingsService.read([&](WebSettings const & settings) {
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
        if (request->hasParam(F_(dhw))) {
            input["dhw"] = Helpers::atoint(request->getParam(F_(dhw))->value().c_str());
        }
    }

    // capture current heap memory before allocating the large return buffer
    emsesp::EMSESP::system_.refreshHeapMem();

    // output json buffer
    auto response = new AsyncJsonResponse(false);

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
        api_fails_++;
    }

    // if we're returning single values, just sent as plain text
    // https://github.com/emsesp/EMS-ESP32/issues/462#issuecomment-1093877210
    const char * api_data = output["api_data"];
    if (api_data) {
        request->send(200, "text/plain; charset=utf-8", api_data);
#if defined(EMSESP_UNITY)
        // store the result so we can test with Unity later
        storeResponse(output);
#endif
#if defined(EMSESP_STANDALONE) && !defined(EMSESP_UNITY)
        Serial.printf("%sweb output: %s[%s] %s(200)%s ", COLOR_WHITE, COLOR_BRIGHT_CYAN, request->url().c_str(), COLOR_BRIGHT_GREEN, COLOR_MAGENTA);
        serializeJson(output, Serial);
        Serial.println(COLOR_RESET);
#endif
        api_count_++;
        delete response;
        return;
    }

    // send the json that came back from the command call
    // sequence matches CommandRet in command.h (FAIL, OK, NOT_FOUND, ERROR, NOT_ALLOWED, INVALID, NO_VALUE)
    // 400 (bad request)
    // 200 (OK)
    // 404 (not found)
    // 401 (unauthorized)
    // 400 (invalid)
    int ret_codes[7] = {400, 200, 404, 400, 401, 400, 404};
    response->setCode(ret_codes[return_code]);
    response->setLength();
    response->setContentType("application/json; charset=utf-8");
    request->send(response);
    api_count_++;

#if defined(EMSESP_UNITY)
    // store the result so we can test with Unity later
    storeResponse(output);
#endif
#if defined(EMSESP_STANDALONE) && !defined(EMSESP_UNITY)
    Serial.printf("%sweb output: %s[%s]", COLOR_WHITE, COLOR_BRIGHT_CYAN, request->url().c_str());
    Serial.printf(" %s(%d)%s ", ret_codes[return_code] == 200 ? COLOR_BRIGHT_GREEN : COLOR_BRIGHT_RED, ret_codes[return_code], COLOR_YELLOW);
    serializeJson(output, Serial);
    Serial.println(COLOR_RESET);
#endif
}

#if defined(EMSESP_UNITY)
// store the result so we can test with Unity later
static JsonDocument storeResponseDoc_;

void WebAPIService::storeResponse(JsonObject response) {
    storeResponseDoc_.clear();       // clear it, so can only recall once
    storeResponseDoc_.add(response); // add the object to our doc
}
const char * WebAPIService::getResponse() {
    static std::string buffer;
    serializeJson(storeResponseDoc_, buffer);
    return buffer.c_str();
}
#endif

} // namespace emsesp
