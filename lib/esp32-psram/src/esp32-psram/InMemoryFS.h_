#pragma once

#include <Arduino.h>

#include <map>

#include "InMemoryFile.h"

// Define Arduino file mode constants if not already defined
#ifndef FILE_READ
#define FILE_READ 0
#endif
#ifndef FILE_WRITE
#define FILE_WRITE 1
#endif
#ifndef FILE_APPEND
#define FILE_APPEND 2
#endif

namespace esp32_psram {

/**
 * @class InMemoryFS
 * @brief Base class for memory-based file systems like PSRAM and HIMEM
 *
 * This class provides a common interface for in-memory file systems,
 * with methods for file management and traversal.
 *
 * @tparam VectorType The vector implementation to use (VectorPSRAM or
 * VectorHIMEM)
 * @tparam FileType The file implementation to return (FilePSRAM or FileHIMEM)
 */
template <typename VectorType, typename FileType>
class InMemoryFS {
 public:
  /**
   * @brief Initialize the file system
   * @return true if initialization was successful, false otherwise
   */
  virtual bool begin() = 0;

  /**
   * @brief Check if a file exists
   * @param filename Name of the file to check
   * @return true if the file exists, false otherwise
   */
  bool exists(const char* filename) {
    if (!initialized) return false;
    return fileData.find(std::string(filename)) != fileData.end();
  }

  /**
   * @brief Open a file
   * @param filename Name of the file to open
   * @param mode Mode to open the file in (FILE_READ, FILE_WRITE, etc.)
   * @return A file object for the opened file
   */
  FileType open(const char* filename, uint8_t mode) {
    ESP_LOGD("InMemoryFS", "Opening file %s with mode %d", filename, mode);

    if (!initialized) {
      ESP_LOGW("InMemoryFS", "Filesystem not initialized");
      FileType emptyFile;
      return emptyFile;
    }

    // Convert Arduino file modes to our enum
    FileMode fileMode;
    if (mode == FILE_READ) {
      fileMode = FileMode::READ;
    } else if (mode == FILE_WRITE) {
      fileMode = FileMode::WRITE;
    } else if (mode == FILE_APPEND) {
      fileMode = FileMode::APPEND;
    } else {
      fileMode = FileMode::READ_WRITE;
    }

    std::string filenameStr(filename);
    auto it = fileData.find(filenameStr);
    FileType file;

    if (it != fileData.end()) {
      // File exists, create a new file pointing to existing data
      ESP_LOGD("InMemoryFS", "File exists, connecting to existing data");
      file.setVector(&(it->second));
    } else if (mode != FILE_READ) {
      // File doesn't exist, create it for writing or appending
      ESP_LOGD("InMemoryFS", "Creating new file for writing");
      fileData[filenameStr] = VectorType();  // Create empty vector in the map
      file.setVector(&(fileData[filenameStr]));
    } else {
      // File doesn't exist and mode is READ
      ESP_LOGW("InMemoryFS", "File doesn't exist and mode is READ");
      return file;  // Return empty file
    }

    // Configure the file
    file.setName(filename);
    file.open(fileMode);

    // Set up the next file callback
    file.setNextFileCallback(
        [this](const char* currentFileName, FileMode currentMode) -> FileType {
          ESP_LOGD("InMemoryFS", "NextFile callback called for %s",
                   currentFileName);

          // Get the next filename
          String nextName = this->getNextFileName(currentFileName);
          if (nextName.isEmpty()) {
            ESP_LOGD("InMemoryFS", "No next file found");
            return FileType();  // Return empty file if no next file
          }

          // Convert FileMode to Arduino mode
          uint8_t arduinoMode;
          if (currentMode == FileMode::READ)
            arduinoMode = FILE_READ;
          else if (currentMode == FileMode::WRITE)
            arduinoMode = FILE_WRITE;
          else if (currentMode == FileMode::APPEND)
            arduinoMode = FILE_APPEND;
          else
            arduinoMode = FILE_READ | FILE_WRITE;

          // Open and return the next file
          ESP_LOGD("InMemoryFS", "Returning next file: %s", nextName.c_str());
          return this->open(nextName.c_str(), arduinoMode);
        });

    ESP_LOGD("InMemoryFS", "File opened successfully");
    return file;
  }

  /**
   * @brief Remove a file
   * @param filename Name of the file to remove
   * @return true if the file was removed, false otherwise
   */
  bool remove(const char* filename) {
    if (!initialized) return false;

    std::string filenameStr(filename);
    auto it = fileData.find(filenameStr);

    if (it != fileData.end()) {
      fileData.erase(it);
      return true;
    }

    return false;
  }

  /**
   * @brief Create a directory (no-op for compatibility)
   * @param dirname Name of the directory
   * @return Always returns true for compatibility
   */
  bool mkdir(const char* dirname) {
    // No directories in this implementation, just for compatibility
    return true;
  }

  /**
   * @brief Remove a directory (no-op for compatibility)
   * @param dirname Name of the directory
   * @return Always returns true for compatibility
   */
  bool rmdir(const char* dirname) {
    // No directories in this implementation, just for compatibility
    return true;
  }

  /**
   * @brief Get the name of the next file after the specified file
   * @param currentFileName Name of the current file
   * @return Name of the next file, or empty string if there are no more files
   */
  String getNextFileName(const char* currentFileName) {
    if (!initialized || fileData.empty()) {
      return String();
    }

    if (currentFileName == nullptr || strlen(currentFileName) == 0 ||
        strcmp(currentFileName, "/") == 0) {
      // Return the first file if current is empty or is root directory
      return String(fileData.begin()->first.c_str());
    }

    std::string current(currentFileName);
    auto it = fileData.find(current);

    if (it == fileData.end()) {
      // Current file not found, find the next one alphabetically
      for (auto& entry : fileData) {
        if (entry.first > current) {
          return String(entry.first.c_str());
        }
      }
      // No file after the specified name
      return String();
    } else {
      // Found the current file, get the next one
      ++it;
      if (it != fileData.end()) {
        return String(it->first.c_str());
      } else {
        // Was the last file
        return String();
      }
    }
  }

  /**
   * @brief Get the first file in the filesystem
   * @return Name of the first file, or empty string if there are no files
   */
  String getFirstFileName() {
    if (!initialized || fileData.empty()) {
      return String();
    }
    return String(fileData.begin()->first.c_str());
  }

  /**
   * @brief Get the total number of files
   * @return Number of files in the filesystem
   */
  size_t fileCount() const {
    if (!initialized) return 0;
    return fileData.size();
  }

  /**
   * @brief Get total space
   * @return Total memory size in bytes
   */
  virtual uint64_t totalBytes() = 0;

  /**
   * @brief Get free space
   * @return Free memory size in bytes
   */
  virtual uint64_t freeBytes() = 0;

 protected:
  bool initialized = false;
  std::map<std::string, VectorType> fileData;
};

}  // namespace esp32_psram