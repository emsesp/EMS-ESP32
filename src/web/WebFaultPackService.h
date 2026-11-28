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

#ifndef WebFaultPackService_h
#define WebFaultPackService_h

// Fault code packs: the explanations of a manufacturer's fault codes, one file per brand and language.
// The Web UI downloads a pack from a source the user chooses and uploads it here. The gateway only stores
// the (gzip compressed) file and sends it back as is; it never parses it or keeps it in RAM.
// See faults/README.md for the format.

#define EMSESP_FAULTPACKS_SERVICE_PATH "/rest/faultPacks"          // GET: the installed packs and the free space
#define EMSESP_FAULTPACK_SERVICE_PATH "/rest/faultPack"            // GET: one pack file
#define EMSESP_FAULTPACK_UPLOAD_PATH "/rest/uploadFaultPack"       // POST: install a pack (multipart, admin only)
#define EMSESP_FAULTPACK_DELETE_PATH "/rest/deleteFaultPack"       // POST: remove a pack (admin only)

#define EMSESP_FAULTPACK_DIR "/faults"

namespace emsesp {

class WebFaultPackService {
  public:
    // limits, also used by the tests
    static constexpr size_t MAX_PACK_SIZE   = 256 * 1024; // largest pack (compressed) that is accepted
    static constexpr size_t MIN_FREE_AFTER  = 32 * 1024;  // free space that must stay after an install
    static constexpr size_t MAX_PACKS       = 16;         // number of packs on the file system
    static constexpr size_t MAX_NAME_LENGTH = 24;         // brand and language
    static constexpr size_t MAX_VERSION_LEN = 32;

    WebFaultPackService(AsyncWebServer * server, SecurityManager * securityManager);

    // true when brand or language only has the characters a-z, 0-9 and -
    static bool validName(const char * name);
    // true when the version only has the characters A-Z, a-z, 0-9, . _ -
    static bool validVersion(const char * version);

  private:
    SecurityManager * _securityManager;

    void list(AsyncWebServerRequest * request);
    void send(AsyncWebServerRequest * request);
    void remove(AsyncWebServerRequest * request, JsonVariant json);

#ifndef EMSESP_STANDALONE
    void handleUpload(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final);
    void uploadComplete(AsyncWebServerRequest * request);
    void uploadError(AsyncWebServerRequest * request, int code);
#endif
};

} // namespace emsesp

#endif
