/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMSESP_FIRMWARE_URL_H
#define EMSESP_FIRMWARE_URL_H

#include <cstddef>
#include <cstring>
#include <strings.h>

// Firmware can only be installed from a URL of an EMS-ESP release on GitHub, other files have to be uploaded
namespace FirmwareURL {

constexpr const char * RELEASES = "https://github.com/emsesp/EMS-ESP32/releases/";

// where GitHub redirects release downloads to
constexpr const char * DOWNLOAD_HOSTS[] = {"release-assets.githubusercontent.com", "objects.githubusercontent.com"};

// no spaces or control characters (they would end up in the request line), no user info, and no ".." or
// escapes: the path must stay in the releases
inline bool clean(const char * url) {
    for (const char * c = url; *c; c++) {
        if (static_cast<unsigned char>(*c) <= 0x20 || static_cast<unsigned char>(*c) >= 0x7f || *c == '\\' || *c == '@' || *c == '%') {
            return false;
        }
    }
    return strstr(url, "..") == nullptr;
}

// the URL given to install from
inline bool allowed(const char * url) {
    return url && clean(url) && strncasecmp(url, RELEASES, strlen(RELEASES)) == 0 && url[strlen(RELEASES)] != '\0';
}

// a URL that the download is redirected to. Those hosts serve files of any repository, but only the redirect
// of an EMS-ESP release leads there
inline bool redirect_allowed(const char * url) {
    if (allowed(url)) {
        return true;
    }
    if (!url || !clean(url) || strncasecmp(url, "https://", 8) != 0) {
        return false;
    }
    const char * host = url + 8;
    for (const char * download_host : DOWNLOAD_HOSTS) {
        const size_t len = strlen(download_host);
        if (strncasecmp(host, download_host, len) == 0 && host[len] == '/') {
            return true;
        }
    }
    return false;
}

} // namespace FirmwareURL

#endif
