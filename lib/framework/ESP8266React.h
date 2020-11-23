#ifndef ESP8266React_h
#define ESP8266React_h

#include <Arduino.h>

#ifdef ESP32
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <FeaturesService.h>
#include <APSettingsService.h>
#include <APStatus.h>
#include <AuthenticationService.h>
#include <FactoryResetService.h>
#include <MqttSettingsService.h>
#include <MqttStatus.h>
#include <NTPSettingsService.h>
#include <NTPStatus.h>
#include <OTASettingsService.h>
#include <UploadFirmwareService.h>
#include <RestartService.h>
#include <SecuritySettingsService.h>
#include <SystemStatus.h>
#include <WiFiScanner.h>
#include <WiFiSettingsService.h>
#include <WiFiStatus.h>

#ifdef PROGMEM_WWW
#include <WWWData.h>
#endif

class ESP8266React {
  public:
    ESP8266React(AsyncWebServer * server, FS * fs);

    void begin();
    void loop();

    SecurityManager * getSecurityManager() {
        return &_securitySettingsService;
    }

#if FT_ENABLED(FT_SECURITY)
    StatefulService<SecuritySettings> * getSecuritySettingsService() {
        return &_securitySettingsService;
    }
#endif

    StatefulService<WiFiSettings> * getWiFiSettingsService() {
        return &_wifiSettingsService;
    }

    StatefulService<APSettings> * getAPSettingsService() {
        return &_apSettingsService;
    }

#if FT_ENABLED(FT_NTP)
    StatefulService<NTPSettings> * getNTPSettingsService() {
        return &_ntpSettingsService;
    }
#endif

#if FT_ENABLED(FT_OTA)
    StatefulService<OTASettings> * getOTASettingsService() {
        return &_otaSettingsService;
    }
#endif

#if FT_ENABLED(FT_MQTT)
    StatefulService<MqttSettings> * getMqttSettingsService() {
        return &_mqttSettingsService;
    }

    AsyncMqttClient * getMqttClient() {
        return _mqttSettingsService.getMqttClient();
    }
#endif

    void factoryReset() {
        _factoryResetService.factoryReset();
    }

  private:
    FeaturesService         _featureService;
    SecuritySettingsService _securitySettingsService;
    WiFiSettingsService     _wifiSettingsService;
    WiFiScanner             _wifiScanner;
    WiFiStatus              _wifiStatus;
    APSettingsService       _apSettingsService;
    APStatus                _apStatus;
#if FT_ENABLED(FT_NTP)
    NTPSettingsService _ntpSettingsService;
    NTPStatus          _ntpStatus;
#endif
#if FT_ENABLED(FT_OTA)
    OTASettingsService _otaSettingsService;
#endif
#if FT_ENABLED(FT_UPLOAD_FIRMWARE)
    UploadFirmwareService _uploadFirmwareService;
#endif
#if FT_ENABLED(FT_MQTT)
    MqttSettingsService _mqttSettingsService;
    MqttStatus          _mqttStatus;
#endif
#if FT_ENABLED(FT_SECURITY)
    AuthenticationService _authenticationService;
#endif
    RestartService      _restartService;
    FactoryResetService _factoryResetService;
    SystemStatus        _systemStatus;
};

#endif
