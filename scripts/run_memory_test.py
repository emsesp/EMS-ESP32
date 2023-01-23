# pre-reqs:
#  1) termcolor - install with "% python3 -m pip install --upgrade termcolor" (e.g. from PlatformIO Core CLI)
# Run with (example):
#  % python3 ./scripts/run_memory_test.py -i 10.10.10.20 -w 30 -t eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWV9.2bHpWya2C7Q12WjNUBD6_7N3RCD7CMl-EGhyQVzFdDg
#  Note the token is required for the Restart command

import argparse
import requests
import time
from timeit import default_timer as timer
import platform    # For getting the operating system name
import subprocess  # For executing a shell command
from termcolor import cprint

def print_success(x): return cprint(x, 'green')
def print_fail(x): return cprint(x, 'red')

def ping_until_up(ip, text):
    print(text + "...", flush=True, end="")
    param = '-n' if platform.system().lower()=='windows' else '-c'
    command = ["ping", param, "2", ip]
    while True:
        if (subprocess.run(args=command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0):
            print_success("Connected")
            return
        print(".", flush=True, end="")
        time.sleep(1)

def run_test(ip, wait, name, token):
    WAIT_REBOOT = 10
    BASE_URL = "http://" + str(ip)
    INFO_URL = BASE_URL + "/api/system/info"
    RESTART_URL = BASE_URL + "/api/system/restart"
    TEST_URL = BASE_URL + "/api?device=system&cmd=test&data=" + name
    GET_HEADERS = { 'Content-Type': 'application/json'}
    GET_HEADERS_SECURE = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + str(token) }
    # BODY = json.dumps({ "value": 22.5 })

    start = timer()

    # Print welcome message
    print()
    print("Benchmarking EMS-ESP, memory profiling")
    print(" Base URL: " + BASE_URL)
    print(" Test Name: " + name)
    print(" 7 steps will run now:")
    print()
    end = timer()

    # check if IP exists
    ping_until_up(ip, "(" + str(round(end - start, 1)) + ")  1. Checking if EMS-ESP is reachable")
    end = timer()

    # Restart EMS-ESP
    print("(" + str(round(end - start, 1)) + ")  2. Doing a cold restart...", end="")
    response = requests.get(RESTART_URL, headers=GET_HEADERS_SECURE, verify=False)
    if (response.status_code != 200):
        print_fail("Failed")
        return
    print_success("Success")
    end = timer()

    # Wait for EMS-ESP to come back up and reconnect to WiFi
    ping_until_up(ip, "(" + str(round(end - start, 1)) + ")  3. Waiting for EMS-ESP to come back online")
    end = timer()

    print("(" + str(round(end - start, 1)) + ")  4. Getting initial memory stats...", flush=True, end="")
    time.sleep(1)
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_a = response.json()['System Info']['uptime (seconds)']
    freemem_a = response.json()['System Info']['free mem']
    maxalloc_a = response.json()['System Info']['max alloc']
    print_success("Uptime is " + str(uptime_a) + " secs, Free mem/Max alloc is " + str(freemem_a) + "/" + str(maxalloc_a) )
    end = timer()

    # run test
    print("(" + str(round(end - start, 1)) + ")  5. Running test called '" + name + "'...", end="")
    response = requests.get(TEST_URL, headers=GET_HEADERS, verify=False)
    test_output = response.json()['message']
    if (test_output != 'OK'):
        print_fail("Test Failed!")
        return
    print_success("Test ran successfully")
    end = timer()

    # wait n seconds
    print("(" + str(round(end - start, 1)) + ")  6. Waiting for " + str(wait) + " seconds...", flush=True, end="")
    time.sleep(wait)
    print_success("Done")
    end = timer()

    # get latest stats
    print("(" +  str(round(end - start, 1)) + ") 7. Getting latest memory stats...", end="")
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_b = response.json()['System Info']['uptime (seconds)']
    freemem_b = response.json()['System Info']['free mem']
    maxalloc_b = response.json()['System Info']['max alloc']
    print_success("Uptime is " + str(uptime_b) + " secs, Free mem/Max alloc is " + str(freemem_b) + "/" + str(maxalloc_b) )
    print()

    # check if it worked and report back
    if (uptime_b <= uptime_a):
        print(" Error! EMS-ESP crashed and restarted :-(")
    else:
        print("In the " + str(uptime_b - uptime_a) + " seconds elasped, we have Free mem/Max alloc: ", end="")
        cprint("before=" + str(freemem_a) + "/" + str(maxalloc_a) +
              " after=" + str(freemem_b) + "/" + str(maxalloc_b) +
              " diff=" + str(freemem_a - freemem_b) + "/" + str(maxalloc_a - maxalloc_b), "cyan")

    # finish
    print()

# main
parser = argparse.ArgumentParser(description="Benchmark EMS-ESP, memory profiler")
parser.add_argument("-i", "--ip", metavar="IP", type=str, default="ems-esp.local", help="IP address of EMS-ESP")
parser.add_argument("-w", "--wait", metavar="WAIT", type=int, default="10", help="time to wait between test")
parser.add_argument("-n", "--name", metavar="NAME", type=str, default="memory", help="Name of test to run")
parser.add_argument("-t", "--token", metavar="TOKEN", type=str, help="Bearer Token")
args = parser.parse_args()
run_test(**vars(args))
