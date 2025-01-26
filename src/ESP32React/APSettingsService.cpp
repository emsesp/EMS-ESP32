#include "APSettingsService.h"

#include <emsesp.h>

APSettingsService::APSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(APSettings::read, APSettings::update, this, server, AP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(APSettings::read, APSettings::update, this, fs, AP_SETTINGS_FILE)
    , _dnsServer(nullptr)
    , _lastManaged(0)
    , _reconfigureAp(false)
    , _connected(0) {
    addUpdateHandler([this] { reconfigureAP(); }, false);
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) { WiFiEvent(event); });
}

void APSettingsService::begin() {
    _fsPersistence.readFromFS();
    // disabled for delayed start, first try station mode
    // reconfigureAP();
}

// wait 10 sec on STA disconnect before starting AP
void APSettingsService::WiFiEvent(WiFiEvent_t event) {
    uint8_t was_connected = _connected;
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        _connected &= ~1;
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        _connected &= ~2;
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        _connected |= 1;
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP6:
        _connected |= 2;
        break;
    default:
        break;
    }
    // wait 10 sec before starting AP
    if (was_connected && !_connected) {
        _lastManaged = uuid::get_uptime();
    }
}

void APSettingsService::reconfigureAP() {
    _lastManaged   = uuid::get_uptime() - MANAGE_NETWORK_DELAY;
    _reconfigureAp = true;
}

void APSettingsService::loop() {
    unsigned long currentMillis = uuid::get_uptime();
    unsigned long manageElapsed = static_cast<uint32_t>(currentMillis - _lastManaged);
    if (manageElapsed >= MANAGE_NETWORK_DELAY) {
        _lastManaged = currentMillis;
        manageAP();
    }

    handleDNS();
}

void APSettingsService::manageAP() {
    WiFiMode_t currentWiFiMode = WiFi.getMode();
    if (_state.provisionMode == AP_MODE_ALWAYS || (_state.provisionMode == AP_MODE_DISCONNECTED && !_connected)) {
        if (_reconfigureAp || currentWiFiMode == WIFI_OFF || currentWiFiMode == WIFI_STA) {
            startAP();
        }
    } else if ((currentWiFiMode == WIFI_AP || currentWiFiMode == WIFI_AP_STA) && _connected && (_reconfigureAp || !WiFi.softAPgetStationNum())) {
        stopAP();
    }
    _reconfigureAp = false;
}

void APSettingsService::startAP() {
#if ESP_IDF_VERSION_MAJOR < 5
    WiFi.softAPenableIpV6(); // force IPV6, same as for WiFi - fixes https://github.com/emsesp/EMS-ESP32/issues/1922
#else
    WiFi.softAPenableIPv6(); // force IPV6, same as for WiFi - fixes https://github.com/emsesp/EMS-ESP32/issues/1922
#endif
    WiFi.softAPConfig(_state.localIP, _state.gatewayIP, _state.subnetMask);
    esp_wifi_set_bandwidth(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), WIFI_BW_HT20);
    WiFi.softAP(_state.ssid.c_str(), _state.password.c_str(), _state.channel, _state.ssidHidden, _state.maxClients);
#if CONFIG_IDF_TARGET_ESP32C3
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html#about-wifi
#endif
    if (!_dnsServer) {
        IPAddress apIp = WiFi.softAPIP();
        emsesp::EMSESP::logger().info("Starting Access Point with captive portal on %s", apIp.toString().c_str());
        _dnsServer = new DNSServer;
        _dnsServer->start(DNS_PORT, "*", apIp);
    }
}

void APSettingsService::stopAP() {
    if (_dnsServer) {
        emsesp::EMSESP::logger().info("Stopping Access Point");
        _dnsServer->stop();
        delete _dnsServer;
        _dnsServer = nullptr;
    }
    WiFi.softAPdisconnect(true);
}

void APSettingsService::handleDNS() {
    if (_dnsServer) {
        _dnsServer->processNextRequest();
    }
}

APNetworkStatus APSettingsService::getAPNetworkStatus() {
    WiFiMode_t currentWiFiMode = WiFi.getMode();
    bool       apActive        = currentWiFiMode == WIFI_AP || currentWiFiMode == WIFI_AP_STA;

    if (apActive && _state.provisionMode != AP_MODE_ALWAYS && WiFi.status() == WL_CONNECTED) {
        return APNetworkStatus::LINGERING;
    }

    return apActive ? APNetworkStatus::ACTIVE : APNetworkStatus::INACTIVE;
}


void APSettings::read(const APSettings & settings, JsonObject root) {
    root["provision_mode"] = settings.provisionMode;
    root["ssid"]           = settings.ssid;
    root["password"]       = settings.password;
    root["channel"]        = settings.channel;
    root["ssid_hidden"]    = settings.ssidHidden;
    root["max_clients"]    = settings.maxClients;
    root["local_ip"]       = settings.localIP.toString();
    root["gateway_ip"]     = settings.gatewayIP.toString();
    root["subnet_mask"]    = settings.subnetMask.toString();
}

StateUpdateResult APSettings::update(JsonObject root, APSettings & settings) {
    APSettings newSettings    = {};
    newSettings.provisionMode = static_cast<uint8_t>(root["provision_mode"] | FACTORY_AP_PROVISION_MODE);

    switch (settings.provisionMode) {
    case AP_MODE_ALWAYS:
    case AP_MODE_DISCONNECTED:
    case AP_MODE_NEVER:
        break;
    default:
        newSettings.provisionMode = AP_MODE_ALWAYS;
    }

    newSettings.ssid       = root["ssid"] | FACTORY_AP_SSID;
    newSettings.password   = root["password"] | FACTORY_AP_PASSWORD;
    newSettings.channel    = static_cast<uint8_t>(root["channel"] | FACTORY_AP_CHANNEL);
    newSettings.ssidHidden = root["ssid_hidden"] | FACTORY_AP_SSID_HIDDEN;
    newSettings.maxClients = static_cast<uint8_t>(root["max_clients"] | FACTORY_AP_MAX_CLIENTS);

    JsonUtils::readIP(root, "local_ip", newSettings.localIP, String(FACTORY_AP_LOCAL_IP));
    JsonUtils::readIP(root, "gateway_ip", newSettings.gatewayIP, String(FACTORY_AP_GATEWAY_IP));
    JsonUtils::readIP(root, "subnet_mask", newSettings.subnetMask, String(FACTORY_AP_SUBNET_MASK));

    if (newSettings == settings) {
        return StateUpdateResult::UNCHANGED;
    }

    settings = newSettings;
    return StateUpdateResult::CHANGED;
}
