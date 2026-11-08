#ifndef JsonUtils_h
#define JsonUtils_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>

#include "IPUtils.h"

class JsonUtils {
  public:
    static void readIP(JsonObject root, const String & key, IPAddress & ip, const String & def) {
        IPAddress defaultIp{};
        if (!defaultIp.fromString(def)) {
            defaultIp = INADDR_NONE;
        }

        readIP(root, key, ip, defaultIp);
    }

    static void readIP(JsonObject root, const String & key, IPAddress & ip, const IPAddress & defaultIp = INADDR_NONE) {
        const JsonVariant value = root[key];
        if (!value.is<String>() || !ip.fromString(value.as<const char *>())) {
            ip = defaultIp;
        }
    }

    static void writeIP(JsonObject root, const String & key, const IPAddress & ip) {
        if (IPUtils::isSet(ip)) {
            char ipStr[16];
            snprintf(ipStr, sizeof(ipStr), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
            root[key] = ipStr;
        }
    }
};

#endif
