#pragma once

#include <Arduino.h>
#include "InMemoryFS.h"
#include "VectorHIMEM.h"

namespace esp32_psram {

/**
 * @class HIMEMClass
 * @brief Class for managing files stored in ESP32's High Memory (HIMEM)
 * 
 * This class provides an interface similar to SD.h for managing files
 * that are stored in HIMEM memory (beyond the 4MB boundary) rather than on an SD card.
 * HIMEM offers larger storage capacity but slightly slower access than regular PSRAM.
 */
class HIMEMClass : public InMemoryFS<VectorHIMEM<uint8_t>, FileHIMEM> {
public:
    /**
     * @brief Initialize the HIMEM filesystem
     * @return true if initialization was successful, false otherwise
     */
    bool begin() override {
        if (esp_himem_get_free_size() > 0) {
            initialized = true;
            return true;
        }
        initialized = false;
        return false;
    }

    /**
     * @brief Get total space (returns available HIMEM)
     * @return Total HIMEM size in bytes
     */
    uint64_t totalBytes() override {
        return esp_himem_get_phys_size();
    }

    /**
     * @brief Get free space (returns free HIMEM)
     * @return Free HIMEM size in bytes
     */
    uint64_t freeBytes() override {
        return esp_himem_get_free_size();
    }
};

/**
 * @brief Global instance of HIMEMClass for easy access
 */
static HIMEMClass HIMEM;

} // namespace esp32_psram