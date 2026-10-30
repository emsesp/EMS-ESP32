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
#if defined(BOARD_HAS_PSRAM) && defined(ENABLE_PSRAM)
    // A small receive buffer relies on the broker honouring the TLS max fragment length extension. Brokers that
    // ignore it send records of up to 16 KB, which fail the connection. With PSRAM the TLS buffers are allocated
    // there, so take a full-size receive buffer.
    client.setBufferSizes(ESP.getPsramSize() > 0 ? 16384 : EMC_RX_BUFFER_SIZE, EMC_TX_BUFFER_SIZE);
#else
    client.setBufferSizes(EMC_RX_BUFFER_SIZE, EMC_TX_BUFFER_SIZE);
#endif
    client.setSessionTimeout(120); // Set the timeout in seconds (>=120 seconds)
}

ClientSecureSync::~ClientSecureSync() {
    stop();
}

bool ClientSecureSync::connect(IPAddress ip, uint16_t port) {
    bool ret = client.connect(ip, port); // implicit conversion of return code int --> bool
    if (ret) {
        // Set TCP option directly to bypass lack of working setNoDelay for WiFiClientSecure
        int val = true;
        basic_client.setSocketOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    }
    return ret;
}

bool ClientSecureSync::connect(const char * host, uint16_t port) {
    bool ret = client.connect(host, port); // implicit conversion of return code int --> bool
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
