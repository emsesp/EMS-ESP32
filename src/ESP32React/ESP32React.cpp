#include "ESP32React.h"

#include "WWWData.h" // include auto-generated static web resources

#include <string.h>

static constexpr const char CACHE_CONTROL[] = "public,max-age=60";

// Single static-content handler serving all assets embedded in WWWData.h
class StaticContentHandler : public AsyncWebHandler {
  public:
    bool canHandle(AsyncWebServerRequest * request) const override {
        const auto method = request->method();
        return method == HTTP_GET || method == HTTP_HEAD || method == HTTP_OPTIONS;
    }

    void handleRequest(AsyncWebServerRequest * request) override {
        // OPTIONS is handled generically - the server-level CORS headers are attached via DefaultHeaders in ESP32React::begin().
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
            return;
        }

        const char *     url   = request->url().c_str();
        const WWWAsset * found = lookup(url);
        const WWWAsset * asset = found ? found : index_asset();

        if (asset == nullptr) {
            request->send(404);
            return;
        }

        // If the client already has this exact ETag, respond 304 Not Modified without sending the body.
        const String & inm = request->header(asyncsrv::T_INM);
        if (inm.length() != 0 && strcmp(inm.c_str(), asset->etag) == 0) {
            request->send(304);
            return;
        }

        AsyncWebServerResponse * response = request->beginResponse(200, asset->contentType, asset->content, asset->len);
        response->addHeader(asyncsrv::T_Content_Encoding, asyncsrv::T_gzip, false);
        response->addHeader(asyncsrv::T_ETag, asset->etag, false);
        response->addHeader(asyncsrv::T_Cache_Control, CACHE_CONTROL, false);
        request->send(response);
    }

  private:
    // Exact-match lookup in the asset table
    static const WWWAsset * lookup(const char * url) {
        for (size_t i = 0; i < WWW_ASSETS_COUNT; i++) {
            if (strcmp(WWW_ASSETS[i].uri, url) == 0) {
                return &WWW_ASSETS[i];
            }
        }

        return nullptr;
    }

    // Returns the /index.html asset, used as the SPA fallback for any GET
    // that didn't match an embedded asset (React Router handles routing on the client side).
    static const WWWAsset * index_asset() {
        static const WWWAsset * cached = nullptr;
        if (cached == nullptr) {
            cached = lookup("/index.html");
        }

        return cached;
    }
};

ESP32React::ESP32React(AsyncWebServer * server, FS * fs)
    : _server(server)
    , _securitySettingsService(server, fs)
    , _networkSettingsService(server, fs, &_securitySettingsService)
    , _wifiScanner(server, &_securitySettingsService)
    , _networkStatus(server, &_securitySettingsService)
    , _apSettingsService(server, fs, &_securitySettingsService)
    , _apStatus(server, &_securitySettingsService, &_apSettingsService)
    , _ntpSettingsService(server, fs, &_securitySettingsService)
    , _ntpStatus(server, &_securitySettingsService)
    , _uploadFileService(server, &_securitySettingsService)
    , _mqttSettingsService(server, fs, &_securitySettingsService)
    , _mqttStatus(server, &_mqttSettingsService, &_securitySettingsService)
    , _authenticationService(server, &_securitySettingsService) {
}

void ESP32React::begin() {
    _networkSettingsService.begin();
    _networkSettingsService.read([&](NetworkSettings & networkSettings) {
        DefaultHeaders & defaultHeaders = DefaultHeaders::Instance();
        if (networkSettings.enableCORS) {
            defaultHeaders.addHeader(asyncsrv::T_CORS_ACAO, networkSettings.CORSOrigin);
            defaultHeaders.addHeader(asyncsrv::T_CORS_ACAH, "Accept, Content-Type, Authorization");
            defaultHeaders.addHeader(asyncsrv::T_CORS_ACAC, "true");
        }

        defaultHeaders.addHeader(asyncsrv::T_Server, networkSettings.hostname);
    });
    _apSettingsService.begin();
    _ntpSettingsService.begin();
    _mqttSettingsService.begin();
    _securitySettingsService.begin();
    _server->addHandler(new StaticContentHandler());
}

void ESP32React::loop() {
    _mqttSettingsService.loop();
    _ntpSettingsService.loop();
}
