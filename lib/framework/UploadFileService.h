#ifndef UploadFileService_h
#define UploadFileService_h

#include <Arduino.h>

#include <Update.h>
#include <WiFi.h>

#include <LittleFS.h>

#include <PsychicHttp.h>
#include <SecurityManager.h>
#include <RestartService.h>

#define UPLOAD_FILE_PATH "/rest/uploadFile"
#define TEMP_FILENAME_PATH "/tmp_upload"

enum FileType { ft_none = 0, ft_firmware = 1, ft_md5 = 2, ft_json = 3 };

class UploadFileService {
  public:
    UploadFileService(PsychicHttpServer * server, SecurityManager * securityManager);

    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t handleUpload(PsychicRequest * request, const String & filename, uint64_t index, uint8_t * data, size_t len, bool final);
    esp_err_t uploadComplete(PsychicRequest * request);
    esp_err_t handleError(PsychicRequest * request, int code);
};

#endif
