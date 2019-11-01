/*
 * customized version of Time library, originally Copyright (c) Michael Margolis 2009-2014
 * modified by Paul S https://github.com/PaulStoffregen/Time
 */

#ifndef _TimeLib_h
#define _TimeLib_h

#include <Arduino.h>

#define SECS_PER_MIN ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY ((time_t)(SECS_PER_HOUR * 24UL))
#define tmYearToCalendar(Y) ((Y) + 1970) // full four digit year

// This ugly hack allows us to define C++ overloaded functions, when included
// from within an extern "C", as newlib's sys/stat.h does.  Actually it is
// intended to include "time.h" from the C library (on ARM, but AVR does not
// have that file at all).  On Mac and Windows, the compiler will find this
// "Time.h" instead of the C library "time.h", so we may cause other weird
// and unpredictable effects by conflicting with the C library header "time.h",
// but at least this hack lets us define C++ functions as intended.  Hopefully
// nothing too terrible will result from overriding the C library header?!
extern "C++" {
typedef enum { timeNotSet, timeNeedsSync, timeSet } timeStatus_t;

typedef struct {
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Wday; // day of week, sunday is day 1
    uint8_t Day;
    uint8_t Month;
    uint8_t Year; // offset from 1970;
} tmElements_t;

typedef time_t (*getExternalTime)();

time_t       now(); // return the current time as seconds since Jan 1 1970
void         setTime(time_t t);
timeStatus_t timeStatus();                                     // indicates if time has been set and recently synchronized
void         setSyncProvider(getExternalTime getTimeFunction); // identify the external time provider
void         setSyncInterval(time_t interval);                 // set the number of seconds between re-sync
time_t       makeTime(const tmElements_t & tm);                // convert time elements into time_t

uint8_t  to_hour(time_t t);    // the hour for the given time
uint8_t  to_minute(time_t t);  // the minute for the given time
uint8_t  to_second(time_t t);  // the second for the given time
uint8_t  to_day(time_t t);     // the day for the given time (0-6)
uint8_t  to_month(time_t t);   // the month for the given time
uint8_t  to_weekday(time_t t); // weekday, sunday is day 1
uint16_t to_year(time_t t);    // the year for the given time

}
#endif
