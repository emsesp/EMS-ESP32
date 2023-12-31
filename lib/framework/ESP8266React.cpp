#include <ESP8266React.h>

ESP8266React::ESP8266React(PsychicHttpServer * server, FS * fs)
    : _networkSettingsService(server, fs, &_securitySettingsService)
    , _networkStatus(server, &_securitySettingsService)
    , _featureService(server)
    , _securitySettingsService(server, fs)
    , _wifiScanner(server, &_securitySettingsService)
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
}

// register services
void ESP8266React::begin() {
    _networkSettingsService.begin();
    _apSettingsService.begin();
    _ntpSettingsService.begin();
    _otaSettingsService.begin();
    _mqttSettingsService.begin();
    _securitySettingsService.begin();
}

// create the web server endpoints
void ESP8266React::registerURI() {
    _featureService.registerURI();
    _authenticationService.registerURI();
    _systemStatus.registerURI();

    _networkSettingsService.registerURI();
    _networkStatus.registerURI();

    _apSettingsService.registerURI();
    _apStatus.registerURI();

    _ntpSettingsService.registerURI();
    _ntpStatus.registerURI();

    _mqttSettingsService.registerURI();
    _mqttStatus.registerURI();

    _securitySettingsService.registerURI();
    _otaSettingsService.registerURI();

    _restartService.registerURI();
    _factoryResetService.registerURI();

    _wifiScanner.registerURI();

    _uploadFileService.registerURI();
}

void ESP8266React::loop() {
    _networkSettingsService.loop();
    _apSettingsService.loop();
    _otaSettingsService.loop();
    _mqttSettingsService.loop();
}
