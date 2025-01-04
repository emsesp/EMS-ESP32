#ifndef ESP32React_h
#define ESP32React_h

#include "APSettingsService.h"
#include "APStatus.h"
#include "AuthenticationService.h"
#include "MqttSettingsService.h"
#include "MqttStatus.h"
#include "NTPSettingsService.h"
#include "NTPStatus.h"
#include "UploadFileService.h"
#include "SecuritySettingsService.h"
#include "WiFiScanner.h"
#include "NetworkSettingsService.h"
#include "NetworkStatus.h"

#include <Arduino.h>
#include <AsyncJson.h>
#include <AsyncMessagePack.h>
#include <AsyncTCP.h>
#include <WiFi.h>

class ESP32React {
  public:
    ESP32React(AsyncWebServer * server, FS * fs);

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

    StatefulService<MqttSettings> * getMqttSettingsService() {
        return &_mqttSettingsService;
    }

    MqttClient * getMqttClient() {
        return _mqttSettingsService.getMqttClient();
    }

    //
    // special functions needed outside scope
    //

    // true if AP is active
    bool apStatus() {
        return _apSettingsService.getAPNetworkStatus() == APNetworkStatus::ACTIVE;
    }

    uint16_t getWifiReconnects() {
        return _networkSettingsService.getWifiReconnects();
    }

  private:
    SecuritySettingsService _securitySettingsService;
    NetworkSettingsService  _networkSettingsService;
    WiFiScanner             _wifiScanner;
    NetworkStatus           _networkStatus;
    APSettingsService       _apSettingsService;
    APStatus                _apStatus;
    NTPSettingsService      _ntpSettingsService;
    NTPStatus               _ntpStatus;
    UploadFileService       _uploadFileService;
    MqttSettingsService     _mqttSettingsService;
    MqttStatus              _mqttStatus;
    AuthenticationService   _authenticationService;
};

#endif
