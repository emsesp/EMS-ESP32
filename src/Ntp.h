/*
 * Ntp.h
 *
 */
#pragma once

#ifndef NTP_H_
#define NTP_H_

#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#include "TimeLib.h" // customized version of the time library

#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message

class NtpClient {
  public:
    void ICACHE_FLASH_ATTR Ntp(const char * server, int8_t tz, time_t syncSecs);
    ICACHE_FLASH_ATTR virtual ~NtpClient();

    static char *    TimeServerName;
    static IPAddress timeServer;
    static int8_t    timezone;
    static time_t    syncInterval;

    static AsyncUDP udpListener;

    static byte NTPpacket[NTP_PACKET_SIZE];

  private:
    static ICACHE_FLASH_ATTR time_t getNtpTime();
};

#endif
