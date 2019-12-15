/*
 * Ntp.h
 *
 */
#pragma once

#ifndef NTP_H_
#define NTP_H_

#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#include "TimeLib.h" // customized version of the Time library
#include "Timezone.h"

#define NTP_PACKET_SIZE 48       // NTP time is in the first 48 bytes of the message
#define NTP_INTERVAL_DEFAULT 720 // every 12 hours
#define NTP_TIMEZONE_DEFAULT 2   // CE
#define NTP_TIMEZONE_MAX 11

class NtpClient {
  public:
    void ICACHE_FLASH_ATTR Ntp(const char * server, time_t syncMins, uint8_t tz_index);
    ICACHE_FLASH_ATTR virtual ~NtpClient();

    static char *           TimeServerName;
    static IPAddress        timeServer;
    static time_t           syncInterval;
    static Timezone *       tz;
    static TimeChangeRule * tcr;

    static AsyncUDP udpListener;

    static byte NTPpacket[NTP_PACKET_SIZE];

    static ICACHE_FLASH_ATTR time_t getNtpTime();
};

#endif
