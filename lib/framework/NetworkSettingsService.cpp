#include <NetworkSettingsService.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

NetworkSettingsService::NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NetworkSettings::read, NetworkSettings::update, this, server, NETWORK_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NetworkSettings::read, NetworkSettings::update, this, fs, NETWORK_SETTINGS_FILE)
    , _lastConnectionAttempt(0) {
    addUpdateHandler([&](const String & originId) { reconfigureWiFiConnection(); }, false);
    // wifi event callbacks
    WiFi.onEvent(std::bind(&NetworkSettingsService::WiFiEvent, this, _1, _2));
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
    // WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);     // default is FAST_SCAN, connect issues in 2.0.14
    // WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL); // is default, no need to set

    _fsPersistence.readFromFS();
    // reconfigureWiFiConnection();
}

void NetworkSettingsService::reconfigureWiFiConnection() {
    // do not disconnect for switching to eth, restart is needed
    if (WiFi.isConnected() && _state.ssid.length() == 0) {
        return;
    }

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
        if (_state.bandwidth20) {
            esp_wifi_set_bandwidth((wifi_interface_t)ESP_IF_WIFI_STA, WIFI_BW_HT20);
        } else {
            esp_wifi_set_bandwidth((wifi_interface_t)ESP_IF_WIFI_STA, WIFI_BW_HT40);
        }
        if (_state.nosleep) {
            WiFi.setSleep(false); // turn off sleep - WIFI_PS_NONE
        }
        // attempt to connect to the network
        uint mac[6];
        if (!_state.bssid.isEmpty() && sscanf(_state.bssid.c_str(), "%X:%X:%X:%X:%X:%X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
            uint8_t mac1[6];
            for (uint8_t i = 0; i < 6; i++) {
                mac1[i] = (uint8_t)mac[i];
            }
            WiFi.begin(_state.ssid.c_str(), _state.password.c_str(), 0, mac1);
        } else {
            WiFi.begin(_state.ssid.c_str(), _state.password.c_str());
        }
        // Set thw Wifi Tx power
        setWiFiPower();
    } else { // not connected but STA-mode active => disconnect
        reconfigureWiFiConnection();
    }
}

// set the Tx WiFi power
// based of RSSI (signal strength) and copied from Tasmota's WiFiSetTXpowerBasedOnRssi() function with is copied from espEasy
void NetworkSettingsService::setWiFiPower() {
// hardcode Tx power for Wemos CS Mini v1
// v1 needs this value, see https://github.com/emsesp/EMS-ESP32/pull/620#discussion_r993173979
// https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html#about-wifi
#ifdef BOARD_C3_MINI_V1
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    return;
#endif

    auto set_power = _state.tx_power; // get user settings. 0 means auto

    // If Auto set the TxPower based on the RSSI (signal strength)
    if (set_power == 0) {
        // Range ESP32  : 2dBm - 20dBm
        // 802.11b - wifi1
        // 802.11a - wifi2
        // 802.11g - wifi3
        // 802.11n - wifi4
        // 802.11ac - wifi5
        // 802.11ax - wifi6
        int max_tx_pwr = MAX_TX_PWR_DBM_n; // assume wifi4
        int threshold  = WIFI_SENSITIVITY_n;
        threshold += 30; // Margin in dBm * 10 on top of threshold

        // Assume AP sends with max set by ETSI standard.
        // 2.4 GHz: 100 mWatt (20 dBm)
        // US and some other countries allow 1000 mW (30 dBm)
        int rssi    = WiFi.RSSI() * 10;
        int newrssi = rssi - 200; // We cannot send with over 20 dBm, thus it makes no sense to force higher TX power all the time.

        int min_tx_pwr = 0;
        if (newrssi < threshold) {
            min_tx_pwr = threshold - newrssi;
        }
        if (min_tx_pwr > max_tx_pwr) {
            min_tx_pwr = max_tx_pwr;
        }

        set_power = min_tx_pwr / 10; // this is the recommended power setting to use
    }

    // use the user setting, which is a value from WiFIGeneric.h
    //  WIFI_POWER_19_5dBm = 78,// 19.5dBm
    //  WIFI_POWER_19dBm = 76,// 19dBm
    //  WIFI_POWER_18_5dBm = 74,// 18.5dBm
    //  WIFI_POWER_17dBm = 68,// 17dBm
    //  WIFI_POWER_15dBm = 60,// 15dBm
    //  WIFI_POWER_13dBm = 52,// 13dBm
    //  WIFI_POWER_11dBm = 44,// 11dBm
    //  WIFI_POWER_8_5dBm = 34,// 8.5dBm
    //  WIFI_POWER_7dBm = 28,// 7dBm
    //  WIFI_POWER_5dBm = 20,// 5dBm
    //  WIFI_POWER_2dBm = 8,// 2dBm
    //  WIFI_POWER_MINUS_1dBm = -4// -1dBm
    wifi_power_t p = WIFI_POWER_2dBm;
    if (set_power > 19)
        p = WIFI_POWER_19_5dBm;
    else if (set_power > 18)
        p = WIFI_POWER_18_5dBm;
    else if (set_power >= 17)
        p = WIFI_POWER_17dBm;
    else if (set_power >= 15)
        p = WIFI_POWER_15dBm;
    else if (set_power >= 13)
        p = WIFI_POWER_13dBm;
    else if (set_power >= 11)
        p = WIFI_POWER_11dBm;
    else if (set_power >= 8)
        p = WIFI_POWER_8_5dBm;
    else if (set_power >= 7)
        p = WIFI_POWER_7dBm;
    else if (set_power >= 5)
        p = WIFI_POWER_5dBm;

    WiFi.setTxPower(p);
}

