#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include <functional>

#define WiFiMode_t wifi_mode_t
#define WIFI_OFF WIFI_MODE_NULL
#define WIFI_STA WIFI_MODE_STA
#define WIFI_AP WIFI_MODE_AP
#define WIFI_AP_STA WIFI_MODE_APSTA

typedef enum {
    ETH_CLOCK_GPIO0_IN   = 0, /*!< RMII clock input to GPIO0 */
    ETH_CLOCK_GPIO0_OUT  = 1, /*!< RMII clock output from GPIO0 */
    ETH_CLOCK_GPIO16_OUT = 2, /*!< RMII clock output from GPIO16 */
    ETH_CLOCK_GPIO17_OUT = 3  /*!< RMII clock output from GPIO17 */
} eth_clock_mode_t;

typedef enum { ETH_PHY_LAN8720, ETH_PHY_TLK110, ETH_PHY_MAX } eth_phy_type_t;

typedef enum {
    SYSTEM_EVENT_WIFI_READY = 0,         /**< ESP32 WiFi ready */
    SYSTEM_EVENT_SCAN_DONE,              /**< ESP32 finish scanning AP */
    SYSTEM_EVENT_STA_START,              /**< ESP32 station start */
    SYSTEM_EVENT_STA_STOP,               /**< ESP32 station stop */
    SYSTEM_EVENT_STA_CONNECTED,          /**< ESP32 station connected to AP */
    SYSTEM_EVENT_STA_DISCONNECTED,       /**< ESP32 station disconnected from AP */
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE,    /**< the auth mode of AP connected by ESP32 station changed */
    SYSTEM_EVENT_STA_GOT_IP,             /**< ESP32 station got IP from connected AP */
    SYSTEM_EVENT_STA_LOST_IP,            /**< ESP32 station lost IP and the IP is reset to 0 */
    SYSTEM_EVENT_STA_WPS_ER_SUCCESS,     /**< ESP32 station wps succeeds in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_FAILED,      /**< ESP32 station wps fails in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,     /**< ESP32 station wps timeout in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_PIN,         /**< ESP32 station wps pin code in enrollee mode */
    SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP, /*!< ESP32 station wps overlap in enrollee mode */
    SYSTEM_EVENT_AP_START,               /**< ESP32 soft-AP start */
    SYSTEM_EVENT_AP_STOP,                /**< ESP32 soft-AP stop */
    SYSTEM_EVENT_AP_STACONNECTED,        /**< a station connected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_STADISCONNECTED,     /**< a station disconnected from ESP32 soft-AP */
    SYSTEM_EVENT_AP_STAIPASSIGNED,       /**< ESP32 soft-AP assign an IP to a connected station */
    SYSTEM_EVENT_AP_PROBEREQRECVED,      /**< Receive probe request packet in soft-AP interface */
    SYSTEM_EVENT_GOT_IP6,                /**< ESP32 station or ap or ethernet interface v6IP addr is preferred */
    SYSTEM_EVENT_ETH_START,              /**< ESP32 ethernet start */
    SYSTEM_EVENT_ETH_STOP,               /**< ESP32 ethernet stop */
    SYSTEM_EVENT_ETH_CONNECTED,          /**< ESP32 ethernet phy link up */
    SYSTEM_EVENT_ETH_DISCONNECTED,       /**< ESP32 ethernet phy link down */
    SYSTEM_EVENT_ETH_GOT_IP,             /**< ESP32 ethernet got IP from connected AP */
    SYSTEM_EVENT_MAX
} system_event_id_t;

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
    uint32_t status; /**< status of scanning APs */
    uint8_t  number;
    uint8_t  scan_id;
} system_event_sta_scan_done_t;

typedef struct {
    uint8_t          ssid[32]; /**< SSID of connected AP */
    uint8_t          ssid_len; /**< SSID length of connected AP */
    uint8_t          bssid[6]; /**< BSSID of connected AP*/
    uint8_t          channel;  /**< channel of connected AP*/
    wifi_auth_mode_t authmode;
} system_event_sta_connected_t;

