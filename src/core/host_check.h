#ifndef EMSESP_HOST_CHECK_H
#define EMSESP_HOST_CHECK_H

#include <cctype>
#include <cstring>
#include <string>
#include <strings.h>

namespace emsesp {

// against DNS rebinding: a browser may only reach the device by its IP address, its hostname, <hostname>.local
// or one of the extra names (comma or space separated, for reverse proxies and router DNS names)
inline bool host_allowed(const char * host_header, const char * hostname, const char * extra_hosts) {
    if (host_header == nullptr || *host_header == '\0') {
        return true;
    }

    // an IPv6 address literal can't be rebound
    if (*host_header == '[') {
        return true;
    }

    std::string host(host_header);
    auto        colon = host.find(':');
    if (colon != std::string::npos) {
        host.erase(colon);
    }

    // an IPv4 address literal can't be rebound
    bool ipv4 = !host.empty();
    for (char c : host) {
        if (!isdigit(static_cast<unsigned char>(c)) && c != '.') {
            ipv4 = false;
            break;
        }
    }
    if (ipv4) {
        return true;
    }

    if (hostname && *hostname) {
        if (strcasecmp(host.c_str(), hostname) == 0) {
            return true;
        }
        std::string mdns = std::string(hostname) + ".local";
        if (strcasecmp(host.c_str(), mdns.c_str()) == 0) {
            return true;
        }
    }

    if (extra_hosts) {
        const char * p = extra_hosts;
        while (*p) {
            while (*p == ',' || *p == ' ') {
                p++;
            }
            const char * start = p;
            while (*p && *p != ',' && *p != ' ') {
                p++;
            }
            if (p > start && static_cast<size_t>(p - start) == host.size() && strncasecmp(start, host.c_str(), host.size()) == 0) {
                return true;
            }
        }
    }

    return false;
}

} // namespace emsesp

#endif
