#!/usr/bin/env bash

# Refresh JS deps, format/lint/build the web UI + mock-api, spellcheck, then
# rebuild PlatformIO doc targets and run native tests.
#
# Usage (from anywhere):
#   ./scripts/update_all.sh
#   sh ./scripts/update_all.sh               # also fine — re-execs under bash
#   CLEAN=1 ./scripts/update_all.sh          # wipe node_modules + pio clean
#   ./scripts/update_all.sh --web-only       # skip PlatformIO
#   ./scripts/update_all.sh --skip-pio       # same as --web-only
#
# Needs: pnpm (or Corepack), node/npx, platformio (unless --web-only), python3

# `sh script` ignores the shebang and runs dash, which rejects bash arrays/[[ ]].
if [ -z "${BASH_VERSION:-}" ]; then
  exec /usr/bin/env bash "$0" "$@"
fi

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

WEB_ONLY=0
for arg in "$@"; do
  case "$arg" in
    --web-only | --skip-pio) WEB_ONLY=1 ;;
    -h | --help)
      sed -n '3,12p' "$0"
      exit 0
      ;;
    *)
      printf 'Unknown option: %s\n' "$arg" >&2
      exit 1
      ;;
  esac
done

# Bold ANSI: 36 cyan, 33 yellow, 32 green, 31 red.
step() {
  local color=$1
  shift
  printf '\n\033[1;%sm▶ %s\033[0m\n' "$color" "$*"
}
note() {
  printf '  \033[2m%s\033[0m\n' "$*"
}
die() {
  printf '\033[1;31m%s\033[0m\n' "$*" >&2
  exit 1
}

need() {
  command -v "$1" >/dev/null 2>&1 || die "Missing required tool: $1"
}

# Copy interface packageManager pin into mock-api so Corepack stays consistent.
sync_package_manager() {
  local pm
  pm="$(node -e "console.log(JSON.parse(require('fs').readFileSync('interface/package.json','utf8')).packageManager)")"
  node -e "
const fs = require('fs');
const path = 'mock-api/package.json';
const pkg = JSON.parse(fs.readFileSync(path, 'utf8'));
pkg.packageManager = process.argv[1];
fs.writeFileSync(path, JSON.stringify(pkg, null, 2) + '\n');
" "$pm"
  note "mock-api packageManager -> $pm"
}

# pnpm refuses to self-update when it's a Corepack shim; Corepack pins via packageManager.
update_pnpm() {
  step 33 "Updating pnpm"
  if command -v pnpm >/dev/null 2>&1 && readlink "$(command -v pnpm)" 2>/dev/null | grep -q corepack; then
    note "Corepack shim detected"
    (cd interface && corepack use pnpm@latest)
    sync_package_manager
  elif command -v pnpm >/dev/null 2>&1; then
    pnpm self-update
    sync_package_manager
  else
    die "pnpm not found (install pnpm or enable Corepack)"
  fi
  note "pnpm $(pnpm -v)"
}

# refresh_pkg <dir> <pnpm-script...>
refresh_pkg() {
  local dir=$1
  shift
  step 36 "Refreshing $dir"
  cd "$ROOT/$dir"
  if [[ "${CLEAN:-0}" == 1 ]]; then
    note "CLEAN=1 — removing node_modules"
    rm -rf node_modules
  fi
  pnpm update
  local script
  for script in "$@"; do
    note "pnpm $script"
    pnpm "$script"
  done
  cd "$ROOT"
}

need node
need npx
need python3
if [[ $WEB_ONLY -eq 0 ]]; then
  if command -v platformio >/dev/null 2>&1; then
    PIO=(platformio)
  elif command -v pio >/dev/null 2>&1; then
    PIO=(pio)
  else
    die "Missing required tool: platformio (or pio)"
  fi
fi

update_pnpm

# Lint before builds so ESLint failures don't wait on Vite.
refresh_pkg interface format lint build-webUI build-recovery
refresh_pkg mock-api format

if [[ $WEB_ONLY -eq 0 ]]; then
  step 33 "PlatformIO doc builds"
  if [[ "${CLEAN:-0}" == 1 ]]; then
    note "CLEAN=1 — cleaning build_modbus / build_standalone"
    "${PIO[@]}" run -e build_modbus -t clean
    "${PIO[@]}" run -e build_standalone -t clean
  fi
  "${PIO[@]}" run -e build_modbus
  "${PIO[@]}" run -e build_standalone

  step 33 "Native tests"
  "${PIO[@]}" run -e native-test -t exec
else
  step 33 "Skipping PlatformIO (--web-only)"
fi

# Last so typos don't skip firmware builds/tests; still fails the script if dirty.
step 33 "Spellcheck"
npx cspell "**"

step 32 "Done"
