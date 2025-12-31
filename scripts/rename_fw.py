import hashlib
import shutil
import re
import os
from pathlib import Path
Import("env")

OUTPUT_DIR = Path("build")


def bin_copy(source, target, env):
    """Optimized firmware renaming and copying function."""
    
    # Get the application version from emsesp_version.h
    version_file = Path('./src/emsesp_version.h')
    if not version_file.exists():
        print("Error: emsesp_version.h not found!")
        return
    
    app_version = None
    version_pattern = re.compile(r'^#define EMSESP_APP_VERSION\s+"(\S+)"')
    
    with version_file.open('r') as f:
        for line in f:
            match = version_pattern.match(line)
            if match:
                app_version = match.group(1)
                break
    
    if not app_version:
        print("Error: Could not find EMSESP_APP_VERSION in emsesp_version.h!")
        return

    # Get the chip type, in uppercase
    mcu = env.get('BOARD_MCU', '').upper()
    if not mcu:
        print("Error: Could not determine MCU type!")
        return

    # Work out the flash memory from the PIO env name
    flash_mem = "4MB"  # default
    pio_env = env.get('PIOENV', '').upper()
    if pio_env:
        parts = pio_env.split('_')
        # If it ends with _P skip (we use this to denote PSRAM)
        index = -2 if parts[-1].endswith("P") else -1
        
        # If it has an M at the end, use it
        if parts[index].endswith("M"):
            flash_mem = parts[index] + "B"

    # Check if BOARD_HAS_PSRAM is in the cppdefines
    cppdefines = env.get("CPPDEFINES", [])
    psram = 'BOARD_HAS_PSRAM' in cppdefines

    print("=" * 90)
    print(f"EMS-ESP version: {app_version}")
    print(f"Has PSRAM: {'Yes' if psram else 'No'}")
    print(f"MCU: {mcu}")
    print(f"Flash Mem: {flash_mem}")

    # Convert . to _ so Windows doesn't complain
    # Format is EMS-ESP-<version>-<mcu>-<flash> with + at the end if it has PSRAM
    variant = f"EMS-ESP-{app_version.replace('.', '_')}-{mcu}-{flash_mem}{'+' if psram else ''}"

    # Create output directories
    firmware_dir = OUTPUT_DIR / "firmware"
    firmware_dir.mkdir(parents=True, exist_ok=True)

    # Define file paths
    bin_file = firmware_dir / f"{variant}.bin"
    md5_file = firmware_dir / f"{variant}.md5"

    # Remove existing files if they exist
    for file_path in [bin_file, md5_file]:
        if file_path.exists():
            file_path.unlink()

    print(f"Filename: {bin_file}")

    # Copy firmware.bin to firmware/<variant>.bin
    shutil.copy2(str(target[0]), str(bin_file))

    # Calculate and write MD5 hash
    with bin_file.open("rb") as f:
        md5_hash = hashlib.md5(f.read()).hexdigest()
    
    print(f"MD5: {md5_hash}")
    md5_file.write_text(md5_hash)

    # Make a copy using the old 3.6.x filename format for backwards compatibility
    # Note: there is a chance newer E32V2s (which use the 16MB partition table and PSRAM) 
    # are running a custom build of the 3.6.5 firmware as 3.6.5 was released before 
    # production of the gateway board. Updating via the WebUI will break the system 
    # and require a manual update.
    
    pio_env = env.get('PIOENV', '')
    extra_variant = None
    
    if pio_env == "s3_16M_P":
        extra_variant = f"EMS-ESP-{app_version.replace('.', '_')}-ESP32_S3"
    elif pio_env == "s_4M":
        extra_variant = f"EMS-ESP-{app_version.replace('.', '_')}-ESP32"

    if extra_variant:
        extra_bin_file = firmware_dir / f"{extra_variant}.bin"
        extra_md5_file = firmware_dir / f"{extra_variant}.md5"
        
        # Remove existing files if they exist
        for file_path in [extra_bin_file, extra_md5_file]:
            if file_path.exists():
                file_path.unlink()
        
        # Copy files
        shutil.copy2(str(bin_file), str(extra_bin_file))
        shutil.copy2(str(md5_file), str(extra_md5_file))
        print(f"Filename copy for 3.6.x: {extra_bin_file}")

    print("=" * 90)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_copy])
env.AddPostAction("$BUILD_DIR/${PROGNAME}.md5", [bin_copy])
