#include <NetworkStatus.h>

#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders; // for `_1` etc

NetworkStatus::NetworkStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    server->on(NETWORK_STATUS_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&NetworkStatus::networkStatus, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
}

void NetworkStatus::networkStatus(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, MAX_NETWORK_STATUS_SIZE);
    JsonObject          root     = response->getRoot();

    bool        ethernet_connected = emsesp::EMSESP::system_.ethernet_connected();
    wl_status_t wifi_status        = WiFi.status();

    // see if Ethernet is connected
    if (ethernet_connected) {
        root["status"] = 10; // custom code #10 - ETHERNET_STATUS_CONNECTED
    } else {
        root["status"] = (uint8_t)wifi_status;
    }

    // for Wifi
    if (wifi_status == WL_CONNECTED) {
        root["local_ip"]    = WiFi.localIP().toString();
        root["local_ipv6"]  = WiFi.localIPv6().toString();
        root["mac_address"] = WiFi.macAddress();
        root["rssi"]        = WiFi.RSSI();
        root["ssid"]        = WiFi.SSID();
        root["bssid"]       = WiFi.BSSIDstr();
        root["channel"]     = WiFi.channel();
        root["subnet_mask"] = WiFi.subnetMask().toString();

        if (WiFi.gatewayIP() != INADDR_NONE) {
            root["gateway_ip"] = WiFi.gatewayIP().toString();
        }

        IPAddress dnsIP1 = WiFi.dnsIP(0);
        IPAddress dnsIP2 = WiFi.dnsIP(1);
        if (dnsIP1 != INADDR_NONE) {
            root["dns_ip_1"] = dnsIP1.toString();
        }
        if (dnsIP2 != INADDR_NONE) {
            root["dns_ip_2"] = dnsIP2.toString();
        }
    } else if (ethernet_connected) {
        // Ethernet
        root["local_ip"]    = ETH.localIP().toString();
        root["local_ipv6"]  = ETH.localIPv6().toString();
        root["mac_address"] = ETH.macAddress();
        root["subnet_mask"] = ETH.subnetMask().toString();
        root["gateway_ip"]  = ETH.gatewayIP().toString();
        IPAddress dnsIP1    = ETH.dnsIP(0);
        IPAddress dnsIP2    = ETH.dnsIP(1);
        if (IPUtils::isSet(dnsIP1)) {
            root["dns_ip_1"] = dnsIP1.toString();
        }
        if (IPUtils::isSet(dnsIP2)) {
            root["dns_ip_2"] = dnsIP2.toString();
        }
    }

    response->setLength();
    request->send(response);
}
