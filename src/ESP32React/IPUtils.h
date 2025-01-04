#ifndef IPUtils_h
#define IPUtils_h

#include <IPAddress.h>

class IPUtils {
  public:
    static bool isSet(const IPAddress & ip) {
        return ip != getNotSetIP();
    }
    static bool isNotSet(const IPAddress & ip) {
        return ip == getNotSetIP();
    }

  private:
    static const IPAddress & getNotSetIP() {
        static const IPAddress IP_NOT_SET = IPAddress(INADDR_NONE);
        return IP_NOT_SET;
    }
};

#endif