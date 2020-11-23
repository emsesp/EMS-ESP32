#ifndef WiFiSettingsService_h
#define WiFiSettingsService_h

#include <StatefulService.h>
#include <FSPersistence.h>
#include <HttpEndpoint.h>
#include <JsonUtils.h>

#define WIFI_SETTINGS_FILE "/config/wifiSettings.json"
#define WIFI_SETTINGS_SERVICE_PATH "/rest/wifiSettings"
#define WIFI_RECONNECTION_DELAY 1000 * 30

#ifndef FACTORY_WIFI_SSID
#define FACTORY_WIFI_SSID ""
#endif

#ifndef FACTORY_WIFI_PASSWORD
#define FACTORY_WIFI_PASSWORD ""
#endif

#ifndef FACTORY_WIFI_HOSTNAME
#define FACTORY_WIFI_HOSTNAME ""
#endif

class WiFiSettings {
  public:
    // core wifi configuration
    String ssid;
    String password;
    String hostname;
    bool   staticIPConfig;

    // optional configuration for static IP address
    IPAddress localIP;
    IPAddress gatewayIP;
    IPAddress subnetMask;
    IPAddress dnsIP1;
    IPAddress dnsIP2;

    static void read(WiFiSettings & settings, JsonObject & root) {
        // connection settings
        root["ssid"]             = settings.ssid;
        root["password"]         = settings.password;
        root["hostname"]         = settings.hostname;
        root["static_ip_config"] = settings.staticIPConfig;

        // extended settings
        JsonUtils::writeIP(root, "local_ip", settings.localIP);
        JsonUtils::writeIP(root, "gateway_ip", settings.gatewayIP);
        JsonUtils::writeIP(root, "subnet_mask", settings.subnetMask);
        JsonUtils::writeIP(root, "dns_ip_1", settings.dnsIP1);
        JsonUtils::writeIP(root, "dns_ip_2", settings.dnsIP2);
    }

    static StateUpdateResult update(JsonObject & root, WiFiSettings & settings) {
        settings.ssid           = root["ssid"] | FACTORY_WIFI_SSID;
        settings.password       = root["password"] | FACTORY_WIFI_PASSWORD;
        settings.hostname       = root["hostname"] | FACTORY_WIFI_HOSTNAME;
        settings.staticIPConfig = root["static_ip_config"] | false;

        // extended settings
        JsonUtils::readIP(root, "local_ip", settings.localIP);
        JsonUtils::readIP(root, "gateway_ip", settings.gatewayIP);
        JsonUtils::readIP(root, "subnet_mask", settings.subnetMask);
        JsonUtils::readIP(root, "dns_ip_1", settings.dnsIP1);
        JsonUtils::readIP(root, "dns_ip_2", settings.dnsIP2);

        // Swap around the dns servers if 2 is populated but 1 is not
        if (settings.dnsIP1 == INADDR_NONE && settings.dnsIP2 != INADDR_NONE) {
            settings.dnsIP1 = settings.dnsIP2;
            settings.dnsIP2 = INADDR_NONE;
        }

        // Turning off static ip config if we don't meet the minimum requirements
        // of ipAddress, gateway and subnet. This may change to static ip only
        // as sensible defaults can be assumed for gateway and subnet
        if (settings.staticIPConfig && (settings.localIP == INADDR_NONE || settings.gatewayIP == INADDR_NONE || settings.subnetMask == INADDR_NONE)) {
            settings.staticIPConfig = false;
        }
        return StateUpdateResult::CHANGED;
    }
};

class WiFiSettingsService : public StatefulService<WiFiSettings> {
  public:
    WiFiSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();

  private:
    HttpEndpoint<WiFiSettings>  _httpEndpoint;
    FSPersistence<WiFiSettings> _fsPersistence;
    unsigned long               _lastConnectionAttempt;

#ifdef ESP32
    bool _stopping;
    void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void onStationModeStop(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ESP8266)
    WiFiEventHandler _onStationModeDisconnectedHandler;
    void             onStationModeDisconnected(const WiFiEventStationModeDisconnected & event);
#endif

    void reconfigureWiFiConnection();
    void manageSTA();
};

#endif // end WiFiSettingsService_h
