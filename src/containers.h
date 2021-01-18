/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2020  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Lightweight queue & array
 * Based ideas from https://github.com/muwerk/ustd
 * Limits to max 255 entries
 * Was used in testing with ESP8266 to overcome heap memory issues - no longer used
 */

#ifndef EMSESP_CONTAINERS_H
#define EMSESP_CONTAINERS_H

#include <Arduino.h>

#if defined EMSESP_ASSERT
#include <assert.h>
#endif

namespace emsesp {

template <typename T>
class queueIterator {
  public:
    queueIterator(T * values_ptr, uint8_t p)
        : values_ptr_{values_ptr}
        , position_{p} {
    }

    bool operator!=(const queueIterator<T> & other) const {
        return !(*this == other);
    }

    bool operator==(const queueIterator<T> & other) const {
        return position_ == other.position_;
    }

    queueIterator & operator++() {
        ++position_;
        return *this;
    }

    T & operator*() const {
        return *(values_ptr_ + position_);
    }

  private:
    T *     values_ptr_;
    uint8_t position_;
};

template <class T>
class queue {
  private:
    T *     que_;
    uint8_t peakSize_;
    uint8_t maxSize_;
    uint8_t size_;
    uint8_t quePtrFront_; // back
    uint8_t quePtrBack_;  // front
    T       bad_;

  public:
    // Constructs a queue object with the maximum number of <T> pointer entries
    queue(uint8_t maxQueueSize)
        : maxSize_(maxQueueSize) {
        memset(&bad_, 0, sizeof(bad_));
        quePtrFront_ = 0;
        quePtrBack_  = 0;
        size_        = 0;
        peakSize_    = 0;
        que_         = (T *)malloc(sizeof(T) * maxSize_);
        if (que_ == nullptr)
            maxSize_ = 0;
    }

    // Deallocate the queue structure
    ~queue() {
        if (que_ != nullptr) {
            free(que_);
            que_ = nullptr;
        }
    }

    // Push a new entry into the queue.
    // true on success, false if queue is full, then the element is relaced with the front one
    bool push(T ent) {
        // Serial.print("quePtrFront_: ");
        // Serial.print(quePtrFront_);
        // Serial.print(" quePtrBack_: ");
        // Serial.print(quePtrBack_);
        // Serial.println();
        if (size_ >= maxSize_) {
            // que_[quePtrFront_] = ent;
            return false;
        }
        que_[quePtrBack_] = ent;
        quePtrBack_       = (quePtrBack_ + 1) % maxSize_;
        ++size_;
        if (size_ > peakSize_) {
            peakSize_ = size_;
        }
        return true;
    }

    bool push_back(T ent) {
        return push(ent);
    }

    // Push a new entry into the front of queue, moving the rest down
    // true on success, false if queue is full
    // there are no good checks for overflow
    bool push_front(T ent) {
        if (size_ >= maxSize_) {
            return false;
        }
        // Serial.print("quePtrFront_: ");
        // Serial.print(quePtrFront_);
        // Serial.print(" quePtrBack_: ");
        // Serial.print(quePtrBack_);
        // Serial.println();

        for (uint8_t i = 0; i <= size_; i++) {
            que_[quePtrBack_ - i + 1] = que_[quePtrBack_ - i]; // move the rest up 1
        }
        que_[quePtrFront_] = ent; // add the new one
        quePtrBack_        = (quePtrBack_ + 1) % maxSize_;
        ++size_;
        if (size_ > peakSize_) {
            peakSize_ = size_;
        }
        return true;
    }

    T & operator[](uint8_t i) {
        return que_[i + quePtrFront_];
    }

    // Pop the oldest entry from the queue
    T pop() {
        if (size_ == 0)
            return bad_;
        T ent        = que_[quePtrFront_];
        quePtrFront_ = (quePtrFront_ + 1) % maxSize_;
        --size_;
        return ent;
    }

    // alias pop_front to keep backwards compatibility with std::list/queue
    T pop_front() {
        return pop();
    }

    T front() {
        return que_[quePtrFront_];
    }

    uint8_t front_i() {
        return quePtrFront_;
    }

    T * front_p() {
        return &que_[quePtrFront_];
    }

    T back() {
        return que_[quePtrBack_];
    }

    // Set the value for <T>entry that's given back, if read from an empty
    // queue is requested. By default, an entry all memset to zero is given
    // back. Using this function, the value of an invalid read can be configured
    void setInvalidValue(T & entryInvalidValue) {
        bad_ = entryInvalidValue;
    }

    // returns true: queue empty, false: not empty
    bool empty() {
        if (size_ == 0)
            return true;
        else
            return false;
    }

    // returns number of entries in the queue
    uint8_t size() {
        return (size_);
    }

    // max number of queue entries that have been in the queue
    uint8_t peak() {
        return (peakSize_);
    }

    // iterators
    queueIterator<T> begin() {
        return queueIterator<T>(que_, quePtrFront_);
    }
    queueIterator<T> end() {
        return queueIterator<T>(que_, quePtrFront_ + size_);
    }

    queueIterator<const T> begin() const {
        return queueIterator<const T>(que_, quePtrFront_);
    }

    queueIterator<const T> end() const {
        return queueIterator<const T>(que_, quePtrFront_ + size_);
    }
};


template <typename T>
class arrayIterator {
  public:
    arrayIterator(T * values_ptr)
        : values_ptr_{values_ptr}
        , position_{0} {
    }

    arrayIterator(T * values_ptr, size_t size_)
        : values_ptr_{values_ptr}
        , position_{size_} {
    }

