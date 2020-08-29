#ifndef JsonUtils_h
#define JsonUtils_h

#include <Arduino.h>
#include <IPAddress.h>
#include <ArduinoJson.h>

class JsonUtils {
 public:
  static void readIP(JsonObject& root, const String& key, IPAddress& ip, const String& def) {
    IPAddress defaultIp = {};
    if (!defaultIp.fromString(def)) {
      defaultIp = INADDR_NONE;
    }
    readIP(root, key, ip, defaultIp);
  }
  static void readIP(JsonObject& root, const String& key, IPAddress& ip, const IPAddress& defaultIp = INADDR_NONE) {
    if (!root[key].is<String>() || !ip.fromString(root[key].as<String>())) {
      ip = defaultIp;
    }
  }
  static void writeIP(JsonObject& root, const String& key, const IPAddress& ip) {
    if (ip != INADDR_NONE) {
      root[key] = ip.toString();
    }
  }
};

#endif  // end JsonUtils
