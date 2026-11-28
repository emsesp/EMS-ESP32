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

#include "emsesp.h"
#include "WebFaultPackService.h"

namespace emsesp {

#define EMSESP_FAULTPACK_TEMP_FILE EMSESP_FAULTPACK_DIR "/upload.tmp"

bool WebFaultPackService::validName(const char * name) {
    if (name == nullptr) {
        return false;
    }
    const size_t len = strlen(name);
    if (len == 0 || len > MAX_NAME_LENGTH) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        const char c = name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) {
            return false;
        }
    }
    return true;
}

bool WebFaultPackService::validVersion(const char * version) {
    if (version == nullptr) {
        return false;
    }
    const size_t len = strlen(version);
    if (len == 0 || len > MAX_VERSION_LEN) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        const char c = version[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) {
            return false;
        }
    }
    return true;
}

// the file names are always built from the validated brand and language, never taken from a request
static String packFile(const char * brand, const char * lang) {
    return String(EMSESP_FAULTPACK_DIR) + "/" + brand + "." + lang + ".json";
}

static String packMeta(const char * brand, const char * lang) {
    return String(EMSESP_FAULTPACK_DIR) + "/" + brand + "." + lang + ".meta";
}

WebFaultPackService::WebFaultPackService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager) {
    // everybody who is signed in may read the packs, so guests see the explanations too
    securityManager->addEndpoint(server, EMSESP_FAULTPACKS_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        list(request);
    });
    securityManager->addEndpoint(server, EMSESP_FAULTPACK_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        send(request);
    });

    // installing and removing is for administrators
    securityManager->addEndpoint(
        server,
        EMSESP_FAULTPACK_DELETE_PATH,
        AuthenticationPredicates::IS_ADMIN,
        [this](AsyncWebServerRequest * request, JsonVariant json) { remove(request, json); },
        HTTP_POST);

#ifndef EMSESP_STANDALONE
    server->on(
        EMSESP_FAULTPACK_UPLOAD_PATH,
        HTTP_POST,
        [this](AsyncWebServerRequest * request) { uploadComplete(request); },
        [this](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final) {
            handleUpload(request, filename, index, data, len, final);
        });
#endif
}

// GET /rest/faultPacks
// {"packs":[{"brand":"buderus","lang":"de","version":"2026-11-22","entries":1160,"size":43000}],"fs_free":1800000,"max_size":262144}
void WebFaultPackService::list(AsyncWebServerRequest * request) {
    auto *     response = new PsramAsyncJsonResponse(false);
    JsonObject root     = response->getRoot();
    JsonArray  packs    = root["packs"].to<JsonArray>();

#ifndef EMSESP_STANDALONE
    File dir = LittleFS.open(EMSESP_FAULTPACK_DIR);
    if (dir && dir.isDirectory()) {
        for (File file = dir.openNextFile(); file; file = dir.openNextFile()) {
            String name = file.name();
            name        = name.substring(name.lastIndexOf('/') + 1); // some cores return the path
            if (!name.endsWith(".json")) {
                continue;
            }
            name.remove(name.length() - 5); // brand.lang
            const int dot = name.indexOf('.');
            if (dot < 1) {
                continue;
            }
            const String brand = name.substring(0, dot);
            const String lang  = name.substring(dot + 1);
            if (!validName(brand.c_str()) || !validName(lang.c_str())) {
                continue;
            }

            JsonObject pack = packs.add<JsonObject>();
            pack["brand"]   = brand;
            pack["lang"]    = lang;
            pack["size"]    = static_cast<uint32_t>(file.size());

            // version and number of entries were sent with the upload and are kept in a small file next to the pack
            File meta = LittleFS.open(packMeta(brand.c_str(), lang.c_str()), "r");
            if (meta) {
                JsonDocument doc;
                if (!deserializeJson(doc, meta)) {
                    pack["version"] = doc["version"] | "";
                    pack["entries"] = doc["entries"] | 0;
                }
                meta.close();
            }
        }
    }
    const size_t used = LittleFS.usedBytes();
    const size_t all  = LittleFS.totalBytes();
    root["fs_free"]   = all > used ? static_cast<uint32_t>(all - used) : 0;
#else
    root["fs_free"] = 0;
#endif
    root["max_size"] = static_cast<uint32_t>(MAX_PACK_SIZE);

    response->setLength();
    request->send(response);
}

