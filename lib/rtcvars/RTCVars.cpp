// https://github.com/highno/rtcvars

#if defined(ESP8266)

#include "Arduino.h"
#include "RTCVars.h"

#define RTC_BASE 28                   // this is a known good offset for unused RTC memory
#define RTC_STATE_HEADER_SIZE 6       // 3 bytes signature, 1 byte state_id, 2 byte
#define RTC_MAX_SIZE (511 - RTC_BASE) // 512 - RTC_BASE - 1 for checksum

#define RTC_STATE_TYPE_NONE 0
#define RTC_STATE_TYPE_INT 1
#define RTC_STATE_TYPE_LONG 2
#define RTC_STATE_TYPE_FLOAT 3
#define RTC_STATE_TYPE_BYTE 4
#define RTC_STATE_TYPE_CHAR 5
#define RTC_STATE_TYPE_BOOL 6

RTCVars::RTCVars() {
    _state_size              = RTC_STATE_HEADER_SIZE;
    _state_variables_counter = 0;
    _state_id                = 0;
    _last_read_state_id      = RTC_STATE_ID_INVALID;
    _last_read_status        = RTC_OK;
}

bool RTCVars::registerVar(char * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_CHAR);
}

bool RTCVars::registerVar(byte * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_BYTE);
}

bool RTCVars::registerVar(int * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_INT);
}

bool RTCVars::registerVar(long * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_LONG);
}

bool RTCVars::registerVar(float * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_FLOAT);
}

bool RTCVars::registerVar(bool * v) {
    return _checkAndReserve((uintptr_t)v, RTC_STATE_TYPE_BOOL);
}

bool RTCVars::_checkAndReserve(uintptr_t v, byte type_of_var) {
    // check if there is enough room for this var
    if ((_state_variables_counter >= RTC_MAX_VARIABLES) || (_state_variable_size[type_of_var] + _state_size >= RTC_MAX_SIZE))
        return false;
    // keep the pointer to the var
    _state_variables_ptr[_state_variables_counter] = v;
    // keep the type of var so we copy the correct number of bytes
    _state_variables_type[_state_variables_counter] = type_of_var;
    // remove these bytes from the free mem counter
    _state_size += _state_variable_size[type_of_var];
    // up to the next one
    _state_variables_counter++;
    return true;
}

bool RTCVars::saveToRTC() {
    unsigned char buf[_state_size + 1];
    int           p = RTC_STATE_HEADER_SIZE;
    int           s = 0;

    // migic bytes signature
    buf[0] = 'M';
    buf[1] = 'G';
    buf[2] = 'C';
    buf[3] = _state_id;
    buf[4] = (unsigned char)((_state_size >> 8) & 0xff);
    buf[5] = (unsigned char)(_state_size & 0xff);

    // copy the values from the local variables' memory places into buffer
    for (int i = 0; i < _state_variables_counter; i++) {
        s = _state_variable_size[_state_variables_type[i]];
        memcpy(&buf[p], reinterpret_cast<void *>(_state_variables_ptr[i]), s);
        p += s;
    }

    buf[_state_size] = 0;
    for (int j = 0; j < _state_size; j++) {
        buf[_state_size] += buf[j]; // simple checksum
    }
    return ESP.rtcUserMemoryWrite(RTC_BASE, (uint32_t *)&buf, _state_size + 1);
}

bool RTCVars::loadFromRTC() {
    if (!_checkValidRTCData())
        return false;
    _last_read_status = RTC_ERROR_READING_FAILED;
    unsigned char buf[_state_size + 1];
    if (!ESP.rtcUserMemoryRead(RTC_BASE, (uint32_t *)&buf, _state_size + 1))
        return false;

    // check if state id is ok
    _last_read_status = RTC_ERROR_STATE_ID;
    if (_last_read_state_id != _state_id)
        return false;
    // finally check if state sizes are equal
    _last_read_status = RTC_ERROR_SIZE;
    int size_in_rtc   = (int)(buf[4] * 256 + buf[5]);
    if (size_in_rtc != _state_size)
        return false;

    // copy the values into the local variables' memory places
    _last_read_status = RTC_ERROR_OTHER;
    int p             = RTC_STATE_HEADER_SIZE;
    int s             = 0;
    for (int i = 0; i < _state_variables_counter; i++) {
        s = _state_variable_size[_state_variables_type[i]];
        memcpy(reinterpret_cast<void *>(_state_variables_ptr[i]), &buf[p], s);
        p += s;
    }
    _last_read_status = RTC_OK;
    return true;
}

bool RTCVars::_checkValidRTCData() {
    // load header only from RTC
    _last_read_status = RTC_ERROR_READING_FAILED;
    unsigned char buf_head[RTC_STATE_HEADER_SIZE];
    if (!ESP.rtcUserMemoryRead(RTC_BASE, (uint32_t *)&buf_head, RTC_STATE_HEADER_SIZE))
        return false;

    _last_read_status = RTC_ERROR_MAGIC_BYTES;
    // check if magic bytes are ok
    if (buf_head[0] != 'M')
        return false;
    if (buf_head[1] != 'G')
        return false;
    if (buf_head[2] != 'C')
        return false;

    _last_read_status = RTC_ERROR_SIZE;
    // check for valid size
    int size_in_rtc = (int)(buf_head[4] * 256 + buf_head[5]);
    if (size_in_rtc > RTC_MAX_SIZE)
        return false;

    _last_read_status = RTC_ERROR_READING_FAILED;
    // load the full state from RTC
    unsigned char buf[size_in_rtc + 1];
    if (!ESP.rtcUserMemoryRead(RTC_BASE, (uint32_t *)&buf, size_in_rtc + 1))
        return false;

    _last_read_status = RTC_ERROR_CHECKSUM;
    // check for checksum
    unsigned char temp = 0;
    for (int j = 0; j < size_in_rtc; j++)
        temp += buf[j]; //checksum
    if (temp != buf[size_in_rtc])
        return false;

    _last_read_status   = RTC_OK;
    _last_read_state_id = buf[3];
    return true;
}

int RTCVars::getFreeRTCMem() {
    return RTC_MAX_SIZE - _state_size;
}

int RTCVars::getFreeRTCVars() {
    return RTC_MAX_VARIABLES - _state_variables_counter;
}

byte RTCVars::getStateID() {
    return _state_id;
}

byte RTCVars::getStateIDFromRTC() {
    if (!_checkValidRTCData())
        return RTC_STATE_ID_INVALID;
    return _last_read_state_id;
}

void RTCVars::setStateID(byte new_state_id) {
    if (new_state_id != RTC_STATE_ID_INVALID)
        _state_id = new_state_id;
}

byte RTCVars::getReadError() {
    return _last_read_status;
}

#endif