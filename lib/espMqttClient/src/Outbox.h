
/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if EMC_USE_MEMPOOL
  #include "MemoryPool/src/MemoryPool.h"
  #include "Config.h"
#else
  #include <new>  // new (std::nothrow)
#endif
#include <utility>  // std::forward

namespace espMqttClientInternals {

/**
 * @brief Singly linked queue with builtin non-invalidating forward iterator
 * 
 * Queue items can only be emplaced, at front and back of the queue.
 * Remove items using an iterator or the builtin iterator.
 */

template <typename T>
class Outbox {
 public:
  Outbox()
  : _first(nullptr)
  , _last(nullptr)
  , _current(nullptr)
  , _prev(nullptr)
  #if EMC_USE_MEMPOOL
  , _memPool()
  #endif
  {}
  ~Outbox() {
    while (_first) {
      Node* n = _first->next;
      #if EMC_USE_MEMPOOL
      _first->~Node();
      _memPool.free(_first);
      #else
      delete _first;
      #endif
      _first = n;
    }
  }

  struct Node {
   public:
    template <typename... Args>
    explicit Node(Args&&... args)
    : data(std::forward<Args>(args) ...)
    , next(nullptr) {
      // empty
    }

    T data;
    Node* next;
  };

  class Iterator {
    friend class Outbox;
   public:
    void operator++() {
      if (_node) {
        _prev = _node;
        _node = _node->next;
      }
    }

    explicit operator bool() const {
      if (_node) return true;
      return false;
    }

    T* get() const {
      if (_node) return &(_node->data);
      return nullptr;
    }

   private:
    Node* _node = nullptr;
    Node* _prev = nullptr;
  };

  // add node to back, advance current to new if applicable
  template <class... Args>
  Iterator emplace(Args&&... args) {
    Iterator it;
    #if EMC_USE_MEMPOOL
    void* buf = _memPool.malloc();
    Node* node = nullptr;
    if (buf) {
      node = new(buf) Node(std::forward<Args>(args) ...);
    }
    #else
    Node* node = new(std::nothrow) Node(std::forward<Args>(args) ...);
    #endif
    if (node != nullptr) {
      if (!_first) {
        // queue is empty
        _first = _current = node;
      } else {
        // queue has at least one item
        _last->next = node;
        it._prev = _last;
      }
      _last = node;
      it._node = node;
      // point current to newly created if applicable
      if (!_current) {
        _current = _last;
      }
    }
    return it;
  }

  // add item to front, current points to newly created front.
  template <class... Args>
  Iterator emplaceFront(Args&&... args) {
    Iterator it;
    #if EMC_USE_MEMPOOL
    void* buf = _memPool.malloc();
    Node* node = nullptr;
    if (buf) {
      node = new(buf) Node(std::forward<Args>(args) ...);
    }
    #else
    Node* node = new(std::nothrow) Node(std::forward<Args>(args) ...);
    #endif
    if (node != nullptr) {
      if (!_first) {
        // queue is empty
        _last = node;
      } else {
        // queue has at least one item
        node->next = _first;
      }
      _current = _first = node;
      _prev = nullptr;
      it._node = node;
    }
    return it;
  }

  // remove node at iterator, iterator points to next
  void remove(Iterator& it) {  // NOLINT(runtime/references)
    if (!it) return;
    Node* node = it._node;
    Node* prev = it._prev;
    ++it;
    _remove(prev, node);
  }

  // remove current node, current points to next
  void removeCurrent() {
    _remove(_prev, _current);
  }

  // Get current item or return nullptr
  T* getCurrent() const {
    if (_current) return &(_current->data);
    return nullptr;
  }

  void resetCurrent() {
    _current = _first;
  }

  Iterator front() const {
    Iterator it;
    it._node = _first;
    return it;
  }

  // Advance current item
  void next() {
    if (_current) {
      _prev = _current;
      _current = _current->next;
    }
  }

  // Outbox is empty
  bool empty() {
    if (!_first) return true;
    return false;
  }

  size_t size() const {
    Node* n = _first;
    size_t count = 0;
    while (n) {
      n = n->next;
      ++count;
    }
    return count;
  }

 private:
  Node* _first;
  Node* _last;
  Node* _current;
  Node* _prev;  // element just before _current
  #if EMC_USE_MEMPOOL
  MemoryPool::Fixed<EMC_NUM_POOL_ELEMENTS, sizeof(Node)> _memPool;
  #endif

  void _remove(Node* prev, Node* node) {
    if (!node) return;

    // set current to next, node->next may be nullptr
    if (_current == node) {
      _current = node->next;
    }

    if (_prev == node) {
      _prev = prev;
    }

    // only one element in outbox
    if (_first == _last) {
      _first = _last = nullptr;

    // delete first el in longer outbox
    } else if (_first == node) {
      _first = node->next;

    // delete last in longer outbox
    } else if (_last == node) {
      _last = prev;
      _last->next = nullptr;

    // delete somewhere in the middle
    } else {
      prev->next = node->next;
    }

    // finally, delete the node
      #if EMC_USE_MEMPOOL
      node->~Node();
      _memPool.free(node);
      #else
      delete node;
      #endif
  }
};

}  // end namespace espMqttClientInternals
