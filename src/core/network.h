/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMSESP_NETWORK_H_
#define EMSESP_NETWORK_H_

#ifndef EMSESP_STANDALONE
#include <esp_mac.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <ETH.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#endif

#include <cstring>
#include <vector>

#include <esp32-psram.h>
#include <uuid/log.h>

namespace emsesp {

#define NETWORK_RECONNECTION_DELAY_SHORT 5000 // 5 seconds

#ifndef EMSESP_DEBUG
#define NETWORK_RECONNECTION_DELAY_LONG 60000 // 1 minute
#else
#define NETWORK_RECONNECTION_DELAY_LONG 10000 // 10 seconds - for debugging
#endif

#define MAX_NETWORK_RECONNECTION_ATTEMPTS 4 // maximum number of network reconnection attempts before going to AP fallback

#define DNS_PORT 53 // dns server port for captive portal

// copied from Tasmota
#if CONFIG_IDF_TARGET_ESP32S2
#define MAX_TX_PWR_DBM_11b 195
#define MAX_TX_PWR_DBM_54g 150
#define MAX_TX_PWR_DBM_n 130
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -750
#define WIFI_SENSITIVITY_n -720
#elif CONFIG_IDF_TARGET_ESP32S3
#define MAX_TX_PWR_DBM_11b 210
#define MAX_TX_PWR_DBM_54g 190
#define MAX_TX_PWR_DBM_n 185
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -760
#define WIFI_SENSITIVITY_n -720
#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
#define MAX_TX_PWR_DBM_11b 210
#define MAX_TX_PWR_DBM_54g 190
#define MAX_TX_PWR_DBM_n 185
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -760
#define WIFI_SENSITIVITY_n -730
#else
#define MAX_TX_PWR_DBM_11b 195
#define MAX_TX_PWR_DBM_54g 160
#define MAX_TX_PWR_DBM_n 140
#define WIFI_SENSITIVITY_11b -880
#define WIFI_SENSITIVITY_54g -750
#define WIFI_SENSITIVITY_n -700
#endif

// which physical interface we are currently using for the active network connection.
// Mapped from the esp-netif description string returned by esp_netif_get_desc(): "sta" -> WIFI,
// "ap" -> AP, "eth"/"eth1"/"eth2"/...
enum class NetIface : uint8_t {
    NONE = 0, // 0
    WIFI,     // 1
    ETHERNET, // 2
    AP,       // 3
};

enum class NetPhase : uint8_t {
    ETHERNET = 0,
    WIFI     = 1,
    AP       = 2,
};

class Network {
  public:
    void begin();
    void loop();

    uint32_t network_ip() const {
        return network_ip_;
    }

    NetIface network_iface() const {
        return network_iface_;
    }

    bool ethernet_connected() const {
        return network_iface_ == NetIface::ETHERNET && network_ip_ != 0;
    }

    bool wifi_connected() const {
        return network_iface_ == NetIface::WIFI && network_ip_ != 0;
    }

    bool ap_connected() const {
        return network_iface_ == NetIface::AP && network_ip_ != 0;
    }

    bool network_connected() const {
        return ethernet_connected() || wifi_connected(); // don't include AP here
    }

    bool has_ipv6() const {
        return has_ipv6_ && (network_iface_ == NetIface::WIFI || network_iface_ == NetIface::ETHERNET);
    }

    uint16_t getNetworkReconnects() {
        return reconnect_count_;
    }

    // the soft-AP's own details, which are what the Access Point status page reports
    std::string getAPIP() const;
    std::string getAPMacAddress() const;
    uint8_t     getAPStationNum() const;

    const char * ethernet_status() const;

#ifndef EMSESP_STANDALONE
    // an IPv6 address together with a readable scope, e.g. "global" or "link local"
    struct IPv6Address {
        IPAddress    ip;
        const char * scope;
    };

    static std::vector<IPv6Address> ipv6_addresses(NetworkInterface & netif);
    static std::vector<IPAddress>   dns_servers(const NetworkInterface & netif);
    static std::vector<IPAddress>   ipv6_gateways(NetworkInterface & netif);
#endif

    void reconnect();

    // ask for a reconnect() to run from the next Network::loop()
    void schedule_reconnect() {
        reconnect_pending_ = true;
    }

