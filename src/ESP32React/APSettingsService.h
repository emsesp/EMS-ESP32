#ifndef APSettingsService_h
#define APSettingsService_h

#include "HttpEndpoint.h"
#include "FSPersistence.h"
#include "JsonUtils.h"

#ifndef FACTORY_AP_PROVISION_MODE
#define FACTORY_AP_PROVISION_MODE AP_MODE_DISCONNECTED
#endif

#ifndef FACTORY_AP_SSID
#define FACTORY_AP_SSID "ems-esp"
#endif

#ifndef FACTORY_AP_PASSWORD
#define FACTORY_AP_PASSWORD "ems-esp-neo"
#endif

#ifndef FACTORY_AP_LOCAL_IP
#define FACTORY_AP_LOCAL_IP "192.168.4.1"
#endif

#ifndef FACTORY_AP_GATEWAY_IP
#define FACTORY_AP_GATEWAY_IP "192.168.4.1"
#endif

#ifndef FACTORY_AP_SUBNET_MASK
#define FACTORY_AP_SUBNET_MASK "255.255.255.0"
#endif

#ifndef FACTORY_AP_CHANNEL
#define FACTORY_AP_CHANNEL 1
#endif

#ifndef FACTORY_AP_SSID_HIDDEN
#define FACTORY_AP_SSID_HIDDEN false
#endif

#ifndef FACTORY_AP_MAX_CLIENTS
#define FACTORY_AP_MAX_CLIENTS 4
#endif

#define AP_SETTINGS_FILE "/config/apSettings.json"
#define AP_SETTINGS_SERVICE_PATH "/rest/apSettings"

#define AP_MODE_DISCONNECTED 1
#define AP_MODE_NEVER 2

enum APNetworkStatus { ACTIVE = 0, INACTIVE };

class APSettings {
  public:
    uint8_t provisionMode = FACTORY_AP_PROVISION_MODE; // 1 = on disconnect, 2 = off
    String  ssid          = FACTORY_AP_SSID;
    String  password      = FACTORY_AP_PASSWORD;
    uint8_t channel       = FACTORY_AP_CHANNEL;
    bool    ssidHidden    = FACTORY_AP_SSID_HIDDEN;
    uint8_t maxClients    = FACTORY_AP_MAX_CLIENTS;

    IPAddress localIP;
    IPAddress gatewayIP;
    IPAddress subnetMask;

    bool operator==(const APSettings & settings) const {
        return provisionMode == settings.provisionMode && channel == settings.channel && ssidHidden == settings.ssidHidden && maxClients == settings.maxClients
               && localIP == settings.localIP && gatewayIP == settings.gatewayIP && subnetMask == settings.subnetMask && ssid == settings.ssid
               && password == settings.password;
    }

    static void              read(const APSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, APSettings & settings);
};

class APSettingsService : public StatefulService<APSettings> {
  public:
    APSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void            begin();
    APNetworkStatus getAPNetworkStatus();

  private:
    HttpEndpoint<APSettings>  _httpEndpoint;
    FSPersistence<APSettings> _fsPersistence;
};

#endif
