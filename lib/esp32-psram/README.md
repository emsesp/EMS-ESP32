# ESP32-PSRAM Library

[![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue.svg)](https://www.arduino.cc/reference/en/libraries/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A comprehensive memory management library for ESP32 microcontrollers, providing efficient data structures that leverage __PSRAM__ and __HIMEM__ for expanded storage capabilities.


## Overview

The ESP32 can only address 4MB of PSRAM directly, so the remaining 4MB (HIMEM) are usually unsued!

__esp32-psram__ is a C++ library that helps you manage large amounts of data on ESP32 devices by utilizing the full external PSRAM (SPI RAM) supporting the high memory (HIMEM) regions. This allows your applications to work with much larger datasets than would be possible using just internal RAM. 

- **Use any STL Class in PSRAM**:
  - Adapt any [STL class](https://en.wikipedia.org/wiki/Standard_Template_Library) that accepts with an Allocator by using the [AllocatorPSRAM](https://pschatzmann.github.io/esp32-psram/html/classesp32__psram_1_1_p_s_r_a_m_allocator.html)

- **Memory-Efficient Data Structures**:
  - `VectorPSRAM`: Vector implementation that automatically stores data in PSRAM
  - `VectorHIMEM`: Vector implementation for extremely large datasets using ESP32's high memory region
  
- **File System Abstractions**:
  - `FilePSRAM`: File-like interface backed by PSRAM
  - `FileHIMEM`: File-like interface backed by high memory
  - SD card-like API using familiar file operations to write to PSRAM or HIMEM.
  
- **Streaming Data Handling**:
  - `RingBufferStreamRAM`: Circular buffer implementation in RAM (Stream-based)
  - `RingBufferStreamPSRAM`: Circular buffer implementation in PSRAM (Stream-based)
  - `RingBufferStreamHIMEM`: Circular buffer implementation in high memory (Stream-based)
  - Fully compatible with Arduino's Stream class
  
- **Typed Ring Buffers**:
  - `TypedRingBufferRAM<T>`: Type-safe circular buffer for any data type using RAM
  - `TypedRingBufferPSRAM<T>`: PSRAM version for storing complex data structures
  - `TypedRingBufferHIMEM<T>`: High memory version for storing complex data structures
  - Optimized for struct/class storage with proper memory management


## Installation

1. Download the latest release from GitHub
2. Extract the ZIP file
3. Move the folder to your Arduino libraries directory (typically `~/Arduino/libraries/` on Linux/macOS or `Documents\Arduino\libraries\` on Windows)
4. Restart Arduino IDE


## API Reference

- [Class Reference](https://pschatzmann.github.io/esp32-psram/html/namespaceesp32__psram.html)
- [Further Information](https://github.com/pschatzmann/esp32-psram/wiki)

## Memory Performance Comparison

| Memory Type | Access Speed | Capacity | Use Case |
|-------------|--------------|----------|----------|
| RAM (DRAM)  | Very Fast    | ~200-300KB | Small, performance-critical data |
| PSRAM       | Fast         | Up to 4MB  | Medium-sized datasets, frequent access |
| HIMEM       | Medium       | Up to 8MB  | Large datasets, less frequent access |

## Hardware Compatibility

| ESP32 Board | PSRAM Size | HIMEM Support | Notes |
|-------------|------------|--------------|-------|
| ESP32       | Up to 4MB  | Yes          | Most common ESP32 modules |
| ESP32-S2    | Up to 2MB  | Limited      | Check board specifications |
| ESP32-S3    | Up to 8MB  | Yes          | Best memory capabilities |
| ESP32-C3    | None       | No           | Not compatible with this library |

## Design Philosophy

The ESP32-PSRAM library is designed with these principles in mind:

1. **Familiar interfaces**: API design mimics standard C++ containers and Arduino libraries
2. **Memory efficiency**: Automatic use of appropriate memory regions
3. **Type safety**: Template-based to work with any data type
4. **Flexibility**: Configurable for different memory types and use cases
5. **Zero overhead**: Direct memory access with minimal abstraction layers


## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgements

This library was inspired by the need for better memory management solutions on ESP32, especially for audio, image processing, and data logging applications.
