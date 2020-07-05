#include <NTPStatus.h>

NTPStatus::NTPStatus(AsyncWebServer* server, SecurityManager* securityManager) {
  server->on(NTP_STATUS_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&NTPStatus::ntpStatus, this, std::placeholders::_1),
                                          AuthenticationPredicates::IS_AUTHENTICATED));
}

String toISOString(tm* time, bool incOffset) {
  char time_string[25];
  strftime(time_string, 25, incOffset ? "%FT%T%z" : "%FT%TZ", time);
  return String(time_string);
}

void NTPStatus::ntpStatus(AsyncWebServerRequest* request) {
  AsyncJsonResponse* response = new AsyncJsonResponse(false, MAX_NTP_STATUS_SIZE);
  JsonObject root = response->getRoot();

  // grab the current instant in unix seconds
  time_t now = time(nullptr);

  // only provide enabled/disabled status for now
  root["status"] = sntp_enabled() ? 1 : 0;

  // the current time in UTC
  root["time_utc"] = toISOString(gmtime(&now), false);

  // local time as ISO String with TZ
  root["time_local"] = toISOString(localtime(&now), true);

  // the sntp server name
  root["server"] = sntp_getservername(0);

  // device uptime in seconds
  root["uptime"] = millis() / 1000;

  response->setLength();
  request->send(response);
}
