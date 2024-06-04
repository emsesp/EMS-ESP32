#ifndef Network_h
#define Network_h

#include "Arduino.h"

#include <functional>
#include <ClientPosixIPAddress.h>

#define WiFiMode_t wifi_mode_t
#define WIFI_OFF WIFI_MODE_NULL
#define WIFI_STA WIFI_MODE_STA
#define WIFI_AP WIFI_MODE_AP
#define WIFI_AP_STA WIFI_MODE_APSTA

typedef enum {
    WIFI_POWER_19_5dBm    = 78, // 19.5dBm
    WIFI_POWER_19dBm      = 76, // 19dBm
    WIFI_POWER_18_5dBm    = 74, // 18.5dBm
    WIFI_POWER_17dBm      = 68, // 17dBm
    WIFI_POWER_15dBm      = 60, // 15dBm
    WIFI_POWER_13dBm      = 52, // 13dBm
    WIFI_POWER_11dBm      = 44, // 11dBm
    WIFI_POWER_8_5dBm     = 34, // 8.5dBm
    WIFI_POWER_7dBm       = 28, // 7dBm
    WIFI_POWER_5dBm       = 20, // 5dBm
    WIFI_POWER_2dBm       = 8,  // 2dBm
    WIFI_POWER_MINUS_1dBm = -4  // -1dBm
} wifi_power_t;

typedef enum {
    ETH_CLOCK_GPIO0_IN   = 0, /*!< RMII clock input to GPIO0 */
    ETH_CLOCK_GPIO0_OUT  = 1, /*!< RMII clock output from GPIO0 */
    ETH_CLOCK_GPIO16_OUT = 2, /*!< RMII clock output from GPIO16 */
    ETH_CLOCK_GPIO17_OUT = 3  /*!< RMII clock output from GPIO17 */
} eth_clock_mode_t;

typedef enum { ETH_PHY_LAN8720, ETH_PHY_TLK110, ETH_PHY_MAX } eth_phy_type_t;

typedef enum {
    ARDUINO_EVENT_WIFI_READY = 0,              /**< ESP32 WiFi ready */
    ARDUINO_EVENT_SCAN_DONE,                   /**< ESP32 finish scanning AP */
    ARDUINO_EVENT_WIFI_STA_START,              /**< ESP32 station start */
    ARDUINO_EVENT_WIFI_STA_STOP,               /**< ESP32 station stop */
    ARDUINO_EVENT_WIFI_STA_CONNECTED,          /**< ESP32 station connected to AP */
    ARDUINO_EVENT_WIFI_STA_DISCONNECTED,       /**< ESP32 station disconnected from AP */
    ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE,    /**< the auth mode of AP connected by ESP32 station changed */
    ARDUINO_EVENT_WIFI_STA_GOT_IP,             /**< ESP32 station got IP from connected AP */
    ARDUINO_EVENT_WIFI_STA_LOST_IP,            /**< ESP32 station lost IP and the IP is reset to 0 */
    ARDUINO_EVENT_WIFI_STA_WPS_ER_SUCCESS,     /**< ESP32 station wps succeeds in enrollee mode */
    ARDUINO_EVENT_WIFI_STA_WPS_ER_FAILED,      /**< ESP32 station wps fails in enrollee mode */
    ARDUINO_EVENT_WIFI_STA_WPS_ER_TIMEOUT,     /**< ESP32 station wps timeout in enrollee mode */
    ARDUINO_EVENT_WIFI_STA_WPS_ER_PIN,         /**< ESP32 station wps pin code in enrollee mode */
    ARDUINO_EVENT_WIFI_STA_WPS_ER_PBC_OVERLAP, /*!< ESP32 station wps overlap in enrollee mode */
    ARDUINO_EVENT_WIFI_AP_START,               /**< ESP32 soft-AP start */
    ARDUINO_EVENT_WIFI_AP_STOP,                /**< ESP32 soft-AP stop */
    ARDUINO_EVENT_WIFI_AP_STACONNECTED,        /**< a station connected to ESP32 soft-AP */
    ARDUINO_EVENT_WIFI_AP_STADISCONNECTED,     /**< a station disconnected from ESP32 soft-AP */
    ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED,       /**< ESP32 soft-AP assign an IP to a connected station */
    ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED,      /**< Receive probe request packet in soft-AP interface */
    ARDUINO_EVENT_WIFI_STA_GOT_IP6,            /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    ARDUINO_EVENT_ETH_GOT_IP6,                 /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    ARDUINO_EVENT_ETH_START,                   /**< ESP32 ethernet start */
    ARDUINO_EVENT_ETH_STOP,                    /**< ESP32 ethernet stop */
    ARDUINO_EVENT_ETH_CONNECTED,               /**< ESP32 ethernet phy link up */
    ARDUINO_EVENT_ETH_DISCONNECTED,            /**< ESP32 ethernet phy link down */
    ARDUINO_EVENT_ETH_GOT_IP,                  /**< ESP32 ethernet got IP from connected AP */
    ARDUINO_EVENT_MAX
} arduino_event_id_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,        /**< authenticate mode : open */
    WIFI_AUTH_WEP,             /**< authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,         /**< authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,        /**< authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,    /**< authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_WPA2_ENTERPRISE, /**< authenticate mode : WPA2_ENTERPRISE */
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

