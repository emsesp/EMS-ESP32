#include <FactoryResetService.h>

using namespace std::placeholders;

FactoryResetService::FactoryResetService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : fs(fs) {
    server->on(FACTORY_RESET_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&FactoryResetService::handleRequest, this, _1), AuthenticationPredicates::IS_ADMIN));
}

void FactoryResetService::handleRequest(AsyncWebServerRequest * request) {
    request->onDisconnect(std::bind(&FactoryResetService::factoryReset, this));
    request->send(200);
}

/**
 * Delete function assumes that all files are stored flat, within the config directory.
 */
void FactoryResetService::factoryReset() {
    /* 
   * Based on LittleFS. Modified by proddy
   * Could be replaced with fs.rmdir(FS_CONFIG_DIRECTORY) in IDF 4.2
   */
    File root = fs->open(FS_CONFIG_DIRECTORY);
    File file;
    while (file = root.openNextFile()) {
        String path = file.path();
        file.close();
        fs->remove(path);
    }

    RestartService::restartNow();
}
