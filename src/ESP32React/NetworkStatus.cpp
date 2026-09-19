#include "NetworkStatus.h"

#include <emsesp.h>

namespace {

void add_ipv6(JsonObject root, NetworkInterface & netif) {
    auto found = emsesp::Network::ipv6_addresses(netif);
    if (found.empty()) {
        return;
    }
    JsonArray addresses = root["ipv6"].to<JsonArray>();
    for (const auto & ipv6 : found) {
        JsonObject entry = addresses.add<JsonObject>();
        entry["address"] = ipv6.ip.toString();
        entry["scope"]   = ipv6.scope;
    }
}

void add_dns(JsonObject root, const NetworkInterface & netif) {
    JsonArray servers = root["dns"].to<JsonArray>();
    for (const auto & ip : emsesp::Network::dns_servers(netif)) {
        servers.add(ip.toString());
    }
}

} // namespace

NetworkStatus::NetworkStatus(AsyncWebServer * server, SecurityManager * securityManager) {
    securityManager->addEndpoint(server, NETWORK_STATUS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        networkStatus(request);
    });
}

void NetworkStatus::networkStatus(AsyncWebServerRequest * request) {
    auto *     response = new emsesp::PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    wl_status_t wifi_status = WiFi.status();

    // see if Ethernet is connected
    if (emsesp::EMSESP::network_.ethernet_connected()) {
        root["status"]   = 10; // custom code #10 - ETHERNET_STATUS_CONNECTED
        root["hostname"] = ETH.getHostname();
    } else {
        root["status"]   = static_cast<uint8_t>(wifi_status);
        root["hostname"] = WiFi.getHostname();
    }

    // for both connections show ethernet
    if (emsesp::EMSESP::network_.ethernet_connected()) {
        // Ethernet
        root["local_ip"]    = ETH.localIP().toString();
        root["mac_address"] = ETH.macAddress();
        root["subnet_mask"] = ETH.subnetMask().toString();
        if (IPUtils::isSet(ETH.gatewayIP())) {
            root["gateway_ip"] = ETH.gatewayIP().toString();
        }
        add_ipv6(root, ETH);
        add_dns(root, ETH);
    } else if (wifi_status == WL_CONNECTED) {
        root["local_ip"]        = WiFi.localIP().toString();
        root["mac_address"]     = WiFi.macAddress();
        root["rssi"]            = WiFi.RSSI();
        root["ssid"]            = WiFi.SSID();
        root["bssid"]           = WiFi.BSSIDstr();
        root["channel"]         = WiFi.channel();
        root["reconnect_count"] = emsesp::EMSESP::network_.getNetworkReconnects();
        root["subnet_mask"]     = WiFi.subnetMask().toString();
        if (IPUtils::isSet(WiFi.gatewayIP())) {
            root["gateway_ip"] = WiFi.gatewayIP().toString();
        }
        add_ipv6(root, WiFi.STA);
        add_dns(root, WiFi.STA);
    }

    response->setLength();
    request->send(response);
}
