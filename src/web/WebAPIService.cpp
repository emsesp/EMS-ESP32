/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

// SUrlParser from https://github.com/Mad-ness/simple-url-parser

#include "emsesp.h"

using namespace std::placeholders; // for `_1` etc

namespace emsesp {

WebAPIService::WebAPIService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _apiHandler("/api", std::bind(&WebAPIService::webAPIService_post, this, _1, _2), 256) { // for POSTS, must use 'Content-Type: application/json' in header
    server->on("/api", HTTP_GET, std::bind(&WebAPIService::webAPIService_get, this, _1));     // for GETS
    server->addHandler(&_apiHandler);
}

// GET /{device}
// GET /{device}/{name}
// GET /device={device}?cmd={name}?data={value}[?id={hc}]
void WebAPIService::webAPIService_get(AsyncWebServerRequest * request) {
    // has no body JSON so create dummy as empty object
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
    JsonObject                                 input = input_doc.to<JsonObject>();
    parse(request, input);
}

// For POSTS with an optional JSON body
// HTTP_POST | HTTP_PUT | HTTP_PATCH
// POST /{device}[/{hc|id}][/{name}]
void WebAPIService::webAPIService_post(AsyncWebServerRequest * request, JsonVariant & json) {
    // if no body then treat it as a secure GET
    if (not json.is<JsonObject>()) {
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
    auto method        = request->method();
    bool authenticated = false;

    if (method == HTTP_GET) {
        // special case if there is no command, then default to 'info'
        if (!input.size()) {
            input["cmd"] = "info";
        }
    } else {
        // if its a POST then check authentication
        EMSESP::webSettingsService.read([&](WebSettings & settings) {
            Authentication authentication = _securityManager->authenticateRequest(request);
            authenticated                 = settings.notoken_api | AuthenticationPredicates::IS_ADMIN(authentication);
        });
    }

    // output json buffer
    PrettyAsyncJsonResponse * response = new PrettyAsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE_DYN);
    JsonObject                output   = response->getRoot();

    // call command
    uint8_t command_ret = Command::process(request->url().c_str(), authenticated, input, output);

    // handle response codes
    // the output will be populated with a message key if an error occurred
    int ret_code;
    if (command_ret == CommandRet::NOT_ALLOWED) {
        ret_code = 401; // Unauthorized
    } else if (command_ret == CommandRet::NOT_FOUND) {
        ret_code = 400; // Bad request
    } else if (command_ret == CommandRet::OK) {
        ret_code = 200; //OK
        if (output.isNull()) {
            output["message"] = "OK"; // only add if there is no json output already
        }
    } else {
        ret_code = 400; // Bad request
    }

    // send the json that came back from the command call
    response->setCode(ret_code);
    response->setLength();
    response->setContentType("application/json");
    request->send(response); // send json response

#if defined(EMSESP_STANDALONE)
    Serial.print(COLOR_YELLOW);
    Serial.print("return code: ");
    Serial.println(ret_code);
    if (output.size() != 0) {
        serializeJsonPretty(output, Serial);
    }
    Serial.println();
    Serial.print(COLOR_RESET);
#endif
}

} // namespace emsesp
