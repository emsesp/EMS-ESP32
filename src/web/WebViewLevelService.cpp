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
#include "WebViewLevelService.h"

namespace emsesp {

bool WebViewLevelService::validKey(const char * key) {
    if (key == nullptr) {
        return false;
    }
    const size_t len = strlen(key);
    if (len == 0 || len > MAX_KEY_LENGTH) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        const unsigned char c = key[i];
        if (c < 0x20 || c > 0x7e || c == '"' || c == '\\') {
            return false;
        }
    }
    return true;
}

WebViewLevelService::WebViewLevelService(AsyncWebServer * server, SecurityManager * securityManager) {
    // everybody who is signed in sees the same levels
    securityManager->addEndpoint(server, EMSESP_VIEWLEVELS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        read(request);
    });
    // only an administrator changes them
    securityManager->addEndpoint(
        server,
        EMSESP_VIEWLEVELS_SERVICE_PATH,
        AuthenticationPredicates::IS_ADMIN,
        [this](AsyncWebServerRequest * request, JsonVariant json) { write(request, json); },
        HTTP_POST);
}

// GET /rest/viewLevels
void WebViewLevelService::read(AsyncWebServerRequest * request) {
#ifndef EMSESP_STANDALONE
    if (LittleFS.exists(EMSESP_VIEWLEVELS_FILE)) {
        request->send(LittleFS, EMSESP_VIEWLEVELS_FILE, "application/json");
        return;
    }
#endif
    request->send(200, "application/json", "{}");
}

// POST /rest/viewLevels {"boiler:dhw/mode":1,"boiler:burnstarts":2}
// the whole object replaces the stored one; it is checked first, so the stored file is always valid
void WebViewLevelService::write(AsyncWebServerRequest * request, JsonVariant json) {
    if (!json.is<JsonObject>()) {
        request->send(400);
        return;
    }
    JsonObject levels = json.as<JsonObject>();
    if (levels.size() > MAX_ENTRIES) {
        request->send(413);
        return;
    }
    for (JsonPair kv : levels) {
        const int level = kv.value() | 0;
        if (!validKey(kv.key().c_str()) || (level != 1 && level != 2)) {
            EMSESP::logger().err("View levels: invalid entry");
            request->send(400);
            return;
        }
    }

#ifndef EMSESP_STANDALONE
    const char * temp = EMSESP_VIEWLEVELS_FILE ".tmp";
    File         file = LittleFS.open(temp, "w");
    if (!file) {
        request->send(500);
        return;
    }
    const size_t written = serializeJson(levels, file);
    file.close();
    if (written == 0 && !levels.isNull() && levels.size() > 0) {
        LittleFS.remove(temp);
        request->send(500);
        return;
    }
    LittleFS.remove(EMSESP_VIEWLEVELS_FILE); // rename doesn't replace an existing file
    if (!LittleFS.rename(temp, EMSESP_VIEWLEVELS_FILE)) {
        LittleFS.remove(temp);
        EMSESP::logger().err("Failed to store the view levels");
        request->send(500);
        return;
    }
    request->send(200);
#else
    request->send(404);
#endif
}

} // namespace emsesp
