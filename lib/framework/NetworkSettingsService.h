#ifndef NetworkSettingsService_h
#define NetworkSettingsService_h

#include <StatefulService.h>
#include <FSPersistence.h>
#include <HttpEndpoint.h>
#include <JsonUtils.h>

#ifndef EMSESP_STANDALONE
#include <esp_wifi.h>
#include <ETH.h>
#endif

#define NETWORK_SETTINGS_FILE "/config/networkSettings.json"
#define NETWORK_SETTINGS_SERVICE_PATH "/rest/networkSettings"
#define WIFI_RECONNECTION_DELAY 1000 * 3

#ifndef FACTORY_WIFI_SSID
#define FACTORY_WIFI_SSID ""
#endif

#ifndef FACTORY_WIFI_PASSWORD
#define FACTORY_WIFI_PASSWORD ""
#endif

#ifndef FACTORY_WIFI_HOSTNAME
#define FACTORY_WIFI_HOSTNAME ""
#endif

class NetworkSettings {
  public:
    // core wifi configuration
    String  ssid;
    String  bssid;
    String  password;
    String  hostname;
    uint8_t channel;
    bool    staticIPConfig;
    bool    enableIPv6;
    bool    bandwidth20;
    int8_t  tx_power;
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

    static void read(NetworkSettings & settings, JsonObject & root) {
        // connection settings
        root["ssid"]             = settings.ssid;
        root["bssid"]            = settings.bssid;
        root["channel"]          = settings.channel;
        root["password"]         = settings.password;
        root["hostname"]         = settings.hostname;
        root["static_ip_config"] = settings.staticIPConfig;
        root["enableIPv6"]       = settings.enableIPv6;
        root["bandwidth20"]      = settings.bandwidth20;
        root["tx_power"]         = settings.tx_power;
        root["nosleep"]          = settings.nosleep;
        root["enableMDNS"]       = settings.enableMDNS;
        root["enableCORS"]       = settings.enableCORS;
        root["CORSOrigin"]       = settings.CORSOrigin;

        // extended settings
        JsonUtils::writeIP(root, "local_ip", settings.localIP);
        JsonUtils::writeIP(root, "gateway_ip", settings.gatewayIP);
        JsonUtils::writeIP(root, "subnet_mask", settings.subnetMask);
        JsonUtils::writeIP(root, "dns_ip_1", settings.dnsIP1);
        JsonUtils::writeIP(root, "dns_ip_2", settings.dnsIP2);
    }

    static StateUpdateResult update(JsonObject & root, NetworkSettings & settings) {
        auto enableCORS         = settings.enableCORS;
        auto CORSOrigin         = settings.CORSOrigin;
        settings.ssid           = root["ssid"] | FACTORY_WIFI_SSID;
        settings.bssid          = root["bssid"] | "";
        settings.channel        = root["channel"] | 0;
        settings.password       = root["password"] | FACTORY_WIFI_PASSWORD;
        settings.hostname       = root["hostname"] | FACTORY_WIFI_HOSTNAME;
        settings.staticIPConfig = root["static_ip_config"] | false;
        settings.enableIPv6     = root["enableIPv6"] | false;
        settings.bandwidth20    = root["bandwidth20"] | false;
        settings.tx_power       = root["tx_power"] | 20;
        settings.nosleep        = root["nosleep"] | false;
        settings.enableMDNS     = root["enableMDNS"] | true;
        settings.enableCORS     = root["enableCORS"] | false;
        settings.CORSOrigin     = root["CORSOrigin"] | "*";

        // extended settings
        JsonUtils::readIP(root, "local_ip", settings.localIP);
        JsonUtils::readIP(root, "gateway_ip", settings.gatewayIP);
        JsonUtils::readIP(root, "subnet_mask", settings.subnetMask);
        JsonUtils::readIP(root, "dns_ip_1", settings.dnsIP1);
        JsonUtils::readIP(root, "dns_ip_2", settings.dnsIP2);

        // Swap around the dns servers if 2 is populated but 1 is not
        if (IPUtils::isNotSet(settings.dnsIP1) && IPUtils::isSet(settings.dnsIP2)) {
            settings.dnsIP1 = settings.dnsIP2;
            settings.dnsIP2 = INADDR_NONE;
        }

        // Turning off static ip config if we don't meet the minimum requirements
        // of ipAddress, gateway and subnet. This may change to static ip only
        // as sensible defaults can be assumed for gateway and subnet
        if (settings.staticIPConfig && (IPUtils::isNotSet(settings.localIP) || IPUtils::isNotSet(settings.gatewayIP) || IPUtils::isNotSet(settings.subnetMask))) {
            settings.staticIPConfig = false;
        }
        if (enableCORS != settings.enableCORS || CORSOrigin != settings.CORSOrigin) {
            return StateUpdateResult::CHANGED_RESTART; // tell WebUI that a restart is needed
        }

        return StateUpdateResult::CHANGED;
    }
};

class NetworkSettingsService : public StatefulService<NetworkSettings> {
  public:
    NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();

  private:
    HttpEndpoint<NetworkSettings>  _httpEndpoint;
    FSPersistence<NetworkSettings> _fsPersistence;
    unsigned long                  _lastConnectionAttempt;

    bool _stopping;
    void WiFiEvent(WiFiEvent_t event);

    void reconfigureWiFiConnection();
    void manageSTA();
};

#endif
