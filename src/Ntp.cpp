/*
 * Ntp.cpp
 */

#include "Ntp.h"
#include "MyESP.h"

char *           NtpClient::TimeServerName;
Timezone *       NtpClient::tz;
TimeChangeRule * NtpClient::tcr;
time_t           NtpClient::syncInterval;
IPAddress        NtpClient::timeServer;
AsyncUDP         NtpClient::udpListener;
byte             NtpClient::NTPpacket[NTP_PACKET_SIZE];

// references:
// https://github.com/filipdanic/compact-timezone-list/blob/master/index.js
// https://github.com/sanohin/google-timezones-json/blob/master/timezones.json
// https://github.com/dmfilipenko/timezones.json/blob/master/timezones.json
// https://home.kpn.nl/vanadovv/time/TZworld.html
// https://www.timeanddate.com/time/zones/

// Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660}; // UTC + 11 hours
TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600}; // UTC + 10 hours
Timezone       ausET(aEDT, aEST);

// Moscow Standard Time (MSK, does not observe DST)
TimeChangeRule msk = {"MSK", Last, Sun, Mar, 1, 180};
Timezone       MSK(msk);

// Turkey
TimeChangeRule trt = {"TRT", Last, Sun, Mar, 1, 180};
Timezone       TRT(trt);

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET  = {"CET ", Last, Sun, Oct, 3, 60};  // Central European Standard Time
Timezone       CE(CEST, CET);

// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60}; // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};  // Standard Time
Timezone       UK(BST, GMT);

// UTC
TimeChangeRule utcRule = {"UTC", Last, Sun, Mar, 1, 0}; // UTC
Timezone       UTC(utcRule);

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time = UTC - 5 hours
Timezone       usET(usEDT, usEST);

// US Central Time Zone (Chicago, Houston)
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
Timezone       usCT(usCDT, usCST);

// US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
Timezone       usMT(usMDT, usMST);

// Arizona is US Mountain Time Zone but does not use DST
Timezone usAZ(usMST);

// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone       usPT(usPDT, usPST);

// build index of all timezones
Timezone * timezones[] = {&ausET, &MSK, &CE, &UK, &UTC, &usET, &usCT, &usMT, &usAZ, &usPT, &TRT};

void ICACHE_FLASH_ATTR NtpClient::Ntp(const char * server, time_t syncMins, uint8_t tz_index) {
    TimeServerName = strdup(server);
    syncInterval   = syncMins * 60; // convert to seconds

    // check for out of bounds
    if (tz_index >= NTP_TIMEZONE_MAX) {
        tz_index = NTP_TIMEZONE_DEFAULT;
    }
    tz = timezones[tz_index]; // set timezone

    WiFi.hostByName(TimeServerName, timeServer);
    setSyncProvider(getNtpTime);
    setSyncInterval(syncInterval);
}

ICACHE_FLASH_ATTR NtpClient::~NtpClient() {
    udpListener.close();
}

// send an NTP request to the time server at the given address
time_t ICACHE_FLASH_ATTR NtpClient::getNtpTime() {
    memset(NTPpacket, 0, sizeof(NTPpacket));
    NTPpacket[0]  = 0b11100011;
    NTPpacket[1]  = 0;
    NTPpacket[2]  = 6;
    NTPpacket[3]  = 0xEC;
    NTPpacket[12] = 49;
    NTPpacket[13] = 0x4E;
    NTPpacket[14] = 49;
    NTPpacket[15] = 52;
    if (udpListener.connect(timeServer, 123)) {
        udpListener.onPacket([](AsyncUDPPacket packet) {
            unsigned long highWord     = word(packet.data()[40], packet.data()[41]);
            unsigned long lowWord      = word(packet.data()[42], packet.data()[43]);
            time_t        UnixUTCtime  = (highWord << 16 | lowWord) - 2208988800UL;
            time_t        adjustedtime = (*tz).toLocal(UnixUTCtime, &tcr);

            myESP.myDebug_P(PSTR("[NTP] Internet time: %02d:%02d:%02d UTC on %d/%d. Local time: %02d:%02d:%02d %s"),
                            to_hour(UnixUTCtime),
                            to_minute(UnixUTCtime),
                            to_second(UnixUTCtime),
                            to_day(UnixUTCtime),
                            to_month(UnixUTCtime),
                            to_hour(adjustedtime),
                            to_minute(adjustedtime),
                            to_second(adjustedtime),
                            tcr->abbrev);

            setTime(adjustedtime);
        });
    }
    udpListener.write(NTPpacket, sizeof(NTPpacket));
    return 0;
}
