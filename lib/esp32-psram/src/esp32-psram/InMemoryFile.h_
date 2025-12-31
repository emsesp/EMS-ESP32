#pragma once

#include <Arduino.h>

#include "VectorHIMEM.h"
#include "VectorPSRAM.h"

namespace esp32_psram {

// Define file modes
enum class FileMode { READ, WRITE, APPEND, READ_WRITE };

/**
 * @class InMemoryFile
 * @brief A file-like interface for vector-backed storage in memory
 *
 * This class provides a file-like interface (compatible with Arduino's Stream)
 * for reading and writing data to vector-backed storage. It can use either
 * standard PSRAM or ESP32's HIMEM (high memory beyond the 4MB boundary)
 * as its underlying storage mechanism through the VectorPSRAM and VectorHIMEM
 * implementations.
 *
 * InMemoryFile offers familiar file operations like open, close, read, write,
 * seek, and truncate while managing the data in memory rather than on disk.
 * This makes it useful for temporary storage, data processing, or situations
 * where file operations are needed but filesystem access is not available
 * or desirable.
 *
 * The class can either manage its own internal vector or reference an external
 * vector supplied by another component (like PSRAMClass or HIMEMClass).
 *
 * @tparam VectorType The vector implementation to use for storage (typically
 *                   VectorPSRAM<uint8_t> or VectorHIMEM<uint8_t>)
 *
 * @note The performance characteristics will depend on the underlying vector
 *       implementation. HIMEM operations are generally slower than regular
 *       PSRAM operations but allow for larger storage capacity.
 */
template <typename VectorType>
class InMemoryFile : public Stream {
 public:
  // Define a single callback type that returns the next file directly
  using NextFileCallback =
      std::function<InMemoryFile<VectorType>(const char*, FileMode)>;
  /**
   * @brief Default constructor - initializes with internal vector
   */
  InMemoryFile() : data_ptr(&internal_data) {
    ESP_LOGD(TAG, "InMemoryFile constructor called");
  }

  /**
   * @brief Constructor with filename and mode
   * @param filename Name of the file
   * @param mode Mode to open the file in
   */
  InMemoryFile(const char* filename, FileMode mode)
      : data_ptr(&internal_data), name_(filename) {
    ESP_LOGD(TAG, "InMemoryFile constructor with name and mode called");
    open(mode);
  }

  /**
   * @brief Destructor - ensures proper cleanup
   */
  virtual ~InMemoryFile() {
    ESP_LOGD(TAG, "InMemoryFile destructor called");
    close();
    if (!using_external_vector) {
      data_ptr = nullptr;
    }
  }

  /**
   * @brief Set the vector to use for this file
   * @param vec Pointer to the vector to use
   */
  void setVector(VectorType* vec) {
    ESP_LOGD(TAG, "Setting external vector pointer: %p", vec);
    if (vec) {
      data_ptr = vec;
      using_external_vector = true;
    } else {
      data_ptr = &internal_data;
      using_external_vector = false;
    }
  }

  /**
   * @brief Set the name of this file
   * @param name The name to set
   */
  void setName(const char* name) {
    ESP_LOGD(TAG, "Setting file name: %s", name);
    name_ = name;
  }

  /**
   * @brief Open the file with the specified mode
   * @param mode Mode to open the file in
   * @return true if opened successfully, false otherwise
   */
  bool open(FileMode mode) {
    ESP_LOGD(TAG, "Opening file '%s' with mode %d", name_.c_str(),
             static_cast<int>(mode));
    this->mode = mode;

    if (mode == FileMode::WRITE) {
      // Clear the vector for writing
      data_ptr->clear();
      position_ = 0;
    } else if (mode == FileMode::APPEND) {
      // Position at end for appending
      position_ = data_ptr->size();
    } else {
      // READ or READ_WRITE - position at beginning
      position_ = 0;
    }

    open_ = true;
    return true;
  }

  /**
   * @brief Close the file
   */
  void close() {
    ESP_LOGD(TAG, "Closing file '%s'", name_.c_str());
    open_ = false;
  }

  /**
   * @brief Check if file is open
   * @return true if open, false otherwise
   */
  bool isOpen() const { return open_; }

