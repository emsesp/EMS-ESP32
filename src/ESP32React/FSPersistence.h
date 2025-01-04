#ifndef FSPersistence_h
#define FSPersistence_h

#include "StatefulService.h"
#include "FS.h"

template <class T>
class FSPersistence {
  public:
    FSPersistence(JsonStateReader<T> stateReader, JsonStateUpdater<T> stateUpdater, StatefulService<T> * statefulService, FS * fs, const char * filePath)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _fs(fs)
        , _filePath(filePath)
        , _updateHandlerId(0) {
        enableUpdateHandler();
    }

    void readFromFS() {
        File settingsFile = _fs->open(_filePath, "r");

        if (settingsFile) {
            JsonDocument         jsonDocument;
            DeserializationError error = deserializeJson(jsonDocument, settingsFile);
            if (error == DeserializationError::Ok && jsonDocument.is<JsonObject>()) {
                JsonObject jsonObject = jsonDocument.as<JsonObject>();
                _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
#if defined(EMSESP_DEBUG)
                // Serial.println();
                // Serial.printf("Reading settings from %s ", _filePath);
                // serializeJson(jsonDocument, Serial);
                // Serial.println();
#endif
                settingsFile.close();
                return;
            }
            settingsFile.close();
        }

// If we reach here we have not been successful in loading the config,
// hard-coded emergency defaults are now applied.
#if defined(EMSESP_DEBUG)
        // Serial.println();
        // Serial.printf("Applying defaults to %s", _filePath);
        // Serial.println();
#endif
        applyDefaults();
        writeToFS(); // added to make sure the initial file is created
    }

    bool writeToFS() {
        // create and populate a new json object
        JsonDocument jsonDocument;
        JsonObject   jsonObject = jsonDocument.to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);

        // make directories if required, for new IDF4.2 & LittleFS
        String path(_filePath);
        int    index = 0;
        while ((index = path.indexOf('/', static_cast<unsigned int>(index) + 1)) != -1) {
            String segment = path.substring(0, static_cast<unsigned int>(index));
            if (!_fs->exists(segment)) {
                _fs->mkdir(segment);
            }
        }

        // serialize it to filesystem
        File settingsFile = _fs->open(_filePath, "w");

        // failed to open file, return false
        if (!settingsFile || !jsonObject.size()) {
            return false;
        }

        // serialize the data to the file
#if defined(EMSESP_DEBUG)
        // Serial.println();
        // Serial.printf("Writing settings to %s ", _filePath);
        // serializeJson(jsonDocument, Serial);
        // Serial.println();
#endif
        serializeJson(jsonDocument, settingsFile);
        settingsFile.close();
        return true;
    }

    void disableUpdateHandler() {
        if (_updateHandlerId) {
            _statefulService->removeUpdateHandler(_updateHandlerId);
            _updateHandlerId = 0;
        }
    }

    void enableUpdateHandler() {
        if (!_updateHandlerId) {
            _updateHandlerId = _statefulService->addUpdateHandler([&] { writeToFS(); });
        }
    }

  private:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;
    FS *                 _fs;
    const char *         _filePath;
    update_handler_id_t  _updateHandlerId;

  protected:
    // We assume the updater supplies sensible defaults if an empty object
    // is supplied, this virtual function allows that to be changed.
    virtual void applyDefaults() {
        JsonDocument jsonDocument;
        JsonObject   jsonObject = jsonDocument.as<JsonObject>();
        _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
    }
};

#endif
