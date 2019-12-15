/*
 * customized version of Time library, originally Copyright (c) Michael Margolis 2009-2014
 * modified by Paul S https://github.com/PaulStoffregen/Time
 */

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

// assemble time elements into time_t
time_t makeTime(const tmElements_t & tm) {
    int      i;
    uint32_t seconds;

    // seconds from 1970 till 1 jan 00:00:00 of the given year
    seconds = tm.Year * (SECS_PER_DAY * 365);
    for (i = 0; i < tm.Year; i++) {
        if (LEAP_YEAR(i)) {
            seconds += SECS_PER_DAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (i = 1; i < tm.Month; i++) {
        if ((i == 2) && LEAP_YEAR(tm.Year)) {
            seconds += SECS_PER_DAY * 29;
        } else {
            seconds += SECS_PER_DAY * monthDays[i - 1]; // monthDay array starts from 0
        }
    }
    seconds += (tm.Day - 1) * SECS_PER_DAY;
    seconds += tm.Hour * SECS_PER_HOUR;
    seconds += tm.Minute * SECS_PER_MIN;
    seconds += tm.Second;
    return (time_t)seconds;
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

uint8_t to_day(time_t t) { // the day for the given time (0-6)
    refreshCache(t);
    return tm.Day;
}

uint8_t to_month(time_t t) { // the month for the given time
    refreshCache(t);
    return tm.Month;
}

uint8_t to_weekday(time_t t) {
    refreshCache(t);
    return tm.Wday;
}

uint16_t to_year(time_t t) { // the year for the given time
    refreshCache(t);
    return tm.Year + 1970;
}

void setTime(time_t t) {
    sysTime      = (uint32_t)t;
    nextSyncTime = (uint32_t)t + syncInterval;
    Status       = timeSet;
    prevMillis   = millis(); // restart counting from now (thanks to Korman for this fix)
}
