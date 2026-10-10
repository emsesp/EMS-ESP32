# Builds the Recovery WebUI into recovery/WWWData.h when it's missing or older than its sources
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


if is_stale():
    print("Building Recovery WebUI...")
    build()
else:
    print("Recovery WebUI is up to date")
