#ifndef Features_h
#define Features_h

#define FT_ENABLED(feature) feature

// project feature off by default
#ifndef FT_PROJECT
#define FT_PROJECT 0
#endif

// security feature on by default
#ifndef FT_SECURITY
#define FT_SECURITY 0
#endif

// mqtt feature on by default
#ifndef FT_MQTT
#define FT_MQTT 0
#endif

// ntp feature on by default
#ifndef FT_NTP
#define FT_NTP 0
#endif

// mqtt feature on by default
#ifndef FT_OTA
#define FT_OTA 0
#endif

// upload firmware feature off by default
#ifndef FT_UPLOAD_FIRMWARE
#define FT_UPLOAD_FIRMWARE 0
#endif


#endif
