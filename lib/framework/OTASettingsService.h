#ifndef OTASettingsService_h
#define OTASettingsService_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#ifdef ESP32
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266mDNS.h>
#endif

#include <ArduinoOTA.h>
#include <WiFiUdp.h>

#ifndef FACTORY_OTA_PORT
#define FACTORY_OTA_PORT 8266
#endif

#ifndef FACTORY_OTA_PASSWORD
#define FACTORY_OTA_PASSWORD "esp-react"
#endif

#ifndef FACTORY_OTA_ENABLED
#define FACTORY_OTA_ENABLED true
#endif

#define OTA_SETTINGS_FILE "/config/otaSettings.json"
#define OTA_SETTINGS_SERVICE_PATH "/rest/otaSettings"

class OTASettings {
 public:
  bool enabled;
  int port;
  String password;

  static void read(OTASettings& settings, JsonObject& root) {
    root["enabled"] = settings.enabled;
    root["port"] = settings.port;
    root["password"] = settings.password;
  }

  static StateUpdateResult update(JsonObject& root, OTASettings& settings) {
    settings.enabled = root["enabled"] | FACTORY_OTA_ENABLED;
    settings.port = root["port"] | FACTORY_OTA_PORT;
    settings.password = root["password"] | FACTORY_OTA_PASSWORD;
    return StateUpdateResult::CHANGED;
  }
};

class OTASettingsService : public StatefulService<OTASettings> {
 public:
  OTASettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

  void begin();
  void loop();

 private:
  HttpEndpoint<OTASettings> _httpEndpoint;
  FSPersistence<OTASettings> _fsPersistence;
  ArduinoOTAClass* _arduinoOTA;

  void configureArduinoOTA();
#ifdef ESP32
  void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ESP8266)
  WiFiEventHandler _onStationModeGotIPHandler;
  void onStationModeGotIP(const WiFiEventStationModeGotIP& event);
#endif
};

#endif  // end OTASettingsService_h
