#include <NTPSettingsService.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

NTPSettingsService::NTPSettingsService(PsychicHttpServer * server, FS * fs, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager)
    , _httpEndpoint(NTPSettings::read, NTPSettings::update, this, server, NTP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NTPSettings::read, NTPSettings::update, this, fs, NTP_SETTINGS_FILE) {
    WiFi.onEvent(std::bind(&NTPSettingsService::WiFiEvent, this, _1));
    addUpdateHandler([&](const String & originId) { configureNTP(); }, false);
}

void NTPSettingsService::begin() {
    _fsPersistence.readFromFS();
    configureNTP();
}

void NTPSettingsService::registerURI() {
    _httpEndpoint.registerURI();

    _server->on(TIME_PATH,
                HTTP_POST,
                _securityManager->wrapCallback(std::bind(&NTPSettingsService::configureTime, this, _1, _2), AuthenticationPredicates::IS_ADMIN));
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

esp_err_t NTPSettingsService::configureTime(PsychicRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        struct tm tm        = {0};
        String    timeLocal = json["local_time"];
        char *    s         = strptime(timeLocal.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
        if (s != nullptr) {
            tm.tm_isdst         = -1; // not set by strptime, tells mktime to determine daylightsaving
            time_t         time = mktime(&tm);
            struct timeval now  = {.tv_sec = time};
            settimeofday(&now, nullptr);
            return request->reply(200);
        }
    }

    return request->reply(400);
}

void NTPSettingsService::ntp_received(struct timeval * tv) {
    // emsesp::EMSESP::logger().info("NTP sync to %d sec", tv->tv_sec);
    emsesp::EMSESP::system_.ntp_connected(true);
}
