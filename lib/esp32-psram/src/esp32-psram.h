#pragma once

#ifdef ESP32
/**
 * @file ESP32-PSRAM.h
 * @brief Main include file for the ESP32-PSRAM library
 * 
 * This header includes all components of the ESP32-PSRAM library and
 * provides a using namespace directive for easier access to library features.
 */

// Include all library components
#include "esp32-psram/AllocatorPSRAM.h"   // PSRAM-backed vector
#include "esp32-psram/VectorPSRAM.h"   // PSRAM-backed vector
#include "esp32-psram/VectorHIMEM.h"   // HIMEM-backed vector
// #include "esp32-psram/InMemoryFile.h"    // File interface using vectors
// #include "esp32-psram/PSRAM.h"         // PSRAM file system
// #include "esp32-psram/HIMEM.h"         // HIMEM file system
#include "esp32-psram/RingBufferStream.h" // Stream-based ring buffer
#include "esp32-psram/TypedRingBuffer.h" // Typed ring buffer for structured data

#ifndef ESP32_PSRAM_NO_NAMESPACE
using namespace esp32_psram;
#endif

#else
#error "This library is only compatible with ESP32 platforms."
#endif  // ESP32

