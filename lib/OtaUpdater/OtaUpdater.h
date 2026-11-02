#ifndef OtaUpdater_h
#define OtaUpdater_h

#include <Arduino.h>
#include <MD5Builder.h>
#include <esp_ota_ops.h>

#include <FirmwareSignature.h>

#include <array>

// Validates and streams firmware (.bin), LittleFS images (*littlefs.bin) and MD5 digests (.md5) into flash.
// Shared between the EMS-ESP application and the recovery application.
// Firmware must be signed by EMS-ESP (see lib/FirmwareSignature). An unsigned firmware or a filesystem image is only
// accepted after allowUnsigned(), for one update.
class OtaUpdater {
  public:
    enum class FileType : uint8_t { UNSUPPORTED, FIRMWARE, FILESYSTEM, JSON, MD5 };

    // min_firmware_size is for the main EMS-ESP image. Filenames containing "recovery"
    // (case-insensitive) use MIN_RECOVERY_FIRMWARE_SIZE instead.
    static FileType classify(const String & filename, size_t filesize, size_t min_firmware_size);

    // project name in the app description of the recovery firmware, set by custom_fw_name in platformio.ini
    static constexpr char RECOVERY_PROJECT_NAME[] = "EMS-ESP-Recovery";

    // recovery firmware is much smaller than the main image (~512 KB–1 MB)
    static constexpr size_t MIN_RECOVERY_FIRMWARE_SIZE = 512 * 1024;

    static constexpr char NOT_SIGNED_ERROR[] = "This firmware is not signed by EMS-ESP";

    // checks the ESP image magic byte and that the chip id matches the chip we're running on
    static bool isCompatibleFirmware(const uint8_t * data, size_t len);

    // checks the app description at the start of an image, needs the first 112 bytes
    static bool isRecoveryFirmware(const uint8_t * data, size_t len);
    // fills in desc if the partition holds the recovery firmware
    static bool isRecoveryPartition(const esp_partition_t * partition, esp_app_desc_t * desc = nullptr);

    // Accepts a raw 32-char hex digest, optionally with trailing newline or a GNU "hash  filename" suffix.
    static bool parseMd5Digest(const uint8_t * data, size_t len, std::array<char, 33> & out);

    // With bootloader rollback a new image boots as pending verification until confirmed, and the
    // IDF refuses to start another firmware update until then. Returns true if it was pending.
    static bool confirmRunningApp();

    // the partition holds an image followed by a signature from one of the EMS-ESP keys, as written by end() or by
    // flashing a signed file over USB. Reads the whole image, so it takes a moment
    static bool isOfficialFirmware(const esp_partition_t * partition);

    // only from the serial console. Cleared by the next successful update and by a restart
    static void allowUnsigned(bool allow) {
        _allow_unsigned = allow;
    }
    static bool unsignedAllowed() {
        return _allow_unsigned;
    }

    bool setMd5(const uint8_t * data, size_t len);
    void clearMd5();
    bool hasMd5() const;
    bool md5Applied() const {
        return _md5_applied;
    }
    const char * md5() const {
        return _md5.data();
    }

    // filesize is the HTTP content length, which is slightly bigger than the image.
    // Without a target the image goes to the next OTA partition after the running one. Writing to a
    // specific partition needs OTA_UPDATER_ANY_PARTITION, which adds ~1.7KB.
    bool beginFirmware(size_t filesize, const esp_partition_t * target = nullptr, bool set_boot = true);
    bool beginFilesystem();
    bool write(const uint8_t * data, size_t len);
    bool end(); // on success the new firmware becomes the boot partition, unless set_boot was false
    void abort();

    size_t       progress() const;
    const char * errorString() const;
    // the last begin or end failed because the file isn't signed by EMS-ESP
    bool notSigned() const {
        return _not_signed;
    }
    // the firmware written by the last successful end() was signed
    bool isSigned() const {
        return _signed;
    }

  private:
    static bool             _allow_unsigned;
    static constexpr size_t READ_CHUNK = 1024;

    std::array<char, 33> _md5{};
    bool                 _md5_applied = false;
    MD5Builder           _md5Builder;
    FirmwareSignature    _signature;
    bool                 _filesystem = false;
    bool                 _not_signed = false;
    bool                 _signed     = false;
    const char *         _error      = "";

    bool writeImage(const uint8_t * data, size_t len);

#ifdef OTA_UPDATER_ANY_PARTITION
    // explicit target (factory/boot or an OTA slot) is written with the partition API;
    // esp_ota_begin() returns ESP_ERR_INVALID_STATE (0x102) on the factory partition
    const esp_partition_t * _target   = nullptr;
    size_t                  _written  = 0;
    size_t                  _erased   = 0;
    bool                    _set_boot = true;
    // the start of the image is written last, so an image that fails a check is never bootable
    uint8_t                 _header[16]{};
    // AsyncTCP may hand us PSRAM pointers; ESP32 flash DMA needs internal RAM
    uint8_t *               _dram      = nullptr;
    static constexpr size_t DRAM_CHUNK   = 1024;
    static constexpr size_t FLASH_SECTOR = 4096;
    char                    _error_buf[48]{};

    void setError(esp_err_t err);
    void releaseTarget();
    bool commitTarget();
#endif
};

#endif
