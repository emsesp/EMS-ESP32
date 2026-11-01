#include "OtaUpdater.h"

#include <Update.h>
#include <esp_app_format.h>
#include <esp_image_format.h>
#ifdef OTA_UPDATER_ANY_PARTITION
#include <esp_heap_caps.h>
#include <esp_partition.h>
#endif

#include <algorithm>
#include <cstddef>

bool OtaUpdater::_allow_unsigned = false;

static String getFilenameExtension(const String & filename) {
    const auto pos = filename.lastIndexOf('.');
    if (pos != -1) {
        return filename.substring(static_cast<unsigned int>(pos) + 1);
    }
    return {};
}

OtaUpdater::FileType OtaUpdater::classify(const String & filename, size_t filesize, size_t min_firmware_size) {
    const String extension = getFilenameExtension(filename);
    if (extension == "bin" && filename.endsWith("littlefs.bin")) {
        return FileType::FILESYSTEM;
    }
    if (extension == "bin") {
        String lower = filename;
        lower.toLowerCase();
        const size_t min_size =
            lower.indexOf("recovery") >= 0 ? MIN_RECOVERY_FIRMWARE_SIZE : min_firmware_size;
        if (filesize >= min_size) {
            return FileType::FIRMWARE;
        }
    }
    if (extension == "json") {
        return FileType::JSON;
    }
    if (extension == "md5") {
        return FileType::MD5;
    }
    return FileType::UNSUPPORTED;
}

bool OtaUpdater::isCompatibleFirmware(const uint8_t * data, size_t len) {
    // 0xE9 magic at offset 0 indicates an esp bin, chip id at offset 12
#if CONFIG_IDF_TARGET_ESP32
    constexpr uint8_t chip_id = ESP_CHIP_ID_ESP32;
#elif CONFIG_IDF_TARGET_ESP32S2
    constexpr uint8_t chip_id = ESP_CHIP_ID_ESP32S2;
#elif CONFIG_IDF_TARGET_ESP32C3
    constexpr uint8_t chip_id = ESP_CHIP_ID_ESP32C3;
#elif CONFIG_IDF_TARGET_ESP32S3
    constexpr uint8_t chip_id = ESP_CHIP_ID_ESP32S3;
#elif CONFIG_IDF_TARGET_ESP32C6
    constexpr uint8_t chip_id = ESP_CHIP_ID_ESP32C6;
#else
    return true;
#endif
    return len <= 12 || (data[0] == ESP_IMAGE_HEADER_MAGIC && data[12] == chip_id);
}

bool OtaUpdater::isRecoveryFirmware(const uint8_t * data, size_t len) {
    constexpr size_t offset = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
    if (len < offset + offsetof(esp_app_desc_t, time)) {
        return false;
    }
    esp_app_desc_t desc;
    memcpy(&desc, data + offset, offsetof(esp_app_desc_t, time));
    return desc.magic_word == ESP_APP_DESC_MAGIC_WORD && strncmp(desc.project_name, RECOVERY_PROJECT_NAME, sizeof(desc.project_name)) == 0;
}

bool OtaUpdater::isRecoveryPartition(const esp_partition_t * partition, esp_app_desc_t * desc) {
    esp_app_desc_t d;
    if (partition == nullptr || esp_ota_get_partition_description(partition, &d) != ESP_OK
        || strncmp(d.project_name, RECOVERY_PROJECT_NAME, sizeof(d.project_name)) != 0) {
        return false;
    }
    if (desc != nullptr) {
        *desc = d;
    }
    return true;
}

bool OtaUpdater::parseMd5Digest(const uint8_t * data, size_t len, std::array<char, 33> & out) {
    size_t i = 0;
    while (i < len && (data[i] == ' ' || data[i] == '\t' || data[i] == '\r' || data[i] == '\n')) {
        ++i;
    }
    if (len - i < 32) {
        return false;
    }
    for (size_t n = 0; n < 32; n++) {
        const char c   = static_cast<char>(data[i + n]);
        const bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!hex) {
            return false;
        }
        out[n] = (c >= 'A' && c <= 'F') ? static_cast<char>(c - 'A' + 'a') : c;
    }
    out[32] = '\0';
    return true;
}

bool OtaUpdater::confirmRunningApp() {
#ifdef CONFIG_APP_ROLLBACK_ENABLE
    esp_ota_img_states_t state;
    if (esp_ota_get_state_partition(esp_ota_get_running_partition(), &state) == ESP_OK && state == ESP_OTA_IMG_PENDING_VERIFY) {
        return esp_ota_mark_app_valid_cancel_rollback() == ESP_OK;
    }
#endif
    return false;
}

bool OtaUpdater::setMd5(const uint8_t * data, size_t len) {
    return parseMd5Digest(data, len, _md5);
}

void OtaUpdater::clearMd5() {
    _md5.front() = '\0';
    _md5_applied = false;
}

bool OtaUpdater::hasMd5() const {
    return strlen(_md5.data()) == _md5.size() - 1;
}

#ifdef OTA_UPDATER_ANY_PARTITION
void OtaUpdater::setError(esp_err_t err) {
    snprintf(_error_buf, sizeof(_error_buf), "%s (0x%x)", esp_err_to_name(err), static_cast<unsigned>(err));
    _error = _error_buf;
}

void OtaUpdater::releaseTarget() {
    _target = nullptr;
    heap_caps_free(_dram);
    _dram = nullptr;
}
#endif