  /**
   * @brief Get the name of the file
   * @return File name
   */
  String name() const { return name_; }

  /**
   * @brief Get the size of the file
   * @return File size in bytes
   */
  size_t size() const { return data_ptr->size(); }

  // Stream interface implementation

  /**
   * @brief Read a single byte from the file
   * @return The next byte, or -1 if no data is available
   */
  int read() override {
    ESP_LOGD(TAG, "InMemoryFile::read() - position %u", (unsigned)position_);
    if (!open_ || (mode != FileMode::READ && mode != FileMode::READ_WRITE)) {
      ESP_LOGE(TAG, "read failed: file not open for reading");
      return -1;
    }

    if (position_ >= data_ptr->size()) {
      ESP_LOGD(TAG, "read at EOF: pos=%u, size=%u", (unsigned)position_,
               (unsigned)data_ptr->size());
      return -1;  // EOF
    }

    uint8_t byte = (*data_ptr)[position_++];
    return byte;
  }

  /**
   * @brief Read multiple bytes from the file
   * @param buffer Buffer to store the read data
   * @param size Maximum number of bytes to read
   * @return Number of bytes actually read
   */
  size_t readBytes(char* buffer, size_t size) override {
    ESP_LOGD(TAG, "InMemoryFile::readBytes: %u", (unsigned)size);
    if (!open_ || (mode != FileMode::READ && mode != FileMode::READ_WRITE)) {
      ESP_LOGE(TAG, "read failed: file not open for reading");
      return 0;
    }

    // Calculate how many bytes we can actually read
    size_t available_bytes = data_ptr->size() - position_;
    size_t bytes_to_read = min(size, available_bytes);

    // Read the bytes
    for (size_t i = 0; i < bytes_to_read; i++) {
      buffer[i] = (*data_ptr)[position_ + i];
    }

    position_ += bytes_to_read;
    return bytes_to_read;
  }

  /**
   * @brief Get the number of bytes available to read
   * @return Number of bytes available
   */
  int available() override {
    if (!open_) return 0;
    return data_ptr->size() - position_;
  }

  /**
   * @brief Peek at the next byte without advancing the position
   * @return The next byte, or -1 if no data is available
   */
  int peek() override {
    if (!open_ || (mode != FileMode::READ && mode != FileMode::READ_WRITE)) {
      return -1;
    }

    if (position_ >= data_ptr->size()) {
      return -1;  // EOF
    }

    return (*data_ptr)[position_];
  }

  /**
   * @brief Ensure all data is committed to the storage
   * No-op for this implementation since data is in memory
   */
  void flush() override {
    // No-op for in-memory implementation
  }

  /**
   * @brief Write a single byte to the file
   * @param b The byte to write
   * @return 1 if the byte was written, 0 otherwise
   */
  size_t write(uint8_t b) override {
    ESP_LOGD(TAG, "InMemoryFile::write: 1 byte");
    if (!open_ || (mode != FileMode::WRITE && mode != FileMode::APPEND &&
                   mode != FileMode::READ_WRITE)) {
      ESP_LOGE(TAG, "write failed: file not open for writing");
      return 0;
    }

    if (position_ >= data_ptr->size()) {
      // Append to the end
      data_ptr->push_back(b);
    } else {
      // Replace existing byte
      (*data_ptr)[position_] = b;
    }

    position_++;
    return 1;
  }

  /**
   * @brief Write multiple bytes to the file
   * @param buffer Buffer containing the data to write
   * @param size Number of bytes to write
   * @return Number of bytes actually written
   */
  size_t write(const uint8_t* buffer, size_t size) override {
    ESP_LOGD(TAG, "InMemoryFile::write: %u", (unsigned)size);
    if (!open_ || (mode != FileMode::WRITE && mode != FileMode::APPEND &&
                   mode != FileMode::READ_WRITE)) {
      ESP_LOGE(TAG, "write failed: file not open for writing");
      return 0;
    }

    if (position_ >= data_ptr->size()) {
      // Append to the end
      data_ptr->reserve(data_ptr->size() + size);  // Optimize capacity
      for (size_t i = 0; i < size; i++) {
        data_ptr->push_back(buffer[i]);
      }
    } else {
      // Replace/insert bytes
      size_t space_available = data_ptr->size() - position_;
      size_t bytes_to_replace = min(size, space_available);
      size_t bytes_to_append = size - bytes_to_replace;

      // Replace existing bytes
      for (size_t i = 0; i < bytes_to_replace; i++) {
        (*data_ptr)[position_ + i] = buffer[i];
      }

      // Append remaining bytes
      for (size_t i = 0; i < bytes_to_append; i++) {
        data_ptr->push_back(buffer[bytes_to_replace + i]);
      }
    }

    position_ += size;
    return size;
  }

