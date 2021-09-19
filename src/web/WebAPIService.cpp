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
    // initialize parameters. These will be extracted from the URL
    std::string device_s("");
    std::string cmd_s("");
    std::string value_s("");
    int         id = -1;

    parse(request, device_s, cmd_s, id, value_s);
}

// For POSTS with an optional JSON body
// HTTP_POST | HTTP_PUT | HTTP_PATCH
// POST /{device}[/{hc|id}][/{name}]
// the body must have 'value'. Optional are device, name, hc and id
void WebAPIService::webAPIService_post(AsyncWebServerRequest * request, JsonVariant & json) {
    // if no body then treat it as a secure GET
    if (not json.is<JsonObject>()) {
        webAPIService_get(request);
        return;
    }

    // extract values from the json. these will be used as default values
    auto && body = json.as<JsonObject>();

#if defined(EMSESP_STANDALONE)
    Serial.println("webAPIService_post: ");
    serializeJson(body, Serial);
    Serial.println();
#endif

    // make sure we have a value. There must always be a value
    if (!body.containsKey(F_(value))) {
        send_message_response(request, 400, "Problems parsing JSON, missing value"); // Bad Request
        return;
    }

    std::string value_s  = body["value"].as<std::string>(); // always convert value to string
    std::string device_s = body["device"].as<std::string>();

    // get the command. It can be either 'name' or 'cmd'
    std::string cmd_s("");
    if (body.containsKey("name")) {
        cmd_s = body["name"].as<std::string>();
    } else if (body.containsKey("cmd")) {
        cmd_s = body["cmd"].as<std::string>();
    }

    // for id, it can be part of the hc or id keys in the json body
    int id = -1;
    if (body.containsKey("id")) {
        id = body["id"];
    } else if (body.containsKey("hc")) {
        id = body["hc"];
    } else {
        id = -1;
    }

    // now parse the URL. The URL is always leading and will overwrite anything provided in the json body
    parse(request, device_s, cmd_s, id, value_s); // pass it defaults
}

// parse the URL looking for query or path parameters
// reporting back any errors
void WebAPIService::parse(AsyncWebServerRequest * request, std::string & device_s, std::string & cmd_s, int id, std::string & value_s) {
    // parse URL for the path names
    SUrlParser p;

    p.parse(request->url().c_str());

    // remove the /api from the path
    if (p.paths().front() == "api") {
        p.paths().erase(p.paths().begin());
    } else {
        return; // bad URL
    }

    uint8_t device_type;
    int8_t  id_n = -1; // default hc

    // check for query parameters first, the old style from v2
    // /device={device}?cmd={name}?data={value}[?id={hc}
    if (p.paths().size() == 0) {
        // get the device
        if (request->hasParam(F_(device))) {
            device_s = request->getParam(F_(device))->value().c_str();
        }

        // get cmd
        if (request->hasParam(F_(cmd))) {
            cmd_s = request->getParam(F_(cmd))->value().c_str();
        }

        // get data, which is optional. This is now replaced with the name 'value' in JSON body
        if (request->hasParam(F_(data))) {
            value_s = request->getParam(F_(data))->value().c_str();
        }
        if (request->hasParam(F_(value))) {
            value_s = request->getParam(F_(value))->value().c_str();
        }

        // get id (or hc), which is optional
        if (request->hasParam(F_(id))) {
            id_n = Helpers::atoint(request->getParam(F_(id))->value().c_str());
        }
        if (request->hasParam("hc")) {
            id_n = Helpers::atoint(request->getParam("hc")->value().c_str());
        }
    } else {
        // parse paths and json data from the newer OpenAPI standard
        // [/{device}][/{hc}][/{name}]
        // all paths are optional. If not set then take the values from the json body (if available)

        // see if we have a device in the path
        size_t num_paths = p.paths().size();
        if (num_paths) {
            // assume the next path is the 'device'. Note this could also have the value of system.
            device_s = p.paths().front();

            if (num_paths == 2) {
                // next path is the name or cmd
                cmd_s = p.paths()[1];
            } else if (num_paths > 2) {
                // check in Command::find_command makes prefix to TAG
                cmd_s = p.paths()[1] + "/" + p.paths()[2];
            }
        }
    }

    // now go and verify the values

    // device check
    if (device_s.empty()) {
        // see if we have a device embedded in the json body, then use that
        send_message_response(request, 422, "Missing device"); // Unprocessable Entity
        return;
    }
    device_type = EMSdevice::device_name_2_device_type(device_s.c_str());
    if (device_type == EMSdevice::DeviceType::UNKNOWN) {
        send_message_response(request, 422, "Invalid call"); // Unprocessable Entity
        return;
    }

    // check that we have permissions first. We require authenticating on 1 or more of these conditions:
    //  1. any HTTP POSTs or PUTs
    //  2. an HTTP GET which has a 'data' parameter which is not empty (to keep v2 compatibility)
    auto method        = request->method();
    bool have_data     = !value_s.empty();
    bool authenticated = false;
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        authenticated                 = settings.notoken_api | AuthenticationPredicates::IS_ADMIN(authentication);
    });

    if ((method != HTTP_GET) || ((method == HTTP_GET) && have_data)) {
        if (!authenticated) {
            send_message_response(request, 401, "Bad credentials"); // Unauthorized
            return;
        }
    }

    PrettyAsyncJsonResponse * response = new PrettyAsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE_DYN);
    JsonObject                json     = response->getRoot();

    // now we have all the parameters go and execute the command
    // the function will also determine if authentication is needed to execute its command
    uint8_t cmd_reply = Command::call(device_type, cmd_s.c_str(), (have_data ? value_s.c_str() : nullptr), authenticated, id_n, json);

    // check for errors
    if (cmd_reply == CommandRet::NOT_FOUND) {
        delete response;
        send_message_response(request, 400, "Command not found"); // Bad Request
        return;
    } else if (cmd_reply == CommandRet::NOT_ALLOWED) {
        delete response;
        send_message_response(request, 401, "Bad credentials"); // Unauthorized
        return;
    } else if (cmd_reply != CommandRet::OK) {
        delete response;
        send_message_response(request, 400, "Problems parsing elements"); // Bad Request
        return;
    }

    if (!json.size()) {
        delete response;
        send_message_response(request, 200, "OK"); // OK
        return;
    }

    // send the json that came back from the command call
    response->setLength();
    request->send(response); // send json response

