#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>

class ESPUtils {
  public:
    static String defaultDeviceValue(const String & prefix = "") {
#ifndef EMSESP_STANDALONE
        return prefix + String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);
#else
        return "ems-esp";
#endif
    }
};

#endif
