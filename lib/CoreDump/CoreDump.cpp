#include "CoreDump.h"

#include <sdkconfig.h>
#include <ESPAsyncWebServer.h>

#include <algorithm>
#include <cinttypes>
#include <memory>

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
#include <esp_core_dump.h>
#include <esp_flash.h>
#define COREDUMP_SUPPORTED
#endif

namespace CoreDump {

std::string summary() {
    std::string result;
#ifdef COREDUMP_SUPPORTED
    if (esp_core_dump_image_check() != ESP_OK) {
        return result; // no partition, no dump or a corrupt one
    }

    // too big for the stack on RISC-V, it holds a copy of the crashing task's stack
    auto summary = std::make_unique<esp_core_dump_summary_t>();
    if (esp_core_dump_get_summary(summary.get()) != ESP_OK) {
        return result;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "task=%.16s pc=0x%08" PRIx32, summary->exc_task, summary->exc_pc);
    result = buf;

#if defined(__XTENSA__)
    snprintf(buf, sizeof(buf), " cause=%" PRIu32 " vaddr=0x%08" PRIx32, summary->ex_info.exc_cause, summary->ex_info.exc_vaddr);
    result += buf;
    result += " bt=";
    for (uint32_t i = 0; i < summary->exc_bt_info.depth && i < 16; i++) {
        snprintf(buf, sizeof(buf), "%s0x%08" PRIx32, i ? " " : "", summary->exc_bt_info.bt[i]);
        result += buf;
    }
    if (summary->exc_bt_info.corrupted) {
        result += " (corrupted)";
    }
#else
    snprintf(buf, sizeof(buf), " ra=0x%08" PRIx32 " mcause=%" PRIu32 " mtval=0x%08" PRIx32, summary->ex_info.ra, summary->ex_info.mcause, summary->ex_info.mtval);
    result += buf;
#endif

    result += " sha=";
    result += (const char *)summary->app_elf_sha256;

    char reason[100];
    if (esp_core_dump_get_panic_reason(reason, sizeof(reason)) == ESP_OK) {
        result += " reason=";
        result += reason;
    }
#endif
    return result;
}

void send(AsyncWebServerRequest * request) {
#ifdef COREDUMP_SUPPORTED
    size_t addr = 0;
    size_t size = 0;
    if (esp_core_dump_image_check() == ESP_OK && esp_core_dump_image_get(&addr, &size) == ESP_OK && size > 0) {
        AsyncWebServerResponse * response =
            request->beginResponse("application/octet-stream", size, [addr, size](uint8_t * buffer, size_t maxLen, size_t index) -> size_t {
                const size_t len = std::min(maxLen, size - index);
                if (len == 0 || esp_flash_read(esp_flash_default_chip, buffer, addr + index, len) != ESP_OK) {
                    return 0;
                }
                return len;
            });
        response->addHeader("Content-Disposition", "attachment; filename=\"coredump.bin\"");
        response->addHeader("Cache-Control", "no-store");
        request->send(response);
        return;
    }
#endif
    request->send(404);
}

} // namespace CoreDump
