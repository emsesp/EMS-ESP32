#include <FactoryResetService.h>

using namespace std::placeholders;

FactoryResetService::FactoryResetService(PsychicHttpServer * server, FS * fs, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void FactoryResetService::registerURI() {
    _server->on(FACTORY_RESET_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapRequest(std::bind(&FactoryResetService::handleRequest, this, _1), AuthenticationPredicates::IS_ADMIN));
}

esp_err_t FactoryResetService::handleRequest(PsychicRequest * request) {
    request->reply(200);
    RestartService::restartNow();
    return ESP_OK;
}

// Delete function assumes that all files are stored flat, within the config directory.
void FactoryResetService::factoryReset() {
    // TODO To replaced with fs.rmdir(FS_CONFIG_DIRECTORY) now we're using IDF 4.2
    File root = fs->open(FS_CONFIG_DIRECTORY);
    File file;
    while (file = root.openNextFile()) {
        String path = file.path();
        file.close();
        fs->remove(path);
    }

    RestartService::restartNow();
}