    bool operator!=(const arrayIterator<T> & other) const {
        return !(*this == other);
    }

    bool operator==(const arrayIterator<T> & other) const {
        return position_ == other.position_;
    }

    arrayIterator & operator++() {
        ++position_;
        return *this;
    }

    T & operator*() const {
        return *(values_ptr_ + position_);
    }

  private:
    T *    values_ptr_;
    size_t position_;
};

#define ARRAY_INIT_SIZE 16
#define ARRAY_MAX_SIZE 255 // fixed for uint8_t
#define ARRAY_INC_SIZE 16

template <typename T>
class array {
  private:
    T *     arr_;
    uint8_t startSize_;
    uint8_t maxSize_;
    uint8_t incSize_ = ARRAY_INC_SIZE;
    uint8_t allocSize_;
    uint8_t size_;
    T       bad_;

  public:
    // Constructs an array object. All allocation-hints are optional, the
    // array class will allocate memory as needed during writes, if
    // startSize_!=maxSize_.
    // @param startSize_ The number of array entries that are allocated
    // during object creation
    // @param maxSize_ The maximal limit of records that will be allocated.
    // If startSize_ < maxSize_, the array size_ will grow automatically as
    // needed.
    // @param incSize_ The number of array entries that are allocated as a
    // chunk if the array needs to grow
    array(uint8_t startSize_ = ARRAY_INIT_SIZE, uint8_t maxSize_ = ARRAY_MAX_SIZE, uint8_t incSize_ = ARRAY_INC_SIZE)
        : startSize_(startSize_)
        , maxSize_(maxSize_)
        , incSize_(incSize_) {
        size_ = 0;
        memset(&bad_, 0, sizeof(bad_));
        if (maxSize_ < startSize_)
            maxSize_ = startSize_;
        allocSize_ = startSize_;
        arr_       = new T[allocSize_];
    }

    ~array() {
        /*! Free resources */
        if (arr_ != nullptr) {
            delete[] arr_;
            arr_ = nullptr;
        }
    }

    // Change the array allocation size_. the new number of array entries, corresponding memory is allocated/free'd as necessary.
    bool resize(uint8_t newSize) {
        uint8_t mv = newSize;
        if (newSize > maxSize_) {
            if (maxSize_ == allocSize_)
                return false;
            else
                newSize = maxSize_;
        }
        if (newSize <= allocSize_)
            return true;
        T * arrn = new T[newSize];
        if (arrn == nullptr)
            return false;
        for (uint8_t i = 0; i < mv; i++) {
            arrn[i] = arr_[i];
        }
        delete[] arr_;
        arr_       = arrn;
        allocSize_ = newSize;
        return true;
    }

    // Set the value for <T>entry that's given back,
    // if read of an invalid index is requested.
    // By default, an entry all memset to zero is given
    // back. Using this function, the value of an invalid read can be configured.
    // returns the value that is given back in case an invalid operation (e.g. read out of bounds) is tried
    void setInvalidValue(T & entryInvalidValue) {
        bad_ = entryInvalidValue;
    }

    // Append an array element after the current end of the array
    // takes entry array element that is appended after the last current
    // entry. The new array size_ must be smaller than maxSize_ as defined
    // during array creation. New array memory is automatically allocated if
    // within maxSize_ boundaries
    int push(T & entry) {
        if (size_ >= allocSize_) {
            if (incSize_ == 0)
                return -1;
            if (!resize(allocSize_ + incSize_))
                return -1;
        }
        arr_[size_] = entry;
        ++size_;
        return size_ - 1;
    }

    // Assign content of array element at i for const's
    T operator[](uint8_t i) const {
        if (i >= allocSize_) {
            if (incSize_ == 0) {
#ifdef EMSESP_ASSERT
                assert(i < allocSize_);
#endif
            }
            if (!resize(allocSize_ + incSize_)) {
#ifdef EMSESP_ASSERT
                assert(i < allocSize_);
#endif
            }
        }
        if (i >= size_ && i <= allocSize_)
            size_ = i + 1;
        if (i >= allocSize_) {
            return bad_;
        }
        return arr_[i];
    }

    // Assign content of array element at i
    T & operator[](uint8_t i) {
        if (i >= allocSize_) {
            if (incSize_ == 0) {
#ifdef EMSESP_ASSERT
                assert(i < allocSize_);
#endif
            }
            if (!resize(allocSize_ + incSize_)) {
#ifdef EMSESP_ASSERT
                assert(i < allocSize_);
#endif
            }
        }
        if (i >= size_ && i <= allocSize_)
            size_ = i + 1;
        if (i >= allocSize_) {
            return bad_;
        }
        return arr_[i];
    }

    // true if array empty, false otherwise
    bool empty() const {
        if (size_ == 0)
            return true;
        else
            return false;
    }

    // return number of array elements
    uint8_t size() const {
        return (size_);
    }

    // returns number of allocated entries which can be larger than the length of the array
    uint8_t alloclen() const {
        return (allocSize_);
    }

    // emplace
    // template <typename... Args>
    // void emplace1(Args... args) {
    //     add(args...);
    // };
    // template <class... Args>
    // void emplace(Args &&... args) {
    //     add(T(std::forward<Args>(args)...));
    // }

    // iterators
    arrayIterator<T> begin() {
        return arrayIterator<T>(arr_);
    }
    arrayIterator<T> end() {
        return arrayIterator<T>(arr_, size_);
    }

    arrayIterator<const T> begin() const {
        return arrayIterator<const T>(arr_);
    }

    arrayIterator<const T> end() const {
        return arrayIterator<const T>(arr_, size_);
    }
};

} // namespace emsesp

#endif
