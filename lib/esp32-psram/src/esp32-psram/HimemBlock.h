#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>
// ESP32 HIMEM headers - using conditional inclusion for compatibility
#if __has_include("esp32/himem.h")
#include "esp32/himem.h"
#elif __has_include("esp_himem.h")
#include "esp_himem.h"
#else
// Fall back to Arduino ESP32 core path
extern "C" {
#include "esp32/himem.h"
}

// Define missing constants if needed
#ifndef ESP_HIMEM_BLKSZ
#define ESP_HIMEM_BLKSZ (32 * 1024)
#endif
#endif

// Make sure ESP_HIMEM_PROT_RW is defined
#ifndef ESP_HIMEM_PROT_RW
#define ESP_HIMEM_PROT_RW 0
#endif

namespace esp32_psram {

static constexpr const char* TAG = "HIMEM";  // Tag for ESP logging

/**
 * @class HimemBlock
 * @brief Manages a block of himem memory with mapping functionality
 */
class HimemBlock {
 public:
  /**
   * @brief Default constructor
   */
  HimemBlock() { ESP_LOGD(TAG, "HimemBlock constructor called"); }

  /**
   * @brief Allocate a block of himem
   * @param block_size Size of memory to allocate in bytes
   * @return the allocated (block) size in bytes, 0 otherwise
   */
  size_t allocate(size_t block_size) {
    ESP_LOGI(TAG, "HimemBlock::allocate(%u bytes) - Current handle: %p",
             block_size, handle);

    if (handle != 0) {
      ESP_LOGW(TAG, "Cannot allocate: Block already allocated");
      return false;  // Already allocated
    }

    // Round up to the nearest multiple of ESP_HIMEM_BLKSZ (32K)
    block_size = ((block_size + ESP_HIMEM_BLKSZ - 1) / ESP_HIMEM_BLKSZ) *
                 ESP_HIMEM_BLKSZ;

    size = block_size;
    esp_err_t err = esp_himem_alloc(block_size, &handle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "HIMEM allocation failed - error: %d, size: %d", err,
               block_size);
      return 0;
    }

    ESP_LOGD(TAG, "- Successfully allocated %u bytes, handle: %p", block_size,
             handle);
    return block_size;
  }

  /**
   * @brief Read data from HIMEM at specified offset
   * @param dest Destination buffer
   * @param offset Offset in HIMEM to read from
   * @param length Number of bytes to read
   * @return Number of bytes actually read
   */
  size_t read(void* dest, size_t offset, size_t length) {
    ESP_LOGD(TAG, "HimemBlock::read(dst=%p, offset=%u, length=%u)", dest,
             offset, length);

    if (!handle || offset >= size) {
      ESP_LOGW(TAG, "Read failed: %s",
               !handle ? "Invalid handle" : "Offset beyond size");
      return 0;
    }

    // Ensure we don't read past the end
    length = std::min(length, size - offset);
    if (length == 0) {
      ESP_LOGW(TAG, "Read failed: Zero length after bounds check");
      return 0;
    }

    // Calculate which block this belongs to
    size_t block_index = offset / ESP_HIMEM_BLKSZ;
    size_t block_offset = offset % ESP_HIMEM_BLKSZ;
    size_t bytes_read = 0;

    ESP_LOGD(TAG, "- Reading from block %u, offset %u", block_index,
             block_offset);
    uint8_t* dest_ptr = static_cast<uint8_t*>(dest);

    while (bytes_read < length) {
      // Ensure the correct block is mapped
      if (!ensure_block_mapped(block_index)) {
        return bytes_read;
      }

      // Calculate how much to read from this block
      size_t block_remain = ESP_HIMEM_BLKSZ - block_offset;
      size_t to_read = std::min(block_remain, length - bytes_read);
      ESP_LOGD(TAG, "- Reading %u bytes from window at offset %u", to_read,
               block_offset);

      // Copy the data
      memcpy(dest_ptr + bytes_read,
             static_cast<uint8_t*>(mapped_ptr) + block_offset, to_read);

      bytes_read += to_read;
      block_index++;
      block_offset = 0;  // Reset offset for next blocks
    }

    ESP_LOGD(TAG, "- Successfully read %u bytes", bytes_read);
    return bytes_read;
  }