  // File-specific methods

  /**
   * @brief Set the current position in the file
   * @param pos The position to seek to
   * @return true if successful, false otherwise
   */
  bool seek(size_t pos) {
    ESP_LOGD(TAG, "InMemoryFile::seek: %u", (unsigned)pos);
    if (!open_ || pos > data_ptr->size()) {
      ESP_LOGE(TAG, "seek failed: file not open or position beyond size");
      return false;
    }

    position_ = pos;
    return true;
  }

  /**
   * @brief Get the current position in the file
   * @return Current position
   */
  size_t position() const { return position_; }

  /**
   * @brief Truncate the file to the current position
   */
  void truncate() {
    ESP_LOGD(TAG, "InMemoryFile::truncate at position %u", (unsigned)position_);
    if (!open_ || (mode != FileMode::WRITE && mode != FileMode::READ_WRITE)) {
      ESP_LOGE(TAG, "truncate failed: file not open for writing");
      return;
    }

    if (position_ < data_ptr->size()) {
      // Create a new vector with just the data up to position_
      VectorType new_data;
      new_data.reserve(position_);
      for (size_t i = 0; i < position_; i++) {
        new_data.push_back((*data_ptr)[i]);
      }

      // Replace the old data
      *data_ptr = std::move(new_data);
    }
  }

  operator bool() const { return open_; }
  /**
   * @brief Set callback for navigating to the next file
   *
   * This function sets a callback that will be used to retrieve the next
   * file when getNextFile() is called. The callback takes the current file name
   * and mode, and returns the next file object directly.
   *
   * @param callback Function that returns the next file
   */
  void setNextFileCallback(NextFileCallback callback) {
    ESP_LOGD(TAG, "Setting next file callback");
    nextFileCallback = callback;
  }

  /**
   * @brief Get the next file in the filesystem
   *
   * This method uses the registered callback to get the next file
   * after this one in the filesystem.
   *
   * @return The next file, or an empty file if there is no next file or
   * callback isn't set
   */
  InMemoryFile<VectorType> getNextFile() {
    if (!nextFileCallback || name_.isEmpty()) {
      // Return empty file if callback isn't set or no name
      InMemoryFile<VectorType> emptyFile;
      return emptyFile;
    }

    // Get the next file directly using the callback
    return nextFileCallback(name_.c_str(), mode);
  }

  /**
   * @brief Reserve storage
   * @param new_cap The new capacity of the file
   */
  bool reserve(size_t new_cap) {
    if (data_ptr == nullptr) return false;
    data_ptr->reserve(new_cap);
    return true;
  }

  /**
   * @brief Get the number of bytes that can be held in current storage.
   * Please note that this might dynamically grow!
   * @return The capacity of the file
   */
  size_t capacity() const {
    if (data_ptr == nullptr) return 0;
    return data_ptr->capacity();
  }

 private:
  VectorType* data_ptr =
      nullptr;               // Pointer to vector data (internal or external)
  VectorType internal_data;  // Internal vector for standalone use
  bool using_external_vector = false;
  size_t position_ = 0;
  bool open_ = false;
  FileMode mode = FileMode::READ;
  String name_;

  // Single callback for getting the next file
  NextFileCallback nextFileCallback = nullptr;

  // Tags for debug logging
  static constexpr const char* TAG = "InMemoryFile";
};

// Type aliases for convenience
using FilePSRAM = InMemoryFile<VectorPSRAM<uint8_t>>;
using FileHIMEM = InMemoryFile<VectorHIMEM<uint8_t>>;

}  // namespace esp32_psram