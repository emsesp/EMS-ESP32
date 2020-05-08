#ifndef RTCVARS_H
#define RTCVARS_H

#include "Arduino.h"

#ifndef RTC_MAX_VARIABLES
#define RTC_MAX_VARIABLES 32
#endif

const static byte RTC_OK                   = 0;
const static byte RTC_ERROR_MAGIC_BYTES    = 1;
const static byte RTC_ERROR_SIZE           = 2;
const static byte RTC_ERROR_READING_FAILED = 3;
const static byte RTC_ERROR_CHECKSUM       = 4;
const static byte RTC_ERROR_STATE_ID       = 5;
const static byte RTC_ERROR_OTHER          = 99;
const static byte RTC_STATE_ID_INVALID     = 255;

class RTCVars {
  public:
    RTCVars();
    bool registerVar(char * v);
    bool registerVar(byte * v);
    bool registerVar(bool * v);
    bool registerVar(int * v);
    bool registerVar(long * v);
    bool registerVar(float * v);
    void debugOutputRTCVars();
    bool saveToRTC();
    bool loadFromRTC();
    int  getFreeRTCMem();
    int  getFreeRTCVars();
    byte getStateID();
    byte getStateIDFromRTC();
    void setStateID(byte new_state_id);
    byte getReadError();

  private:
    byte       _state_id;
    byte       _last_read_state_id;
    byte       _last_read_status;
    int        _state_size;
    int        _state_variables_counter;
    uintptr_t  _state_variables_ptr[RTC_MAX_VARIABLES];
    byte       _state_variables_type[RTC_MAX_VARIABLES];
    const byte _state_variable_size[7] = {0, sizeof(int), sizeof(long), sizeof(float), sizeof(byte), sizeof(char), sizeof(bool)};
    bool       _checkAndReserve(uintptr_t v, byte type_of_var);
    bool       _checkValidRTCData();
};

#endif
