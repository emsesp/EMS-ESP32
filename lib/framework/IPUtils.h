#ifndef IPUtils_h
#define IPUtils_h

#include <IPAddress.h>

const IPAddress IP_NOT_SET = IPAddress(INADDR_NONE);

class IPUtils {
  public:
    static bool isSet(const IPAddress & ip) {
        return ip != IP_NOT_SET;
    }
    static bool isNotSet(const IPAddress & ip) {
        return ip == IP_NOT_SET;
    }
};

#endif // end IPUtils_h
