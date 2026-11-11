#ifndef EMSESP_ORIGIN_CHECK_H
#define EMSESP_ORIGIN_CHECK_H

#include <cstring>
#include <strings.h>

namespace emsesp {

// a page from another site may only use the API from a browser when the CORS setting allows its origin.
// Requests without an Origin header (scripts, Home Assistant, curl) and the Web UI itself are always allowed.
// This only limits browsers, it is not authentication
inline bool origin_allowed(const char * origin, const char * host, bool cors_enabled, const char * cors_origin) {
    if (origin == nullptr || *origin == '\0') {
        return true;
    }

    const char * authority = nullptr;
    if (strncmp(origin, "http://", 7) == 0) {
        authority = origin + 7;
    } else if (strncmp(origin, "https://", 8) == 0) {
        authority = origin + 8;
    }
    if (authority && host && *host && strcasecmp(authority, host) == 0) {
        return true;
    }

    if (!cors_enabled || cors_origin == nullptr || *cors_origin == '\0') {
        return false;
    }
    return strcmp(cors_origin, "*") == 0 || strcasecmp(cors_origin, origin) == 0;
}

} // namespace emsesp

#endif
