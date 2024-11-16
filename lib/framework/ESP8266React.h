#ifndef ESP8266React_h
#define ESP8266React_h

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
#include <AsyncTCP.h>
#include <WiFi.h>

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

    StatefulService<MqttSettings> * getMqttSettingsService() {
        return &_mqttSettingsService;
    }

    MqttClient * getMqttClient() {
        return _mqttSettingsService.getMqttClient();
    }

    //
    // special functions needed outside scope
    //

    void setWill(const char * will_topic) {
        _mqttSettingsService.setWill(will_topic);
    }

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
