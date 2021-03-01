#ifndef ESP8266React_h
#define ESP8266React_h

#include <Arduino.h>

#include <AsyncTCP.h>
#include <WiFi.h>

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
#include <NetworkSettingsService.h>
#include <NetworkStatus.h>

#include <WWWData.h>

class ESP8266React {
  public:
    ESP8266React(AsyncWebServer * server, FS * fs);

    void begin();
    void loop();

    SecurityManager * getSecurityManager() {
        return &_securitySettingsService;
    }

    StatefulService<SecuritySettings> * getSecuritySettingsService() {
        return &_securitySettingsService;
    }

    StatefulService<NetworkSettings> * getNetworkSettingsService() {
        return &_networkSettingsService;
    }

    StatefulService<APSettings> * getAPSettingsService() {
        return &_apSettingsService;
    }

    StatefulService<NTPSettings> * getNTPSettingsService() {
        return &_ntpSettingsService;
    }

    StatefulService<OTASettings> * getOTASettingsService() {
        return &_otaSettingsService;
    }

    StatefulService<MqttSettings> * getMqttSettingsService() {
        return &_mqttSettingsService;
    }

    AsyncMqttClient * getMqttClient() {
        return _mqttSettingsService.getMqttClient();
    }

    void factoryReset() {
        _factoryResetService.factoryReset();
    }

  private:
    FeaturesService         _featureService;
    SecuritySettingsService _securitySettingsService;
    NetworkSettingsService  _networkSettingsService;
    WiFiScanner             _wifiScanner;
    NetworkStatus           _networkStatus;
    APSettingsService       _apSettingsService;
    APStatus                _apStatus;
    NTPSettingsService      _ntpSettingsService;
    NTPStatus               _ntpStatus;
    OTASettingsService      _otaSettingsService;
    UploadFirmwareService   _uploadFirmwareService;
    MqttSettingsService     _mqttSettingsService;
    MqttStatus              _mqttStatus;
    AuthenticationService   _authenticationService;
    RestartService          _restartService;
    FactoryResetService     _factoryResetService;
    SystemStatus            _systemStatus;
};

#endif
