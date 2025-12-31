#pragma once

#include "HimemBlock.h"

namespace esp32_psram {


/**
 * @class VectorHIMEM
 * @brief Vector implementation that uses ESP32's high memory (himem) for
 * storage
 * @tparam T Type of elements stored in the vector
 */
template <typename T>
class VectorHIMEM {
 public:
  // Type definitions
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;

  /**
   * @brief Default constructor - creates an empty vector
   */
  VectorHIMEM() = default;

  /**
   * @brief Constructs a vector with the given number of default-initialized
   * elements
   * @param count The size of the vector
   */
  explicit VectorHIMEM(size_type count) { resize(count); }

  /**
   * @brief Constructs a vector with the given number of copies of a value
   * @param count The size of the vector
   * @param value The value to initialize elements with
   */
  VectorHIMEM(size_type count, const T& value) { resize(count, value); }

  /**
   * @brief Copy constructor
   * @param other The vector to copy from
   */
  VectorHIMEM(const VectorHIMEM& other) {
    if (other.element_count > 0) {
      if (reallocate(other.element_count)) {
        T temp;
        VectorHIMEM& other_ = const_cast<VectorHIMEM&>(other);
        for (size_t i = 0; i < other.element_count; ++i) {
          other_.memory.read((void*)&temp, i * sizeof(T), sizeof(T));
          memory.write((void*)&temp, i * sizeof(T), sizeof(T));
        }
        element_count = other.element_count;
      }
    }
  }

  /**
   * @brief Move constructor
   * @param other The vector to move from
   */
  VectorHIMEM(VectorHIMEM&& other) noexcept
      : memory(std::move(other.memory)),
        element_count(other.element_count),
        element_capacity(other.element_capacity) {
    other.element_count = 0;
    other.element_capacity = 0;
  }

  /**
   * @brief Initializer list constructor
   * @param init The initializer list to copy from
   */
  VectorHIMEM(std::initializer_list<T> init) {
    if (init.size() > 0) {
      if (reallocate(init.size())) {
        size_t i = 0;
        for (const auto& item : init) {
          memory.write(&item, i * sizeof(T), sizeof(T));
          ++i;
        }
        element_count = init.size();
      }
    }
  }

  /**
   * @brief Destructor - ensures all elements are properly destroyed
   */
  ~VectorHIMEM() { clear(); }

  /**
   * @brief Copy assignment operator
   * @param other The vector to copy from
   * @return Reference to this vector
   */
  VectorHIMEM& operator=(const VectorHIMEM& other) {
    if (this != &other) {
      clear();
      if (other.element_count > 0) {
        if (reallocate(other.element_count)) {
          T temp;
          for (size_t i = 0; i < other.element_count; ++i) {
            const_cast<esp32_psram::HimemBlock*>(&other.memory)->read(&temp, i * sizeof(T), sizeof(T));
            memory.write(&temp, i * sizeof(T), sizeof(T));
          }
          element_count = other.element_count;
        }
      }
    }
    return *this;
  }

  /**
   * @brief Move assignment operator
   * @param other The vector to move from
   * @return Reference to this vector
   */
  VectorHIMEM& operator=(VectorHIMEM&& other) noexcept {
    if (this != &other) {
      clear();
      memory = std::move(other.memory);
      element_count = other.element_count;
      element_capacity = other.element_capacity;
      other.element_count = 0;
      other.element_capacity = 0;
    }
    return *this;
  }

  /**
   * @brief Initializer list assignment operator
   * @param ilist The initializer list to copy from
   * @return Reference to this vector
   */
  VectorHIMEM& operator=(std::initializer_list<T> ilist) {
    clear();
    if (ilist.size() > 0) {
      if (reallocate(ilist.size())) {
        size_t i = 0;
        for (const auto& item : ilist) {
          memory.write(&item, i * sizeof(T), sizeof(T));
          ++i;
        }
        element_count = ilist.size();
      }
    }
    return *this;
  }

  /**
   * @brief Access element with bounds checking
   * @param pos The position of the element
   * @return Reference to the element at position pos
   * @throws std::out_of_range if pos is not within the range of the vector
   */
  reference at(size_type pos) {
    if (pos >= element_count) {
      throw std::out_of_range("VectorHIMEM: index out of range");
    }
    static T temp;
    memory.read(&temp, pos * sizeof(T), sizeof(T));
    return temp;
  }

