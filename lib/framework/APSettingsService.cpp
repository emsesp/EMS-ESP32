#include <APSettingsService.h>

#include "../../src/emsesp_stub.hpp"

APSettingsService::APSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(APSettings::read, APSettings::update, this, server, AP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(APSettings::read, APSettings::update, this, fs, AP_SETTINGS_FILE)
    , _dnsServer(nullptr)
    , _lastManaged(0)
    , _reconfigureAp(false)
    , _connected(0) {
    addUpdateHandler([&](const String & originId) { reconfigureAP(); }, false);
    WiFi.onEvent(std::bind(&APSettingsService::WiFiEvent, this, _1));
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
    unsigned long manageElapsed = (uint32_t)(currentMillis - _lastManaged);
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
    WiFi.softAPConfig(_state.localIP, _state.gatewayIP, _state.subnetMask);
    esp_wifi_set_bandwidth((wifi_interface_t)ESP_IF_WIFI_AP, WIFI_BW_HT20);
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
