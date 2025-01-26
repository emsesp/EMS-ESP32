#include "NetworkStatus.h"

#include <emsesp.h>

#ifdef TASMOTA_SDK
#include "lwip/dns.h"
#endif

NetworkStatus::NetworkStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    securityManager->addEndpoint(server, NETWORK_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        networkStatus(request);
    });
}

void NetworkStatus::networkStatus(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    bool        ethernet_connected = emsesp::EMSESP::system_.ethernet_connected();
    wl_status_t wifi_status        = WiFi.status();

    // see if Ethernet is connected
    if (ethernet_connected) {
        root["status"]   = 10; // custom code #10 - ETHERNET_STATUS_CONNECTED
        root["hostname"] = ETH.getHostname();
    } else {
        root["status"]   = static_cast<uint8_t>(wifi_status);
        root["hostname"] = WiFi.getHostname();
    }

    // for both connections show ethernet
    if (ethernet_connected) {
        // Ethernet
        root["local_ip"] = ETH.localIP().toString();
#if ESP_IDF_VERSION_MAJOR < 5
        root["local_ipv6"] = ETH.localIPv6().toString();
#else
        root["local_ipv6"] = ETH.linkLocalIPv6().toString();
#endif
        root["mac_address"] = ETH.macAddress();
        root["subnet_mask"] = ETH.subnetMask().toString();
        root["gateway_ip"]  = ETH.gatewayIP().toString();
#ifdef TASMOTA_SDK
        IPAddress dnsIP1 = IPAddress(dns_getserver(0));
        IPAddress dnsIP2 = IPAddress(dns_getserver(1));
#else
        IPAddress dnsIP1 = ETH.dnsIP(0);
        IPAddress dnsIP2 = ETH.dnsIP(1);
#endif
        if (IPUtils::isSet(dnsIP1)) {
            root["dns_ip_1"] = dnsIP1.toString();
        }
        if (IPUtils::isSet(dnsIP2)) {
            root["dns_ip_2"] = dnsIP2.toString();
        }
    } else if (wifi_status == WL_CONNECTED) {
        root["local_ip"] = WiFi.localIP().toString();
// #if ESP_ARDUINO_VERSION_MAJOR < 3
#if ESP_IDF_VERSION_MAJOR < 5
        root["local_ipv6"] = WiFi.localIPv6().toString();
#else
        root["local_ipv6"] = WiFi.linkLocalIPv6().toString();
#endif
        root["mac_address"]     = WiFi.macAddress();
        root["rssi"]            = WiFi.RSSI();
        root["ssid"]            = WiFi.SSID();
        root["bssid"]           = WiFi.BSSIDstr();
        root["channel"]         = WiFi.channel();
        root["reconnect_count"] = emsesp::EMSESP::esp32React.getWifiReconnects();
        root["subnet_mask"]     = WiFi.subnetMask().toString();

        if (WiFi.gatewayIP() != INADDR_NONE) {
            root["gateway_ip"] = WiFi.gatewayIP().toString();
        }

#ifdef TASMOTA_SDK
        IPAddress dnsIP1 = IPAddress(dns_getserver(0));
        IPAddress dnsIP2 = IPAddress(dns_getserver(1));
#else
        IPAddress dnsIP1 = WiFi.dnsIP(0);
        IPAddress dnsIP2 = WiFi.dnsIP(1);
#endif
        if (dnsIP1 != INADDR_NONE) {
            root["dns_ip_1"] = dnsIP1.toString();
        }
        if (dnsIP2 != INADDR_NONE) {
            root["dns_ip_2"] = dnsIP2.toString();
        }
    }

    response->setLength();
    request->send(response);
}
