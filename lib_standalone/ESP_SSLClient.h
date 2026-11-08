// standalone stub for ESP_SSLClient (BearSSL) - no-op TLS on host build
// Inherits from Stream so it gets print/println via Print, matching the real API.

#ifndef ESP_SSLClient_h
#define ESP_SSLClient_h

#include <stddef.h>
#include <stdint.h>

#include "Arduino.h"
#include "Stream.h"
#include "WiFiClient.h"
#include <ClientPosixIPAddress.h>

class ESP_SSLClient : public Stream {
  public:
    ESP_SSLClient() = default;

    void setInsecure() {
    }
    void setCACert(const char *) {
    }
    void setCertificate(const char *) {
    }
    void setPrivateKey(const char *) {
    }
    void setBufferSizes(int, int) {
    }
    void setSessionTimeout(int) {
    }
    void setClient(WiFiClient *) {
    }
    void setClient(WiFiClient *, bool) {
    }

    int connect(IPAddress, uint16_t) {
        return 0;
    }
    int connect(const char *, uint16_t) {
        return 0;
    }
    bool connected() {
        return false;
    }
    bool isSecure() {
        return false;
    }
    bool connectSSL() {
        return false;
    }

    // Stream / Print overrides
    int available() override {
        return 0;
    }
    int read() override {
        return -1;
    }
    int read(uint8_t *, size_t) {
        return 0;
    }
    int peek() override {
        return -1;
    }
    size_t write(uint8_t) override {
        return 0;
    }
    size_t write(const uint8_t *, size_t) override {
        return 0;
    }
    void flush() override {
    }
    void stop() {
    }
    void setTimeout(uint32_t) {

    }
};

#endif // ESP_SSLClient_h
