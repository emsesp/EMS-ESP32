
#pragma once

#include <Arduino.h>
#include <vector>
#include "VectorPSRAM.h"
#include "VectorHIMEM.h"

namespace esp32_psram {

/**
 * @class TypedRingBuffer
 * @brief A generic ring buffer implementation for any data type
 * @tparam T The data type to store in the buffer
 * @tparam VectorType The vector type to use as underlying storage
 * 
 * This class implements a ring buffer (circular buffer) that uses a vector container
 * for storage. Unlike the Stream-based RingBuffer, this can store any data type.
 */
template <typename T, typename VectorType>
class TypedRingBuffer {
private:
    VectorType buffer;
    size_t readIndex = 0;
    size_t writeIndex = 0;
    bool full = false;
    size_t maxSize;

public:
    /**
     * @brief Constructor with specified buffer capacity
     * @param capacity The maximum number of elements the buffer can hold
     */
    TypedRingBuffer(size_t capacity) : maxSize(capacity) {
        buffer.resize(capacity);
    }

    /**
     * @brief Push an element to the buffer
     * @param value The value to add
     * @return true if the element was added, false if the buffer is full
     */
    bool push(const T& value) {
        if (full) {
            return false;
        }

        buffer[writeIndex] = value;
        advanceWriteIndex();
        return true;
    }

    /**
     * @brief Push an element to the buffer, overwriting oldest data if full
     * @param value The value to add
     * @return true if an old element was overwritten, false otherwise
     */
    bool pushOverwrite(const T& value) {
        bool overwritten = full;
        
        buffer[writeIndex] = value;
        advanceWriteIndex();
        
        return overwritten;
    }

    /**
     * @brief Pop an element from the buffer
     * @param value Reference to store the popped value
     * @return true if an element was popped, false if the buffer is empty
     */
    bool pop(T& value) {
        if (isEmpty()) {
            return false;
        }

        value = buffer[readIndex];
        advanceReadIndex();
        return true;
    }

    /**
     * @brief Peek at the next element without removing it
     * @param value Reference to store the peeked value
     * @return true if an element was peeked, false if the buffer is empty
     */
    bool peek(T& value) const {
        if (isEmpty()) {
            return false;
        }

        value = buffer[readIndex];
        return true;
    }

    /**
     * @brief Get the element at a specific index relative to the read position
     * @param index The relative index from current read position
     * @param value Reference to store the value
     * @return true if the element exists, false otherwise
     */
    bool peekAt(size_t index, T& value) const {
        if (isEmpty() || index >= available()) {
            return false;
        }

        size_t actualIndex = (readIndex + index) % maxSize;
        value = buffer[actualIndex];
        return true;
    }

    /**
     * @brief Clear the buffer, removing all content
     */
    void clear() {
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
     * @brief Get the number of elements in the buffer
     * @return Number of elements in the buffer
     */
    size_t available() const {
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
     * @brief Get the number of empty slots in the buffer
     * @return Number of empty slots
     */
    size_t availableForWrite() const {
        return maxSize - available();
    }

    /**
     * @brief Get the total capacity of the buffer
     * @return Total buffer capacity
     */
    size_t capacity() const {
        return maxSize;
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

private:
    /**
     * @brief Advance the write index
     */
    void advanceWriteIndex() {
        writeIndex = (writeIndex + 1) % maxSize;
        if (writeIndex == readIndex) {
            full = true;
        }
    }

    /**
     * @brief Advance the read index
     */
    void advanceReadIndex() {
        readIndex = (readIndex + 1) % maxSize;
        full = false;
    }
};

/**
 * @brief Type alias for a typed ring buffer that uses std::vector (in RAM)
 */
template<typename T>
using TypedRingBufferRAM = TypedRingBuffer<T, std::vector<T>>;
/**
 * @brief Type alias for a typed ring buffer that uses HIMEM-backed vector storage
 */
template<typename T>
using TypedRingBufferHIMEM = TypedRingBuffer<T, VectorHIMEM<T>>;

/**
 * @brief Type alias for a typed ring buffer that uses PSRAM-backed vector storage
 */
template<typename T>
using TypedRingBufferPSRAM = TypedRingBuffer<T, VectorPSRAM<T>>;

/**
 * @brief Type alias for a typed ring buffer that uses HIMEM-backed vector storage
 */
// template<typename T>
// using TypedRingBufferHIMEM = TypedRingBuffer<T, VectorHIMEM<T>>;

} // namespace esp32_psram