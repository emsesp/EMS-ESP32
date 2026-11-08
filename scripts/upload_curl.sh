#!/usr/bin/env bash
#
# EMS-ESP firmware upload via curl (same job as upload_cli.py).
#
# Usage:
#   ./scripts/upload_curl.sh -i 10.10.10.175 -f build/firmware/EMS-ESP-….bin
#   ./scripts/upload_curl.sh -i ems-esp.local -f firmware.bin -u admin -p admin
#
# Prefer a numeric IP over .local — mDNS (especially under WSL2) can stall on IPv6.
#
# Requires: curl, and either md5sum or openssl. python3 is used only to parse the
# sign-in JSON and (when needed) resolve a hostname to IPv4.

set -euo pipefail

FILE=""
IP="ems-esp.local"
USERNAME="admin"
PASSWORD="admin"

USER_AGENT='Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0'

green() { printf '\033[32m%s\033[0m\n' "$*"; }
red()   { printf '\033[31m%s\033[0m\n' "$*" >&2; }

usage() {
    cat <<EOF
Usage: $(basename "$0") -f FILE [-i IP] [-u USERNAME] [-p PASSWORD]

  -f, --file       firmware .bin file (required)
  -i, --ip         device address (default: ems-esp.local)
  -u, --username   admin user (default: admin)
  -p, --password   admin password (default: admin)
  -h, --help       show this help
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--file)     FILE="${2:-}"; shift 2 ;;
        -i|--ip)       IP="${2:-}"; shift 2 ;;
        -u|--username) USERNAME="${2:-}"; shift 2 ;;
        -p|--password) PASSWORD="${2:-}"; shift 2 ;;
        -h|--help)     usage; exit 0 ;;
        *) red "Unknown option: $1"; usage; exit 1 ;;
    esac
done

if [[ -z "$FILE" ]]; then
    red "Missing -f/--file"
    usage
    exit 1
fi

if [[ ! -f "$FILE" ]]; then
    red "File not found: $FILE"
    exit 1
fi

if ! command -v curl >/dev/null; then
    red "curl is required"
    exit 1
fi

if ! command -v python3 >/dev/null; then
    red "python3 is required (sign-in JSON / IPv4 resolve)"
    exit 1
fi

# Strip optional http:// and trailing slash / path
HOST="$IP"
HOST="${HOST#http://}"
HOST="${HOST#https://}"
HOST="${HOST%%/*}"
# Drop :port for resolve (device always serves HTTP on 80 for this flow)
HOST_NO_PORT="${HOST%%:*}"
if [[ "$HOST" == \[* ]]; then
    # literal IPv6 — leave as-is
    HOST_IP="$HOST"
else
    HOST_IP="$(python3 - "$HOST_NO_PORT" <<'PY'
import socket, sys
host = sys.argv[1]
try:
    socket.inet_pton(socket.AF_INET, host)
    print(host)
except OSError:
    infos = socket.getaddrinfo(host, 80, socket.AF_INET, socket.SOCK_STREAM)
    if not infos:
        sys.exit(f"No IPv4 address for {host}")
    print(infos[0][4][0])
PY
)"
fi

BASE="http://${HOST_IP}"
FILE_PATH="$(cd "$(dirname "$FILE")" && pwd)/$(basename "$FILE")"
FILE_NAME="$(basename "$FILE_PATH")"

echo
echo "EMS-ESP Firmware Upload (curl)"
if [[ "$HOST_IP" != "$HOST_NO_PORT" ]]; then
    green "Resolved ${HOST_NO_PORT} -> ${HOST_IP}"
fi

