#pragma once

#include <esp_heap_caps.h>

/**
 * @namespace esp32_psram
 * @brief Namespace containing ESP32 PSRAM-specific implementations
 */
namespace esp32_psram {

/**
 * @class AllocatorPSRAM
 * @brief Custom allocator that uses ESP32's PSRAM for memory allocation
 * @tparam T Type of elements to allocate
 *
 * This allocator uses ESP32's heap_caps_malloc with MALLOC_CAP_SPIRAM flag
 * to ensure all memory is allocated in PSRAM instead of regular RAM. If PSRAM
 * allocation fails, it falls back to regular RAM.
 */
template <typename T>
class AllocatorPSRAM {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /**
   * @brief Default constructor
   */
  AllocatorPSRAM() noexcept {}

  /**
   * @brief Copy constructor from another allocator type
   * @tparam U Type of the other allocator
   * @param other The other allocator
   */
  template <typename U>
  AllocatorPSRAM(const AllocatorPSRAM<U>&) noexcept {}

  /**
   * @brief Allocate memory from PSRAM
   * @param n Number of elements to allocate
   * @return Pointer to allocated memory
   * @throws std::bad_alloc If allocation fails or size is too large
   */
  pointer allocate(size_type n) {
    // if (n > std::numeric_limits<size_type>::max() / sizeof(T))
    //     throw std::bad_alloc();
    // in Arduino excepitons are disabled!
    assert(n <= std::numeric_limits<size_type>::max() / sizeof(T));

    pointer p = static_cast<pointer>(
        heap_caps_malloc(n * sizeof(T), MALLOC_CAP_SPIRAM));
    if (p == nullptr) {
      p = static_cast<pointer>(malloc(n * sizeof(T)));
    }

    // if (!p) throw std::bad_alloc();

    // in Arduino excepitons are disabled!
    assert(p);

    return p;
  }

  /**
   * @brief Deallocate memory
   * @param p Pointer to memory to deallocate
   * @param size Size of allocation (unused)
   */
  void deallocate(pointer p, size_type) noexcept { heap_caps_free(p); }

  /**
   * @brief Rebind allocator to another type
   * @tparam U Type to rebind the allocator to
   */
  template <typename U>
  struct rebind {
    using other = AllocatorPSRAM<U>;
  };
};

/**
 * @class AllocatorOnlyPSRAM
 * @brief Custom allocator that uses ESP32's PSRAM for memory allocation
 * @tparam T Type of elements to allocate
 *
 * This allocator uses ESP32's heap_caps_malloc with MALLOC_CAP_SPIRAM flag
 * to ensure all memory is allocated in PSRAM instead of regular RAM.
 * If PSRAM allocation fails, it does not fall back to regular RAM.
 */
template <typename T>
class AllocatorOnlyPSRAM {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  /**
   * @brief Default constructor
   */
  AllocatorOnlyPSRAM() noexcept {}

  /**
   * @brief Copy constructor from another allocator type
   * @tparam U Type of the other allocator
   * @param other The other allocator
   */
  template <typename U>
  AllocatorOnlyPSRAM(const AllocatorOnlyPSRAM<U>&) noexcept {}

  /**
   * @brief Allocate memory from PSRAM
   * @param n Number of elements to allocate
   * @return Pointer to allocated memory
   * @throws std::bad_alloc If allocation fails or size is too large
   */
  pointer allocate(size_type n) {
    // if (n > std::numeric_limits<size_type>::max() / sizeof(T))
    //     throw std::bad_alloc();
    // in Arduino excepitons are disabled!
    assert(n <= std::numeric_limits<size_type>::max() / sizeof(T));

    pointer p = static_cast<pointer>(
        heap_caps_malloc(n * sizeof(T), MALLOC_CAP_SPIRAM));
    // if (!p) throw std::bad_alloc();

    // in Arduino excepitons are disabled!
    assert(p);

    return p;
  }

  /**
   * @brief Deallocate memory
   * @param p Pointer to memory to deallocate
   * @param size Size of allocation (unused)
   */
  void deallocate(pointer p, size_type) noexcept { heap_caps_free(p); }

  /**
   * @brief Rebind allocator to another type
   * @tparam U Type to rebind the allocator to
   */
  template <typename U>
  struct rebind {
    using other = AllocatorOnlyPSRAM<U>;
  };
};

}  // namespace esp32_psram