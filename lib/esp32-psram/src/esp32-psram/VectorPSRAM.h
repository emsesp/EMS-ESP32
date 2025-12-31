#pragma once

#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
#include "AllocatorPSRAM.h"

/**
 * @namespace esp32_psram
 * @brief Namespace containing ESP32 PSRAM-specific implementations
 */
namespace esp32_psram {
    

/**
 * @brief Equality comparison operator for allocators
 * @tparam T Type of first allocator
 * @tparam U Type of second allocator
 * @return Always true since all instances are equivalent
 */
template <typename T, typename U>
bool operator==(const AllocatorPSRAM<T>&, const AllocatorPSRAM<U>&) noexcept {
    return true;
}

/**
 * @brief Inequality comparison operator for allocators
 * @tparam T Type of first allocator
 * @tparam U Type of second allocator
 * @return Always false since all instances are equivalent
 */
template <typename T, typename U>
bool operator!=(const AllocatorPSRAM<T>&, const AllocatorPSRAM<U>&) noexcept {
    return false;
}

/**
 * @class VectorPSRAM
 * @brief Vector implementation that uses ESP32's PSRAM for storage
 * @tparam T Type of elements stored in the vector
 * 
 * This class provides an interface identical to std::vector but allocates
 * all memory in ESP32's PSRAM, which helps preserve the limited internal RAM.
 * It wraps std::vector with a custom allocator that uses PSRAM.
 */
template <typename T>
class VectorPSRAM {
private:
    using vector_type = std::vector<T, AllocatorPSRAM<T>>;
    vector_type vec;

public:
    // Type definitions
    using value_type = typename vector_type::value_type;
    using allocator_type = typename vector_type::allocator_type;
    using size_type = typename vector_type::size_type;
    using difference_type = typename vector_type::difference_type;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;
    using pointer = typename vector_type::pointer;
    using const_pointer = typename vector_type::const_pointer;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reverse_iterator = typename vector_type::reverse_iterator;
    using const_reverse_iterator = typename vector_type::const_reverse_iterator;
    
    /**
     * @brief Default constructor - creates an empty vector
     */
    VectorPSRAM() : vec(AllocatorPSRAM<T>()) {}
    
    /**
     * @brief Constructs a vector with the given number of default-initialized elements
     * @param count The size of the vector
     */
    explicit VectorPSRAM(size_type count) : vec(count, AllocatorPSRAM<T>()) {}
    
    /**
     * @brief Constructs a vector with the given number of copies of a value
     * @param count The size of the vector
     * @param value The value to initialize elements with
     */
    VectorPSRAM(size_type count, const T& value) : vec(count, value, AllocatorPSRAM<T>()) {}
    
    /**
     * @brief Constructs a vector with the contents of the range [first, last)
     * @tparam InputIt Input iterator type
     * @param first Iterator to the first element in the range
     * @param last Iterator to one past the last element in the range
     */
    template <typename InputIt>
    VectorPSRAM(InputIt first, InputIt last) : vec(first, last, AllocatorPSRAM<T>()) {}
    
    /**
     * @brief Copy constructor
     * @param other The vector to copy from
     */
    VectorPSRAM(const VectorPSRAM& other) : vec(other.vec) {}
    
    /**
     * @brief Move constructor
     * @param other The vector to move from
     */
    VectorPSRAM(VectorPSRAM&& other) noexcept : vec(std::move(other.vec)) {}
    
    /**
     * @brief Initializer list constructor
     * @param init The initializer list to copy from
     */
    VectorPSRAM(std::initializer_list<T> init) : vec(init, AllocatorPSRAM<T>()) {}
    
    /**
     * @brief Copy assignment operator
     * @param other The vector to copy from
     * @return Reference to this vector
     */
    VectorPSRAM& operator=(const VectorPSRAM& other) {
        vec = other.vec;
        return *this;
    }
    
    /**
     * @brief Move assignment operator
     * @param other The vector to move from
     * @return Reference to this vector
     */
    VectorPSRAM& operator=(VectorPSRAM&& other) noexcept {
        vec = std::move(other.vec);
        return *this;
    }
    
    /**
     * @brief Initializer list assignment operator
     * @param ilist The initializer list to copy from
     * @return Reference to this vector
     */
    VectorPSRAM& operator=(std::initializer_list<T> ilist) {
        vec = ilist;
        return *this;
    }
    
    /**
     * @brief Access element with bounds checking
     * @param pos The position of the element
     * @return Reference to the element at position pos
     * @throws std::out_of_range if pos is not within the range of the vector
     */
    reference at(size_type pos) { return vec.at(pos); }
    
