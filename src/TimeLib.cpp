#include "TimeLib.h"

static tmElements_t tm;                 // a cache of time elements
static time_t       cacheTime;          // the time the cache was updated
static uint32_t     syncInterval = 300; // time sync will be attempted after this many seconds
static uint32_t     sysTime      = 0;
static uint32_t     prevMillis   = 0;
static uint32_t     nextSyncTime = 0;
static timeStatus_t Status       = timeNotSet;
getExternalTime     getTimePtr; // pointer to external sync function

#define LEAP_YEAR(Y) (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) && (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))
static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0

time_t now() {
    // calculate number of seconds passed since last call to now()
    while (millis() - prevMillis >= 1000) {
        // millis() and prevMillis are both unsigned ints thus the subtraction will always be the absolute value of the difference
        sysTime++;
        prevMillis += 1000;
    }
    if (nextSyncTime <= sysTime) {
        if (getTimePtr != 0) {
            time_t t = getTimePtr();
            if (t != 0) {
                setTime(t);
            } else {
                nextSyncTime = sysTime + syncInterval;
                Status       = (Status == timeNotSet) ? timeNotSet : timeNeedsSync;
            }
        }
    }
    return (time_t)sysTime;
}

void setSyncProvider(getExternalTime getTimeFunction) {
    getTimePtr   = getTimeFunction;
    nextSyncTime = sysTime;
    now(); // this will sync the clock
}

void setSyncInterval(time_t interval) { // set the number of seconds between re-sync
    syncInterval = (uint32_t)interval;
    nextSyncTime = sysTime + syncInterval;
}

void breakTime(time_t timeInput, tmElements_t & tm) {
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint8_t       year;
    uint8_t       month, monthLength;
    uint32_t      time;
    unsigned long days;

    time      = (uint32_t)timeInput;
    tm.Second = time % 60;
    time /= 60; // now it is minutes
    tm.Minute = time % 60;
    time /= 60; // now it is hours
    tm.Hour = time % 24;
    time /= 24;                     // now it is days
    tm.Wday = ((time + 4) % 7) + 1; // Sunday is day 1

    year = 0;
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
        year++;
    }
    tm.Year = year; // year is offset from 1970

    days -= LEAP_YEAR(year) ? 366 : 365;
    time -= days; // now it is days in this year, starting at 0

    month       = 0;
    monthLength = 0;
    for (month = 0; month < 12; month++) {
        if (month == 1) { // february
            if (LEAP_YEAR(year)) {
                monthLength = 29;
            } else {
                monthLength = 28;
            }
        } else {
            monthLength = monthDays[month];
        }

        if (time >= monthLength) {
            time -= monthLength;
        } else {
            break;
        }
    }
    tm.Month = month + 1; // jan is month 1
    tm.Day   = time + 1;  // day of month
}

void refreshCache(time_t t) {
    if (t != cacheTime) {
        breakTime(t, tm);
        cacheTime = t;
    }
}

uint8_t to_second(time_t t) { // the second for the given time
    refreshCache(t);
    return tm.Second;
}

uint8_t to_minute(time_t t) { // the minute for the given time
    refreshCache(t);
    return tm.Minute;
}

uint8_t to_hour(time_t t) { // the hour for the given time
    refreshCache(t);
    return tm.Hour;
}

void setTime(time_t t) {
    sysTime      = (uint32_t)t;
    nextSyncTime = (uint32_t)t + syncInterval;
    Status       = timeSet;
    prevMillis   = millis(); // restart counting from now (thanks to Korman for this fix)
}