// GET /rest/faultPack?brand=buderus&lang=de
// the stored file is already compressed, so it is sent as it is
void WebFaultPackService::send(AsyncWebServerRequest * request) {
    const AsyncWebParameter * brand = request->getParam("brand");
    const AsyncWebParameter * lang  = request->getParam("lang");
    if (!brand || !lang || !validName(brand->value().c_str()) || !validName(lang->value().c_str())) {
        request->send(400);
        return;
    }
#ifndef EMSESP_STANDALONE
    const String path = packFile(brand->value().c_str(), lang->value().c_str());
    if (!LittleFS.exists(path)) {
        request->send(404);
        return;
    }
    AsyncWebServerResponse * response = request->beginResponse(LittleFS, path, "application/json");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
#else
    request->send(404);
#endif
}

// POST /rest/deleteFaultPack {"brand":"buderus","lang":"de"}
void WebFaultPackService::remove(AsyncWebServerRequest * request, JsonVariant json) {
    const char * brand = json["brand"] | "";
    const char * lang  = json["lang"] | "";
    if (!validName(brand) || !validName(lang)) {
        request->send(400);
        return;
    }
#ifndef EMSESP_STANDALONE
    const String path = packFile(brand, lang);
    if (!LittleFS.exists(path)) {
        request->send(404);
        return;
    }
    LittleFS.remove(path);
    LittleFS.remove(packMeta(brand, lang));
    EMSESP::logger().info("Fault code pack %s (%s) removed", brand, lang);
    request->send(200);
#else
    request->send(404);
#endif
}

#ifndef EMSESP_STANDALONE

// number of packs on the file system and the size of the one that is replaced, if there is one
static size_t countPacks(const String & replace, size_t & replaceSize) {
    size_t count = 0;
    replaceSize  = 0;
    File dir     = LittleFS.open(EMSESP_FAULTPACK_DIR);
    if (!dir || !dir.isDirectory()) {
        return 0;
    }
    for (File file = dir.openNextFile(); file; file = dir.openNextFile()) {
        String name = file.name();
        name        = name.substring(name.lastIndexOf('/') + 1);
        if (name.endsWith(".json")) {
            count++;
            if (String(EMSESP_FAULTPACK_DIR) + "/" + name == replace) {
                replaceSize = file.size();
            }
        }
    }
    return count;
}

// POST /rest/uploadFaultPack?brand=buderus&lang=de&version=2026-11-22&entries=1160
// the body is one gzip compressed pack file; it is written to a temporary file and renamed when it is complete
void WebFaultPackService::handleUpload(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final) {
    // an error that was already sent stops the later chunks
    if (request->_tempObject) {
        return;
    }

    if (!index) {
        Authentication authentication = _securityManager->authenticateRequest(request);
        if (!AuthenticationPredicates::IS_ADMIN(authentication)) {
            uploadError(request, 403);
            return;
        }

        const AsyncWebParameter * brand = request->getParam("brand");
        const AsyncWebParameter * lang  = request->getParam("lang");
        const AsyncWebParameter * ver   = request->getParam("version");
        if (!brand || !lang || !ver || !validName(brand->value().c_str()) || !validName(lang->value().c_str()) || !validVersion(ver->value().c_str())) {
            EMSESP::logger().err("Fault code pack upload: brand, language or version missing or not valid");
            uploadError(request, 400);
            return;
        }

        // the content length includes a little overhead of the form
        const size_t size = request->contentLength();
        if (size == 0 || size > MAX_PACK_SIZE + 1024) {
            EMSESP::logger().err("Fault code pack too large: %u bytes", static_cast<unsigned>(size));
            uploadError(request, 413);
            return;
        }

        // only a gzip file is accepted
        if (len < 2 || data[0] != 0x1f || data[1] != 0x8b) {
            EMSESP::logger().err("Fault code pack is not a gzip file");
            uploadError(request, 415);
            return;
        }

        LittleFS.mkdir(EMSESP_FAULTPACK_DIR);

        size_t replaceSize = 0;
        const size_t count = countPacks(packFile(brand->value().c_str(), lang->value().c_str()), replaceSize);
        if (count >= MAX_PACKS && replaceSize == 0) {
            EMSESP::logger().err("Too many fault code packs");
            uploadError(request, 507);
            return;
        }

        const size_t used = LittleFS.usedBytes();
        const size_t all  = LittleFS.totalBytes();
        const size_t freeBytes = (all > used ? all - used : 0) + replaceSize;
        if (freeBytes < size + MIN_FREE_AFTER) {
            EMSESP::logger().err("Not enough space for the fault code pack (%u KB free)", static_cast<unsigned>(freeBytes / 1024));
            uploadError(request, 507); // Insufficient Storage
            return;
        }

        request->_tempFile = LittleFS.open(EMSESP_FAULTPACK_TEMP_FILE, "w");
        if (!request->_tempFile) {
            uploadError(request, 500);
            return;
        }
    }

    if (len && len != request->_tempFile.write(data, len)) {
        uploadError(request, 507);
        return;
    }
    if (index + len > MAX_PACK_SIZE) {
        uploadError(request, 413);
    }
}

