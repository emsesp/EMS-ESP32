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
        // Serial.println();
        // Serial.print("Fake reading file ");
        // Serial.println(_filePath);
        applyDefaults();
    }

    bool writeToFS() {
        JsonDocument jsonDocument;
        JsonObject   jsonObject = jsonDocument.to<JsonObject>();
        _statefulService->read(jsonObject, _stateReader);
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
            _updateHandlerId = _statefulService->addUpdateHandler([this] { writeToFS(); });
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
    virtual void applyDefaults() {
        JsonDocument jsonDocument;
        JsonObject   jsonObject = jsonDocument.as<JsonObject>();
        _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
    }
};

#endif
