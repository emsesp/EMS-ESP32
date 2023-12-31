#include <NTPStatus.h>
#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

NTPStatus::NTPStatus(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}


void NTPStatus::registerURI() {
    _server->on(NTP_STATUS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&NTPStatus::ntpStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

/*
 * Formats the time using the format provided.
 *
 * Uses a 25 byte buffer, large enough to fit an ISO time string with offset.
 */
String formatTime(tm * time, const char * format) {
    char time_string[25];
    strftime(time_string, 25, format, time);
    return String(time_string);
}

String toUTCTimeString(tm * time) {
    return formatTime(time, "%FT%TZ");
}

String toLocalTimeString(tm * time) {
    return formatTime(time, "%FT%T");
}

esp_err_t NTPStatus::ntpStatus(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, MAX_NTP_STATUS_SIZE);
    JsonObject          root     = response.getRoot();

    // grab the current instant in unix seconds
    time_t now = time(nullptr);

    // only provide enabled/disabled status for now
    root["status"] = esp_sntp_enabled() ? emsesp::EMSESP::system_.ntp_connected() ? 2 : 1 : 0;

    // the current time in UTC
    root["utc_time"] = toUTCTimeString(gmtime(&now));

    // local time with offset
    root["local_time"] = toLocalTimeString(localtime(&now));

    // the sntp server name
    root["server"] = esp_sntp_getservername(0);

    return response.send();
}