  /**
   * @brief Write data to HIMEM at specified offset
   * @param src Source buffer
   * @param offset Offset in HIMEM to write to
   * @param length Number of bytes to write
   * @return Number of bytes actually written
   */
  size_t write(const void* src, size_t offset, size_t length) {
    ESP_LOGD(TAG, "HimemBlock::write(src=%p, offset=%u, length=%u)", src,
             offset, length);

    if (!handle || offset >= size) {
      ESP_LOGW(TAG, "Write failed: %s",
               !handle ? "Invalid handle" : "Offset beyond size");
      return 0;
    }

    // Ensure we don't write past the end
    length = std::min(length, size - offset);
    if (length == 0) {
      ESP_LOGW(TAG, "Write failed: Zero length after bounds check");
      return 0;
    }

    // Calculate which block this belongs to
    size_t block_index = offset / ESP_HIMEM_BLKSZ;
    size_t block_offset = offset % ESP_HIMEM_BLKSZ;
    size_t bytes_written = 0;

    ESP_LOGD(TAG, "- Writing to block %u, offset %u", block_index,
             block_offset);
    const uint8_t* src_ptr = static_cast<const uint8_t*>(src);

    while (bytes_written < length) {
      // Ensure the correct block is mapped
      if (!ensure_block_mapped(block_index)) {
        return bytes_written;
      }

      // Calculate how much to write to this block
      size_t block_remain = ESP_HIMEM_BLKSZ - block_offset;
      size_t to_write = std::min(block_remain, length - bytes_written);
      ESP_LOGD(TAG, "- Writing %u bytes to window at offset %u", to_write,
               block_offset);

      // Copy the data
      memcpy(static_cast<uint8_t*>(mapped_ptr) + block_offset,
             src_ptr + bytes_written, to_write);

      bytes_written += to_write;
      block_index++;
      block_offset = 0;  // Reset offset for next blocks
    }

    ESP_LOGD(TAG, "- Successfully wrote %u bytes", bytes_written);
    return bytes_written;
  }

  bool getAddress(size_t offset, void* &result, size_t &available) {
    size_t block_index = offset / ESP_HIMEM_BLKSZ;
    size_t block_offset = offset % ESP_HIMEM_BLKSZ;
    if (!ensure_block_mapped(block_index)) return false;
    available = ESP_HIMEM_BLKSZ - block_offset;
    result = static_cast<uint8_t*>(mapped_ptr)+block_offset;
    return true;
  }

  /**
   * @brief Unmap the himem block
   */
  void unmap() {
    ESP_LOGD(TAG, "HimemBlock::unmap() - mapped_ptr=%p, range=%p", mapped_ptr,
             range);

    if (mapped_ptr && range) {
      ESP_LOGD(TAG, "- Unmapping memory and freeing range");
      esp_himem_unmap(range, mapped_ptr, ESP_HIMEM_BLKSZ);
      esp_himem_free_map_range(range);
      mapped_ptr = nullptr;
      range = 0;
      current_mapped_block = SIZE_MAX;  // Reset currently mapped block
      ESP_LOGD(TAG, "- Unmapped successfully");
    } else {
      ESP_LOGD(TAG, "- Nothing to unmap");
    }
  }

  /**
   * @brief Free the himem block
   */
  void free() {
    ESP_LOGD(TAG, "HimemBlock::free() - handle=%p", handle);

    if (handle) {
      ESP_LOGD(TAG, "- Unmapping before freeing");
      unmap();
      ESP_LOGD(TAG, "- Freeing HIMEM handle %p", handle);
      esp_himem_free(handle);
      handle = 0;
      size = 0;
      ESP_LOGD(TAG, " - Successfully freed HIMEM block");
    } else {
      ESP_LOGD(TAG, "- Nothing to free");
    }
  }

