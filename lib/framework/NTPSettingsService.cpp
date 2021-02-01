#include <NTPSettingsService.h>

NTPSettingsService::NTPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NTPSettings::read, NTPSettings::update, this, server, NTP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NTPSettings::read, NTPSettings::update, this, fs, NTP_SETTINGS_FILE)
    , _timeHandler(TIME_PATH, securityManager->wrapCallback(std::bind(&NTPSettingsService::configureTime, this, std::placeholders::_1, std::placeholders::_2), AuthenticationPredicates::IS_ADMIN)) {
    _timeHandler.setMethod(HTTP_POST);
    _timeHandler.setMaxContentLength(MAX_TIME_SIZE);
    server->addHandler(&_timeHandler);

    WiFi.onEvent(std::bind(&NTPSettingsService::WiFiEvent, this, std::placeholders::_1));

    addUpdateHandler([&](const String & originId) { configureNTP(); }, false);
}

void NTPSettingsService::begin() {
    _fsPersistence.readFromFS();
    configureNTP();
}

// handles both WiFI and Ethernet
void NTPSettingsService::WiFiEvent(WiFiEvent_t event) {
    switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
        // Serial.println(F("WiFi connection dropped, stopping NTP."));
        connected_ = false;
        configureNTP();
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
    case SYSTEM_EVENT_ETH_GOT_IP:
        // Serial.println(F("Got IP address, starting NTP Synchronization"));
        connected_ = true;
        configureNTP();
        break;

    default:
        break;
    }
}

void NTPSettingsService::configureNTP() {
    if (connected_ && _state.enabled) {
        // Serial.println(F("Starting NTP..."));
        configTzTime(_state.tzFormat.c_str(), _state.server.c_str());
    } else {
        setenv("TZ", _state.tzFormat.c_str(), 1);
        tzset();
        sntp_stop();
    }
}

void NTPSettingsService::configureTime(AsyncWebServerRequest * request, JsonVariant & json) {
    if (!sntp_enabled() && json.is<JsonObject>()) {
        struct tm tm        = {0};
        String    timeLocal = json["local_time"];
        char *    s         = strptime(timeLocal.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
        if (s != nullptr) {
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
