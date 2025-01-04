#ifndef APSettingsConfig_h
#define APSettingsConfig_h

#include "HttpEndpoint.h"
#include "FSPersistence.h"
#include "JsonUtils.h"

#include <DNSServer.h>
#include <IPAddress.h>

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

#define AP_MODE_ALWAYS 0
#define AP_MODE_DISCONNECTED 1
#define AP_MODE_NEVER 2

#define MANAGE_NETWORK_DELAY 10000
#define DNS_PORT 53

enum APNetworkStatus { ACTIVE = 0, INACTIVE, LINGERING };

class APSettings {
  public:
    uint8_t provisionMode;
    String  ssid;
    String  password;
    uint8_t channel;
    bool    ssidHidden;
    uint8_t maxClients;

    IPAddress localIP;
    IPAddress gatewayIP;
    IPAddress subnetMask;

    bool operator==(const APSettings & settings) const {
        return provisionMode == settings.provisionMode && ssid == settings.ssid && password == settings.password && channel == settings.channel
               && ssidHidden == settings.ssidHidden && maxClients == settings.maxClients && localIP == settings.localIP && gatewayIP == settings.gatewayIP
               && subnetMask == settings.subnetMask;
    }

    static void              read(const APSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, APSettings & settings);
};

class APSettingsService : public StatefulService<APSettings> {
  public:
    APSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void            begin();
    void            loop();
    APNetworkStatus getAPNetworkStatus();

  private:
    HttpEndpoint<APSettings>  _httpEndpoint;
    FSPersistence<APSettings> _fsPersistence;

    // for the captive portal
    DNSServer * _dnsServer;

    // for the management delay loop
    volatile unsigned long _lastManaged;
    volatile boolean       _reconfigureAp;
    uint8_t                _connected;

    void reconfigureAP();
    void manageAP();
    void startAP();
    void stopAP();
    void handleDNS();
    void WiFiEvent(WiFiEvent_t event);
};

#endif
