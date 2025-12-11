#pragma once

#include <Arduino.h>
#include "InMemoryFS.h"
#include "VectorPSRAM.h"


namespace esp32_psram {

/**
 * @class PSRAMClass
 * @brief Class for managing files stored in ESP32's PSRAM
 * 
 * This class provides an interface similar to SD.h for managing files
 * that are stored in PSRAM memory rather than on an SD card.
 */
class PSRAMClass : public InMemoryFS<VectorPSRAM<uint8_t>, FilePSRAM> {
public:
    /**
     * @brief Initialize the PSRAM filesystem
     * @return true if initialization was successful, false otherwise
     */
    bool begin() override {
        if (ESP.getFreePsram() > 0) {
            initialized = true;
            return true;
        }
        initialized = false;
        return false;
    }

    /**
     * @brief Get total space (returns available PSRAM)
     * @return Total PSRAM size in bytes
     */
    uint64_t totalBytes() override {
        return ESP.getPsramSize();
    }

    /**
     * @brief Get free space (returns free PSRAM)
     * @return Free PSRAM size in bytes
     */
    uint64_t freeBytes() override {
        return ESP.getFreePsram();
    }
};

/**
 * @brief Global instance of PSRAMClass for easy access
 */
static PSRAMClass PSRAM;

} // namespace esp32_psram