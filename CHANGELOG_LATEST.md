# Changelog

For more details go to [emsesp.org](https://emsesp.org/).

## [3.9.0]

This release is based on the latest Espressif/Arduino core version 3. It brings in many memory and performance optimizations, for example 25% more available memory and 30% less PSRAM fragmentation.

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

## Fixed

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
- NTP time zone strings updated to IANA tz 2026e. About 32 zones had the wrong UTC offset now or within the next year (Mexico no longer uses DST, Egypt does again, Morocco is permanent UTC+0, Kazakhstan, and from 1 November 2026 western Canada stays on permanent offsets). On boot EMS-ESP rewrites the stored POSIX string from the saved IANA label so existing installs pick up the new rules without re-selecting the zone [#3308](https://github.com/emsesp/EMS-ESP32/pull/3308)

## Changed

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
