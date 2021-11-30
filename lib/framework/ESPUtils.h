#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>

class ESPUtils {
  public:
    static String defaultDeviceValue(String prefix = "") {
        return prefix + String((uint32_t)ESP.getEfuseMac(), HEX);
    }
};

#endif