bool OtaUpdater::beginFirmware(size_t filesize, const esp_partition_t * target, bool set_boot) {
    _md5_applied = false;
    _not_signed  = false;
    _filesystem  = false;
    _error       = "";

    confirmRunningApp();

    _signature.begin();
    if (hasMd5()) {
        _md5Builder.begin();
        _md5_applied = true;
    }

    if (target != nullptr) {
#ifdef OTA_UPDATER_ANY_PARTITION
        if (filesize > target->size + 4096) { // allow for the multipart overhead
            _error = "Firmware does not fit in the partition";
            return false;
        }
        _dram = static_cast<uint8_t *>(heap_caps_malloc(DRAM_CHUNK, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        if (_dram == nullptr) {
            _error = "No internal RAM for OTA buffer";
            return false;
        }
        _target   = target;
        _written  = 0;
        _erased   = 0;
        _set_boot = set_boot;
        return true;
#else
        _error = "Writing to a specific partition is not supported";
        return false;
#endif
    }

    const esp_partition_t * next = esp_ota_get_next_update_partition(nullptr);
    if (next != nullptr && filesize > next->size + 4096) {
        _error = "Firmware does not fit in the partition";
        return false;
    }
    // the content length isn't the image size, end() takes what was written
    return Update.begin(UPDATE_SIZE_UNKNOWN);
}

bool OtaUpdater::beginFilesystem() {
    clearMd5(); // a digest is only for firmware
    _not_signed = false;
    _filesystem = true;
    _error      = "";
    if (!_allow_unsigned) {
        _not_signed = true;
        _error      = "A filesystem image can only be uploaded after allowing unsigned firmware on the serial console";
        return false;
    }
    // the HTTP content length is the multipart body size, not the file size, so it can exceed
    // the partition by a few hundred bytes. Let the Update library size against the whole partition.
    return Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
}

bool OtaUpdater::write(const uint8_t * data, size_t len) {
    if (_md5_applied) {
        _md5Builder.add(data, len);
    }
    if (_filesystem) {
        return writeImage(data, len);
    }
    return _signature.write(data, len, [this](const uint8_t * d, size_t n) { return writeImage(d, n); });
}

bool OtaUpdater::writeImage(const uint8_t * data, size_t len) {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        while (len) {
            const size_t n = len < DRAM_CHUNK ? len : DRAM_CHUNK;
            memcpy(_dram, data, n);
            if (_written < sizeof(_header)) {
                const size_t h = std::min(n, sizeof(_header) - _written);
                memcpy(_header + _written, _dram, h);
                memset(_dram, 0xFF, h); // erased flash, the image isn't valid until commitTarget()
            }

            const size_t need = _written + n;
            if (need > _erased) {
                size_t erase_to = (need + FLASH_SECTOR - 1) / FLASH_SECTOR * FLASH_SECTOR;
                if (erase_to > _target->size) {
                    erase_to = _target->size;
                }
                const size_t erase_len = erase_to - _erased;
                if (erase_len > 0) {
                    const esp_err_t err = esp_partition_erase_range(_target, _erased, erase_len);
                    if (err != ESP_OK) {
                        setError(err);
                        return false;
                    }
                    _erased = erase_to;
                }
            }

            const esp_err_t err = esp_partition_write(_target, _written, _dram, n);
            if (err != ESP_OK) {
                setError(err);
                return false;
            }
            _written += n;
            data += n;
            len -= n;
        }
        return true;
    }
#endif
    return Update.write(const_cast<uint8_t *>(data), len) == len;
}

bool OtaUpdater::end() {
    if (_md5_applied) {
        _md5Builder.calculate();
        if (strcmp(_md5Builder.toString().c_str(), _md5.data()) != 0) {
            _error = "MD5 check failed";
            abort();
            return false;
        }
    }

    if (!_filesystem && !_signature.verify()) {
        if (!_allow_unsigned) {
            _not_signed = true;
            _error      = NOT_SIGNED_ERROR;
            abort();
            return false;
        }
        // not signed, so the held back bytes are the end of the image
        if (!writeImage(_signature.tail(), _signature.tail_size())) {
            abort();
            return false;
        }
    }

    bool ok;
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        ok = commitTarget();
        releaseTarget();
    } else
#endif
    {
        ok = Update.end(true);
    }

    if (ok) {
        _allow_unsigned = false;
    }
    return ok;
}

#ifdef OTA_UPDATER_ANY_PARTITION
bool OtaUpdater::commitTarget() {
    if (_written < sizeof(_header)) {
        _error = "Firmware is too small";
        return false;
    }
    memcpy(_dram, _header, sizeof(_header));
    esp_err_t err = esp_partition_write(_target, 0, _dram, sizeof(_header));
    if (err != ESP_OK) {
        setError(err);
        return false;
    }

    esp_image_metadata_t meta     = {};
    esp_partition_pos_t  part_pos = {.offset = _target->address, .size = _target->size};
    err                           = esp_image_verify(ESP_IMAGE_VERIFY_SILENT, &part_pos, &meta);
    if (err == ESP_OK && _set_boot) {
        err = esp_ota_set_boot_partition(_target);
    }
    if (err != ESP_OK) {
        setError(err);
        esp_partition_erase_range(_target, 0, FLASH_SECTOR); // so it can't be booted
        return false;
    }
    return true;
}
#endif

void OtaUpdater::abort() {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        releaseTarget();
        return;
    }
#endif
    if (Update.isRunning()) {
        Update.abort();
    }
}

size_t OtaUpdater::progress() const {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        return _written;
    }
#endif
    return Update.progress();
}

const char * OtaUpdater::errorString() const {
    return _error[0] != '\0' ? _error : Update.errorString();
}
