/*----------------------------------------------------------------------*
 * Arduino Timezone Library                                             *
 * Jack Christensen Mar 2012                                            *
 *                                                                      *
 * Arduino Timezone Library Copyright (C) 2018 by Jack Christensen and  *
 * licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html   *
 *----------------------------------------------------------------------*/

#ifndef TIMEZONE_H_INCLUDED
#define TIMEZONE_H_INCLUDED
#include <Arduino.h>
#include <TimeLib.h>

// convenient constants for TimeChangeRules
enum week_t { Last, First, Second, Third, Fourth };
enum dow_t { Sun = 1, Mon, Tue, Wed, Thu, Fri, Sat };
enum month_t { Jan = 1, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec };

// structure to describe rules for when daylight/summer time begins,
// or when standard time begins.
struct TimeChangeRule {
    char    abbrev[6]; // five chars max
    uint8_t week;      // First, Second, Third, Fourth, or Last week of the month
    uint8_t dow;       // day of week, 1=Sun, 2=Mon, ... 7=Sat
    uint8_t month;     // 1=Jan, 2=Feb, ... 12=Dec
    uint8_t hour;      // 0-23
    int     offset;    // offset from UTC in minutes
};

class Timezone {
  public:
    Timezone(TimeChangeRule dstStart, TimeChangeRule stdStart);
    Timezone(TimeChangeRule stdTime);
    time_t toLocal(time_t utc);
    time_t toLocal(time_t utc, TimeChangeRule ** tcr);
    time_t toUTC(time_t local);
    bool   utcIsDST(time_t utc);
    bool   locIsDST(time_t local);
    void   setRules(TimeChangeRule dstStart, TimeChangeRule stdStart);

  private:
    void           calcTimeChanges(int yr);
    void           initTimeChanges();
    time_t         toTime_t(TimeChangeRule r, int yr);
    TimeChangeRule m_dst;    // rule for start of dst or summer time for any year
    TimeChangeRule m_std;    // rule for start of standard time for any year
    time_t         m_dstUTC; // dst start for given/current year, given in UTC
    time_t         m_stdUTC; // std time start for given/current year, given in UTC
    time_t         m_dstLoc; // dst start for given/current year, given in local time
    time_t         m_stdLoc; // std time start for given/current year, given in local time
};

#endif
