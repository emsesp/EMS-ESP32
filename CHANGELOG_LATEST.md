# Changelog

For more details go to [emsesp.org](https://emsesp.org/).

## [3.9.0]

This release is based on the latest Espressif/Arduino core version 3. It brings in many memory and performance optimizations, for example 25% more available memory and 30% less PSRAM fragmentation and security hardening.

## *Breaking Changes*:

Please read this before upgrading. Details are under "Changed" below.

1. **Make a System Backup before upgrading** (Settings > Download/Upload). Passwords are stored hashed after the upgrade and 3.8.x can't read them, so going back to 3.8.x needs this backup, otherwise a factory reset.
2. **Factory passwords must be changed.** After signing in with a factory password, an admin must set a new admin password before the Web UI can be used. If no WiFi network is set yet, a new access point password is needed too. Integrations using API keys keep working in the meantime.
3. **API access tokens stop working and are replaced by API keys.** After the upgrade, create an API key in Security > API Keys for each integration, such as Home Assistant REST sensors and scripts, and send it as `Authorization: Bearer <key>`. Pick "Read" for integrations that only read values, "Read and Write" for ones that change settings or values. The key icon in Manage Users is gone.
4. **Web UI sign-ins expire after 30 days**, and changing a user's password signs that user out on all browsers.
5. **Some `/api` calls need a "Read and Write" API key**: changing settings (for example `system/mqtt/enabled`) and reading the definitions of named Commands (`/api/commands/...`).
6. **"Bypass Access Token authorization on API calls" only gives guest rights.** Calls without a token can still read values and run Commands that guests may run, but can no longer change values or settings. If an integration writes values without a token, for example to set a temperature, give it a "Read and Write" API key.
7. **Guest users have less access.** Only admins can open or change the Scheduler, Commands, Custom Entities and Modules, rename devices or change entity customizations. New Commands can't be run by guests until "Guests may run this command" is ticked; Commands from before the upgrade keep guest access.
8. **The live System Log needs a login**, also for tools that read `/es/log` directly.
9. **Tokens and API keys are only accepted in the `Authorization` header.** A token in the URL (`?access_token=...`) no longer works on `/api`, `/rest` or `/es/log`. Send `Authorization: Bearer <key>` instead, for example `curl -H "Authorization: Bearer emsesp_..." http://ems-esp.local/api/boiler`.
10. **Telnet changes.** Log in with a Web UI user: admin users get admin rights, `su` no longer works over telnet. At most 2 sessions at a time. On new installs telnet is off, turn it on in Application Settings if you use it.
11. **The Super User password is gone.** On the serial console, `su` asks for the username and password of a Web UI admin user. The `passwd` command and the Super User password field in Security settings are removed.
12. **Backups leave out passwords by default.** To move settings to a new device, tick "Include passwords and keys" when making the System Backup, and keep that file safe.
13. **MQTT `info` topic:** `IPv6 address` is now an array, and `IPv4 DNS Server` is replaced by a `DNS Server` array. Update any automations that read these fields.
14. **New installs have no `guest` user.** A fresh install or factory reset only creates `admin`. Add a view-only user in the setup screen or under Security > Users. Existing users are kept when upgrading.
15. **Web pages on other addresses need CORS to use `/api`.** A browser request to `/api` from a page on another address (for example a home-made dashboard) is refused with 403, unless CORS is enabled with that page's address as the origin, for example `http://homeassistant.local:8123`. Home Assistant's integration, Node-RED, scripts and `curl` are not affected. `*` still works but shows a warning.
16. **Opening the Web UI by another name needs that name allowed.** The IP address, the hostname and `<hostname>.local` always work. If you open EMS-ESP in a browser by another name, for example `ems-esp.fritz.box` or through a reverse proxy, open it once by its IP address and add the name under Network Settings > Allowed host names. Home Assistant, Node-RED and scripts are not affected.
17. **Only admins can pass their own value to a custom Command.** Guests and "Read" API keys can still run guest-allowed Commands, but only with the value stored in the Command. Values sent by non-admins (for example to `system/message`) can no longer fetch a `{"url":...}`; use an admin sign-in or a "Read and Write" API key, or store the URL in the Command or Schedule itself.
18. **Only official EMS-ESP firmware can be installed from the Web UI.** Firmware uploaded in the Web UI, installed from a URL or through the Recovery must be signed by EMS-ESP; official releases and development builds are. To go back to 3.8.6, use the files from the 3.8.6 release page (they're signed too). To install your own or a community build, sign in on the serial console with `su`, run `allow unsigned`, then upload it; this allows one upload until the next restart. Flashing over USB isn't affected. Uploading a filesystem image (`*littlefs.bin`) needs `allow unsigned` as well. If you build your own firmware, see [Installing your own build](https://github.com/emsesp/EMS-ESP32/blob/dev/CONTRIBUTING.md#installing-your-own-build) for how to sign it with your own key.

## Added

- user-requested LED blink [#3063](https://github.com/emsesp/EMS-ESP32/issues/3063)
- Commands Service that can be called via MQTT or API or used in the Scheduler Service
- option to disable factory reset [#3150](https://github.com/emsesp/EMS-ESP32/issues/3150)
- TLS support with 4MB boards without PSRAM
- added eth_10mbit to Network Settings, which forces 10BASE-T half-duplex & autonegotiation off, halving the PHY’s power draw. It’s off by default as it will cause issues on managed switches with a port forced to 100Mb [#3213](https://github.com/emsesp/EMS-ESP32/pull/3213)
- cleaner MD5 and firmware uploading in WebUI [#3222](https://github.com/emsesp/EMS-ESP32/issues/3222)
- pc0Flow in l/min [#3224](https://github.com/emsesp/EMS-ESP32/issues/3224)
- remove HA config for excluded entities [#3229](https://github.com/emsesp/EMS-ESP32/issues/3229)
- `show system` reports the Ethernet status on boards with a PHY, so a failed initialisation or a link that never came up is visible instead of Ethernet being omitted completely. Also shows the loop task's lowest free stack, to help diagnose reboots
- new Application Setting "Automatically check for firmware updates", off by default. When enabled EMS-ESP asks emsesp.org for the latest version once a day on its own, otherwise it only asks when the WebUI wants to show it, so an unattended system never contacts emsesp.org
- Automatically check and convert older 3.8.x filesystem to the new 3.9.0 format
- heatpump pool: `poolon` (switch, 0x48A), `hppoolon` (pool heating active, 0x488), MP100 `poolshuntset` (0x5C4) and `poolrequest` (0x5BA), shorter pool names
- EMS-ESP Recovery, a small firmware for the factory (`boot`) partition on 16MB boards. It starts a `<AP name>-recovery` access point and connects to the network with the saved EMS-ESP settings (Ethernet if configured, otherwise WiFi, using the static IP if one is set), and has a WebUI to sign in with an existing admin user, upload a new firmware into either OTA partition, or start one of the installed firmwares. It only reads the EMS-ESP settings, never changes them. Released as `EMS-ESP-Recovery-<version>-<chip>-16MB[+].bin`, and installed by uploading it in the EMS-ESP WebUI like a normal firmware, or flashed with `pio run -e recovery_s3_16M_P -t upload`
- firmware rollback: a newly installed firmware is only kept once it has run for 2 minutes. If it crashes or restarts before then, the device goes back to the previous firmware, or to the Recovery if there is none
- on 16MB boards with the Recovery installed, EMS-ESP switches to it after crashing 5 times in a row. The Recovery shows why it was started and which firmware failed to start
- the Version page shows the installed Recovery version on 16MB boards
- crash test builds to try out the rollback and recovery, see `[env:crashtest_s3_16M_P]` in `platformio.ini`
- 16MB builds use EMS-ESP's own Arduino/IDF libs from [esp32-arduino-lib-builder](https://github.com/emsesp/esp32-arduino-lib-builder) instead of Tasmota's prebuilt cores
- optional `ccache` wrapping when it is installed (`scripts/enable_ccache.py`) to speed up CI
- heat pump `dhw/disinfectduration`, the time allowed to reach the thermal disinfection temperature (0xEA offset 15, 120-240 min) [#3278](https://github.com/emsesp/EMS-ESP32/issues/3278)
- Junkers FW/FR thermostat display settings (main display, language, date format, contrast) are now also shown on older models like the FR120, plus the room temperature sensor calibration `intoffset` (0x11E offset 8, -3.0 to 3.0 K) [#3329](https://github.com/emsesp/EMS-ESP32/issues/3329)
- Hungarian language (HU) 
- heat pump backup heater energy per mode in 0.01 kWh: `metereheatheat` and `dhw/metereheat` (0x4AF offsets 28 and 36) [#3325](https://github.com/emsesp/EMS-ESP32/issues/3325)

## Fixed

- large numbers in analog sensor values (e.g. pulse counters above 1 million), custom entities and API/MQTT writes could be cut off or lose digits [#3367](https://github.com/emsesp/EMS-ESP32/issues/3367)
- ESP32-C6 boards without a board profile got invalid GPIOs (99), so on every start the settings were rejected and fell back to empty values (blank Board Profile, EMS Tx Mode and Bus ID), and saving any Application setting failed with a 400 error. A new C6 now starts with the CUSTOM profile and pins for the Seeed XIAO ESP32-C6: Rx GPIO20 (D9), Tx GPIO19 (D8), button GPIO9
- heat-pump operating state (`hpoperatingstate`: heating/off/cooling) is published for every heating circuit. On BC400/UI800 it was only filled for hc1, because hc2 never answers telegram 0x472 while the state is still broadcast in 0x2A6 offset 20 [#3097](https://github.com/emsesp/EMS-ESP32/issues/3097)
- restart from the WebUI (Restart button, firmware/settings upload) only happened while the browser tab stayed open. If the tab was closed the restart was silently dropped after 5 minutes, leaving a new firmware uninstalled. EMS-ESP now restarts on its own if the WebUI hasn't picked up the pending restart within 10 seconds
- a firmware upload from a script or `curl` was accepted (`md5_ok`) but EMS-ESP didn't restart into it: the client closing the connection after the upload was treated as an aborted upload, which cancelled the pending restart
- EMS-ESP crashed and restarted when an upload was rejected, for example an empty or invalid MD5 file. The error code was kept in the request's `_tempObject`, which the web server frees as heap memory when the request ends
- shunting yard show json
- `system/sendmail` called from a Command or the Scheduler computed its value twice, which stripped the quotes from literal text and then failed on characters like `!`. Also shunting yard treated a `?` inside a quoted string as a ternary, so text like `"too hot?"` returned nothing
- memory leak when using different timezones for ems-esp and thermostat[#3184](https://github.com/emsesp/EMS-ESP32/issues/3184)
- LED stayed off on a healthy system when "Disable LED" was unchecked
- Ethernet MAC address changed with the new SDK, breaking DHCP reservations (currently disabled)
- "IPv4 nameserver" showed an IPv6 address when IPv6 was in use
- the Network status page showed at most one IPv6 address and could hide the IPv4 DNS server entirely. It only read the link-local address, so a dual-stack host's ULA and global addresses were never reported, and it only read two of the three DNS slots, which are shared between IPv4 and IPv6. It now lists every address the interface holds, each labelled with its IPv6 scope, and every configured DNS server [#3246](https://github.com/emsesp/EMS-ESP32/issues/3246)
- the Gateway was only ever reported for IPv4, so on a dual-stack network the IPv6 default router was missing from the Network status page, `show system` and the MQTT `info` topic. Routers are learnt from router advertisements rather than configured, so all of them are now listed [#3246](https://github.com/emsesp/EMS-ESP32/issues/3246)
- `show system` had the same gaps, reporting a link-local IPv6 address only and a single IPv4 DNS server. The WiFi and Ethernet sections now list every IPv6 address with its scope, and every DNS server labelled with its own family [#3246](https://github.com/emsesp/EMS-ESP32/issues/3246)
- the Access Point status page reported whichever interface happened to be active, so with the AP down it showed the station's IP and MAC address under an "Inactive" heading. It now always reports the soft-AP's own details, and shows `none` when the AP isn't running
- the MQTT `info` topic published no addresses at all when connected over Ethernet, and over WiFi only an IPv4 address plus a link-local IPv6 one. Both interfaces now report their MAC, IPv4 address and gateway, every IPv6 address and every DNS server. Note that `IPv6 address` is now an array, and `IPv4 DNS Server` is replaced by a `DNS Server` array that covers both families [#3246](https://github.com/emsesp/EMS-ESP32/issues/3246)
- HA Discovery warning on Uptime after EMS-ESP boot due to NTP not ready
- hc/control setting for UI800 thermostats [#3181](https://github.com/emsesp/EMS-ESP32/discussions/3181)
- solar module switches [#3223](https://github.com/emsesp/EMS-ESP32/issues/3223)
- UI800/HMI800.2 thermostats without telegram 0x470 reported `summertemp` as 0 and lost the per-hc heat/cool delays. `summermode` is now also shown for UI800 heatpumps [#3257](https://github.com/emsesp/EMS-ESP32/issues/3257)
- possible crash when the WebUI asked for version information while the versions.json refresh was rewriting the cache from the main loop task
- HTTP client could write past a fixed 64 byte buffer when a URL had a hostname of 64 characters or more, could busy-spin for the whole read budget if the socket reported bytes it wouldn't return, and buffered responses without any size limit
- the "Ethernet clock mode (GPIO16/17) conflicts with PSRAM" error was never logged because Ethernet had already been skipped by the time the check ran
- heat limit entities (`maxheatcomp`, `maxheatheat`, `dhw.maxheat`) read as unknown on heat pumps with an auxiliary heater larger than 9 kW - `enum_maxHeat` was missing the 12 kW and 15 kW steps
- add 0kW to heat limits [#3262](https://github.com/emsesp/EMS-ESP32/issues/3262), [#3265](https://github.com/emsesp/EMS-ESP32/issues/3265)
- show temperature value for sensor 0x40 [#3272](https://github.com/emsesp/EMS-ESP32/issues/3272)
- no EMS bus on custom boards whose interface relies on a pull-up on the Rx line. The newer ESP-IDF no longer enables the internal pull-up on the UART Rx pin, so it is now set explicitly as it was in 3.8 [#3258](https://github.com/emsesp/EMS-ESP32/discussions/3258)
- no EMS bus after restart when Rx uses a pin that boots with another IO_MUX function (e.g. GPIO43/44 = UART0 on ESP32-S3). IDF 5 `uart_set_pin()` no longer selects the GPIO function on the Rx pad when routing via the matrix, so the pin is reset before UART setup [#3258](https://github.com/emsesp/EMS-ESP32/discussions/3258)
- with no EMS bus attached (e.g. only powered over USB), noise on the Rx line was taken for a bus: a single random byte matching our poll marked the bus as connected, after which every noise frame was logged as `Incomplete Rx` or `Invalid source` and counted as an error. A noise frame passing the CRC could also set the wrong EMS bus type. A poll to our bus id is now only trusted when the 3 telegrams before it all had a valid CRC, the bus type is taken from the poll, and nothing is processed before then
- Tx mode Auto preferred HT3 pacing over EMS+ when the bus used the HT3 address mask. Some Bosch heat pumps (e.g. Compress 7000i AW, CS5800i) need EMS+ timing; Auto now selects EMS+ when boiler monitor 0xE4 is seen (not merely any extended telegram, which Junkers HT3 also uses) [#3283](https://github.com/emsesp/EMS-ESP32/issues/3283), [#3245](https://github.com/emsesp/EMS-ESP32/issues/3245)
- Guru Meditation (cache error) on HybridCompile builds when the EMS or console UART ISR ran from flash during a SPI flash read (WiFi power-down, mDNS, NVS). The UART ISR and ringbuf ISR helpers are now in IRAM
- compile errors against Arduino 3.3.12 from Tasmota/Arduino headers named `console.h`, `network.h` and `httpClient.h`. Those EMS-ESP headers are now `emsesp_console.h`, `emsesp_network.h` and `emsesp_http_client.h`
- HTTP OTA from WSL/mDNS stalled on IPv6 and used tiny write chunks. Uploads now resolve IPv4 and stream in 64 KB blocks (`scripts/upload.py`, `scripts/upload_cli.py`)
- BC400/UI800/HMC310 `dhw.circmode` labels: `auto` was "According to DHW time program" and `own prog` was the circulation's own Auto program. Renamed to match the CS5800i UI [#3296](https://github.com/emsesp/EMS-ESP32/issues/3296)
- resetting a customized min/max now restores the original limits immediately. The register limits are kept and the custom range is applied only when the limits are read, so a restart is no longer needed
- a device's custom name and brand were wiped when saving an entity customization, and the name was gone after the next restart. Saving entities now keeps the name and brand stored in the same record
- `dhw/comfdiff`, `dhw/ecodiff` and `dhw/ecoplusdiff` now accept a minimum of 4 K instead of 6 K, matching the CS5800i [#3273](https://github.com/emsesp/EMS-ESP32/issues/3273)
- syslog dropped messages at DEBUG because the queue held only 10 lines, less than one bus burst. The queue is now 100. Syslog success and failure counts are included in the MQTT heartbeat, and the Activity page rounds the quality percentage instead of truncating it [#3284](https://github.com/emsesp/EMS-ESP32/issues/3284)
- a read or write aimed at EMS-ESP's own bus id was left unanswered, so the sender waited for a timeout. EMS-ESP now sends an empty reply to those reads and acknowledges those writes [#3094](https://github.com/emsesp/EMS-ESP32/issues/3094)
- a second boiler monitor `0xE4` carrying `0xFFFF` cleared a valid `pc0Flow`. That empty value is now ignored [#3224](https://github.com/emsesp/EMS-ESP32/issues/3224)
- writing a negative custom entity value was always 1 too high because float-to-int rounding always added +0.5. Negative values now round the same way as positive ones [#3297](https://github.com/emsesp/EMS-ESP32/issues/3297)
- after a cold boot, SoftAP fallback no longer abandons STA forever. If a home WiFi SSID is configured, EMS-ESP keeps retrying STA while the captive portal is up so a late-booting router (e.g. after a power outage) can still be joined; SoftAP stops once STA gets an IP
- MQTT with "Enable TLS" on a port that isn't one of a few well-known TLS ports (8883 is, 8884 or 18883 are not) connected without TLS, sending the MQTT username and password in clear text
- NTP time zone strings updated to IANA tz 2026e. About 32 zones had the wrong UTC offset now or within the next year (Mexico no longer uses DST, Egypt does again, Morocco is permanent UTC+0, Kazakhstan, and from 1 November 2026 western Canada stays on permanent offsets). On boot EMS-ESP rewrites the stored POSIX string from the saved IANA label so existing installs pick up the new rules without re-selecting the zone [#3308](https://github.com/emsesp/EMS-ESP32/pull/3308)
- MQTT with TLS and a CA certificate crashed on every connect (a boot loop) on boards with PSRAM when the broker's certificate chain is EC, such as Let's Encrypt ISRG Root X2: verifying it needs about 6 KB of stack and the MQTT task only had 5120 bytes. The MQTT task now gets an 8 KB stack when TLS verifies a CA certificate, and keeps 5120 bytes for insecure TLS and plain MQTT
- a command value containing a `/` (e.g. the date format `mm/dd/yyyy`) sent via MQTT or the API was taken as a reference to another entity (`device/entity`) and failed with `no 'dd' in unknown`. It is now only treated as a reference when the part before the `/` is a known device [#3329](https://github.com/emsesp/EMS-ESP32/issues/3329)
- Junkers `contrast` is shown and set in % (25 to 75 in steps of 2.5) instead of the raw value 10 to 30, and the Junkers `display` setting no longer lists two empty options [#3329](https://github.com/emsesp/EMS-ESP32/issues/3329)
- RC300/RC310 `nofrostmode` missing `off` state [#3311](https://github.com/emsesp/EMS-ESP32/issues/3311)

## Changed

- security: Web UI sign-in tokens are signed with a random key that is unique to each device, so existing access tokens stop working after the upgrade. Admins see a notice in the WebUI until it is closed. Changing an admin password replaces the key, which signs everyone out
- security: Web UI sign-ins expire after 30 days (once the clock is set by NTP), and changing a user's password, or removing the user, signs that user out everywhere [#3353](https://github.com/emsesp/EMS-ESP32/issues/3353)
- security: API keys replace the per-user access tokens for `/api`. Admins create them in Security > API Keys with a name and "Read" or "Read and Write" access, and can revoke each one. A key is shown once when created and only stored as a hash. Read keys act like a guest user, Read and Write keys like an admin, and keys only work on `/api`, not for the Web UI [#3354](https://github.com/emsesp/EMS-ESP32/issues/3354)
- security: an admin who signs in while factory passwords are in use must set a new admin password first, plus an access point password if no WiFi network is set [#3357](https://github.com/emsesp/EMS-ESP32/issues/3357)
- security: a fresh install or factory reset no longer creates the `guest` user [#3357](https://github.com/emsesp/EMS-ESP32/issues/3357)
- security: a warning is shown while the access point still uses the factory password [#3359](https://github.com/emsesp/EMS-ESP32/issues/3359)
- security: firmware updates (Web UI upload, install from URL, Recovery) only accept firmware signed by EMS-ESP (ECDSA P-256). Unsigned firmware and filesystem images need `allow unsigned` on the serial console first. Releases are signed in CI by `scripts/sign_firmware.py`. A refused upload, also from the version list, says so in the Web UI and links to the documentation. Recovery 1.0.2 [#3369](https://github.com/emsesp/EMS-ESP32/issues/3369)
- security: the firmware signing key policy (who holds the keys, how they're replaced) is in `SECURITY.md`, and only the dev and stable release workflows can use the key [#3370](https://github.com/emsesp/EMS-ESP32/issues/3370)
- security: the command parser and expression evaluator are fuzzed with AddressSanitizer and UBSan on every pull request (`pio run -e native-fuzz -t exec`). It found an unmatched `)` in an expression corrupting memory, an integer overflow in number parsing and a `%` in a command value being read as a log format, all fixed [#3368](https://github.com/emsesp/EMS-ESP32/issues/3368)
- security: hardened parsing of API, MQTT and expression input: large numbers, `%` by zero, very long numbers or hex values and oversized request bodies no longer crash the device. Request bodies are only read after the login check [#3367](https://github.com/emsesp/EMS-ESP32/issues/3367)
- security: custom Commands run by guests use the stored value only, and URL fetches in values only work in text saved by an admin (Commands, Schedules, Custom Entities) or sent by an admin [#3366](https://github.com/emsesp/EMS-ESP32/issues/3366)
- security: browsers can only open the device by its IP address, hostname, `<hostname>.local` or a name added under Network Settings > Allowed host names, against DNS rebinding. Requests from Home Assistant, scripts and through the access point are not checked [#3364](https://github.com/emsesp/EMS-ESP32/issues/3364)
- security: the Web UI is served with `Content-Security-Policy`, `X-Content-Type-Options: nosniff` and `Referrer-Policy: no-referrer` headers [#3365](https://github.com/emsesp/EMS-ESP32/issues/3365)
- security: CORS needs an explicit origin, `Access-Control-Allow-Credentials` is no longer sent, and `/api` refuses browser requests from other sites unless their origin is allowed by the CORS setting [#3361](https://github.com/emsesp/EMS-ESP32/issues/3361)
- security: tokens and API keys are only accepted in the `Authorization: Bearer` header, no longer as an `access_token` URL parameter, so they don't end up in browser history or server logs [#3363](https://github.com/emsesp/EMS-ESP32/issues/3363)
- security: changing settings through `/api` (for example `system/mqtt/enabled`) needs a "Read and Write" API key
- security: "Bypass Access Token authorization on API calls" no longer grants admin rights. Calls without a token act like a guest: they can read values and run Commands that guests may run, but not change values or settings [#3360](https://github.com/emsesp/EMS-ESP32/issues/3360)
- security: reading the definitions of named Commands through `/api` (`/api/commands/values`, `/info`, `/entities` or `/api/commands/<name>/value`) needs a "Read and Write" API key. Integrations that read these without one get "authentication failed". Calling a command by name still runs it
- security: only admins can open or change the Scheduler, Commands, Custom Entities and Modules, rename devices or change entity customizations and log settings, or use admin actions such as exports and partition switching. Guests can still view the log
- security: each Command has a "Guests may run this command" option. Commands created before 3.9.0 keep guest access, new ones start without it. It applies to the WebUI and `/api`
- security: the live System Log needs a login
- security: the Super User password is removed. On the serial console `su` asks for a Web UI admin user's username and password, `passwd` is removed. The console `read` command needs admin
- security: telnet asks for a Web UI user's login, admin users get admin rights and `su` is only on the serial console. At most 2 telnet sessions, closed after 10 minutes idle. Telnet is off on new installs (upgrades keep their setting) and the Web UI shows a notice while it's on. Telnet is not encrypted, so use the Web UI or serial for admin work, or a VPN [#3358](https://github.com/emsesp/EMS-ESP32/issues/3358)
- security: repeated failed logins to the Web UI, telnet, serial `su` and the recovery firmware are slowed down, per source address and for all sources together. The Web UI sign-in answers HTTP 429 while locked out [#3362](https://github.com/emsesp/EMS-ESP32/issues/3362)
- security: to go back to 3.8.x after upgrading, restore a System Backup made before the upgrade. 3.8.x can't read the hashed passwords, so without that backup a factory reset is needed
- security: user passwords are stored hashed (PBKDF2-SHA256), existing passwords are converted on the first boot. The Web UI and `show users` no longer show stored passwords; leave a password field empty to keep the current one [#3355](https://github.com/emsesp/EMS-ESP32/issues/3355)
- security: settings exports and System Backups leave out passwords and keys, unless an admin ticks "Include passwords and keys". Restoring keeps the passwords already on the device; a restored user with no password can't log in until an admin sets one [#3356](https://github.com/emsesp/EMS-ESP32/issues/3356)
- various memory optimizations [#3083](https://github.com/emsesp/EMS-ESP32/issues/3083)
- Scheduler name is now mandatory
- Access Point provision mode label renamed from "When network connection is lost" to "When network connection not found" (all languages)
- network fallback to AP only after start [#3090](https://github.com/emsesp/EMS-ESP32/issues/3090)
- replaced Web async-validator with custom validator and toast with native snackbar to reduce bundle size
- Gateway and Connect devices are shown in the Devices page, but disabled [3126](https://github.com/emsesp/EMS-ESP32/discussions/3126)
- show control setting only for master thermostats (0x10) [#3173](https://github.com/emsesp/EMS-ESP32/issues/3173)
- remove devices without entities not listed in 0x07 telegram
- changes to the Dialogs in Dashboard page, removed the run icon in the Commands page. Renamed Execute to Run.
- set `None` for undefined values in HA `val_tpl`
- changes in WebUI - remove multiple calls systemInfo endpoint, Vewrsion pages checks for internet connection [#3195](https://github.com/emsesp/EMS-ESP32/pull/3195)
- multiple optimization in network, power down WiFi when Ethernet and WiFi nosleep is instant when selected [#3213](https://github.com/emsesp/EMS-ESP32/pull/3213)
- TLS buffers (~17KB per HTTPS request) are taken from PSRAM on boards that have it, instead of fragmenting the internal heap
- failed versions.json fetches back off from 5 up to 160 minutes instead of retrying every 5 minutes, as each attempt blocks the main loop for as long as the DNS, connect and read timeouts allow
- optimized the LED library to support WS2812B V5 and the newer V6 RGB LEDs
- hold Tx while a K30RF/KM200 gateway is syncing [#3094](https://github.com/emsesp/EMS-ESP32/issues/3094)
- Tasmota platform 2026.05.50 (Arduino 3.3.8 / IDF 5.5.4) to 2026.09.50 (Arduino 3.3.12 / IDF 5.5.5). Building that platform needs PlatformIO Core ≥6.2.0; install `pioarduino==6.2.0` (official `platformio` is still 6.1.19). See CONTRIBUTING.md
- CI dev-release firmware jobs run in parallel (webUI, firmware targets, standalone) instead of one serial job