typedef struct {
    uint32_t status;  /**< status of scanning APs: 0 — success, 1 - failure */
    uint8_t  number;  /**< number of scan results */
    uint8_t  scan_id; /**< scan sequence number, used for block scan */
} wifi_event_sta_scan_done_t;

/** Argument structure for WIFI_EVENT_STA_CONNECTED event */
typedef struct {
    uint8_t          ssid[32]; /**< SSID of connected AP */
    uint8_t          ssid_len; /**< SSID length of connected AP */
    uint8_t          bssid[6]; /**< BSSID of connected AP*/
    uint8_t          channel;  /**< channel of connected AP*/
    wifi_auth_mode_t authmode; /**< authentication mode used by AP*/
} wifi_event_sta_connected_t;

/** Argument structure for WIFI_EVENT_STA_DISCONNECTED event */
typedef struct {
    uint8_t ssid[32]; /**< SSID of disconnected AP */
    uint8_t ssid_len; /**< SSID length of disconnected AP */
    uint8_t bssid[6]; /**< BSSID of disconnected AP */
    uint8_t reason;   /**< reason of disconnection */
} wifi_event_sta_disconnected_t;

/** Argument structure for WIFI_EVENT_STA_AUTHMODE_CHANGE event */
typedef struct {
    wifi_auth_mode_t old_mode; /**< the old auth mode of AP */
    wifi_auth_mode_t new_mode; /**< the new auth mode of AP */
} wifi_event_sta_authmode_change_t;

/** Argument structure for WIFI_EVENT_STA_WPS_ER_PIN event */
typedef struct {
    uint8_t pin_code[8]; /**< PIN code of station in enrollee mode */
} wifi_event_sta_wps_er_pin_t;

/** Argument structure for WIFI_EVENT_STA_WPS_ER_FAILED event */
typedef enum {
    WPS_FAIL_REASON_NORMAL = 0, /**< ESP32 WPS normal fail reason */
    WPS_FAIL_REASON_RECV_M2D,   /**< ESP32 WPS receive M2D frame */
    WPS_FAIL_REASON_MAX
} wifi_event_sta_wps_fail_reason_t;

typedef union {
    wifi_event_sta_scan_done_t       wifi_scan_done;
    wifi_event_sta_authmode_change_t wifi_sta_authmode_change;
    wifi_event_sta_connected_t       wifi_sta_connected;
    wifi_event_sta_disconnected_t    wifi_sta_disconnected;
} arduino_event_info_t;

typedef struct {
    arduino_event_id_t   event_id;   /**< event ID */
    arduino_event_info_t event_info; /**< event information */
} arduino_event_t;

#define WiFiEvent_t arduino_event_id_t
#define WiFiEventInfo_t arduino_event_info_t
#define WiFiEventId_t wifi_event_id_t

typedef enum {
    WL_NO_SHIELD       = 255, // for compatibility with WiFi Shield library
    WL_IDLE_STATUS     = 0,
    WL_NO_SSID_AVAIL   = 1,
    WL_SCAN_COMPLETED  = 2,
    WL_CONNECTED       = 3,
    WL_CONNECT_FAILED  = 4,
    WL_CONNECTION_LOST = 5,
    WL_DISCONNECTED    = 6
} wl_status_t;

typedef void (*WiFiEventCb)(arduino_event_id_t event);
typedef std::function<void(arduino_event_id_t event, arduino_event_info_t info)> WiFiEventFuncCb;
typedef void (*WiFiEventSysCb)(arduino_event_t * event);
typedef size_t wifi_event_id_t;

class WiFiClass {
  public:
    wifi_event_id_t onEvent(WiFiEventCb cbEvent, arduino_event_id_t event = ARDUINO_EVENT_MAX) {
        return 0;
    };
    wifi_event_id_t onEvent(WiFiEventFuncCb cbEvent, arduino_event_id_t event = ARDUINO_EVENT_MAX) {
        return 0;
    };
    wifi_event_id_t onEvent(WiFiEventSysCb cbEvent, arduino_event_id_t event = ARDUINO_EVENT_MAX) {
        return 0;
    };

    void disconnect(bool v) {};

    char * getHostname() {
        return nullptr;
    }

    char * localIP() {
        return nullptr;
    }

    wl_status_t status() {
        return WL_CONNECTED;
    }
};

class ETHClass {
  public:
    bool begin(uint8_t phy_addr, int power, int mdc, int mdio, eth_phy_type_t type, eth_clock_mode_t clock_mode) {
        return false;
    };

    void setHostname(const char * s) {};

    char * getHostname() {
        return nullptr;
    }

    char * localIP() {
        return nullptr;
    }

    int linkSpeed() {
        return 100;
    }

    bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
        return false;
    }
};



#endif
