#ifndef FSPersistence_h
#define FSPersistence_h

#include <StatefulService.h>
#include <FS.h>

template <class T>
class FSPersistence {
  public:
    FSPersistence(JsonStateReader<T>   stateReader,
                  JsonStateUpdater<T>  stateUpdater,
                  StatefulService<T> * statefulService,
                  FS *                 fs,
                  const char *         filePath,
                  size_t               bufferSize = DEFAULT_BUFFER_SIZE)
        : _stateReader(stateReader)
        , _stateUpdater(stateUpdater)
        , _statefulService(statefulService)
        , _fs(fs)
        , _filePath(filePath)
        , _bufferSize(bufferSize)
        , _updateHandlerId(0) {
        enableUpdateHandler();
    }

    void readFromFS() {
        applyDefaults();
    }

    bool writeToFS() {
        DynamicJsonDocument jsonDocument = DynamicJsonDocument(_bufferSize);
        JsonObject          jsonObject   = jsonDocument.to<JsonObject>();
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
            _updateHandlerId = _statefulService->addUpdateHandler([&](const String & originId) { writeToFS(); });
        }
    }

  private:
    JsonStateReader<T>   _stateReader;
    JsonStateUpdater<T>  _stateUpdater;
    StatefulService<T> * _statefulService;
    FS *                 _fs;
    const char *         _filePath;
    size_t               _bufferSize;
    update_handler_id_t  _updateHandlerId;

  protected:
    virtual void applyDefaults() {
        DynamicJsonDocument jsonDocument = DynamicJsonDocument(_bufferSize);
        JsonObject          jsonObject   = jsonDocument.as<JsonObject>();
        _statefulService->updateWithoutPropagation(jsonObject, _stateUpdater);
    }
};

#endif // end FSPersistence
