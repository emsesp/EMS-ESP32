# Memory Pool

EARLY VERSION. USE AT OWN RISK.

### Description

This is a simple memory pool that doesn't solve the fragmentation problem but contains it. Inside the pool you will still suffer memory fragmentation. The upside is that you're not restricted on memory size. As long as it fits in the pool, you can request any size!

For applications where the (maximum) size to allocate is known, a simple fixed block size memory pool is available. There is no memory fragmentation happening in this case. The downside is wastage of memory if you need less then the specified blocksize.

#### Features

- pool memory is statically allocated
- pool size adjusts on architecture
- no size calculation required: input number of blocks and size of block
- header-only library
- Variable size pool: no restriction on allocated size
- Variable size pool: malloc and free are O(n); The number of allocated blocks affects lookup.
- Fixed size pool: malloc and free are O(1).

[![Test with Platformio](https://github.com/bertmelis/MemoryPool/actions/workflows/test-platformio.yml/badge.svg)](https://github.com/bertmelis/MemoryPool/actions/workflows/test-platformio.yml)
[![cpplint](https://github.com/bertmelis/MemoryPool/actions/workflows/cpplint.yml/badge.svg)](https://github.com/bertmelis/MemoryPool/actions/workflows/cpplint.yml)
<!---[![cppcheck](https://github.com/bertmelis/MemoryPool/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/bertmelis/MemoryPool/actions/workflows/cppcheck.yml)--->

### Usage

#### Variable size pool

```cpp
#include <MemoryPool.h>

Struct MyStruct {
  unsigned int id;
  std::size_t size;
  unsigned char data[256];
};

// pool will be able to hold 10 blocks the size of MyStruct
MemoryPool::Variable<10, sizeof(MyStruct)> pool;

// you can allocate the specified blocksize
// allocation is done in number of 'unsigned char'
MyStruct* s = reinterpret_cast<MyStruct*>(pool.malloc(sizeof(MyStruct)));

// you can allocate less than the specified blocksize
int* i = reinterpret_cast<int*>(pool.malloc(sizeof(int)));

// you can allocate more than the specified blocksize
unsigned char* m = reinterpret_cast<unsigned char*>(pool.malloc(400));

pool.free(s);
pool.free(i);
pool.free(m);
```

#### Fixed size pool

```cpp
#include <MemoryPool.h>

Struct MyStruct {
  unsigned int id;
  std::size_t size;
  unsigned char data[256];
};

// pool will be able to hold 10 blocks the size of MyStruct
MemoryPool::Fixed<10, sizeof(MyStruct)> pool;

// there is no size argument in the malloc function!
MyStruct* s = reinterpret_cast<MyStruct*>(pool.malloc());

// you can allocate less than the specified blocksize
int* i = reinterpret_cast<int*>(pool.malloc());

pool.free(s);
pool.free(i);
```

#### How it works

##### Variable size pool

Free blocks are organized as a linked list with their header (contains pointer to next and size). An allocated block also has this header with it's pointer set to `nullptr`. Therefore, each allocation wastes memory the size of the header (`sizeof(void*) + sizeof(std::size_t)`). On creation, the pool calculations the needed space to store the number of blocks wich each their header.

However, memory allocation isn't restricted the the specified blocksize. So in reality, you can allocate more if you allocate larger chunks because less memory blocks means less headers. After all, memory needs to be contiguous.

If you inspect the pool you'll see that a free pool only has one big block.

Allocation is linear: the pool is iterated until a suitable spot is found.
Freeing is also linear as the pool is traversed to insert the chunk in the linked list of free blocks

When freeing, free blocks which are adjacent are combined into one.

##### Fixed size pool

The fixed size pool is implemented as an array. Free blocks are saved as a linked list in this array.

### Bugs and feature requests

Please use Github's facilities to get in touch.

### License

This library is released under the MIT Licence. A copy is included in the repo.
