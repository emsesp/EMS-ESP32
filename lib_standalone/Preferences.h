#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include "Arduino.h"

typedef enum { PT_I8, PT_U8, PT_I16, PT_U16, PT_I32, PT_U32, PT_I64, PT_U64, PT_STR, PT_BLOB, PT_INVALID } PreferenceType;

class Preferences {
  protected:
    uint32_t _handle;
    bool     _started;
    bool     _readOnly;

  public:
    Preferences(){};
    ~Preferences(){};

    bool begin(const char * name, bool readOnly = false, const char * partition_label = NULL) {
        return true;
    }

    bool remove(const char * key) {
        return true;
    }

    size_t putChar(const char * key, int8_t value) {
        return 0;
    }
    size_t putUChar(const char * key, uint8_t value) {
        return 0;
    }

    size_t putDouble(const char * key, double_t value) {
        return 0;
    }

    uint8_t getUChar(const char * key, uint8_t defaultValue = 0) {
        return 0;
    }

    double_t getDouble(const char * key, double_t defaultValue = NAN) {
        return NAN;
    }

    // unused

    void           end();
    bool           clear();
    size_t         putShort(const char * key, int16_t value);
    size_t         putUShort(const char * key, uint16_t value);
    size_t         putInt(const char * key, int32_t value);
    size_t         putUInt(const char * key, uint32_t value);
    size_t         putLong(const char * key, int32_t value);
    size_t         putULong(const char * key, uint32_t value);
    size_t         putLong64(const char * key, int64_t value);
    size_t         putULong64(const char * key, uint64_t value);
    size_t         putFloat(const char * key, float_t value);
    size_t         putBool(const char * key, bool value);
    size_t         putString(const char * key, const char * value);
    size_t         putString(const char * key, String value);
    size_t         putBytes(const char * key, const void * value, size_t len);
    bool           isKey(const char * key);
    PreferenceType getType(const char * key);
    int8_t         getChar(const char * key, int8_t defaultValue = 0);
    int16_t        getShort(const char * key, int16_t defaultValue = 0);
    uint16_t       getUShort(const char * key, uint16_t defaultValue = 0);
    int32_t        getInt(const char * key, int32_t defaultValue = 0);
    uint32_t       getUInt(const char * key, uint32_t defaultValue = 0);
    int32_t        getLong(const char * key, int32_t defaultValue = 0);
    uint32_t       getULong(const char * key, uint32_t defaultValue = 0);
    int64_t        getLong64(const char * key, int64_t defaultValue = 0);
    uint64_t       getULong64(const char * key, uint64_t defaultValue = 0);
    float_t        getFloat(const char * key, float_t defaultValue = NAN);
    bool           getBool(const char * key, bool defaultValue = false);
    size_t         getString(const char * key, char * value, size_t maxLen);
    String         getString(const char * key, String defaultValue = String());
    size_t         getBytesLength(const char * key);
    size_t         getBytes(const char * key, void * buf, size_t maxLen);
    size_t         freeEntries();
};

#endif