    /**
     * @brief Access element with bounds checking (const version)
     * @param pos The position of the element
     * @return Const reference to the element at position pos
     * @throws std::out_of_range if pos is not within the range of the vector
     */
    const_reference at(size_type pos) const { return vec.at(pos); }
    
    /**
     * @brief Access element without bounds checking
     * @param pos The position of the element
     * @return Reference to the element at position pos
     */
    reference operator[](size_type pos) { return vec[pos]; }
    
    /**
     * @brief Access element without bounds checking (const version)
     * @param pos The position of the element
     * @return Const reference to the element at position pos
     */
    const_reference operator[](size_type pos) const { return vec[pos]; }
    
    /**
     * @brief Access the first element
     * @return Reference to the first element
     */
    reference front() { return vec.front(); }
    
    /**
     * @brief Access the first element (const version)
     * @return Const reference to the first element
     */
    const_reference front() const { return vec.front(); }
    
    /**
     * @brief Access the last element
     * @return Reference to the last element
     */
    reference back() { return vec.back(); }
    
    /**
     * @brief Access the last element (const version)
     * @return Const reference to the last element
     */
    const_reference back() const { return vec.back(); }
    
    /**
     * @brief Get pointer to the underlying array
     * @return Pointer to the underlying array
     */
    T* data() noexcept { return vec.data(); }
    
    /**
     * @brief Get pointer to the underlying array (const version)
     * @return Const pointer to the underlying array
     */
    const T* data() const noexcept { return vec.data(); }
    
    /**
     * @brief Get iterator to the beginning
     * @return Iterator to the first element
     */
    iterator begin() noexcept { return vec.begin(); }
    
    /**
     * @brief Get const iterator to the beginning
     * @return Const iterator to the first element
     */
    const_iterator begin() const noexcept { return vec.begin(); }
    
    /**
     * @brief Get const iterator to the beginning
     * @return Const iterator to the first element
     */
    const_iterator cbegin() const noexcept { return vec.cbegin(); }
    
    /**
     * @brief Get iterator to the end
     * @return Iterator to one past the last element
     */
    iterator end() noexcept { return vec.end(); }
    
    /**
     * @brief Get const iterator to the end
     * @return Const iterator to one past the last element
     */
    const_iterator end() const noexcept { return vec.end(); }
    
    /**
     * @brief Get const iterator to the end
     * @return Const iterator to one past the last element
     */
    const_iterator cend() const noexcept { return vec.cend(); }
    
    /**
     * @brief Get reverse iterator to the beginning
     * @return Reverse iterator to the first element
     */
    reverse_iterator rbegin() noexcept { return vec.rbegin(); }
    
    /**
     * @brief Get const reverse iterator to the beginning
     * @return Const reverse iterator to the first element
     */
    const_reverse_iterator rbegin() const noexcept { return vec.rbegin(); }
    
    /**
     * @brief Get const reverse iterator to the beginning
     * @return Const reverse iterator to the first element
     */
    const_reverse_iterator crbegin() const noexcept { return vec.crbegin(); }
    
    /**
     * @brief Get reverse iterator to the end
     * @return Reverse iterator to one past the last element
     */
    reverse_iterator rend() noexcept { return vec.rend(); }
    
    /**
     * @brief Get const reverse iterator to the end
     * @return Const reverse iterator to one past the last element
     */
    const_reverse_iterator rend() const noexcept { return vec.rend(); }
    
    /**
     * @brief Get const reverse iterator to the end
     * @return Const reverse iterator to one past the last element
     */
    const_reverse_iterator crend() const noexcept { return vec.crend(); }
    
    /**
     * @brief Check if the vector is empty
     * @return true if the vector is empty, false otherwise
     */
    bool empty() const noexcept { return vec.empty(); }
    
    /**
     * @brief Get the number of elements
     * @return The number of elements in the vector
     */
    size_type size() const noexcept { return vec.size(); }
    
    /**
     * @brief Get the maximum possible number of elements
     * @return The maximum possible number of elements the vector can hold
     */
    size_type max_size() const noexcept { return vec.max_size(); }
    
    /**
     * @brief Reserve storage
     * @param new_cap The new capacity of the vector
     * @throws std::length_error if new_cap > max_size()
     */
    void reserve(size_type new_cap) { vec.reserve(new_cap); }
    
    /**
     * @brief Get the number of elements that can be held in current storage
     * @return The capacity of the vector
     */
    size_type capacity() const noexcept { return vec.capacity(); }
    
    /**
     * @brief Reduce memory usage by freeing unused memory
     */
    void shrink_to_fit() { vec.shrink_to_fit(); }
    
    /**
     * @brief Clear the contents
     */
    void clear() noexcept { vec.clear(); }
    
