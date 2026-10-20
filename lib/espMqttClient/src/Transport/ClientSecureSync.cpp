/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "ClientSecureSync.h"
#include <lwip/sockets.h>
#include "../Config.h"

namespace espMqttClientInternals {

ClientSecureSync::ClientSecureSync()
    : client() {
    client.setClient(&basic_client, true);
    client.setBufferSizes(EMC_RX_BUFFER_SIZE, EMC_TX_BUFFER_SIZE);
    client.setSessionTimeout(120); // Set the timeout in seconds (>=120 seconds)
}

ClientSecureSync::~ClientSecureSync() {
    stop();
}

// ESP_SSLClient::connect() only starts TLS on its built-in list of well-known ports (8883 among them) and stays
// plain TCP on any other port, which would send the MQTT credentials in clear. Finish the TLS handshake here instead.
bool ClientSecureSync::connect(IPAddress ip, uint16_t port) {
    // shortcut: connectSSL() verifies against the host of the last connect by name (none if there was none), fine while setServer() is not switched from a host to an IP
    bool ret = client.connect(ip, port) && (client.isSecure() || client.connectSSL());
    if (ret) {
        // Set TCP option directly to bypass lack of working setNoDelay for WiFiClientSecure
        int val = true;
        basic_client.setSocketOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    }
    return ret;
}

bool ClientSecureSync::connect(const char * host, uint16_t port) {
    bool ret = client.connect(host, port) && (client.isSecure() || client.connectSSL());
    if (ret) {
        // Set TCP option directly to bypass lack of working setNoDelay for WiFiClientSecure
        int val = true;
        basic_client.setSocketOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    }
    return ret;
}

size_t ClientSecureSync::write(const uint8_t * buf, size_t size) {
    return client.write(buf, size);
}

int ClientSecureSync::read(uint8_t * buf, size_t size) {
    return client.read(buf, size);
}

void ClientSecureSync::stop() {
    client.stop();
}

bool ClientSecureSync::connected() {
    return client.connected();
}

bool ClientSecureSync::disconnected() {
    return !client.connected();
}

} // namespace espMqttClientInternals
