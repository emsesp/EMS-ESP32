# Testing the Recovery Firmware, Rollback and Crash-Loop Fallback

This guide covers testing on hardware of:

- the EMS-ESP Recovery firmware in the factory `boot` partition (16MB boards only);
- installing the recovery from the EMS-ESP WebUI;
- the bootloader rollback of a new firmware that fails to start;
- the fallback to the recovery after repeated crashes;
- Ethernet and static IP support in the recovery.

It's written for an ESP32-S3 16MB board, since the crash test build is only defined for `s3_16M_P`. Ethernet testing needs an E32V2. Keep a USB serial cable connected with a serial monitor open throughout, as most of the checks are log lines.

## 0. Preparation

Build everything:

```bash
pio run -e s3_16M_P -e crashtest_s3_16M_P -e recovery_s3_16M_P
pio run -e s_16M -e s_16M_P -e s_4M -e recovery_s_16M -e recovery_s_16M_P
```

The firmware files are written to `build/firmware/`:

| File | What it is |
| --- | --- |
| `EMS-ESP-<version>-ESP32S3-16MB+.bin` | EMS-ESP |
| `EMS-ESP-CrashTest-<version>-ESP32S3-16MB+.bin` | crash test build |
| `EMS-ESP-Recovery-<version>-ESP32S3-16MB+.bin` | the recovery |

Each has a matching `.md5` file.

Then:

1. Run the unit tests with `pio run -e native-test -t exec`. All tests should pass.
2. Optionally, check the recovery UI against mock data first: `cd interface && pnpm recovery`, then open <http://localhost:3001>.
3. Open the serial monitor with `pio device monitor -e s3_16M_P`. Recovery log lines start with `[recovery]`.

The crash counter is kept in RTC memory. It survives crashes and software restarts but not a power cycle, so don't unplug the board during the crash tests.

## 1. Baseline: EMS-ESP over serial, no recovery

1. Flash with `pio run -e s3_16M_P -t upload`. This also writes the current bootloader, which has rollback enabled. Boards flashed with an older bootloader don't roll back until they've been flashed over serial once.
2. In the WebUI, open **Settings → Version**. Check that:
   - the **Recovery** row shows "not installed";
   - the firmware partition list doesn't include `boot`.
3. Wait more than 2 minutes. "New firmware confirmed as working" should **not** appear in the log, because a serial flash isn't in the pending-verify state.

## 2. Install the recovery from the EMS-ESP WebUI

1. On **Settings → Version**, upload `EMS-ESP-Recovery-…bin`. Expect:
   - a "recovery installed" toast;
   - **no** restart;
   - the Recovery row now shows the recovery version;
   - the install logged.
2. Repeat, uploading the `.md5` first and then the `.bin`. The MD5 check should pass.
3. Negative tests:
   - Upload the recovery built for another chip, e.g. the ESP32 file on an S3. It should be rejected, and nothing should be written.
   - Start a recovery upload and close the browser tab halfway through. EMS-ESP should keep running and the Recovery row should show "not installed". Reinstall it properly afterwards.
4. Restart EMS-ESP normally. It should boot EMS-ESP again, not the recovery, because installing the recovery doesn't change the boot partition.

## 3. Enter the recovery on request

1. Enter the recovery in one of these ways:
   - in the console (telnet or serial) as admin, run `restart boot`;
   - long-press the button.
2. On the serial monitor, check that:
   - `[recovery] EMS-ESP Recovery v…` is logged;
   - LittleFS is mounted;
   - the access point has started;
   - WiFi connects.
3. Join the access point. Its name is the AP SSID plus `-recovery` (default `ems-esp-recovery`), and it uses the AP password (default `ems-esp-neo`). Open <http://192.168.4.1>; a captive portal page may open by itself. Alternatively, use the device's normal IP or `http://<hostname>.local` from the LAN.
4. Sign in with an EMS-ESP admin account (default `admin`/`admin`). A non-admin user must be refused.
5. On the dashboard, check that:
   - there's **no** red reason box, because the restart was requested;
   - `Recovery (boot)` has the "running" chip;
   - the installed firmware shows the correct version;
   - the System section shows the chip, flash, MAC, access point, and WiFi with its IP.
6. Click **Restart**. It should come back into the recovery, still without a reason box, since the reason is only shown once.
7. Click **Start** on the EMS-ESP slot. EMS-ESP should boot, and the Version page should still show the recovery version.

## 4. Upload firmware from the recovery

1. Enter the recovery again with `restart boot`.
2. Check the "upload target" chip:
   - if there's an empty slot, that slot is chosen;
   - if both slots hold a firmware, the one with the older install date is chosen, so the newest firmware is kept.