void WebFaultPackService::uploadError(AsyncWebServerRequest * request, int code) {
    // the request frees _tempObject, so it has to be heap memory
    auto * error = static_cast<int *>(malloc(sizeof(int)));
    if (error) {
        *error = code;
    }
    request->_tempObject = error;
    if (request->_tempFile) {
        request->_tempFile.close();
    }
    LittleFS.remove(EMSESP_FAULTPACK_TEMP_FILE);
}

void WebFaultPackService::uploadComplete(AsyncWebServerRequest * request) {
    if (request->_tempObject) {
        request->send(request->beginResponse(*static_cast<int *>(request->_tempObject)));
        return;
    }

    // a request without a file part never reaches handleUpload
    Authentication authentication = _securityManager->authenticateRequest(request);
    if (!AuthenticationPredicates::IS_ADMIN(authentication)) {
        request->send(403);
        return;
    }
    if (!request->_tempFile) {
        EMSESP::logger().err("Fault code pack upload: no file received");
        request->send(400);
        return;
    }

    // the size of the written file, read after closing it
    request->_tempFile.close();
    size_t size = 0;
    {
        File written = LittleFS.open(EMSESP_FAULTPACK_TEMP_FILE, "r");
        if (written) {
            size = written.size();
            written.close();
        }
    }

    const AsyncWebParameter * brand   = request->getParam("brand");
    const AsyncWebParameter * lang    = request->getParam("lang");
    const AsyncWebParameter * ver     = request->getParam("version");
    const AsyncWebParameter * entries = request->getParam("entries");
    if (!brand || !lang || !ver || size == 0) {
        EMSESP::logger().err("Fault code pack upload: missing details or empty file (%u bytes)", static_cast<unsigned>(size));
        LittleFS.remove(EMSESP_FAULTPACK_TEMP_FILE);
        request->send(400);
        return;
    }

    const String path = packFile(brand->value().c_str(), lang->value().c_str());
    LittleFS.remove(path); // rename doesn't replace an existing file
    if (!LittleFS.rename(EMSESP_FAULTPACK_TEMP_FILE, path)) {
        LittleFS.remove(EMSESP_FAULTPACK_TEMP_FILE);
        EMSESP::logger().err("Failed to store the fault code pack");
        request->send(500);
        return;
    }

    File meta = LittleFS.open(packMeta(brand->value().c_str(), lang->value().c_str()), "w");
    if (meta) {
        JsonDocument doc;
        doc["version"] = ver->value();
        doc["entries"] = entries ? entries->value().toInt() : 0;
        serializeJson(doc, meta);
        meta.close();
    }

    EMSESP::logger().info("Fault code pack %s (%s) installed, %u KB", brand->value().c_str(), lang->value().c_str(), static_cast<unsigned>((size + 1023) / 1024));
    request->send(200);
}

#endif

} // namespace emsesp