// start the multicast UDP service so EMS-ESP is discoverable via .local
void NetworkSettingsService::mDNS_start() const {
#ifndef EMSESP_STANDALONE
    MDNS.end();

    if (_state.enableMDNS) {
        if (!MDNS.begin(emsesp::EMSESP::system_.hostname().c_str())) {
            emsesp::EMSESP::logger().warning("Failed to start mDNS responder service");
            return;
        }

        std::string address_s = emsesp::EMSESP::system_.hostname() + ".local";

        MDNS.addService("http", "tcp", 80);   // add our web server and rest API
        MDNS.addService("telnet", "tcp", 23); // add our telnet console

        MDNS.addServiceTxt("http", "tcp", "version", EMSESP_APP_VERSION);
        MDNS.addServiceTxt("http", "tcp", "address", address_s.c_str());

        emsesp::EMSESP::logger().info("mDNS responder service started");
    }
#else
    if (_state.enableMDNS) {
        EMSESP::logger().info("mDNS responder service started");
    }
#endif
}

const char * NetworkSettingsService::disconnectReason(uint8_t code) {
#ifndef EMSESP_STANDALONE
    switch (code) {
    case WIFI_REASON_UNSPECIFIED: // = 1,
        return "unspecified";
    case WIFI_REASON_AUTH_EXPIRE: // = 2,
        return "auth expire";
    case WIFI_REASON_AUTH_LEAVE: // = 3,
        return "auth leave";
    case WIFI_REASON_ASSOC_EXPIRE: // = 4,
        return "assoc expired";
    case WIFI_REASON_ASSOC_TOOMANY: // = 5,
        return "assoc too many";
    case WIFI_REASON_NOT_AUTHED: // = 6,
        return "not authenticated";
    case WIFI_REASON_NOT_ASSOCED: // = 7,
        return "not assoc";
    case WIFI_REASON_ASSOC_LEAVE: // = 8,
        return "assoc leave";
    case WIFI_REASON_ASSOC_NOT_AUTHED: // = 9,
        return "assoc not authed";
    case WIFI_REASON_DISASSOC_PWRCAP_BAD: // = 10,
        return "disassoc powerCAP bad";
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD: // = 11,
        return "disassoc supchan bad";
    case WIFI_REASON_IE_INVALID: // = 13,
        return "IE invalid";
    case WIFI_REASON_MIC_FAILURE: // = 14,
        return "MIC failure";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: // = 15,
        return "4way handshake timeout";
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT: // = 16,
        return "group key-update timeout";
    case WIFI_REASON_IE_IN_4WAY_DIFFERS: // = 17,
        return "IE in 4way differs";
    case WIFI_REASON_GROUP_CIPHER_INVALID: // = 18,
        return "group cipher invalid";
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID: // = 19,
        return "pairwise cipher invalid";
    case WIFI_REASON_AKMP_INVALID: // = 20,
        return "AKMP invalid";
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION: // = 21,
        return "unsupported RSN_IE version";
    case WIFI_REASON_INVALID_RSN_IE_CAP: // = 22,
        return "invalid RSN_IE_CAP";
    case WIFI_REASON_802_1X_AUTH_FAILED: // = 23,
        return "802 X1 auth failed";
    case WIFI_REASON_CIPHER_SUITE_REJECTED: // = 24,
        return "cipher suite rejected";
    case WIFI_REASON_BEACON_TIMEOUT: // = 200,
        return "beacon timeout";
    case WIFI_REASON_NO_AP_FOUND: // = 201,
        return "no AP found";
    case WIFI_REASON_AUTH_FAIL: // = 202,
        return "auth fail";
    case WIFI_REASON_ASSOC_FAIL: // = 203,
        return "assoc fail";
    case WIFI_REASON_HANDSHAKE_TIMEOUT: // = 204,
        return "handshake timeout";
    case WIFI_REASON_CONNECTION_FAIL: // 205,
        return "connection fail";
    case WIFI_REASON_AP_TSF_RESET: // 206,
        return "AP tsf reset";
    case WIFI_REASON_ROAMING: // 207,
        return "roaming";
    case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG: // 208,
        return "assoc comeback time too long";
    case WIFI_REASON_SA_QUERY_TIMEOUT: // 209,
        return "sa query timeout";
    default:
        return "unknown";
    }
#endif

    return "";
}

