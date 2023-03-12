#include <ESP8266React.h>

#include <WWWData.h>

ESP8266React::ESP8266React(AsyncWebServer * server, FS * fs)
    : _featureService(server)
    , _securitySettingsService(server, fs)
    , _networkSettingsService(server, fs, &_securitySettingsService)
    , _wifiScanner(server, &_securitySettingsService)
    , _networkStatus(server, &_securitySettingsService)
    , _apSettingsService(server, fs, &_securitySettingsService)
    , _apStatus(server, &_securitySettingsService, &_apSettingsService)
    , _ntpSettingsService(server, fs, &_securitySettingsService)
    , _ntpStatus(server, &_securitySettingsService)
    , _otaSettingsService(server, fs, &_securitySettingsService)
    , _uploadFileService(server, &_securitySettingsService)
    , _mqttSettingsService(server, fs, &_securitySettingsService)
    , _mqttStatus(server, &_mqttSettingsService, &_securitySettingsService)
    , _authenticationService(server, &_securitySettingsService)
    , _restartService(server, &_securitySettingsService)
    , _factoryResetService(server, fs, &_securitySettingsService)
    , _systemStatus(server, &_securitySettingsService) {
    // Serve static resources from PROGMEM
    WWWData::registerRoutes([server, this](const String & uri, const String & contentType, const uint8_t * content, size_t len) {
        ArRequestHandlerFunction requestHandler = [contentType, content, len](AsyncWebServerRequest * request) {
            AsyncWebServerResponse * response = request->beginResponse_P(200, contentType, content, len);
            response->addHeader("Content-Encoding", "gzip");
            // response->addHeader("Content-Encoding", "br"); // only works over HTTPS
            request->send(response);
        };
        server->on(uri.c_str(), HTTP_GET, requestHandler);
        // Serving non matching get requests with "/index.html"
        // OPTIONS get a straight up 200 response
        if (uri.equals("/index.html")) {
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

void ESP8266React::begin() {
    _networkSettingsService.begin();
    _networkSettingsService.read([&](NetworkSettings & networkSettings) {
        if (networkSettings.enableCORS) {
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", networkSettings.CORSOrigin);
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
        }
    });
    _apSettingsService.begin();
    _ntpSettingsService.begin();
    _otaSettingsService.begin();
    _mqttSettingsService.begin();
    _securitySettingsService.begin();
}

void ESP8266React::loop() {
    _networkSettingsService.loop();
    _apSettingsService.loop();
    _otaSettingsService.loop();
    _mqttSettingsService.loop();
}
