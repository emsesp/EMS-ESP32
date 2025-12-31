#pragma once

#include <Arduino.h>
#include <Stream.h>
#include "VectorPSRAM.h"
#include "VectorHIMEM.h"

namespace esp32_psram {

/**
 * @class RingBufferStream
 * @brief A circular buffer implementation that extends Arduino's Stream
 * @tparam VectorType The vector type to use as underlying storage
 * 
 * This class implements a ring buffer (circular buffer) that uses a vector container
 * for storage. It extends Arduino's Stream class to provide standard stream functionality.
 * The buffer can be used with any vector type, including VectorPSRAM and VectorHIMEM.
 */
template <typename VectorType>
class RingBufferStream : public Stream {
private:
    VectorType buffer;
    size_t readIndex = 0;
    size_t writeIndex = 0;
    bool full = false;
    size_t maxSize;

public:
    /**
     * @brief Constructor with specified buffer size
     * @param size The size of the buffer in bytes
     */
    RingBufferStream(size_t size) : maxSize(size) {
        buffer.resize(size);
    }

    /**
     * @brief Get the number of bytes available for reading
     * @return Number of bytes available
     */
    int available() override {
        if (full) {
            return maxSize;
        }
        if (writeIndex >= readIndex) {
            return writeIndex - readIndex;
        } else {
            return maxSize - (readIndex - writeIndex);
        }
    }

    /**
     * @brief Read a byte from the buffer
     * @return The byte read, or -1 if the buffer is empty
     */
    int read() override {
        if (isEmpty()) {
            return -1;
        }

        uint8_t value = buffer[readIndex];
        readIndex = (readIndex + 1) % maxSize;
        full = false;
        return value;
    }

    /**
     * @brief Look at the next byte in the buffer without removing it
     * @return The next byte to be read, or -1 if the buffer is empty
     */
    int peek() override {
        if (isEmpty()) {
            return -1;
        }

        return buffer[readIndex];
    }

    /**
     * @brief Write a byte to the buffer
     * @param value The byte to write
     * @return 1 if the byte was written, 0 if the buffer is full
     */
    size_t write(uint8_t value) override {
        if (full) {
            return 0;
        }

        buffer[writeIndex] = value;
        writeIndex = (writeIndex + 1) % maxSize;
        
        // Check if buffer is now full
        if (writeIndex == readIndex) {
            full = true;
        }
        
        return 1;
    }

    /**
     * @brief Write multiple bytes to the buffer
     * @param data Pointer to the data to write
     * @param size Number of bytes to write
     * @return Number of bytes written
     */
    size_t write(const uint8_t *data, size_t size) {
        size_t bytesWritten = 0;
        
        for (size_t i = 0; i < size; i++) {
            if (write(data[i])) {
                bytesWritten++;
            } else {
                break;
            }
        }
        
        return bytesWritten;
    }

    /**
     * @brief Clear the buffer, removing all content
     */
    void flush() override {
        readIndex = 0;
        writeIndex = 0;
        full = false;
    }

    /**
     * @brief Check if the buffer is empty
     * @return true if the buffer is empty, false otherwise
     */
    bool isEmpty() const {
        return !full && (readIndex == writeIndex);
    }

    /**
     * @brief Check if the buffer is full
     * @return true if the buffer is full, false otherwise
     */
    bool isFull() const {
        return full;
    }

    /**
     * @brief Get the number of bytes that can be written without blocking
     * @return Number of bytes available for writing
     */
    int availableForWrite() override {
        if (full) {
            return 0;
        }
        if (readIndex > writeIndex) {
            return readIndex - writeIndex;
        } else {
            return maxSize - (writeIndex - readIndex);
        }
    }

    /**
     * @brief Get the total capacity of the buffer
     * @return Total buffer capacity in bytes
     */
    size_t size() const {
        return maxSize;
    }

    /**
     * @brief Get the number of bytes currently in the buffer
     * @return Number of bytes in the buffer
     */
    size_t used() const {
        return available();
    }

    /**
     * @brief Get the number of bytes still available in the buffer
     * @return Number of free bytes in the buffer
     */
    size_t free() const {
        return maxSize - available();
    }

    /**
     * @brief Read multiple bytes from the buffer
     * @param buffer Buffer to store the read data
     * @param size Maximum number of bytes to read
     * @return Number of bytes actually read
     */
    size_t readBytes(char* buffer, size_t size) override {
        size_t bytesRead = 0;
        
        for (size_t i = 0; i < size; i++) {
            int value = read();
            if (value >= 0) {
                buffer[i] = (char)value;
                bytesRead++;
            } else {
                break;
            }
        }
        
        return bytesRead;
    }

    /**
     * @brief Read multiple bytes from the buffer
     * @param buffer Buffer to store the read data
     * @param size Maximum number of bytes to read
     * @return Number of bytes actually read
     */
    size_t readBytes(uint8_t* buffer, size_t size) {
        return readBytes(reinterpret_cast<char*>(buffer), size);
    }

    /**
     * @brief Get direct access to the underlying vector
     * @return Reference to the underlying vector
     */
    VectorType& getVector() {
        return buffer;
    }

    /**
     * @brief Get const access to the underlying vector
     * @return Const reference to the underlying vector
     */
    const VectorType& getVector() const {
        return buffer;
    }
};

/**
 * @brief Type alias for a RingBufferStream that uses PSRAM-backed vector storage
 */
using RingBufferStreamPSRAM = RingBufferStream<VectorPSRAM<uint8_t>>;

/**
 * @brief Type alias for a RingBufferStream that uses HIMEM-backed vector storage
 */
using RingBufferStreamHIMEM = RingBufferStream<VectorHIMEM<uint8_t>>;

/**
 * @brief Type alias for a RingBufferStream that uses std::vector storage
 */

using RingBufferStreamRAM = RingBufferStream<std::vector<uint8_t>>;


} // namespace esp32_psram