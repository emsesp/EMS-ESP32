#include "NTPStatus.h"

#include <emsesp.h>

#include <array>

NTPStatus::NTPStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    securityManager->addEndpoint(server, NTP_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        ntpStatus(request);
    });
}

/*
 * Formats the time using the format provided.
 *
 * Uses a 25 byte buffer, large enough to fit an ISO time string with offset.
 */
String formatTime(tm * time, const char * format) {
    std::array<char, 25> time_string{};
    strftime(time_string.data(), time_string.size(), format, time);
    return {time_string.data()};
}

String toUTCTimeString(tm * time) {
    return formatTime(time, "%FT%TZ");
}

String toLocalTimeString(tm * time) {
    return formatTime(time, "%FT%T");
}

void NTPStatus::ntpStatus(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    // grab the current instant in unix seconds
    time_t now = time(nullptr);

    // only provide enabled/disabled status for now
    root["status"] = [] {
        if (esp_sntp_enabled()) {
            if (emsesp::EMSESP::system_.ntp_connected()) {
                return 2;
            } else {
                return 1;
            }
        }
        return 0;
    }();

    // the current time in UTC
    root["utc_time"] = toUTCTimeString(gmtime(&now));

    // local time with offset
    root["local_time"] = toLocalTimeString(localtime(&now));

    // the sntp server name
    root["server"] = esp_sntp_getservername(0);

    response->setLength();
    request->send(response);
}