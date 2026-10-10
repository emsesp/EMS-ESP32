# Makes `pio run -t upload` write the app into the factory partition instead of ota_0.
#
# Set `custom_app_offset` to the factory partition offset. The otadata image (boot_app0.bin) that
# PlatformIO normally writes selects ota_0, so it's swapped for a blank one, which makes the
# bootloader start the factory app.
from pathlib import Path

Import("env")

OTADATA_SIZE = 0x2000

offset = env.GetProjectOption("custom_app_offset", "")
if offset:
    # ESP32_APP_OFFSET itself is recalculated from the partition table later in the build
    env.Replace(UPLOADCMD=env.get("UPLOADCMD", "").replace("$ESP32_APP_OFFSET", offset))

    blank_otadata = Path(env.subst("$BUILD_DIR")) / "otadata_blank.bin"
    blank_otadata.parent.mkdir(parents=True, exist_ok=True)
    blank_otadata.write_bytes(b"\xff" * OTADATA_SIZE)

    def swap(path):
        return str(blank_otadata) if Path(env.subst(str(path))).name == "boot_app0.bin" else path

    # the platform has already expanded FLASH_EXTRA_IMAGES into UPLOADERFLAGS, so patch both
    env.Replace(FLASH_EXTRA_IMAGES=[(o, swap(p)) for o, p in env.get("FLASH_EXTRA_IMAGES", [])])
    env.Replace(UPLOADERFLAGS=[swap(flag) for flag in env.get("UPLOADERFLAGS", [])])

    print(f"Upload: app at {offset}, otadata blanked")