  /**
   * @brief Get the size of the allocated block
   * @return Size of the allocated block in bytes
   */
  size_t get_size() const {
    ESP_LOGD(TAG, "HimemBlock::get_size() = %u", size);
    return size;
  }

  /**
   * @brief Destructor - ensures memory is properly freed
   */
  ~HimemBlock() {
    ESP_LOGD(TAG, "HimemBlock destructor called");
    free();
  }

  /**
   * @brief Copy constructor (deleted)
   */
  HimemBlock(const HimemBlock&) = delete;

  /**
   * @brief Move constructor
   */
  HimemBlock(HimemBlock&& other) noexcept
      : handle(other.handle),
        range(other.range),
        mapped_ptr(other.mapped_ptr),
        size(other.size),
        current_mapped_block(other.current_mapped_block) {
    ESP_LOGD(TAG, "HimemBlock move constructor - moving handle=%p, size=%u",
             other.handle, other.size);
    other.handle = 0;
    other.range = 0;
    other.mapped_ptr = nullptr;
    other.size = 0;
    other.current_mapped_block = SIZE_MAX;
  }

  /**
   * @brief Copy assignment operator (deleted)
   */
  HimemBlock& operator=(const HimemBlock&) = delete;

  /**
   * @brief Move assignment operator
   */
  HimemBlock& operator=(HimemBlock&& other) noexcept {
    ESP_LOGD(TAG, "HimemBlock move assignment - from handle=%p to handle=%p",
             other.handle, handle);

    if (this != &other) {
      ESP_LOGD(TAG, "Freeing current resources before move assignment");
      free();
      handle = other.handle;
      range = other.range;
      mapped_ptr = other.mapped_ptr;
      size = other.size;
      current_mapped_block = other.current_mapped_block;
      ESP_LOGD(TAG, "Moved resources, new size=%u", size);
      other.handle = 0;
      other.range = 0;
      other.mapped_ptr = nullptr;
      other.size = 0;
      other.current_mapped_block = SIZE_MAX;
    } else {
      ESP_LOGD(TAG, "Self-assignment detected, no action taken");
    }
    return *this;
  }

 protected:
  esp_himem_handle_t handle = 0;
  esp_himem_rangehandle_t range = 0;
  void* mapped_ptr = nullptr;
  size_t size = 0;
  size_t current_mapped_block =
      SIZE_MAX;  // Track which block is currently mapped

  /**
   * @brief Ensure a specific block is mapped into memory
   * @param block_index The index of the block to map
   * @return true if mapping successful, false otherwise
   */
  bool ensure_block_mapped(size_t block_index) {
    // If the requested block is already mapped, we're done
    if (block_index == current_mapped_block) {
      return true;
    }

    // Unmap previous block if any
    if (mapped_ptr) {
      ESP_LOGD(TAG, "- Unmapping block %u before mapping new block %u",
               current_mapped_block, block_index);
      unmap();  // Unmap previous block
    }

    // Allocate map range
    ESP_LOGD(TAG, "- Allocating map range for block %u", block_index);
    esp_err_t err = esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &range);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to allocate map range: %d", err);
      return false;
    }

    // Map the current block
    ESP_LOGD(TAG, "- Mapping block %u (offset %u)", block_index,
             block_index * ESP_HIMEM_BLKSZ);
    err = esp_himem_map(handle, range, block_index * ESP_HIMEM_BLKSZ, 0,
                        ESP_HIMEM_BLKSZ, ESP_HIMEM_PROT_RW, &mapped_ptr);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to map memory: %d", err);
      esp_himem_free_map_range(range);
      range = 0;
      return false;
    }

    current_mapped_block = block_index;
    return true;
  }
};

}  // namespace esp32_psram