typedef struct {
    uint8_t ssid[32]; /**< SSID of disconnected AP */
    uint8_t ssid_len; /**< SSID length of disconnected AP */
    uint8_t bssid[6]; /**< BSSID of disconnected AP */
    uint8_t reason;   /**< reason of disconnection */
} system_event_sta_disconnected_t;

typedef struct {
    wifi_auth_mode_t old_mode; /**< the old auth mode of AP */
    wifi_auth_mode_t new_mode; /**< the new auth mode of AP */
} system_event_sta_authmode_change_t;

typedef struct {
    uint8_t pin_code[8]; /**< PIN code of station in enrollee mode */
} system_event_sta_wps_er_pin_t;

typedef struct {
    uint8_t mac[6]; /**< MAC address of the station connected to ESP32 soft-AP */
    uint8_t aid;    /**< the aid that ESP32 soft-AP gives to the station connected to  */
} system_event_ap_staconnected_t;

typedef struct {
    uint8_t mac[6]; /**< MAC address of the station disconnects to ESP32 soft-AP */
    uint8_t aid;    /**< the aid that ESP32 soft-AP gave to the station disconnects to  */
} system_event_ap_stadisconnected_t;

typedef struct {
    int     rssi;   /**< Received probe request signal strength */
    uint8_t mac[6]; /**< MAC address of the station which send probe request */
} system_event_ap_probe_req_rx_t;

typedef enum {
    WPS_FAIL_REASON_NORMAL = 0, /**< ESP32 WPS normal fail reason */
    WPS_FAIL_REASON_RECV_M2D,   /**< ESP32 WPS receive M2D frame */
    WPS_FAIL_REASON_MAX
} system_event_sta_wps_fail_reason_t;

typedef union {
    system_event_sta_connected_t       connected;          /**< ESP32 station connected to AP */
    system_event_sta_disconnected_t    disconnected;       /**< ESP32 station disconnected to AP */
    system_event_sta_scan_done_t       scan_done;          /**< ESP32 station scan (APs) done */
    system_event_sta_authmode_change_t auth_change;        /**< the auth mode of AP ESP32 station connected to changed */
    system_event_sta_wps_fail_reason_t sta_er_fail_reason; /**< ESP32 station WPS enrollee mode failed reason code received */
    system_event_ap_staconnected_t     sta_connected;      /**< a station connected to ESP32 soft-AP */
    system_event_ap_stadisconnected_t  sta_disconnected;   /**< a station disconnected to ESP32 soft-AP */
    system_event_ap_probe_req_rx_t     ap_probereqrecved;  /**< ESP32 soft-AP receive probe request packet */
} system_event_info_t;

typedef struct {
    system_event_id_t   event_id;   /**< event ID */
    system_event_info_t event_info; /**< event information */
} system_event_t;

#define WiFiEvent_t system_event_id_t
#define WiFiEventInfo_t system_event_info_t
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

typedef void (*WiFiEventCb)(system_event_id_t event);
typedef std::function<void(system_event_id_t event, system_event_info_t info)> WiFiEventFuncCb;
typedef void (*WiFiEventSysCb)(system_event_t * event);
typedef size_t wifi_event_id_t;

class WiFiClass {
  public:
    wifi_event_id_t onEvent(WiFiEventCb cbEvent, system_event_id_t event = SYSTEM_EVENT_MAX) {
        return 0;
    };
    wifi_event_id_t onEvent(WiFiEventFuncCb cbEvent, system_event_id_t event = SYSTEM_EVENT_MAX) {
        return 0;
    };
    wifi_event_id_t onEvent(WiFiEventSysCb cbEvent, system_event_id_t event = SYSTEM_EVENT_MAX) {
        return 0;
    };

    void disconnect(bool v){};

    char * getHostname() {
        return nullptr;
    }

    char * localIP() {
        return nullptr;
    }
};

class ETHClass {
  public:
    bool begin(uint8_t phy_addr, int power, int mdc, int mdio, eth_phy_type_t type, eth_clock_mode_t clock_mode) {
        return false;
    };

    void setHostname(const char * s){};

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
