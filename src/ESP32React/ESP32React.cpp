#include "ESP32React.h"

#include "WWWData.h" // include auto-generated static web resources

ESP32React::ESP32React(AsyncWebServer * server, FS * fs)
    : _securitySettingsService(server, fs)
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
    //
    // Serve static web resources
    //

    // Populate the last modification date based on build datetime
    static char last_modified[50];
    sprintf(last_modified, "%s %s CET", __DATE__, __TIME__);

    WWWData::registerRoutes([server](const char * uri, const String & contentType, const uint8_t * content, size_t len, const String & hash) {
        ArRequestHandlerFunction requestHandler = [contentType, content, len, hash](AsyncWebServerRequest * request) {
            // Check if the client already has the same version and respond with a 304 (Not modified)
            if (request->header("If-Modified-Since").indexOf(last_modified) > 0) {
                return request->send(304);
            } else if (request->header("If-None-Match").equals(hash)) {
                return request->send(304);
            }

            AsyncWebServerResponse * response = request->beginResponse(200, contentType, content, len);

            response->addHeader("Content-Encoding", "gzip");
            // response->addHeader("Content-Encoding", "br"); // only works over HTTPS
            // response->addHeader("Cache-Control", "public, immutable, max-age=31536000");
            response->addHeader("Cache-Control", "must-revalidate"); // ensure that a client will check the server for a change
            response->addHeader("Last-Modified", last_modified);
            response->addHeader("ETag", hash);

            request->send(response);
        };

        server->on(uri, HTTP_GET, requestHandler);

        // Serving non matching get requests with "/index.html"
        // OPTIONS get a straight up 200 response
        if (strncmp(uri, "/index.html", 11) == 0) {
            server->onNotFound([requestHandler](AsyncWebServerRequest * request) {
                if (request->method() == HTTP_GET) {
                    requestHandler(request);
                } else if (request->method() == HTTP_OPTIONS) {
                    request->send(200);
                } else {
                    request->send(404);
                }
            });
        }
    });
}

void ESP32React::begin() {
    _networkSettingsService.begin();
    _networkSettingsService.read([&](NetworkSettings & networkSettings) {
        DefaultHeaders & defaultHeaders = DefaultHeaders::Instance();
        if (networkSettings.enableCORS) {
            defaultHeaders.addHeader("Access-Control-Allow-Origin", networkSettings.CORSOrigin);
            defaultHeaders.addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
            defaultHeaders.addHeader("Access-Control-Allow-Credentials", "true");
        }
        defaultHeaders.addHeader("Server", networkSettings.hostname);
    });
    _apSettingsService.begin();
    _ntpSettingsService.begin();
    _mqttSettingsService.begin();
    _securitySettingsService.begin();
}

void ESP32React::loop() {
    _networkSettingsService.loop();
    _apSettingsService.loop();
    _mqttSettingsService.loop();
}