  /**
   * @brief Access element with bounds checking (const version)
   * @param pos The position of the element
   * @return Const reference to the element at position pos
   * @throws std::out_of_range if pos is not within the range of the vector
   */
  const_reference at(size_type pos) const {
    if (pos >= element_count) {
      throw std::out_of_range("VectorHIMEM: index out of range");
    }
    static T temp;
    memory.read(&temp, pos * sizeof(T), sizeof(T));
    return temp;
  }

  /**
   * @brief Access element without bounds checking
   * @param pos The position of the element
   * @return Reference to the element at position pos
   */
  reference operator[](size_type pos) {
    static T temp;
    memory.read(&temp, pos * sizeof(T), sizeof(T));
    return temp;
  }

  // Add const version of operator[]
  const_reference operator[](size_type pos) const {
     static T result;
    // Need to cast away const for the read operation since it doesn't modify content
    HimemBlock& non_const_memory = const_cast<HimemBlock&>(memory);
    non_const_memory.read(&result, pos * sizeof(T), sizeof(T));
    return result;
  }


  /**
   * @brief Access the first element
   * @return Reference to the first element
   */
  reference front() {
    static T temp;
    memory.read(&temp, 0, sizeof(T));
    return temp;
  }

  /**
   * @brief Access the first element (const version)
   * @return Const reference to the first element
   */
  const_reference front() const {
    static T temp;
    memory.read(&temp, 0, sizeof(T));
    return temp;
  }

  /**
   * @brief Access the last element
   * @return Reference to the last element
   */
  reference back() {
    static T temp;
    memory.read(&temp, (element_count - 1) * sizeof(T), sizeof(T));
    return temp;
  }

  /**
   * @brief Access the last element (const version)
   * @return Const reference to the last element
   */
  const_reference back() const {
    static T temp;
    memory.read(&temp, (element_count - 1) * sizeof(T), sizeof(T));
    return temp;
  }

  /**
   * @brief Check if the vector is empty
   * @return true if the vector is empty, false otherwise
   */
  bool empty() const { return element_count == 0; }

  /**
   * @brief Get the number of elements
   * @return The number of elements in the vector
   */
  size_type size() const { return element_count; }

  /**
   * @brief Get the maximum possible number of elements
   * @return The maximum possible number of elements the vector can hold
   */
  size_type max_size() const {
    return std::numeric_limits<size_type>::max() / sizeof(T);
  }

  /**
   * @brief Reserve storage
   * @param new_cap The new capacity of the vector
   */
  void reserve(size_type new_cap) {
    if (new_cap > element_capacity) {
      reallocate(new_cap);
    }
  }

  /**
   * @brief Get the number of elements that can be held in current storage
   * @return The capacity of the vector
   */
  size_type capacity() const { return element_capacity; }

  /**
   * @brief Clear the contents
   */
  void clear() {
    // We're using plain memory so no destructors to call
    element_count = 0;
  }

  /**
   * @brief Add an element to the end
   * @param value The value to append
   */
  void push_back(const T& value) {
    if (element_count >= element_capacity) {
      size_t new_capacity =
          element_capacity == 0 ? min_elements : element_capacity * 2;
      if (!reallocate(new_capacity)) {
        ESP_LOGE(TAG, "Failed to reallocate for push_back");
        return;
      }
    }

    // Write the new element at the end
    memory.write(&value, element_count * sizeof(T), sizeof(T));
    ++element_count;
  }

  /**
   * @brief Add an element to the end by moving it
   * @param value The value to append
   */
  void push_back(T&& value) {
    // For POD types, this is the same as the const& version
    push_back(static_cast<const T&>(value));
  }

  /**
   * @brief Remove the last element
   */
  void pop_back() {
    if (element_count > 0) {
      --element_count;
    }
  }

  /**
   * @brief Change the number of elements stored
   * @param count The new size of the vector
   */
  void resize(size_type count) {
    if (count > element_capacity) {
      if (!reallocate(count)) {
        return;
      }
    }
    element_count = count;
  }

