import hashlib
import shutil
import re
import os
from pathlib import Path
Import("env")

OUTPUT_DIR = Path("build")


def bin_copy(source, target, env):
    """Optimized firmware renaming and copying function."""
    
    # Get the application version, from emsesp_version.h unless overridden in the env
    fw_name = env.GetProjectOption("custom_fw_name", "EMS-ESP")
    version_file = Path(env.GetProjectOption("custom_version_file", "./src/emsesp_version.h"))
    version_define = env.GetProjectOption("custom_version_define", "EMSESP_APP_VERSION")
    if not version_file.exists():
        print(f"Error: {version_file} not found!")
        return
    
    app_version = None
    version_pattern = re.compile(rf'^#define {version_define}\s+"(\S+)"')
    
    with version_file.open('r') as f:
        for line in f:
            match = version_pattern.match(line)
            if match:
                app_version = match.group(1)
                break
    
    if not app_version:
        print(f"Error: Could not find {version_define} in {version_file}!")
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
    variant = f"{fw_name}-{app_version.replace('.', '_')}-{mcu}-{flash_mem}{'+' if psram else ''}"

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

    print("=" * 90)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_copy])
env.AddPostAction("$BUILD_DIR/${PROGNAME}.md5", [bin_copy])
