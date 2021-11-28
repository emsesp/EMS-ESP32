#include <OTASettingsService.h>

#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders; // for `_1` etc

OTASettingsService::OTASettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(OTASettings::read, OTASettings::update, this, server, OTA_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(OTASettings::read, OTASettings::update, this, fs, OTA_SETTINGS_FILE)
    , _arduinoOTA(nullptr) {
    WiFi.onEvent(std::bind(&OTASettingsService::WiFiEvent, this, _1, _2));
    addUpdateHandler([&](const String & originId) { configureArduinoOTA(); }, false);
}

void OTASettingsService::begin() {
    _fsPersistence.readFromFS();
    configureArduinoOTA();
}

void OTASettingsService::loop() {
    if (_state.enabled && _arduinoOTA) {
        _arduinoOTA->handle();
    }
}

void OTASettingsService::configureArduinoOTA() {
    if (_arduinoOTA) {
        _arduinoOTA->end();
        delete _arduinoOTA;
        _arduinoOTA = nullptr;
    }

    if (_state.enabled) {
        _arduinoOTA = new ArduinoOTAClass;
        _arduinoOTA->setPort(_state.port);
        _arduinoOTA->setPassword(_state.password.c_str());

        _arduinoOTA->onStart([]() {
            Serial.println(F("Starting"));
            emsesp::EMSESP::system_.upload_status(true);
        });
        _arduinoOTA->onEnd([]() {
            Serial.println(F("\r\nEnd"));
            emsesp::EMSESP::system_.upload_status(false);
        });

        _arduinoOTA->onProgress([](unsigned int progress, unsigned int total) { Serial.printf_P(PSTR("Progress: %u%%\r\n"), (progress / (total / 100))); });
        _arduinoOTA->onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                Serial.println(F("Auth Failed"));
            else if (error == OTA_BEGIN_ERROR)
                Serial.println(F("Begin Failed"));
            else if (error == OTA_CONNECT_ERROR)
                Serial.println(F("Connect Failed"));
            else if (error == OTA_RECEIVE_ERROR)
                Serial.println(F("Receive Failed"));
            else if (error == OTA_END_ERROR)
                Serial.println(F("End Failed"));
        });

        _arduinoOTA->setMdnsEnabled(false); // disable as handled in NetworkSettingsService.cpp. https://github.com/emsesp/EMS-ESP32/issues/161
        _arduinoOTA->begin();
    }
}

void OTASettingsService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
    case SYSTEM_EVENT_ETH_GOT_IP:
        configureArduinoOTA();
        break;
    default:
        break;
    }
}
