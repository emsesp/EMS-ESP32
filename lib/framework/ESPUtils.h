#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>

class ESPUtils {
  public:
    static String defaultDeviceValue(String prefix = "") {
#ifdef ESP32
        return prefix + String((uint32_t)ESP.getEfuseMac(), HEX);
#elif defined(ESP8266)
        return prefix + String(ESP.getChipId(), HEX);
#endif
    }
};

#endif // end ESPUtils