// handles both WiFI and Ethernet
void NetworkSettingsService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
#ifndef EMSESP_STANDALONE

    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_STOP:
        if (_stopping) {
            _lastConnectionAttempt = 0;
            _stopping              = false;
        }
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        emsesp::EMSESP::logger().warning("WiFi disconnected. Reason: %s (%d)",
                                         disconnectReason(info.wifi_sta_disconnected.reason),
                                         info.wifi_sta_disconnected.reason); // IDF 4.0
        emsesp::EMSESP::system_.has_ipv6(false);
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        char result[10];
        emsesp::EMSESP::logger().info("WiFi connected with IP=%s, hostname=%s, TxPower=%s",
                                      WiFi.localIP().toString().c_str(),
                                      WiFi.getHostname(),
                                      emsesp::Helpers::render_value(result, (double)(WiFi.getTxPower() / 4), 1));

        mDNS_start();
        break;

    case ARDUINO_EVENT_ETH_START:
        ETH.setHostname(emsesp::EMSESP::system_.hostname().c_str());

        // configure for static IP
        if (_state.staticIPConfig) {
            ETH.config(_state.localIP, _state.gatewayIP, _state.subnetMask, _state.dnsIP1, _state.dnsIP2);
        }
        break;

    case ARDUINO_EVENT_ETH_GOT_IP:
        // prevent double calls
        if (!emsesp::EMSESP::system_.ethernet_connected()) {
            emsesp::EMSESP::logger().info("Ethernet connected with IP=%s, speed %d Mbps", ETH.localIP().toString().c_str(), ETH.linkSpeed());
            emsesp::EMSESP::system_.ethernet_connected(true);
            mDNS_start();
        }
        break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
        emsesp::EMSESP::logger().warning("Ethernet disconnected");
        emsesp::EMSESP::system_.ethernet_connected(false);
        emsesp::EMSESP::system_.has_ipv6(false);
        break;

    case ARDUINO_EVENT_ETH_STOP:
        emsesp::EMSESP::logger().info("Ethernet stopped");
        emsesp::EMSESP::system_.ethernet_connected(false);
        emsesp::EMSESP::system_.has_ipv6(false);
        break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        if (_state.enableIPv6) {
            WiFi.enableIpV6();
        }
        break;

    case ARDUINO_EVENT_ETH_CONNECTED:
        if (_state.enableIPv6) {
            ETH.enableIpV6();
        }
        break;

        // IPv6 specific
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    case ARDUINO_EVENT_ETH_GOT_IP6:
        if (emsesp::EMSESP::system_.ethernet_connected()) {
            emsesp::EMSESP::logger().info("Ethernet connected with IPv6=%s, speed %d Mbps", ETH.localIPv6().toString().c_str(), ETH.linkSpeed());
        } else {
            emsesp::EMSESP::logger().info("WiFi connected with IPv6=%s, hostname=%s", WiFi.localIPv6().toString().c_str(), WiFi.getHostname());
        }
        mDNS_start();
        emsesp::EMSESP::system_.has_ipv6(true);
        break;

    default:
        break;
    }
#endif
}
