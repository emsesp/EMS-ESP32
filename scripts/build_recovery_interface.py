# Builds the Recovery WebUI into recovery/WWWData.h when it's missing or older than its sources.
# In the build-recovery environment it only builds the WebUI, always.
from pathlib import Path
import os
import shutil
import subprocess

Import("env")

INTERFACE_DIR = Path("interface")
OUTPUT_FILE = Path("recovery") / "WWWData.h"
SOURCES = [
    INTERFACE_DIR / "recovery.html",
    INTERFACE_DIR / "vite.config.ts",
    INTERFACE_DIR / "progmem-generator.js",
    INTERFACE_DIR / "src" / "recovery",
    INTERFACE_DIR / "src" / "components",
    INTERFACE_DIR / "src" / "CustomTheme.tsx",
    INTERFACE_DIR / "public",
]


def newest_mtime(paths):
    newest = 0.0
    for path in paths:
        if path.is_dir():
            for f in path.rglob("*"):
                if f.is_file():
                    newest = max(newest, f.stat().st_mtime)
        elif path.exists():
            newest = max(newest, path.stat().st_mtime)
    return newest


def is_stale():
    if not OUTPUT_FILE.exists():
        return True
    return newest_mtime(SOURCES) > OUTPUT_FILE.stat().st_mtime


def build():
    pnpm = next((n for n in ("pnpm", "pnpm.cmd", "pnpm.exe") if shutil.which(n)), None)
    if pnpm is None:
        print("Error: pnpm not found in PATH, cannot build the Recovery WebUI")
        env.Exit(1)

    os.environ["CI"] = "true"
    for command in (f"{pnpm} install", f"{pnpm} build-recovery"):
        print(f"Running: {command}")
        result = subprocess.run(command, shell=True, cwd=INTERFACE_DIR)
        if result.returncode != 0:
            print(f"Error: '{command}' failed")
            env.Exit(1)


if env.get("PIOENV") == "build-recovery":
    # the WebUI is only built by the "build" target. Skip any other, e.g. `pio run -t upload` also runs this environment
    # when it's one of the default_envs, and it has nothing to upload
    if "build" not in [t.strip() for t in COMMAND_LINE_TARGETS]:
        print("Skipping the Recovery WebUI build, the target is not 'build'")
        env.Exit(0)

    # `pio run -e build-recovery` only builds the WebUI, always, and exits without building any firmware
    def build_recovery_webUI(*args, **kwargs):
        print("Building Recovery WebUI...")
        build()
        print("Recovery WebUI build completed successfully!")
        env.Exit(0)

    env.AddCustomTarget(
        name="build",
        dependencies=None,
        actions=[build_recovery_webUI],
        title="build recovery web interface",
        description="installs pnpm packages and builds the recovery web UI into recovery/WWWData.h",
        always_build=True,
    )
elif is_stale():
    print("Building Recovery WebUI...")
    build()
else:
    print("Recovery WebUI is up to date")
