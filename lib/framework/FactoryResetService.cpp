#include "FactoryResetService.h"

FactoryResetService::FactoryResetService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : fs(fs) {
    server->on(FACTORY_RESET_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { handleRequest(request); }, AuthenticationPredicates::IS_ADMIN));
}

void FactoryResetService::handleRequest(AsyncWebServerRequest * request) {
    request->onDisconnect([this]() { factoryReset(); });
    request->send(200);
}

/**
 * Delete function assumes that all files are stored flat, within the config directory.
 */
void FactoryResetService::factoryReset() {
    // TODO To replaced with fs.rmdir(FS_CONFIG_DIRECTORY) now we're using IDF 4.2
    File root = fs->open(FS_CONFIG_DIRECTORY);
    File file;
    while ((file = root.openNextFile())) {
        String path = file.path();
        file.close();
        fs->remove(path);
    }

    RestartService::restartNow();
}
