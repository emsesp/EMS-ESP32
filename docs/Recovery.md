# EMS-ESP Recovery Firmware

Small standalone app that lives in the factory (`boot`) partition on **16MB** boards. If EMS-ESP will not start, the recovery still can: it brings up an access point (and the saved WiFi/Ethernet if it can) and serves a cut-down WebUI to upload a new EMS-ESP image into `app0`/`app1` or choose which slot to boot.

4MB boards have no factory partition and no recovery.

Hardware test steps are in [Recovery-Testing.md](Recovery-Testing.md).

## Why it exists

ESP-IDF already rolls a failed OTA back to the **other** OTA slot (`app0` ↔ `app1`). That does not help when:

- both OTA slots are empty, corrupt, or crash on start
- a serial flash left otadata pointing at an empty slot
- the user needs a known-good way back in without a USB cable

Recovery is a third, smaller image that EMS-ESP does not overwrite during a normal upgrade. Installing it does **not** change which app boots.

## Partitions (16MB)

From `partitions/esp32_partition_16M.csv`:

| Label | Subtype | Role |
| --- | --- | --- |
| `boot` | factory | Recovery image |
| `app0` | ota_0 | EMS-ESP |
| `app1` | ota_1 | EMS-ESP (the other OTA slot) |
| `otadata` | ota | Which OTA slot the bootloader starts |

The bootloader reads **otadata**, not “whatever was flashed last”:

1. Blank otadata (`0xFF`, e.g. after `erase-flash` or a recovery serial upload) → start **factory** (`boot`).
2. Valid otadata → start that OTA slot (`app0` or `app1`).
3. If that image is invalid, it walks **down** toward factory, then **up** through remaining OTA slots. A valid recovery in `boot` is therefore preferred over a valid image sitting only in `app1` while otadata still points at empty `app0`.

PlatformIO’s `boot_app0.bin` always selects `app0`. The recovery upload script (`scripts/upload_factory.py`) replaces it with a blank otadata so a serial flash of recovery actually boots recovery.

A normal serial flash of EMS-ESP goes to `app0` and writes `boot_app0.bin`, so the board starts EMS-ESP. Recovery, if already in `boot`, is left in place.

## How recovery is installed

From a running EMS-ESP (**Settings → Version**, or the upload drop zone):

- Drop `EMS-ESP-Recovery-*.bin` (optional `.md5` first).
- EMS-ESP recognises it from the app description (`project_name` = `EMS-ESP-Recovery`) and writes it to the factory partition **without** calling `esp_ota_set_boot_partition`.
- EMS-ESP keeps running. The Version page shows the recovery version once it is in flash.

From serial, recovery must be written at **`0x10000`**, not at `0x1000` (that is the bootloader; the recovery `.bin` overlaps the partition table). See `scripts/do_fresh_install.sh`.

The recovery app itself **refuses** to install a recovery image. It only writes EMS-ESP into `app0`/`app1`.

## How EMS-ESP boots into recovery

Only 16MB builds with `-DEMSESP_HAS_RECOVERY` and a valid recovery image in `boot`.

| How | What happens |
| --- | --- |
| **Settings → Version** | Developer mode shows a Recovery row. If a version is installed, **Restart** confirms, then `setPartition("boot")`. |
| **Console** (serial or telnet, admin) | `restart boot` → `system_restart("boot")`. |
| **Button** | Long press ~3 s (not the ~10 s factory reset). |
| **Crash loop** | 5 panics or watchdog resets in a row (RTC counter; survives software restart, not a power cycle). |

The first three record NVS `rec_reason=request`. The crash path records `crash`. Recovery reads that once and clears it, so a user-requested entry does not show the red crash banner.

These do **not** enter recovery:

- Uploading the recovery `.bin` (install only).
- `/api/system/restart` or MQTT `system/restart` (ignore any partition argument; reboot the current app).
- Bootloader rollback of a pending OTA (goes to the previous **OTA** slot).

## What recovery does

- Access point `ems-esp-recovery` (AP SSID plus `-recovery`), plus the saved LAN (Ethernet preferred, else WiFi, including a static IP if one is set).
- Sign-in with an EMS-ESP admin account (settings are read from LittleFS, never written).
- List `boot` / `app0` / `app1`, show which is running, which will boot next, and rollback state.
- Upload an EMS-ESP `.bin` into an OTA slot (empty slot, or the older of the two if both are filled) and boot it.
- **Start** a chosen OTA slot without uploading.

After a WebUI/OTA install from recovery, the new EMS-ESP image is pending verification. After ~2 minutes of uptime it is confirmed so the bootloader will not roll it back.

## Rough boot flow

```
reset
  → ESP-IDF bootloader
      otadata blank?     → factory/boot (recovery)
      otadata → app0/1   → that slot if the image is valid
      selected image bad → try factory, then the other OTA slot
  → recovery  or  EMS-ESP

EMS-ESP, if it starts:
  crash counter (RTC)
    5 crashes in a row + recovery present → set boot=factory, restart
  healthy for 2 minutes → confirm OTA image, clear crash counter
```

## Builds

| Env | Image |
| --- | --- |
| `recovery_s_16M` / `recovery_s_16M_P` / `recovery_s3_16M_P` | Recovery, flashed into `boot` at `0x10000` |
| `s_16M` / `s_16M_P` / `s3_16M_P` | EMS-ESP (`app0` on serial upload) |

Firmware files land in `build/firmware/` as `EMS-ESP-Recovery-<ver>-<chip>-16MB[+].bin`.
