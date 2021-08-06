#include <APSettingsService.h>

#include "../../src/emsesp_stub.hpp" // proddy added

APSettingsService::APSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(APSettings::read, APSettings::update, this, server, AP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(APSettings::read, APSettings::update, this, fs, AP_SETTINGS_FILE)
    , _dnsServer(nullptr)
    , _lastManaged(0)
    , _reconfigureAp(false) {
    addUpdateHandler([&](const String & originId) { reconfigureAP(); }, false);
}

void APSettingsService::begin() {
    _fsPersistence.readFromFS();
    reconfigureAP();
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
    WiFiMode_t currentWiFiMode   = WiFi.getMode();
    bool       network_connected = (emsesp::EMSESP::system_.ethernet_connected() || (WiFi.status() == WL_CONNECTED));
    if (_state.provisionMode == AP_MODE_ALWAYS || (_state.provisionMode == AP_MODE_DISCONNECTED && !network_connected)) {
        if (_reconfigureAp || currentWiFiMode == WIFI_OFF || currentWiFiMode == WIFI_STA) {
            startAP();
        }
    } else if ((currentWiFiMode == WIFI_AP || currentWiFiMode == WIFI_AP_STA) && (_reconfigureAp || !WiFi.softAPgetStationNum())) {
        stopAP();
    }
    _reconfigureAp = false;
}

void APSettingsService::startAP() {
    WiFi.softAPConfig(_state.localIP, _state.gatewayIP, _state.subnetMask);
    esp_wifi_set_bandwidth(ESP_IF_WIFI_AP, WIFI_BW_HT20);
    WiFi.softAP(_state.ssid.c_str(), _state.password.c_str());
    if (!_dnsServer) {
        IPAddress apIp = WiFi.softAPIP();
        emsesp::EMSESP::logger().info(F("Starting Access Point with captive portal on %s"), apIp.toString().c_str());
        _dnsServer = new DNSServer;
        _dnsServer->start(DNS_PORT, "*", apIp);
    }
}

void APSettingsService::stopAP() {
    if (_dnsServer) {
        emsesp::EMSESP::logger().info(F("Stopping Access Point"));
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