3. Upload the `.md5`, then the EMS-ESP `.bin`. EMS-ESP should start from that slot. About 2 minutes after it boots, "New firmware confirmed as working" should appear in the log.
4. The recovery must refuse these uploads:
   - a recovery `.bin`: a 409 with a message that it can't install itself;
   - a settings `.json`: a 406;
   - an EMS-ESP `.bin` for another chip: rejected as incompatible.
5. In **Write to**, pick the other slot and upload there. That should also work.

## 5. Bootloader rollback (crash test mode 1)

`platformio.ini` ships with `-D EMSESP_CRASH_TEST=1`.

1. Boot a working EMS-ESP and let it run for at least 2 minutes so it's confirmed.
2. Upload `EMS-ESP-CrashTest-…bin` from the **WebUI**, not over serial.
3. On the serial monitor you should see:
   - `Crash test, aborting`, a panic, and a restart;
   - the bootloader rolling back to the previous slot, which then starts normally.
4. In the EMS-ESP log (serial, or **Status → System Log**) you should see:
   - `Restarted after a crash (1 in a row)`;
   - `Firmware in partition appX failed to start and was rolled back`.
5. Enter the recovery with `restart boot`. Check that:
   - the red box says the firmware in appX failed to start;
   - that slot has the "failed to start" chip;
   - that slot is the upload target.
6. Click **Start** on the failed slot. The dialog should show a warning; cancel it.
7. Start the good slot.

If step 3 ends in a crash loop instead of a rollback, the bootloader on the board doesn't have rollback enabled. Flash over serial as in section 1, step 1.

## 6. Crash-loop fallback to the recovery (crash test mode 2)

1. In `platformio.ini`, change `-D EMSESP_CRASH_TEST=1` to `=2`, then run `pio run -e crashtest_s3_16M_P`.
2. Upload the crash test build from the WebUI. It runs normally.
3. Wait for "New firmware confirmed as working", about 2 minutes after boot.
4. Restart it from the WebUI. It should crash 5 times in a row, logging the "N in a row" count each time.
5. After the 5th crash you should see `Crashed 5 times in a row, restarting into the boot partition`, and the recovery starts.
6. In the recovery:
   - the red box should say "EMS-ESP crashed repeatedly and switched to the recovery";
   - after restarting the recovery once, the box should be gone.
7. Clean up: in **Write to**, pick the crash test slot **by hand** and upload a good EMS-ESP, or Start the good slot.
   - Switching to the recovery wipes the per-slot rollback states, so the default upload target may be the slot with the good firmware.
8. Change the flag back to `=1`.

Optional, without a recovery installed:

1. Erase the boot partition with `esptool --chip esp32s3 erase-region 0x10000 0x480000`. Older esptool versions use `erase_region`.
2. Repeat steps 2 to 4. It should keep crashing past 5, and needs to be reflashed over serial.

## 7. Network in the recovery

| Test | Setup | Expected in the recovery |
| --- | --- | --- |
| WiFi with static IP | Enable a static IP in the EMS-ESP Network settings, then `restart boot` | Same IP as EMS-ESP, and the WiFi row ends with ", static IP" |
| Ethernet (E32V2) | Cable plugged in, `recovery_s_16M_P` installed, then `restart boot` | Log shows "Ethernet started" and "Ethernet connected, IP …". The Ethernet row shows "connected", and WiFi shows "not used, Ethernet is connected" |
| Ethernet with static IP | As above, with a static IP set | Ethernet uses the static IP and WiFi isn't started |
| No cable | Unplugged, then `restart boot` | Ethernet shows "no cable connected" and WiFi shows "waiting for Ethernet". After about 15 seconds the log shows "No Ethernet connection" and WiFi connects |
| Cable plugged in later | Plug in after WiFi has joined | Ethernet gets an IP as well, and WiFi stays connected |
| Access point | Any of the above | `ems-esp-recovery` is always reachable on 192.168.4.1 |

## 8. Other checks

- **Online update during the pending window:** after a WebUI upgrade, start an online update from the Version page within 2 minutes. It should install, not fail.
- **4MB boards:** flash `s_4M` and check that:
  - there's no Recovery row;
  - a normal WebUI upgrade still gets confirmed after 2 minutes.
- **Translations:** switch the WebUI language and check the Recovery row and toast.
- **CI:** run the dev release workflow manually (`workflow_dispatch`) and check that the `EMS-ESP-Recovery-*` files for all three 16MB boards are among the release assets.

## Reset to a clean state

`pio run -e s3_16M_P -t upload` restores EMS-ESP at any point. It leaves the recovery in `boot` and the settings untouched.