# --- sign in ---
SIGNIN_RESP="$(curl -sS -f \
    -H "Host: ${HOST_IP}" \
    -H "User-Agent: ${USER_AGENT}" \
    -H "Accept: */*" \
    -H "Content-Type: application/json" \
    -H "Referer: ${BASE}" \
    -d "{\"username\":\"${USERNAME}\",\"password\":\"${PASSWORD}\"}" \
    "${BASE}/rest/signIn")" || {
    red "Authentication failed"
    exit 1
}

TOKEN="$(python3 -c 'import json,sys; print(json.load(sys.stdin)["access_token"])' <<<"$SIGNIN_RESP")" || {
    red "Authentication failed (no access_token)"
    exit 1
}
green "Authentication successful"

# MD5 of the firmware — upload as a .md5 file first (same as the WebUI / upload_cli.py).
if command -v md5sum >/dev/null; then
    MD5="$(md5sum "$FILE_PATH" | awk '{print $1}')"
elif command -v openssl >/dev/null; then
    MD5="$(openssl md5 -r "$FILE_PATH" | awk '{print $1}')"
else
    red "Need md5sum or openssl to compute the digest"
    exit 1
fi

MD5_TMP="$(mktemp "${TMPDIR:-/tmp}/EMS-ESP.XXXXXX.md5")"
RESP_TMP="$(mktemp "${TMPDIR:-/tmp}/EMS-ESP.XXXXXX.rsp")"
trap 'rm -f "$MD5_TMP" "$RESP_TMP"' EXIT
printf '%s\n' "$MD5" >"$MD5_TMP"

AUTH_HDR=( -H "Authorization: Bearer ${TOKEN}" )
COMMON_HDR=(
    -H "Host: ${HOST_IP}"
    -H "User-Agent: ${USER_AGENT}"
    -H "Accept: */*"
    -H "Origin: ${BASE}"
    -H "Referer: ${BASE}"
)

echo "Uploading MD5 (${MD5})"
MD5_HTTP="$(curl -sS -o "$RESP_TMP" -w '%{http_code}' \
    "${COMMON_HDR[@]}" "${AUTH_HDR[@]}" \
    -F "file=@${MD5_TMP};filename=${FILE_NAME%.bin}.md5;type=application/octet-stream" \
    "${BASE}/rest/uploadFile")" || true
MD5_BODY="$(cat "$RESP_TMP" 2>/dev/null || true)"

if [[ "$MD5_HTTP" != "200" ]]; then
    red "MD5 upload failed (HTTP ${MD5_HTTP}${MD5_BODY:+: $MD5_BODY})"
    exit 1
fi
green "MD5 accepted"

echo "Uploading firmware (${FILE_NAME})"
START_NS="$(date +%s%N 2>/dev/null || echo 0)"
BIN_HTTP="$(curl -sS --progress-bar -o "$RESP_TMP" -w '%{http_code}' \
    "${COMMON_HDR[@]}" "${AUTH_HDR[@]}" \
    -F "file=@${FILE_PATH};filename=${FILE_NAME};type=application/octet-stream" \
    "${BASE}/rest/uploadFile")" || true
BIN_BODY="$(cat "$RESP_TMP" 2>/dev/null || true)"

if [[ "$BIN_HTTP" != "200" ]]; then
    red "Upload failed (HTTP ${BIN_HTTP}${BIN_BODY:+: $BIN_BODY})"
    exit 1
fi

END_NS="$(date +%s%N 2>/dev/null || echo 0)"
SIZE="$(wc -c <"$FILE_PATH" | tr -d ' ')"
if [[ "$START_NS" != "0" && "$END_NS" != "0" ]]; then
    ELAPSED_MS=$(( (END_NS - START_NS) / 1000000 ))
    if [[ "$ELAPSED_MS" -gt 0 ]]; then
        KIBS=$(( SIZE * 1000 / ELAPSED_MS / 1024 ))
        ELAPSED_S="$(python3 -c "print(f'{${ELAPSED_MS}/1000:.1f}')")"
        green "Upload successful (${KIBS} KiB/s in ${ELAPSED_S}s). Rebooting device."
    else
        green "Upload successful. Rebooting device."
    fi
else
    green "Upload successful. Rebooting device."
fi

# Device may drop the connection as it reboots — ignore curl failures here.
curl -sS -m 10 \
    "${COMMON_HDR[@]}" "${AUTH_HDR[@]}" \
    -H "Content-Type: application/json" \
    "${BASE}/api/system/restart" >/dev/null 2>&1 || true

echo
