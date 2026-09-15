#ifndef WebStatusService_h
#define WebStatusService_h

#define EMSESP_SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"
#define EMSESP_ACTION_SERVICE_PATH "/rest/action"

#include "../core/firmwareVersion.h"
#include "../emsesp_version.h"

#include <atomic>
#include <mutex>

namespace emsesp {

class WebStatusService {
  public:
    WebStatusService(AsyncWebServer * server, SecurityManager * securityManager);
    void set_current_version(const std::string & version) {
        current_version_s = version;
    }
    std::string get_current_version() {
        return current_version_s;
    }

    // called from EMSESP::loop() to refresh the cached versions.json from emsesp.org
    // so that the web request handler never has to do blocking HTTPS on the small AsyncTCP stack
    // fetches on a daily schedule when auto_fw_check is on, otherwise only when getVersions asks
    void loop();

    // true once we've had at least one successful versions.json fetch
    bool versions_cache_valid() const {
        return versions_cache_valid_;
    }

    // schedule a refresh of the versions.json cache. Defers the fetch by
    // VERSIONS_INITIAL_FETCH_DELAY_MS so the network stack (DHCP, default netif, DNS server)
    // has time to settle after the link first comes up — otherwise hostByName() can fail
    // immediately on boot with a noisy "DNS Failed ... error '-54'".
    void schedule_versions_refresh();

    bool current_upgradeable() const; // true if a newer version is available

// make action function public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  protected:
#endif
    void systemStatus(AsyncWebServerRequest * request);
    void action(AsyncWebServerRequest * request, JsonVariant json);

  private:
    SecurityManager * _securityManager;

    // actions
    void    getVersions(JsonObject root);
    bool    exportData(JsonObject root, std::string & type);
    bool    getCustomSupport(JsonObject root);
    bool    uploadURL(const char * url);
    bool    setSystemStatus(const char * status);
    void    allvalues(JsonObject output);
    uint8_t upgradeImportantMessages(std::string & version);

    std::string current_version_s = EMSESP_APP_VERSION;

    // cached emsesp.org/versions.json. Refreshed from the main loop task, which has more stack.
    struct VersionInfo {
        std::string version;
        std::string date;
        bool        upgradeable = false;
    };
    VersionInfo versions_stable_;
    VersionInfo versions_dev_;
    bool        versions_cache_valid_   = false; // true once we've had at least one successful fetch
    uint32_t    versions_next_fetch_ms_ = 0;     // uuid::get_uptime() of the next attempt; 0 = idle
    uint8_t     versions_failures_      = 0;     // consecutive failed fetches, drives the retry backoff

    // set by getVersions() on the AsyncTCP task, consumed by the loop task, so that the WebUI still
    // gets version information when auto_fw_check is off
    std::atomic<bool> versions_fetch_requested_{false};

    // the cache is written by the loop task in refresh_versions_cache() and read by the AsyncTCP
    // task in getVersions()/current_upgradeable(), so the std::strings need guarding - without it a
    // request landing mid-update reads a buffer that has just been freed
    mutable std::mutex versions_mutex_;

    bool refresh_versions_cache(); // does the actual HTTPS fetch + parse, returns true on success

    static constexpr const char * VERSIONS_URL = "http://emsesp.org/versions.json";

    static constexpr uint32_t VERSIONS_REFRESH_INTERVAL_MS    = 24UL * 60UL * 60UL * 1000UL; // once a day on success
    static constexpr uint32_t VERSIONS_RETRY_INTERVAL_MS      = 5UL * 60UL * 1000UL;         // 5 min after the first failure
    static constexpr uint32_t VERSIONS_INITIAL_FETCH_DELAY_MS = 5UL * 1000UL;                // 5 s after a link comes up

    // each attempt blocks the loop task for as long as the DNS lookup, connect and read timeouts
    // allow, so double the retry delay per consecutive failure: 5, 10, 20, 40, 80, 160 minutes
    static constexpr uint8_t VERSIONS_MAX_BACKOFF_STEPS = 6;
};

} // namespace emsesp

#endif