    /**
     * @brief Insert an element
     * @param pos Iterator to the position before which the element will be inserted
     * @param value The value to insert
     * @return Iterator to the inserted element
     */
    iterator insert(const_iterator pos, const T& value) { return vec.insert(pos, value); }
    
    /**
     * @brief Insert an element by moving it
     * @param pos Iterator to the position before which the element will be inserted
     * @param value The value to insert
     * @return Iterator to the inserted element
     */
    iterator insert(const_iterator pos, T&& value) { return vec.insert(pos, std::move(value)); }
    
    /**
     * @brief Insert multiple copies of an element
     * @param pos Iterator to the position before which the elements will be inserted
     * @param count Number of copies to insert
     * @param value The value to insert
     * @return Iterator to the first inserted element
     */
    iterator insert(const_iterator pos, size_type count, const T& value) { return vec.insert(pos, count, value); }
    
    /**
     * @brief Insert elements from a range
     * @tparam InputIt Input iterator type
     * @param pos Iterator to the position before which the elements will be inserted
     * @param first Iterator to the first element in the range
     * @param last Iterator to one past the last element in the range
     * @return Iterator to the first inserted element
     */
    template <typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) { return vec.insert(pos, first, last); }
    
    /**
     * @brief Insert elements from an initializer list
     * @param pos Iterator to the position before which the elements will be inserted
     * @param ilist The initializer list to insert from
     * @return Iterator to the first inserted element
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) { return vec.insert(pos, ilist); }
    
    /**
     * @brief Construct an element in-place
     * @tparam Args Types of arguments to forward to the constructor
     * @param pos Iterator to the position before which the element will be constructed
     * @param args Arguments to forward to the constructor
     * @return Iterator to the inserted element
     */
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) { return vec.emplace(pos, std::forward<Args>(args)...); }
    
    /**
     * @brief Erase an element
     * @param pos Iterator to the element to erase
     * @return Iterator to the element after the erased element
     */
    iterator erase(const_iterator pos) { return vec.erase(pos); }
    
    /**
     * @brief Erase a range of elements
     * @param first Iterator to the first element to erase
     * @param last Iterator to one past the last element to erase
     * @return Iterator to the element after the erased range
     */
    iterator erase(const_iterator first, const_iterator last) { return vec.erase(first, last); }
    
    /**
     * @brief Add an element to the end
     * @param value The value to append
     */
    void push_back(const T& value) { vec.push_back(value); }
    
    /**
     * @brief Add an element to the end by moving it
     * @param value The value to append
     */
    void push_back(T&& value) { vec.push_back(std::move(value)); }
    
    /**
     * @brief Construct an element in-place at the end
     * @tparam Args Types of arguments to forward to the constructor
     * @param args Arguments to forward to the constructor
     * @return Reference to the inserted element
     */
    template <typename... Args>
    reference emplace_back(Args&&... args) { return vec.emplace_back(std::forward<Args>(args)...); }
    
    /**
     * @brief Remove the last element
     */
    void pop_back() { vec.pop_back(); }
    
    /**
     * @brief Change the number of elements stored
     * @param count The new size of the vector
     */
    void resize(size_type count) { vec.resize(count); }
    
    /**
     * @brief Change the number of elements stored
     * @param count The new size of the vector
     * @param value The value to initialize new elements with
     */
    void resize(size_type count, const value_type& value) { vec.resize(count, value); }
    
    /**
     * @brief Swap the contents
     * @param other Vector to swap with
     */
    void swap(VectorPSRAM& other) noexcept { vec.swap(other.vec); }
};

/**
 * @brief Equality comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if the vectors are equal, false otherwise
 */
template <typename T>
bool operator==(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief Inequality comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if the vectors are not equal, false otherwise
 */
template <typename T>
bool operator!=(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less than comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if lhs is lexicographically less than rhs, false otherwise
 */
template <typename T>
bool operator<(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief Less than or equal comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if lhs is lexicographically less than or equal to rhs, false otherwise
 */
template <typename T>
bool operator<=(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater than comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if lhs is lexicographically greater than rhs, false otherwise
 */
template <typename T>
bool operator>(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Greater than or equal comparison operator
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 * @return true if lhs is lexicographically greater than or equal to rhs, false otherwise
 */
template <typename T>
bool operator>=(const VectorPSRAM<T>& lhs, const VectorPSRAM<T>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief Swap the contents of two vectors
 * @tparam T Type of elements in the vectors
 * @param lhs First vector
 * @param rhs Second vector
 */
template <typename T>
void swap(VectorPSRAM<T>& lhs, VectorPSRAM<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace esp32_psram

