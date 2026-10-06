#ifndef OtaUpdater_h
#define OtaUpdater_h

#include <Arduino.h>
#include <esp_ota_ops.h>

#include <array>

#ifdef OTA_UPDATER_ANY_PARTITION
#include <MD5Builder.h>
#endif

// Validates and streams firmware (.bin), LittleFS images (*littlefs.bin) and MD5 digests (.md5) into flash.
// Shared between the EMS-ESP application and the recovery application.
class OtaUpdater {
  public:
    enum class FileType : uint8_t { UNSUPPORTED, FIRMWARE, FILESYSTEM, JSON, MD5 };

    static FileType classify(const String & filename, size_t filesize, size_t min_firmware_size);

    // project name in the app description of the recovery firmware, set by custom_fw_name in platformio.ini
    static constexpr char RECOVERY_PROJECT_NAME[] = "EMS-ESP-Recovery";

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

  private:
    std::array<char, 33> _md5{};
    bool                 _md5_applied = false;
    const char *         _error       = "";

#ifdef OTA_UPDATER_ANY_PARTITION
    // set when writing to an explicit target partition with the IDF OTA API instead of Update
    const esp_partition_t * _target   = nullptr;
    esp_ota_handle_t        _handle   = 0;
    size_t                  _written  = 0;
    bool                    _set_boot = true;
    MD5Builder              _md5Builder;
#endif
};

#endif
