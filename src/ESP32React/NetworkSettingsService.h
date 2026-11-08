#ifndef NetworkSettingsService_h
#define NetworkSettingsService_h

#include "StatefulService.h"
#include "FSPersistence.h"
#include "HttpEndpoint.h"
#include "JsonUtils.h"

#ifndef EMSESP_STANDALONE
#include <esp_wifi.h>
#endif

#define NETWORK_SETTINGS_FILE "/config/networkSettings.json"
#define NETWORK_SETTINGS_SERVICE_PATH "/rest/networkSettings"

#ifndef FACTORY_WIFI_SSID
#define FACTORY_WIFI_SSID ""
#endif

#ifndef FACTORY_WIFI_PASSWORD
#define FACTORY_WIFI_PASSWORD ""
#endif

#ifndef FACTORY_NETWORK_HOSTNAME
#define FACTORY_NETWORK_HOSTNAME ""
#endif

class NetworkSettings {
  public:
    // core wifi configuration
    String  ssid           = FACTORY_WIFI_SSID;
    String  bssid          = "";
    String  password       = FACTORY_WIFI_PASSWORD;
    String  hostname       = FACTORY_NETWORK_HOSTNAME;
    bool    staticIPConfig = false;
    bool    bandwidth20    = false;
    uint8_t tx_power       = 0;
    bool    nosleep        = true;
    bool    enableMDNS     = true;
    bool    enableCORS     = false;
    String  CORSOrigin     = "*";
    bool    eth_10mbit     = false; // force 10BASE-T half-duplex, no auto-negotiation

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

  private:
    HttpEndpoint<NetworkSettings>  _httpEndpoint;
    FSPersistence<NetworkSettings> _fsPersistence;
};

#endif
