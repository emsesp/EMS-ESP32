#include <NTPSettingsService.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

NTPSettingsService::NTPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NTPSettings::read, NTPSettings::update, this, server, NTP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NTPSettings::read, NTPSettings::update, this, fs, NTP_SETTINGS_FILE)
    , _timeHandler(TIME_PATH, securityManager->wrapCallback(std::bind(&NTPSettingsService::configureTime, this, _1, _2), AuthenticationPredicates::IS_ADMIN)) {
    _timeHandler.setMethod(HTTP_POST);
    _timeHandler.setMaxContentLength(MAX_TIME_SIZE);
    server->addHandler(&_timeHandler);

    WiFi.onEvent(std::bind(&NTPSettingsService::WiFiEvent, this, _1));

    addUpdateHandler([&] { configureNTP(); }, false);
}

void NTPSettingsService::begin() {
    _fsPersistence.readFromFS();
    configureNTP();
}

// handles both WiFI and Ethernet
void NTPSettingsService::WiFiEvent(WiFiEvent_t event) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        if (connected_) {
            emsesp::EMSESP::logger().info("WiFi connection dropped, stopping NTP");
            connected_ = false;
            configureNTP();
        }
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP:
        // emsesp::EMSESP::logger().info("Got IP address, starting NTP synchronization");
        connected_ = true;
        configureNTP();
        break;

    default:
        break;
    }
}

// https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
void NTPSettingsService::configureNTP() {
    emsesp::EMSESP::system_.ntp_connected(false);
    if (connected_ && _state.enabled) {
        emsesp::EMSESP::logger().info("Starting NTP service");
        esp_sntp_set_sync_interval(3600000); // one hour
        esp_sntp_set_time_sync_notification_cb(ntp_received);
        configTzTime(_state.tzFormat.c_str(), _state.server.c_str());
    } else {
        setenv("TZ", _state.tzFormat.c_str(), 1);
        tzset();
        esp_sntp_stop();
    }
}

void NTPSettingsService::configureTime(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        struct tm tm        = {0};
        String    timeLocal = json["local_time"];
        char *    s         = strptime(timeLocal.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
        if (s != nullptr) {
            tm.tm_isdst         = -1; // not set by strptime, tells mktime to determine daylightsaving
            time_t         time = mktime(&tm);
            struct timeval now  = {.tv_sec = time};
            settimeofday(&now, nullptr);
            AsyncWebServerResponse * response = request->beginResponse(200);
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(400);
    request->send(response);
}

void NTPSettingsService::ntp_received(struct timeval * tv) {
    // emsesp::EMSESP::logger().info("NTP sync to %d sec", tv->tv_sec);
    emsesp::EMSESP::system_.ntp_connected(true);
}

void NTPSettings::read(NTPSettings & settings, JsonObject root) {
    root["enabled"]   = settings.enabled;
    root["server"]    = settings.server;
    root["tz_label"]  = settings.tzLabel;
    root["tz_format"] = settings.tzFormat;
}

StateUpdateResult NTPSettings::update(JsonObject root, NTPSettings & settings) {
    settings.enabled  = root["enabled"] | FACTORY_NTP_ENABLED;
    settings.server   = root["server"] | FACTORY_NTP_SERVER;
    settings.tzLabel  = root["tz_label"] | FACTORY_NTP_TIME_ZONE_LABEL;
    settings.tzFormat = root["tz_format"] | FACTORY_NTP_TIME_ZONE_FORMAT;
    return StateUpdateResult::CHANGED;
}