  /**
   * @brief Change the number of elements stored
   * @param count The new size of the vector
   * @param value The value to initialize new elements with
   */
  void resize(size_type count, const value_type& value) {
    size_t old_size = element_count;

    if (count > element_count) {
      // Need to add elements
      if (count > element_capacity) {
        if (!reallocate(count)) {
          return;
        }
      }

      // Initialize new elements with value
      for (size_t i = old_size; i < count; ++i) {
        memory.write(&value, i * sizeof(T), sizeof(T));
      }
    }

    element_count = count;
  }

  /**
   * @brief Erase an element
   * @param pos Index of the element to erase
   */
  void erase(size_type pos) {
    if (pos >= element_count) {
      return;
    }

    // Move each element down by one, overwriting the erased element
    T temp;
    for (size_t i = pos + 1; i < element_count; ++i) {
      memory.read(&temp, i * sizeof(T), sizeof(T));
      memory.write(&temp, (i - 1) * sizeof(T), sizeof(T));
    }

    --element_count;
  }

  /**
   * @brief Insert an element at a specific position
   * @param pos Position where the element should be inserted
   * @param value The value to insert
   */
  void insert(size_type pos, const T& value) {
    if (pos > element_count) {
      return;
    }

    if (element_count >= element_capacity) {
      size_t new_capacity =
          element_capacity == 0 ? min_elements : element_capacity * 2;
      if (!reallocate(new_capacity)) {
        return;
      }
    }

    // Move elements up to make space
    T temp;
    for (size_t i = element_count; i > pos; --i) {
      memory.read(&temp, (i - 1) * sizeof(T), sizeof(T));
      memory.write(&temp, i * sizeof(T), sizeof(T));
    }

    // Insert new element
    memory.write(&value, pos * sizeof(T), sizeof(T));
    ++element_count;
  }

  /**
   * @brief Swap the contents of this vector with another
   * @param other Vector to swap with
   */
  void swap(VectorHIMEM& other) noexcept {
    std::swap(memory, other.memory);
    std::swap(element_count, other.element_count);
    std::swap(element_capacity, other.element_capacity);
  }

 protected:
  HimemBlock memory;
  size_t element_count = 0;
  size_t element_capacity = 0;
  static constexpr size_t min_elements = 16;  // Minimum allocation size

  /**
   * @brief Calculate required memory size in bytes for a given number of
   * elements
   * @param count Number of elements
   * @return Size in bytes
   */
  static size_t calculate_size_bytes(size_t count) { return count * sizeof(T); }

  /**
   * @brief Reallocate memory with a new capacity
   * @param new_capacity The new capacity to allocate
   * @return true if reallocation was successful, false otherwise
   */
  bool reallocate(size_t new_capacity) {
    if (new_capacity <= element_capacity) {
      return true;  // No need to reallocate
    }

    // Calculate new size (at least min_elements)
    new_capacity = std::max(new_capacity, min_elements);

    // Create a new memory block
    HimemBlock new_memory;
    element_capacity = new_memory.allocate(calculate_size_bytes(new_capacity)) / sizeof(T);
    if (element_capacity == 0) {
      ESP_LOGE(TAG, "Failed to allocate new memory block");
      return false;
    }

    // Copy existing elements if any
    if (element_count > 0) {
      T temp;
      for (size_t i = 0; i < element_count; ++i) {
        // Read from old memory
        memory.read(&temp, i * sizeof(T), sizeof(T));

        // Write to new memory
        new_memory.write(&temp, i * sizeof(T), sizeof(T));
      }
    }

    // Swap the memory blocks
    memory = std::move(new_memory);

    return true;
  }


};

/**
 * @brief Equality comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if the vectors are equal, false otherwise
 */
template <typename T>
bool operator==(const VectorHIMEM<T>& lhs, const VectorHIMEM<T>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  T lhs_val, rhs_val;
  for (size_t i = 0; i < lhs.size(); ++i) {
    lhs_val = lhs[i];  // Uses operator[] which calls read()
    rhs_val = rhs[i];
    if (!(lhs_val == rhs_val)) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Inequality comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if the vectors are not equal, false otherwise
 */
template <typename T>
bool operator!=(const VectorHIMEM<T>& lhs, const VectorHIMEM<T>& rhs) {
  return !(lhs == rhs);
}

/**
 * @brief Swap the contents of two vectors
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 */
template <typename T>
void swap(VectorHIMEM<T>& lhs, VectorHIMEM<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace esp32_psram