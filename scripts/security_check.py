#!/usr/bin/env python3
"""Network checks of the 3.10.0 security changes against a running EMS-ESP.

Usage: python3 scripts/security_check.py <ip> <admin password>

Creates two temporary API keys and revokes them at the end. Only sends out-of-range writes, which are refused before
anything reaches the EMS bus, and writes syslog/enabled with its current value (off).
"""

import sys
import time

import requests

D, PW = sys.argv[1], sys.argv[2]
B = f"http://{D}"
results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(f"{'PASS' if ok else 'FAIL'}  {name}{'  (' + detail + ')' if detail else ''}")


def sign_in(password=PW):
    return requests.post(f"{B}/rest/signIn", json={"username": "admin", "password": password}, timeout=10)


def h(token, **extra):
    return {"Authorization": f"Bearer {token}", **extra}


def uptime(token):
    return requests.get(f"{B}/api/system/info", headers=h(token), timeout=10).json()["system"]["uptimeSec"]


T = sign_in().json()["access_token"]
start_uptime = uptime(T)

# password re-entry (#3371)
r = requests.post(f"{B}/rest/apiKeys", headers=h(T), json={"action": "create", "label": "netcheck-read"}, timeout=10)
check("API key create without password re-entry gets 428", r.status_code == 428, str(r.status_code))
r = requests.post(f"{B}/rest/confirmPassword", headers=h(T), json={"password": "wrong"}, timeout=10)
check("confirmPassword with a wrong password gets 403", r.status_code == 403, str(r.status_code))
r = requests.post(f"{B}/rest/confirmPassword", headers=h(T), json={"password": PW}, timeout=10)
check("confirmPassword with the right password gets 200", r.status_code == 200, str(r.status_code))

# API keys (#3354)
keys = {}
for label, write in (("netcheck-read", False), ("netcheck-rw", True)):
    r = requests.post(f"{B}/rest/apiKeys", headers=h(T), json={"action": "create", "label": label, "write": write}, timeout=10)
    keys[label] = r.json().get("key", "") if r.ok else ""
    check(f"create API key {label}", r.ok and keys[label].startswith("emsesp_"), str(r.status_code))
RK, WK = keys["netcheck-read"], keys["netcheck-rw"]

