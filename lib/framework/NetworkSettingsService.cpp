#include <NetworkSettingsService.h>

using namespace std::placeholders; // for `_1` etc

NetworkSettingsService::NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NetworkSettings::read, NetworkSettings::update, this, server, NETWORK_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NetworkSettings::read, NetworkSettings::update, this, fs, NETWORK_SETTINGS_FILE)
    , _lastConnectionAttempt(0) {
    addUpdateHandler([&](const String & originId) { reconfigureWiFiConnection(); }, false);
}

void NetworkSettingsService::begin() {
    // We want the device to come up in opmode=0 (WIFI_OFF), when erasing the flash this is not the default.
    // If needed, we save opmode=0 before disabling persistence so the device boots with WiFi disabled in the future.
    if (WiFi.getMode() != WIFI_OFF) {
        WiFi.mode(WIFI_OFF);
    }

    // Disable WiFi config persistance and auto reconnect
    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);

    WiFi.mode(WIFI_MODE_MAX);
    WiFi.mode(WIFI_MODE_NULL);

    WiFi.onEvent(std::bind(&NetworkSettingsService::WiFiEvent, this, _1));

    _fsPersistence.readFromFS();
    reconfigureWiFiConnection();
}

void NetworkSettingsService::reconfigureWiFiConnection() {
    // reset last connection attempt to force loop to reconnect immediately
    _lastConnectionAttempt = 0;

    // disconnect and de-configure wifi
    if (WiFi.disconnect(true)) {
        _stopping = true;
    }
}

void NetworkSettingsService::loop() {
    unsigned long currentMillis = millis();
    if (!_lastConnectionAttempt || (uint32_t)(currentMillis - _lastConnectionAttempt) >= WIFI_RECONNECTION_DELAY) {
        _lastConnectionAttempt = currentMillis;
        manageSTA();
    }
}

void NetworkSettingsService::manageSTA() {
    // Abort if already connected, or if we have no SSID
    if (WiFi.isConnected() || _state.ssid.length() == 0) {
        return;
    }

    // Connect or reconnect as required
    if ((WiFi.getMode() & WIFI_STA) == 0) {
        if (_state.staticIPConfig) {
            WiFi.config(_state.localIP, _state.gatewayIP, _state.subnetMask, _state.dnsIP1, _state.dnsIP2); // configure for static IP
        }

        WiFi.setHostname(_state.hostname.c_str()); // set hostname

        // www.esp32.com/viewtopic.php?t=12055
        read([&](NetworkSettings & networkSettings) {
            if (networkSettings.bandwidth20) {
                esp_wifi_set_bandwidth((wifi_interface_t)ESP_IF_WIFI_STA, WIFI_BW_HT20);
            } else {
                esp_wifi_set_bandwidth((wifi_interface_t)ESP_IF_WIFI_STA, WIFI_BW_HT40);
            }
            if (networkSettings.nosleep) {
                WiFi.setSleep(false);                                 // turn off sleep - WIFI_PS_NONE
            }
            WiFi.begin(_state.ssid.c_str(), _state.password.c_str()); // attempt to connect to the network
            esp_wifi_set_max_tx_power(networkSettings.tx_power * 4);  // set power after wifi is startet for C3
        });
#ifdef BOARD_C3_MINI_V1
        // v1 needs this value, see https://github.com/emsesp/EMS-ESP32/pull/620#discussion_r993173979
        WiFi.setTxPower(WIFI_POWER_8_5dBm); // https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html#about-wifi
#endif
    }
}

// handles if wifi stopped
void NetworkSettingsService::WiFiEvent(WiFiEvent_t event) {
    if (event == ARDUINO_EVENT_WIFI_STA_STOP) {
        if (_stopping) {
            _lastConnectionAttempt = 0;
            _stopping              = false;
        }
    }
}