#if defined(EMSESP_STANDALONE)
    Serial.print(COLOR_YELLOW);
    if (json.size() != 0) {
        serializeJsonPretty(json, Serial);
    }
    Serial.println();
    Serial.print(COLOR_RESET);
#endif
}

// send a HTTP error back, with optional JSON body data
void WebAPIService::send_message_response(AsyncWebServerRequest * request, uint16_t error_code, const char * error_message) {
    if (error_message == nullptr) {
        AsyncWebServerResponse * response = request->beginResponse(error_code); // just send the code
        request->send(response);
    } else {
        // build a return message and send it
        PrettyAsyncJsonResponse * response = new PrettyAsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
        JsonObject                json     = response->getRoot();
        json["message"]                    = error_message;
        response->setCode(error_code);
        response->setLength();
        response->setContentType("application/json");
        request->send(response);
    }

    EMSESP::logger().debug(F("API returns, code: %d, message: %s"), error_code, error_message);
}

/**
 * Extract only the path component from the passed URI and normalized it.
 * Ex. //one/two////three/// becomes /one/two/three
 */
std::string SUrlParser::path() {
    std::string s = "/"; // set up the beginning slash
    for (std::string & f : m_folders) {
        s += f;
        s += "/";
    }
    s.pop_back(); // deleting last letter, that is slash '/'
    return std::string(s);
}

SUrlParser::SUrlParser(const char * uri) {
    parse(uri);
}

bool SUrlParser::parse(const char * uri) {
    m_folders.clear();
    m_keysvalues.clear();
    enum Type { begin, folder, param, value };
    std::string s;

    const char * c = uri;
    enum Type    t = Type::begin;
    std::string  last_param;

    if (c != NULL || *c != '\0') {
        do {
            if (*c == '/') {
                if (s.length() > 0) {
                    m_folders.push_back(s);
                    s.clear();
                }
                t = Type::folder;
            } else if (*c == '?' && (t == Type::folder || t == Type::begin)) {
                if (s.length() > 0) {
                    m_folders.push_back(s);
                    s.clear();
                }
                t = Type::param;
            } else if (*c == '=' && (t == Type::param || t == Type::begin)) {
                m_keysvalues[s] = "";
                last_param      = s;
                s.clear();
                t = Type::value;
            } else if (*c == '&' && (t == Type::value || t == Type::param || t == Type::begin)) {
                if (t == Type::value) {
                    m_keysvalues[last_param] = s;
                } else if ((t == Type::param || t == Type::begin) && (s.length() > 0)) {
                    m_keysvalues[s] = "";
                    last_param      = s;
                }
                t = Type::param;
                s.clear();
            } else if (*c == '\0' && s.length() > 0) {
                if (t == Type::value) {
                    m_keysvalues[last_param] = s;
                } else if (t == Type::folder || t == Type::begin) {
                    m_folders.push_back(s);
                } else if (t == Type::param) {
                    m_keysvalues[s] = "";
                    last_param      = s;
                }
                s.clear();
            } else if (*c == '\0' && s.length() == 0) {
                if (t == Type::param && last_param.length() > 0) {
                    m_keysvalues[last_param] = "";
                }
                s.clear();
            } else {
                s += *c;
            }
        } while (*c++ != '\0');
    }
    return true;
}

} // namespace emsesp
