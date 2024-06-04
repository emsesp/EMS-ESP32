/*
Copyright (c) 2024 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <cstddef>  // std::size_t
#include <cassert>  // assert
#if _GLIBCXX_HAS_GTHREADS
#include <mutex>  // NOLINT [build/c++11] std::mutex, std::lock_guard
#else
#warning "The memory pool is not thread safe"
#endif

#ifdef MEMPOL_DEBUG
#include <iostream>
#endif

namespace MemoryPool {

template <std::size_t nrBlocks, std::size_t blocksize>
class Variable {
 public:
  Variable()
    : _buffer{0}
    , _head(nullptr)
    #ifdef MEMPOL_DEBUG
    , _bufferSize(0)
    #endif
      {
      std::size_t _normBlocksize = blocksize / sizeof(BlockHeader) + ((blocksize % sizeof(BlockHeader)) ? 1 : 0);
      size_t nrBlocksToAlloc = nrBlocks * (_normBlocksize + 1);
      BlockHeader* h = reinterpret_cast<BlockHeader*>(_buffer);
      h->next = nullptr;
      h->size = nrBlocksToAlloc;
      _head = h;

      #ifdef MEMPOL_DEBUG
      _bufferSize = nrBlocksToAlloc;
      #endif
    }

  // no copy nor move
  Variable (const Variable&) = delete;
  Variable& operator= (const Variable&) = delete;

  void* malloc(size_t size) {
    #if _GLIBCXX_HAS_GTHREADS
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    #endif
    if (size == 0) return nullptr;

    size = (size / sizeof(BlockHeader) + (size % sizeof(BlockHeader) != 0)) + 1;  // count by BlockHeader size, add 1 for header

    #ifdef MEMPOL_DEBUG
    std::cout << "malloc (raw) " << size << std::endl;
    std::cout << "malloc (adj) " << size << " - ";
    #endif

    BlockHeader* currentBlock = _head;
    BlockHeader* previousBlock = nullptr;
    void* retVal = nullptr;

    // iterate through linked free blocks
    while (currentBlock) {
      // consume whole block is size equals required size
      if (currentBlock->size == size) {
        if (previousBlock) previousBlock->next = currentBlock->next;
        break;

      // split block if size is larger and add second part to list of free blocks
      } else if (currentBlock->size > size) {
        BlockHeader* newBlock = currentBlock + size;
        if (previousBlock) previousBlock->next = newBlock;
        newBlock->next = currentBlock->next;
        newBlock->size = currentBlock->size - size;
        currentBlock->next = newBlock;
        break;
      }
      previousBlock = currentBlock;
      currentBlock = currentBlock->next;
    }

    if (currentBlock) {
      if (currentBlock == _head) {
        _head = currentBlock->next;
      }
      currentBlock->size = size;
      currentBlock->next = nullptr;  // used when freeing memory
      retVal = currentBlock + 1;
      #ifdef MEMPOL_DEBUG
      std::cout << "ok" << std::endl;
      #endif
    } else {
      #ifdef MEMPOL_DEBUG
      std::cout << "nok" << std::endl;
      #endif
      (void)0;
    }

    return retVal;
  }

  void free(void* ptr) {
    if (!ptr) return;
    // check if ptr points to region in _buffer

    #ifdef MEMPOL_DEBUG
    std::cout << "free " << static_cast<void*>(reinterpret_cast<BlockHeader*>(ptr) - 1) << std::endl;
    #endif

    #if _GLIBCXX_HAS_GTHREADS
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    #endif

    BlockHeader* toFree = reinterpret_cast<BlockHeader*>(ptr) - 1;
    BlockHeader* previous = reinterpret_cast<BlockHeader*>(_buffer);
    BlockHeader* next = _head;

    // toFree is the only free block
    if (!next) {
      _head = toFree;
      return;
    }

    while (previous) {
      if (!next || toFree < next) {
        // 1. add block to linked list of free blocks
        if (toFree < _head) {
          toFree->next = _head;
          _head = toFree;
        } else {
          previous->next = toFree;
          toFree->next = next;
        }

        // 2. merge with previous if adjacent
        if (toFree > _head && toFree == previous + previous->size) {
          previous->size += toFree->size;
          previous->next = toFree->next;
          toFree = previous;  // used in next check
        }

        // 3. merge with next if adjacent
        if (toFree + toFree->size == next) {
          toFree->size += next->size;
          toFree->next = next->next;
        }

        // 4. done
        return;
      }
      previous = next;
      next = next->next;
    }
  }

  std::size_t freeMemory() {
    #if _GLIBCXX_HAS_GTHREADS
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    #endif
    size_t retVal = 0;
    BlockHeader* currentBlock = reinterpret_cast<BlockHeader*>(_head);

    while (currentBlock) {
      retVal += currentBlock->size - 1;
      currentBlock = currentBlock->next;
    }

    return retVal * sizeof(BlockHeader);
  }

  std::size_t maxBlockSize() {
    #if _GLIBCXX_HAS_GTHREADS
    const std::lock_guard<std::mutex> lockGuard(_mutex);
    #endif
    size_t retVal = 0;
    BlockHeader* currentBlock = reinterpret_cast<BlockHeader*>(_head);

    while (currentBlock) {
      retVal = (currentBlock->size - 1 > retVal) ? currentBlock->size - 1 : retVal;
      currentBlock = currentBlock->next;
    }

    return retVal * sizeof(BlockHeader);
  }

  #ifdef MEMPOL_DEBUG
  void print() {
    std::cout << "+--------------------" << std::endl;
    std::cout << "|start:" << static_cast<void*>(_buffer) << std::endl;
    std::cout << "|size:" << _bufferSize << std::endl;
    std::cout << "|headersize:" << sizeof(BlockHeader) << std::endl;
    std::cout << "|head: " << static_cast<void*>(_head) << std::endl;
    BlockHeader* nextFreeBlock = _head;
    BlockHeader* currentBlock = reinterpret_cast<BlockHeader*>(_buffer);
    size_t blockNumber = 1;
    while (currentBlock < reinterpret_cast<BlockHeader*>(_buffer) + _bufferSize) {
      std::cout << "|" << blockNumber << ": " << static_cast<void*>(currentBlock) << std::endl;
      std::cout << "|   " << static_cast<void*>(currentBlock->next) << std::endl;
      std::cout << "|   " << currentBlock->size << std::endl;
      if (currentBlock == nextFreeBlock) {
        std::cout << "|   free" << std::endl;
        nextFreeBlock = nextFreeBlock->next;
      } else {
        std::cout << "|   allocated" << std::endl;
      }
      ++blockNumber;
      currentBlock += currentBlock->size;
    }
    std::cout << "+--------------------" << std::endl;
  }
  #endif

 private:
  struct BlockHeader {
    BlockHeader* next;
    std::size_t size;
  };
  /*
  pool size is aligned to sizeof(BlockHeader).
  requested blocksize is therefore multiple of blockheader (rounded up)
  total size = nr requested blocks * multiplier * blockheadersize

  see constructor for calculation
  */
  unsigned char _buffer[(nrBlocks * ((blocksize / sizeof(BlockHeader) + ((blocksize % sizeof(BlockHeader)) ? 1 : 0)) + 1)) * sizeof(BlockHeader)];
  BlockHeader* _head;
  #if _GLIBCXX_HAS_GTHREADS
  std::mutex _mutex;
  #endif

  #ifdef MEMPOL_DEBUG
  std::size_t _bufferSize;
  #endif
};

}  // end namespace MemoryPool
