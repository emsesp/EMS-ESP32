#include <OTASettingsService.h>

OTASettingsService::OTASettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager) :
    _httpEndpoint(OTASettings::read, OTASettings::update, this, server, OTA_SETTINGS_SERVICE_PATH, securityManager),
    _fsPersistence(OTASettings::read, OTASettings::update, this, fs, OTA_SETTINGS_FILE),
    _arduinoOTA(nullptr) {
#ifdef ESP32
  WiFi.onEvent(std::bind(&OTASettingsService::onStationModeGotIP, this, std::placeholders::_1, std::placeholders::_2),
               WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
  _onStationModeGotIPHandler =
      WiFi.onStationModeGotIP(std::bind(&OTASettingsService::onStationModeGotIP, this, std::placeholders::_1));
#endif
  addUpdateHandler([&](const String& originId) { configureArduinoOTA(); }, false);
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
#ifdef ESP32
    _arduinoOTA->end();
#endif
    delete _arduinoOTA;
    _arduinoOTA = nullptr;
  }
  if (_state.enabled) {
    Serial.println(F("Starting OTA Update Service..."));
    _arduinoOTA = new ArduinoOTAClass;
    _arduinoOTA->setPort(_state.port);
    _arduinoOTA->setPassword(_state.password.c_str());
    _arduinoOTA->onStart([]() { Serial.println(F("Starting")); });
    _arduinoOTA->onEnd([]() { Serial.println(F("\r\nEnd")); });
    _arduinoOTA->onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf_P(PSTR("Progress: %u%%\r\n"), (progress / (total / 100)));
    });
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
    _arduinoOTA->begin();
  }
}

#ifdef ESP32
void OTASettingsService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  configureArduinoOTA();
}
#elif defined(ESP8266)
void OTASettingsService::onStationModeGotIP(const WiFiEventStationModeGotIP& event) {
  configureArduinoOTA();
}
#endif
