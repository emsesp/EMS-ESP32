#include "NetworkSettingsService.h"

#include <emsesp.h>

NetworkSettingsService::NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NetworkSettings::read, NetworkSettings::update, this, server, NETWORK_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NetworkSettings::read, NetworkSettings::update, this, fs, NETWORK_SETTINGS_FILE)
    , _lastConnectionAttempt(0)
    , _stopping(false) {
    addUpdateHandler([this] { reconfigureWiFiConnection(); }, false);
    // Eth is also bound to the WifiGeneric event handler
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) { WiFiEvent(event, info); });
}

static bool formatBssid(const String & bssid, uint8_t (&mac)[6]) {
    uint tmp[6];
    if (bssid.isEmpty() || sscanf(bssid.c_str(), "%X:%X:%X:%X:%X:%X", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
        return false;
    }
    for (uint8_t i = 0; i < 6; i++) {
        mac[i] = static_cast<uint8_t>(tmp[i]);
    }
    return true;
}

void NetworkSettingsService::begin() {
    // TODO: may need to change this for Arduino Core 3.1 / IDF 5.x
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
    if (!_lastConnectionAttempt || static_cast<uint32_t>(currentMillis - _lastConnectionAttempt) >= WIFI_RECONNECTION_DELAY) {
        _lastConnectionAttempt = currentMillis;
        manageSTA();
    }
}

void NetworkSettingsService::manageSTA() {
    // Abort if already connected, or if we have no SSID
    if (WiFi.isConnected() || _state.ssid.length() == 0) {
#if ESP_IDF_VERSION_MAJOR >= 5
        if (_state.ssid.length() == 0) {
            ETH.enableIPv6(true);
        }
#endif
        return;
    }

    // Connect or reconnect as required
    if ((WiFi.getMode() & WIFI_STA) == 0) {
#if ESP_IDF_VERSION_MAJOR >= 5
        WiFi.enableIPv6(true);
#endif
        if (_state.staticIPConfig) {
            WiFi.config(_state.localIP, _state.gatewayIP, _state.subnetMask, _state.dnsIP1, _state.dnsIP2); // configure for static IP
        }
        WiFi.setHostname(_state.hostname.c_str()); // set hostname

        // www.esp32.com/viewtopic.php?t=12055
        if (_state.bandwidth20) {
            esp_wifi_set_bandwidth(static_cast<wifi_interface_t>(ESP_IF_WIFI_STA), WIFI_BW_HT20);
        } else {
            esp_wifi_set_bandwidth(static_cast<wifi_interface_t>(ESP_IF_WIFI_STA), WIFI_BW_HT40);
        }
        if (_state.nosleep) {
            WiFi.setSleep(false); // turn off sleep - WIFI_PS_NONE
        }

        // attempt to connect to the network
        uint8_t bssid[6];
        if (formatBssid(_state.bssid, bssid)) {
            WiFi.begin(_state.ssid.c_str(), _state.password.c_str(), 0, bssid);
        } else {
            WiFi.begin(_state.ssid.c_str(), _state.password.c_str());
        }

#ifdef BOARD_C3_MINI_V1
        // always hardcode Tx power for Wemos CS Mini v1
        // v1 needs this value, see https://github.com/emsesp/EMS-ESP32/pull/620#discussion_r993173979
        // https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html#about-wifi
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
#else
        if (_state.tx_power != 0) {
            // if not set to Auto (0) set the Tx power now
            if (!WiFi.setTxPower(static_cast<wifi_power_t>(_state.tx_power))) {
                emsesp::EMSESP::logger().warning("Failed to set WiFi Tx Power");
            }
        }
#endif
    } else { // not connected but STA-mode active => disconnect
        reconfigureWiFiConnection();
    }
}

// set the TxPower based on the RSSI (signal strength), picking the lowest value
// code is based of RSSI (signal strength) and copied from Tasmota's WiFiSetTXpowerBasedOnRssi() which is copied ESPEasy's ESPEasyWifi.SetWiFiTXpower() function
void NetworkSettingsService::setWiFiPowerOnRSSI() {
    // Range ESP32  : 2dBm - 20dBm
    // 802.11b - wifi1
    // 802.11a - wifi2
    // 802.11g - wifi3
    // 802.11n - wifi4
    // 802.11ac - wifi5
    // 802.11ax - wifi6

    int max_tx_pwr = MAX_TX_PWR_DBM_n;         // assume wifi4
    int threshold  = WIFI_SENSITIVITY_n + 120; // Margin in dBm * 10 on top of threshold

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

    // from WiFIGeneric.h use:
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
    if (min_tx_pwr > 185)
        p = WIFI_POWER_19_5dBm;
    else if (min_tx_pwr > 170)
        p = WIFI_POWER_18_5dBm;
    else if (min_tx_pwr > 150)
        p = WIFI_POWER_17dBm;
    else if (min_tx_pwr > 130)
        p = WIFI_POWER_15dBm;
    else if (min_tx_pwr > 110)
        p = WIFI_POWER_13dBm;
    else if (min_tx_pwr > 85)
        p = WIFI_POWER_11dBm;
    else if (min_tx_pwr > 70)
        p = WIFI_POWER_8_5dBm;
    else if (min_tx_pwr > 50)
        p = WIFI_POWER_7dBm;
    else if (min_tx_pwr > 20)
        p = WIFI_POWER_5dBm;

#if defined(EMSESP_DEBUG)
    // emsesp::EMSESP::logger().debug("Recommended WiFi Tx Power (set_power %d, new power %d, rssi %d, threshold %d)", min_tx_pwr / 10, p, rssi, threshold);
#endif

    if (!WiFi.setTxPower(p)) {
        emsesp::EMSESP::logger().warning("Failed to set WiFi Tx Power");
    }
}

// start the multicast UDP service so EMS-ESP is discoverable via .local
void NetworkSettingsService::mDNS_start() const {
#ifndef EMSESP_STANDALONE
    MDNS.end();

    if (_state.enableMDNS) {
        if (!MDNS.begin(emsesp::EMSESP::system_.hostname().c_str())) {
            emsesp::EMSESP::logger().warning("Failed to start mDNS Responder service");
            return;
        }

        std::string address_s = emsesp::EMSESP::system_.hostname() + ".local";

        MDNS.addService("http", "tcp", 80);   // add our web server and rest API
        MDNS.addService("telnet", "tcp", 23); // add our telnet console

        // MDNS.addServiceTxt("http", "tcp", "version", EMSESP_APP_VERSION);
        MDNS.addServiceTxt("http", "tcp", "address", address_s.c_str());

        emsesp::EMSESP::logger().info("Starting mDNS Responder service");
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
        connectcount_++; // count the number of WiFi reconnects
        emsesp::EMSESP::logger().warning("WiFi disconnected (#%d). Reason: %s (%d)",
                                         connectcount_,
                                         disconnectReason(info.wifi_sta_disconnected.reason),
                                         info.wifi_sta_disconnected.reason); // IDF 4.0
        emsesp::EMSESP::system_.has_ipv6(false);


        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        char result[10];
        emsesp::EMSESP::logger().info("WiFi connected (Local IP=%s, hostname=%s, TxPower=%s dBm)",
                                      WiFi.localIP().toString().c_str(),
                                      WiFi.getHostname(),
                                      emsesp::Helpers::render_value(result, ((double)(WiFi.getTxPower()) / 4), 1));
        mDNS_start();
        break;

    case ARDUINO_EVENT_ETH_START:
        // configure for static IP
        if (_state.staticIPConfig) {
            ETH.config(_state.localIP, _state.gatewayIP, _state.subnetMask, _state.dnsIP1, _state.dnsIP2);
        }
        ETH.setHostname(emsesp::EMSESP::system_.hostname().c_str());
        break;

    case ARDUINO_EVENT_ETH_GOT_IP:
        // prevent double calls to mDNS
        if (!emsesp::EMSESP::system_.ethernet_connected()) {
            emsesp::EMSESP::logger().info("Ethernet connected (Local IP=%s, speed %d Mbps)", ETH.localIP().toString().c_str(), ETH.linkSpeed());
            emsesp::EMSESP::system_.ethernet_connected(true);
            mDNS_start();
        }
        break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
        emsesp::EMSESP::logger().warning("Ethernet disconnected. Reason: %s (%d)",
                                         disconnectReason(info.wifi_sta_disconnected.reason),
                                         info.wifi_sta_disconnected.reason);
        emsesp::EMSESP::system_.ethernet_connected(false);
        emsesp::EMSESP::system_.has_ipv6(false);
        break;

    case ARDUINO_EVENT_ETH_STOP:
        emsesp::EMSESP::logger().info("Ethernet stopped");
        emsesp::EMSESP::system_.ethernet_connected(false);
        emsesp::EMSESP::system_.has_ipv6(false);
        break;

    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        // Set the TxPower after the connection is established, if we're using TxPower = 0 (Auto)
        if (_state.tx_power == 0) {
            setWiFiPowerOnRSSI();
        }
#if ESP_IDF_VERSION_MAJOR < 5
        WiFi.enableIpV6(); // force ipv6
#endif
        break;

    case ARDUINO_EVENT_ETH_CONNECTED:
#if ESP_IDF_VERSION_MAJOR < 5
        ETH.enableIpV6(); // force ipv6
#endif
        break;

        // IPv6 specific - WiFi/Eth
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    case ARDUINO_EVENT_ETH_GOT_IP6: {
#if !TASMOTA_SDK && ESP_IDF_VERSION_MAJOR < 5
        auto ip6 = IPv6Address((uint8_t *)info.got_ip6.ip6_info.ip.addr).toString();
#else
        auto ip6 = IPAddress(IPv6, (uint8_t *)info.got_ip6.ip6_info.ip.addr, 0).toString();
#endif
        const char * link = event == ARDUINO_EVENT_ETH_GOT_IP6 ? "Eth" : "WiFi";
        if (ip6.startsWith("fe80")) {
            emsesp::EMSESP::logger().info("IPv6 (%s) local: %s", link, ip6.c_str());
        } else if (ip6.startsWith("fd") || ip6.startsWith("fc")) {
            emsesp::EMSESP::logger().info("IPv6 (%s) ULA: %s", link, ip6.c_str());
        } else {
            emsesp::EMSESP::logger().info("IPv6 (%s) global: %s", link, ip6.c_str());
        }
        emsesp::EMSESP::system_.has_ipv6(true);
    } break;

    default:
        break;
    }
#endif
}

void NetworkSettings::read(NetworkSettings & settings, JsonObject root) {
    // connection settings
    root["ssid"]             = settings.ssid;
    root["bssid"]            = settings.bssid;
    root["password"]         = settings.password;
    root["hostname"]         = settings.hostname;
    root["static_ip_config"] = settings.staticIPConfig;
    root["bandwidth20"]      = settings.bandwidth20;
    root["nosleep"]          = settings.nosleep;
    root["enableMDNS"]       = settings.enableMDNS;
    root["enableCORS"]       = settings.enableCORS;
    root["CORSOrigin"]       = settings.CORSOrigin;
    root["tx_power"]         = settings.tx_power;

    // extended settings
    JsonUtils::writeIP(root, "local_ip", settings.localIP);
    JsonUtils::writeIP(root, "gateway_ip", settings.gatewayIP);
    JsonUtils::writeIP(root, "subnet_mask", settings.subnetMask);
    JsonUtils::writeIP(root, "dns_ip_1", settings.dnsIP1);
    JsonUtils::writeIP(root, "dns_ip_2", settings.dnsIP2);
}

StateUpdateResult NetworkSettings::update(JsonObject root, NetworkSettings & settings) {
    // keep copy of original settings
    auto enableCORS = settings.enableCORS;
    auto CORSOrigin = settings.CORSOrigin;
    auto ssid       = settings.ssid;
    auto tx_power   = settings.tx_power;

    settings.ssid           = root["ssid"] | FACTORY_WIFI_SSID;
    settings.bssid          = root["bssid"] | "";
    settings.password       = root["password"] | FACTORY_WIFI_PASSWORD;
    settings.hostname       = root["hostname"] | FACTORY_WIFI_HOSTNAME;
    settings.staticIPConfig = root["static_ip_config"];
    settings.bandwidth20    = root["bandwidth20"];
    settings.tx_power       = static_cast<uint8_t>(root["tx_power"] | 0);
    settings.nosleep        = root["nosleep"] | true;
    settings.enableMDNS     = root["enableMDNS"] | true;
    settings.enableCORS     = root["enableCORS"];
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

    // see if we need to inform the user of a restart
    if (tx_power != settings.tx_power || enableCORS != settings.enableCORS || CORSOrigin != settings.CORSOrigin
        || (ssid != settings.ssid && settings.ssid.isEmpty())) {
        return StateUpdateResult::CHANGED_RESTART; // tell WebUI that a restart is needed
    }

    return StateUpdateResult::CHANGED;
}