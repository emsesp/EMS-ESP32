#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>

class ESPUtils {
  public:
    static String defaultDeviceValue(const String & prefix = "") {
        return prefix + String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);
    }
};

#endif
