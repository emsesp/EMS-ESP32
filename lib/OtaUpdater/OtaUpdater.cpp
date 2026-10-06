#include "OtaUpdater.h"

#include <Update.h>
#include <esp_app_format.h>
#include <esp_image_format.h>

#include <cstddef>

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
    if (extension == "bin" && filesize >= min_firmware_size) {
        return FileType::FIRMWARE;
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

bool OtaUpdater::beginFirmware(size_t filesize, const esp_partition_t * target, bool set_boot) {
    _md5_applied = false;
    _error       = "";

    confirmRunningApp();

    if (target != nullptr) {
#ifdef OTA_UPDATER_ANY_PARTITION
        if (filesize > target->size + 4096) { // allow for the multipart overhead
            _error = "Firmware does not fit in the partition";
            return false;
        }
        const esp_err_t err = esp_ota_begin(target, OTA_WITH_SEQUENTIAL_WRITES, &_handle);
        if (err != ESP_OK) {
            _error = esp_err_to_name(err);
            return false;
        }
        _target   = target;
        _written  = 0;
        _set_boot = set_boot;
        if (hasMd5()) {
            _md5Builder.begin();
            _md5_applied = true;
        }
        return true;
#else
        _error = "Writing to a specific partition is not supported";
        return false;
#endif
    }

    if (!Update.begin(filesize - sizeof(esp_image_header_t))) {
        return false;
    }
    if (hasMd5()) {
        Update.setMD5(_md5.data());
        _md5_applied = true;
    }
    return true;
}

bool OtaUpdater::beginFilesystem() {
    clearMd5(); // so Update.end() doesn't compare against a stale digest
    _error = "";
    // the HTTP content length is the multipart body size, not the file size, so it can exceed
    // the partition by a few hundred bytes. Let the Update library size against the whole partition.
    return Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
}

bool OtaUpdater::write(const uint8_t * data, size_t len) {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        const esp_err_t err = esp_ota_write(_handle, data, len);
        if (err != ESP_OK) {
            _error = esp_err_to_name(err);
            return false;
        }
        if (_md5_applied) {
            _md5Builder.add(data, len);
        }
        _written += len;
        return true;
    }
#endif
    return Update.write(const_cast<uint8_t *>(data), len) == len;
}

bool OtaUpdater::end() {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        const esp_partition_t * target = _target;
        _target                        = nullptr;

        if (_md5_applied) {
            _md5Builder.calculate();
            if (strcmp(_md5Builder.toString().c_str(), _md5.data()) != 0) {
                esp_ota_abort(_handle);
                _error = "MD5 check failed";
                return false;
            }
        }

        // esp_ota_end() validates the image
        esp_err_t err = esp_ota_end(_handle);
        if (err == ESP_OK && _set_boot) {
            err = esp_ota_set_boot_partition(target);
        }
        if (err != ESP_OK) {
            _error = esp_err_to_name(err);
            return false;
        }
        return true;
    }
#endif
    return Update.end(true);
}

void OtaUpdater::abort() {
#ifdef OTA_UPDATER_ANY_PARTITION
    if (_target != nullptr) {
        esp_ota_abort(_handle);
        _target = nullptr;
        return;
    }
#endif
    Update.abort();
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
