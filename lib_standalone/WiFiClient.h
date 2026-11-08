// standalone stub for WiFiClient (no-op networking on host build)

#ifndef WiFiClient_h
#define WiFiClient_h

#include <stddef.h>
#include <stdint.h>

#include "Arduino.h"
#include <ClientPosixIPAddress.h>

class WiFiClient {
  public:
    WiFiClient() = default;

    int connect(IPAddress, uint16_t) {
        return 0;
    }
    int connect(const char *, uint16_t) {
        return 0;
    }
    bool connected() {
        return false;
    }
    int available() {
        return 0;
    }
    int read() {
        return -1;
    }
    int read(uint8_t *, size_t) {
        return 0;
    }
    size_t write(uint8_t) {
        return 0;
    }
    size_t write(const uint8_t *, size_t) {
        return 0;
    }
    void stop() {
    }
    void setTimeout(uint32_t){
        
    }
    void setConnectionTimeout(uint32_t) {
    }
    // ESP32 socket option passthrough (e.g. TCP_NODELAY)
    int setSocketOption(int, int, const void *, size_t) {
        return 0;
    }
};

#endif // WiFiClient_h
