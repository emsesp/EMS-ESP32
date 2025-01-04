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

WebActivityService::WebActivityService(AsyncWebServer * server, SecurityManager * securityManager) {
    securityManager->addEndpoint(server, EMSESP_ACTIVITY_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        webActivityService(request);
    });
}

void WebActivityService::webActivityService(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    JsonArray  statsJson = root["stats"].to<JsonArray>();
    JsonObject statJson;

    statJson       = statsJson.add<JsonObject>();
    statJson["id"] = 0;
    statJson["s"]  = EMSESP::rxservice_.telegram_count();
    statJson["f"]  = EMSESP::rxservice_.telegram_error_count();
    statJson["q"]  = EMSESP::rxservice_.quality();

    statJson       = statsJson.add<JsonObject>();
    statJson["id"] = 1;
    statJson["s"]  = EMSESP::txservice_.telegram_read_count();
    statJson["f"]  = EMSESP::txservice_.telegram_read_fail_count();
    statJson["q"]  = EMSESP::txservice_.read_quality();

    statJson       = statsJson.add<JsonObject>();
    statJson["id"] = 2;
    statJson["s"]  = EMSESP::txservice_.telegram_write_count();
    statJson["f"]  = EMSESP::txservice_.telegram_write_fail_count();
    statJson["q"]  = EMSESP::txservice_.write_quality();

    if (EMSESP::sensor_enabled()) {
        statJson       = statsJson.add<JsonObject>();
        statJson["id"] = 3;
        statJson["s"]  = EMSESP::temperaturesensor_.reads() - EMSESP::temperaturesensor_.fails();
        statJson["f"]  = EMSESP::temperaturesensor_.fails();
        statJson["q"] =
            EMSESP::temperaturesensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::temperaturesensor_.fails()) / EMSESP::temperaturesensor_.reads());
    }
    if (EMSESP::analog_enabled()) {
        statJson       = statsJson.add<JsonObject>();
        statJson["id"] = 4;
        statJson["s"]  = EMSESP::analogsensor_.reads() - EMSESP::analogsensor_.fails();
        statJson["f"]  = EMSESP::analogsensor_.fails();
        statJson["q"]  = EMSESP::analogsensor_.reads() == 0 ? 100 : 100 - (uint8_t)((100 * EMSESP::analogsensor_.fails()) / EMSESP::analogsensor_.reads());
    }
    if (Mqtt::enabled()) {
        statJson       = statsJson.add<JsonObject>();
        statJson["id"] = 5;
        statJson["s"]  = Mqtt::publish_count() - Mqtt::publish_fails();
        statJson["f"]  = Mqtt::publish_fails();
        statJson["q"]  = Mqtt::publish_count() == 0 ? 100 : 100 - (uint8_t)((100 * Mqtt::publish_fails()) / Mqtt::publish_count());
    }

    statJson       = statsJson.add<JsonObject>();
    statJson["id"] = 6;
    statJson["s"]  = WebAPIService::api_count(); // + WebAPIService::api_fails();
    statJson["f"]  = WebAPIService::api_fails();
    statJson["q"]  = (WebAPIService::api_count() + WebAPIService::api_fails()) == 0
                         ? 100
                         : 100 - (uint8_t)((100 * WebAPIService::api_fails()) / (WebAPIService::api_count() + WebAPIService::api_fails()));

#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.syslog_enabled()) {
        statJson       = statsJson.add<JsonObject>();
        statJson["id"] = 7;
        statJson["s"]  = EMSESP::system_.syslog_count();
        statJson["f"]  = EMSESP::system_.syslog_fails();
        statJson["q"]  = (EMSESP::system_.syslog_count() + EMSESP::system_.syslog_fails()) == 0
                             ? 100
                             : 100 - (uint8_t)((100 * EMSESP::system_.syslog_fails()) / (EMSESP::system_.syslog_count() + EMSESP::system_.syslog_fails()));
    }
#endif

    response->setLength();
    request->send(response);
}

} // namespace emsesp