#ifndef StatefulService_h
#define StatefulService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include <vector>
#include <list>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

enum class StateUpdateResult {
    CHANGED = 0,     // The update changed the state and propagation should take place if required
    CHANGED_RESTART, // a restart of the device is needed
    UNCHANGED,       // The state was unchanged, propagation should not take place
    ERROR            // There was a problem updating the state, propagation should not take place
};

template <typename T>
using JsonStateUpdater = std::function<StateUpdateResult(JsonObject root, T & settings)>;

template <typename T>
using JsonStateReader = std::function<void(T & settings, JsonObject root)>;

typedef size_t                update_handler_id_t;
typedef std::function<void()> StateUpdateCallback;

typedef struct StateUpdateHandlerInfo {
    static update_handler_id_t currentUpdatedHandlerId;
    update_handler_id_t        _id;
    StateUpdateCallback        _cb;
    bool                       _allowRemove;
    StateUpdateHandlerInfo(StateUpdateCallback cb, bool allowRemove)
        : _id(++currentUpdatedHandlerId)
        , _cb(std::move(cb))
        , _allowRemove(allowRemove) {};
} StateUpdateHandlerInfo_t;

template <class T>
class StatefulService {
  public:
    template <typename... Args>
    explicit StatefulService(Args &&... args)
        : _state(std::forward<Args>(args)...)
        , _accessMutex(xSemaphoreCreateRecursiveMutex()) {
    }

    update_handler_id_t addUpdateHandler(StateUpdateCallback cb, bool allowRemove = true) {
        if (!cb) {
            return 0;
        }
        StateUpdateHandlerInfo_t updateHandler(std::move(cb), allowRemove);
        _updateHandlers.push_back(std::move(updateHandler));
        return updateHandler._id;
    }

    void removeUpdateHandler(update_handler_id_t id) {
        for (auto it = _updateHandlers.begin(); it != _updateHandlers.end();) {
            auto & elem = *it;
            if (elem._allowRemove && elem._id == id) {
                it = _updateHandlers.erase(it);
            } else {
                ++it;
            }
        }
    }

    StateUpdateResult update(std::function<StateUpdateResult(T &)> stateUpdater) {
        beginTransaction();
        StateUpdateResult result = stateUpdater(_state);
        endTransaction();
        if (result == StateUpdateResult::CHANGED) {
            callUpdateHandlers();
        }
        return result;
    }

    StateUpdateResult updateWithoutPropagation(std::function<StateUpdateResult(T &)> stateUpdater) {
        beginTransaction();
        StateUpdateResult result = stateUpdater(_state);
        endTransaction();
        return result;
    }

    StateUpdateResult update(JsonObject jsonObject, JsonStateUpdater<T> stateUpdater) {
        beginTransaction();
        StateUpdateResult result = stateUpdater(jsonObject, _state);
        endTransaction();
        if (result == StateUpdateResult::CHANGED) {
            callUpdateHandlers();
        }
        return result;
    }

    StateUpdateResult updateWithoutPropagation(JsonObject jsonObject, JsonStateUpdater<T> stateUpdater) {
        beginTransaction();
        StateUpdateResult result = stateUpdater(jsonObject, _state);
        endTransaction();
        return result;
    }

    void read(std::function<void(T &)> stateReader) {
        beginTransaction();
        stateReader(_state);
        endTransaction();
    }

    void read(JsonObject jsonObject, JsonStateReader<T> stateReader) {
        beginTransaction();
        stateReader(_state, jsonObject);
        endTransaction();
    }

    void callUpdateHandlers() {
        for (const StateUpdateHandlerInfo_t & updateHandler : _updateHandlers) {
            updateHandler._cb();
        }
    }

  protected:
    T _state;

    inline void beginTransaction() {
        xSemaphoreTakeRecursive(_accessMutex, portMAX_DELAY);
    }

    inline void endTransaction() {
        xSemaphoreGiveRecursive(_accessMutex);
    }

  private:
    SemaphoreHandle_t                     _accessMutex;
    std::vector<StateUpdateHandlerInfo_t> _updateHandlers;
};

#endif
