/*
 * Ntp.cpp
 */

#include "Ntp.h"

char *    NtpClient::TimeServerName;
time_t    NtpClient::syncInterval;
IPAddress NtpClient::timeServer;
AsyncUDP  NtpClient::udpListener;
byte      NtpClient::NTPpacket[NTP_PACKET_SIZE];

void ICACHE_FLASH_ATTR NtpClient::Ntp(const char * server, time_t syncMins) {
    TimeServerName = strdup(server);
    syncInterval   = syncMins * 60; // convert to seconds
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
            unsigned long highWord    = word(packet.data()[40], packet.data()[41]);
            unsigned long lowWord     = word(packet.data()[42], packet.data()[43]);
            time_t        UnixUTCtime = (highWord << 16 | lowWord) - 2208988800UL;
            setTime(UnixUTCtime);
        });
    }
    udpListener.write(NTPpacket, sizeof(NTPpacket));
    return 0;
}