    // map a netif description string (from esp_netif_get_desc) to a NetIface
    static NetIface iface_from_desc(const char * desc) {
        if (!desc) {
            return NetIface::NONE;
        }
        if (strcmp(desc, "sta") == 0) {
            return NetIface::WIFI;
        }
        if (strcmp(desc, "ap") == 0) {
            return NetIface::AP;
        }
        // any "eth*" (eth, eth0, eth1, ...) is treated as Ethernet
        if (strncmp(desc, "eth", 3) == 0) {
            return NetIface::ETHERNET;
        }
        return NetIface::NONE;
    }

    static const char * network_iface_to_string(NetIface iface) {
        switch (iface) {
        case NetIface::WIFI:
            return "WiFi";
        case NetIface::ETHERNET:
            return "Ethernet";
        case NetIface::AP:
            return "AP";
        case NetIface::NONE:
        default:
            return "None";
        }
        return "unknown";
    }

  private:
    static uuid::log::Logger logger_;

    void         findNetworks();
    void         checkConnection();
    void         startmDNS() const;
    bool         formatBSSID(const String & bssid, uint8_t (&mac)[6]);
    void         startAP();
    void         startWIFI();
    void         startEthernet();
    void         startWiFiRadio();
    void         stopWiFiRadio();
    void         setWiFiPower(uint8_t tx_power);
    const char * disconnectReason(uint8_t code);
    void         stopAP();
    NetPhase     initialPhase() const;
    bool         ethClockConflictsPsram() const;

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif

    unsigned long    lastConnectionAttempt_  = 0;
    uint16_t         reconnect_count_        = 0; // number of network reconnects
    uint32_t         network_ip_             = 0;
    NetIface         network_iface_          = NetIface::NONE;
    bool             has_ipv6_               = false;
    bool             juststopped_            = false;
    volatile uint8_t last_disconnect_reason_ = 0;
    uint16_t         connect_retry_          = 0; // number of network re-connection attempts

    volatile bool wifi_connect_pending_     = false;
    volatile bool ethernet_connect_pending_ = false;
    volatile bool reconnect_pending_        = false; // set by schedule_reconnect(), actioned in loop()

    NetPhase phase_ = NetPhase::ETHERNET;

    bool wifi_events_registered_          = false; // ensure WiFi.onEvent() handlers are registered only once across begin()/reconnect() cycles
    bool eth_hostname_handler_registered_ = false; // for the ETHERNET_EVENT_START hostname handler
    bool ethernet_started_                = false; // ETH.begin() succeeded; the driver runs for the lifetime of the boot
    bool ethernet_init_failed_            = false; // ETH.begin() failed, so the driver was never installed
    bool wifi_ever_connected_             = false; // set true once we've successfully obtained an IP
    bool ethernet_ever_connected_         = false; // set true once we've successfully obtained an IP
    bool wifi_radio_off_                  = false; // STA powered down because Ethernet is carrying the traffic

    // Network and AP settings
    bool      enableMDNS_;
    bool      staticIPConfig_;
    IPAddress localIP_;
    IPAddress gatewayIP_;
    IPAddress subnetMask_;
    IPAddress dnsIP1_;
    IPAddress dnsIP2_;
    String    hostname_;
    String    ssid_;
    String    password_;
    bool      bandwidth20_;
    bool      nosleep_;
    uint8_t   tx_power_;
    String    bssid_;
    uint8_t   phy_type_;
    int8_t    eth_power_;
    uint8_t   eth_phy_addr_;
    uint8_t   eth_clock_mode_;
    bool      eth_10mbit_;

    // AP settings
    uint8_t   ap_provisionMode_;
    String    ap_ssid_;
    String    ap_password_;
    uint8_t   ap_channel_;
    bool      ap_ssid_hidden_;
    uint8_t   ap_max_clients_;
    IPAddress ap_localIP_;
    IPAddress ap_gatewayIP_;
    IPAddress ap_subnetMask_;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// for the captive portal in AP mode
#ifndef EMSESP_STANDALONE
    DNSServer * ap_dnsServer_ = nullptr;
#endif
};

} // namespace emsesp

#endif
