#ifndef NetworkSettingsService_h
#define NetworkSettingsService_h

#include "StatefulService.h"
#include "FSPersistence.h"
#include "HttpEndpoint.h"
#include "JsonUtils.h"

#ifndef EMSESP_STANDALONE
#include <esp_wifi.h>
#include <ETH.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#endif

#define NETWORK_SETTINGS_FILE "/config/networkSettings.json"
#define NETWORK_SETTINGS_SERVICE_PATH "/rest/networkSettings"
#define WIFI_RECONNECTION_DELAY (1000 * 3)

#ifndef FACTORY_WIFI_SSID
#define FACTORY_WIFI_SSID ""
#endif

#ifndef FACTORY_WIFI_PASSWORD
#define FACTORY_WIFI_PASSWORD ""
#endif

#ifndef FACTORY_WIFI_HOSTNAME
#define FACTORY_WIFI_HOSTNAME ""
#endif

// copied from Tasmota
#if CONFIG_IDF_TARGET_ESP32S2
#define MAX_TX_PWR_DBM_11b 195
#define MAX_TX_PWR_DBM_54g 150
#define MAX_TX_PWR_DBM_n 130
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -750
#define WIFI_SENSITIVITY_n -720
#elif CONFIG_IDF_TARGET_ESP32S3
#define MAX_TX_PWR_DBM_11b 210
#define MAX_TX_PWR_DBM_54g 190
#define MAX_TX_PWR_DBM_n 185
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -760
#define WIFI_SENSITIVITY_n -720
#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
#define MAX_TX_PWR_DBM_11b 210
#define MAX_TX_PWR_DBM_54g 190
#define MAX_TX_PWR_DBM_n 185
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -760
#define WIFI_SENSITIVITY_n -730
#else
#define MAX_TX_PWR_DBM_11b 195
#define MAX_TX_PWR_DBM_54g 160
#define MAX_TX_PWR_DBM_n 140
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -750
#define WIFI_SENSITIVITY_n -700
#endif

class NetworkSettings {
  public:
    // core wifi configuration
    String  ssid;
    String  bssid;
    String  password;
    String  hostname;
    bool    staticIPConfig;
    bool    bandwidth20;
    uint8_t tx_power;
    bool    nosleep;
    bool    enableMDNS;
    bool    enableCORS;
    String  CORSOrigin;

    // optional configuration for static IP address
    IPAddress localIP;
    IPAddress gatewayIP;
    IPAddress subnetMask;
    IPAddress dnsIP1;
    IPAddress dnsIP2;

    static void              read(NetworkSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, NetworkSettings & settings);
};

class NetworkSettingsService : public StatefulService<NetworkSettings> {
  public:
    NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();

    uint16_t getWifiReconnects() const {
        return connectcount_;
    }

  private:
    HttpEndpoint<NetworkSettings>  _httpEndpoint;
    FSPersistence<NetworkSettings> _fsPersistence;

    unsigned long _lastConnectionAttempt;
    bool          _stopping;

    uint16_t connectcount_ = 0; // number of wifi reconnects

    void         WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void         mDNS_start() const;
    const char * disconnectReason(uint8_t code);
    void         reconfigureWiFiConnection();
    void         manageSTA();
    void         setWiFiPowerOnRSSI();
};

#endif
