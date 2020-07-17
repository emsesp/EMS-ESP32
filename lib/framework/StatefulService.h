#ifndef StatefulService_h
#define StatefulService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include <list>
#include <functional>
#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#endif

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE 1024
#endif

enum class StateUpdateResult {
  CHANGED = 0,  // The update changed the state and propagation should take place if required
  UNCHANGED,    // The state was unchanged, propagation should not take place
  ERROR         // There was a problem updating the state, propagation should not take place
};

template <typename T>
using JsonStateUpdater = std::function<StateUpdateResult(JsonObject& root, T& settings)>;

template <typename T>
using JsonStateReader = std::function<void(T& settings, JsonObject& root)>;

typedef size_t update_handler_id_t;
typedef std::function<void(const String& originId)> StateUpdateCallback;

typedef struct StateUpdateHandlerInfo {
  static update_handler_id_t currentUpdatedHandlerId;
  update_handler_id_t _id;
  StateUpdateCallback _cb;
  bool _allowRemove;
  StateUpdateHandlerInfo(StateUpdateCallback cb, bool allowRemove) :
      _id(++currentUpdatedHandlerId), _cb(cb), _allowRemove(allowRemove){};
} StateUpdateHandlerInfo_t;

template <class T>
class StatefulService {
 public:
  template <typename... Args>
#ifdef ESP32
  StatefulService(Args&&... args) :
      _state(std::forward<Args>(args)...), _accessMutex(xSemaphoreCreateRecursiveMutex()) {
  }
#else
  StatefulService(Args&&... args) : _state(std::forward<Args>(args)...) {
  }
#endif

  update_handler_id_t addUpdateHandler(StateUpdateCallback cb, bool allowRemove = true) {
    if (!cb) {
      return 0;
    }
    StateUpdateHandlerInfo_t updateHandler(cb, allowRemove);
    _updateHandlers.push_back(updateHandler);
    return updateHandler._id;
  }

  void removeUpdateHandler(update_handler_id_t id) {
    for (auto i = _updateHandlers.begin(); i != _updateHandlers.end();) {
      if ((*i)._allowRemove && (*i)._id == id) {
        i = _updateHandlers.erase(i);
      } else {
        ++i;
      }
    }
  }

  StateUpdateResult update(std::function<StateUpdateResult(T&)> stateUpdater, const String& originId) {
    beginTransaction();
    StateUpdateResult result = stateUpdater(_state);
    endTransaction();
    if (result == StateUpdateResult::CHANGED) {
      callUpdateHandlers(originId);
    }
    return result;
  }

  StateUpdateResult updateWithoutPropagation(std::function<StateUpdateResult(T&)> stateUpdater) {
    beginTransaction();
    StateUpdateResult result = stateUpdater(_state);
    endTransaction();
    return result;
  }

  StateUpdateResult update(JsonObject& jsonObject, JsonStateUpdater<T> stateUpdater, const String& originId) {
    beginTransaction();
    StateUpdateResult result = stateUpdater(jsonObject, _state);
    endTransaction();
    if (result == StateUpdateResult::CHANGED) {
      callUpdateHandlers(originId);
    }
    return result;
  }

  StateUpdateResult updateWithoutPropagation(JsonObject& jsonObject, JsonStateUpdater<T> stateUpdater) {
    beginTransaction();
    StateUpdateResult result = stateUpdater(jsonObject, _state);
    endTransaction();
    return result;
  }

  void read(std::function<void(T&)> stateReader) {
    beginTransaction();
    stateReader(_state);
    endTransaction();
  }

  void read(JsonObject& jsonObject, JsonStateReader<T> stateReader) {
    beginTransaction();
    stateReader(_state, jsonObject);
    endTransaction();
  }

  void callUpdateHandlers(const String& originId) {
    for (const StateUpdateHandlerInfo_t& updateHandler : _updateHandlers) {
      updateHandler._cb(originId);
    }
  }

 protected:
  T _state;

  inline void beginTransaction() {
#ifdef ESP32
    xSemaphoreTakeRecursive(_accessMutex, portMAX_DELAY);
#endif
  }

  inline void endTransaction() {
#ifdef ESP32
    xSemaphoreGiveRecursive(_accessMutex);
#endif
  }

 private:
#ifdef ESP32
  SemaphoreHandle_t _accessMutex;
#endif
  std::list<StateUpdateHandlerInfo_t> _updateHandlers;
};

#endif  // end StatefulService_h