try:
    # /api access (#3343, #3354, #3360, #3363)
    r = requests.get(f"{B}/api/system/info", timeout=10)
    check("/api reads values without a token (as in 3.8)", r.status_code == 200, str(r.status_code))
    r = requests.post(f"{B}/api/system/syslog/enabled", json={"value": False}, timeout=10)
    check("/api write without a token is refused", r.status_code == 401, f"{r.status_code} {r.text[:60]}")
    r = requests.get(f"{B}/api/system/info", headers=h(RK), timeout=10)
    check("/api with a Read key reads", r.status_code == 200, str(r.status_code))
    r = requests.post(f"{B}/api/system/syslog/enabled?access_token={T}", json={"value": False}, timeout=10)
    check("admin token in the URL gives no write rights (401)", r.status_code == 401, str(r.status_code))
    r = requests.post(f"{B}/api/system/syslog/enabled", headers=h(RK), json={"value": False}, timeout=10)
    check("Read key can't change a setting", r.status_code in (400, 401, 403), f"{r.status_code} {r.text[:60]}")
    r = requests.post(f"{B}/api/system/syslog/enabled", headers=h(WK), json={"value": False}, timeout=10)
    check("Read and Write key can change a setting (to its current value)", r.status_code == 200, f"{r.status_code} {r.text[:60]}")
    r = requests.get(f"{B}/rest/settings", headers=h(WK), timeout=10)
    check("API keys don't work on /rest", r.status_code == 401, str(r.status_code))
    r = requests.get(f"{B}/api/commands/values", headers=h(RK), timeout=10)
    check("Read key can't read Command definitions", r.status_code in (401, 403), str(r.status_code))
    r = requests.get(f"{B}/api/system/info", headers=h(WK, Origin="http://evil.example"), timeout=10)
    check("/api from another web origin gets 403 (#3361)", r.status_code == 403, str(r.status_code))
    r = requests.get(f"{B}/api/system/info", headers=h(WK, Origin=f"http://{D}"), timeout=10)
    check("/api from the device's own origin works", r.status_code == 200, str(r.status_code))

    # out-of-range writes are refused before anything is sent (#3372)
    r = requests.post(f"{B}/api/boiler/selflowtemp", headers=h(WK), json={"value": 250}, timeout=10)
    check("boiler/selflowtemp 250 is refused as out of range", r.status_code != 200 and "range" in r.text, f"{r.status_code} {r.text[:80]}")

    # log stream (#3346)
    SSE = {"Accept": "text/event-stream"}
    r = requests.get(f"{B}/es/log", headers=SSE, timeout=5, stream=True)
    check("/es/log without a sign-in gets 401", r.status_code == 401, str(r.status_code))
    r.close()
    r = requests.get(f"{B}/es/log", headers=h(T, **SSE), timeout=5, stream=True)
    check("/es/log with a sign-in streams", r.status_code == 200, str(r.status_code))
    r.close()

    # web server (#3364, #3365)
    r = requests.get(f"{B}/", timeout=10)
    check("nosniff header", r.headers.get("X-Content-Type-Options") == "nosniff")
    check("Referrer-Policy no-referrer", r.headers.get("Referrer-Policy") == "no-referrer")
    check("Content-Security-Policy in the Web UI", "Content-Security-Policy" in r.text or "Content-Security-Policy" in r.headers)
    r = requests.get(f"{B}/", headers={"Host": "evil.example", "Sec-Fetch-Mode": "navigate"}, timeout=10)
    check("browser request with an unknown Host gets 403 (DNS rebinding)", r.status_code == 403, str(r.status_code))
    r = requests.get(f"{B}/", headers={"Host": "evil.example"}, timeout=10)
    check("non-browser request with an unknown Host still works", r.status_code == 200, str(r.status_code))

    # console (new)
    r = requests.post(f"{B}/rest/console", headers=h(T), json={"command": "show system"}, timeout=15)
    check("Web UI console runs 'show system' for an admin", r.ok and "3.10" in r.text, f"{r.status_code} {r.text[:60]!r}")
    r = requests.post(f"{B}/rest/console", json={"command": "show version"}, timeout=10)
    check("Web UI console without a sign-in gets 401", r.status_code == 401, str(r.status_code))
    r = requests.post(f"{B}/rest/console", headers=h(WK), json={"command": "show version"}, timeout=10)
    check("Web UI console with an API key gets 401", r.status_code == 401, str(r.status_code))

    # install from URL only from EMS-ESP releases (#3371)
    r = requests.post(f"{B}/rest/action", headers=h(T), json={"action": "uploadURL", "param": "https://example.com/fw.bin"}, timeout=10)
    check("install from a non-GitHub URL is refused", r.status_code == 400, f"{r.status_code} {r.text[:60]}")

    # firmware upload needs the password again on a new sign-in (#3371)
    T2 = sign_in().json()["access_token"]
    r = requests.post(f"{B}/rest/uploadFile", headers=h(T2), files={"file": ("x.bin", b"\xe9" + b"\x00" * 4095)}, timeout=20)
    check("firmware upload from a new sign-in without re-entry gets 428", r.status_code == 428, str(r.status_code))

    # inputs that used to crash the device (#3367, #3368)
    for value in (12345678901.5, -2147483648, "1 + )", "10 % 0", "9" * 400, "0x" + "f" * 300, "%n%n%n%s%s"):
        requests.post(f"{B}/api/system/message", headers=h(RK), json={"value": value}, timeout=10)
    r = requests.post(f"{B}/api/system/message", headers=h(WK, **{"Content-Type": "application/json"}),
                      data='{"value":"' + "a" * 20000 + '"}', timeout=10)
    check("JSON body over 16 KB gets 413", r.status_code == 413, str(r.status_code))
    time.sleep(2)
    now = uptime(T)
    check("no restart after the crash inputs", now >= start_uptime, f"uptime {start_uptime}s -> {now}s")

finally:
    # revoke the test keys
    r = requests.get(f"{B}/rest/apiKeys", headers=h(T), timeout=10)
    for k in r.json().get("keys", []):
        if k["label"].startswith("netcheck-"):
            rr = requests.post(f"{B}/rest/apiKeys", headers=h(T), json={"action": "revoke", "id": k["id"]}, timeout=10)
            print(f"revoked {k['label']}: {rr.status_code}")
    if WK:
        r = requests.post(f"{B}/api/system/syslog/enabled", headers=h(WK), json={"value": False}, timeout=10)
        check("a revoked Read and Write key can't write any more", r.status_code == 401, str(r.status_code))

print(f"\n{sum(results)}/{len(results)} passed")
