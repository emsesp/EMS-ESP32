#include "NetworkSettingsService.h"

#include <emsesp.h>

NetworkSettingsService::NetworkSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(NetworkSettings::read, NetworkSettings::update, this, server, NETWORK_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(NetworkSettings::read, NetworkSettings::update, this, fs, NETWORK_SETTINGS_FILE) {
}

void NetworkSettingsService::begin() {
    _fsPersistence.readFromFS();
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
    root["eth_10mbit"]       = settings.eth_10mbit;

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
    settings.hostname       = root["hostname"] | FACTORY_NETWORK_HOSTNAME;
    settings.staticIPConfig = root["static_ip_config"];
    settings.bandwidth20    = root["bandwidth20"];
    settings.tx_power       = static_cast<uint8_t>(root["tx_power"] | 0);
    settings.nosleep        = root["nosleep"] | true;
    settings.enableMDNS     = root["enableMDNS"] | true;
    settings.enableCORS     = root["enableCORS"];
    settings.CORSOrigin     = root["CORSOrigin"] | "*";
    settings.eth_10mbit     = root["eth_10mbit"];

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

    // always best to do a restart after changing network settings
    return StateUpdateResult::CHANGED_RESTART;
}