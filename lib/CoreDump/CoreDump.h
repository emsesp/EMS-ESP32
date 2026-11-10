#pragma once

#include <string>

class AsyncWebServerRequest;

// the core dump the last crash left in the coredump partition, shared by EMS-ESP and the recovery firmware
namespace CoreDump {

// one line with the crashed task, PC, backtrace and the firmware's ELF SHA256, empty if there is no dump
std::string summary();

// sends the raw dump as coredump.bin, for decoding with esp-coredump. 404 if there is no dump
void send(AsyncWebServerRequest * request);

// erases the coredump partition. The dump holds memory contents (passwords, tokens), so a factory reset removes it too.
// true if there is no partition to erase
bool erase();

} // namespace CoreDump
