# see run_memory_test.sh for usage

import argparse
import requests
import time
from timeit import default_timer as timer
import platform    # For getting the operating system type
import subprocess  # For executing a shell command
from termcolor import cprint


def print_success(x): return cprint(x, 'green')
def print_fail(x): return cprint(x, 'red')


def ping_until_up(ip, text):
    print(text + "...", flush=True, end="")
    time.sleep(1)
    param = '-n' if platform.system().lower() == 'windows' else '-c'
    command = ["ping", param, "2", ip]
    while True:
        if (subprocess.run(args=command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0):
            print_success("Connected")
            time.sleep(1)
            return
        print(".", flush=True, end="")
        time.sleep(1)


def run_test(skip, ip, wait, name, count, url, token):
    BASE_URL = "http://" + str(ip)

    INFO_URL = BASE_URL + "/api/system/info"
    RESTART_URL = BASE_URL + "/api/system/restart"
    SYSTEMSTATUS_URL = BASE_URL + "/rest/systemStatus"
    TEST_URL = BASE_URL + "/api?device=system&cmd=test&data=" + name

    GET_HEADERS = {'Content-Type': 'application/json'}
    GET_HEADERS_SECURE = {'Content-Type': 'application/json',
                          'Authorization': 'Bearer ' + str(token)}
    # BODY = json.dumps({ "value": 22.5 })

    start = timer()

    # Print welcome message
    print()
    print("Benchmarking EMS-ESP, memory profiling")
    print(" Base URL: " + BASE_URL)
    print(" Test Name: " + name)
    print()
    end = timer()

# set this to True if you want to use the test API
    using_test = False

    if not skip:

        # check if IP exists
        ping_until_up(ip, "(" + str(round(end - start, 1)) +
                      ")\t1. Checking if EMS-ESP is reachable")
        end = timer()

        # check if it has been compiled with test
        response = requests.get(
            SYSTEMSTATUS_URL, headers=GET_HEADERS, verify=False)
        build_flags = response.json()['build_flags']
        # see if build_flags contains "TEST"
        if "TEST" not in build_flags:
            print_fail("Error! EMS-ESP not compiled with -DTEST flag")
            exit(1)

        # Restart EMS-ESP
        print("(" + str(round(end - start, 1)) +
              ")\t2. Doing a cold restart...", end="")
        response = requests.get(
            RESTART_URL, headers=GET_HEADERS_SECURE, verify=False)
        if (response.status_code != 200):
            print_fail("Failed")
            return
        print_success("Success")
        end = timer()

        # Wait for EMS-ESP to come back up and reconnect to WiFi
        ping_until_up(ip, "(" + str(round(end - start, 1)) +
                      ")\t3. Waiting for EMS-ESP to come back online")
        end = timer()

        print("(" + str(round(end - start, 1)) +
              ")\t4. Getting initial memory stats...", flush=True, end="")
        time.sleep(1)
        response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
        uptime_a = response.json()['system']['uptimeSec']
        freemem_a = response.json()['system']['freeMem']
        maxalloc_a = response.json()['system']['maxAlloc']
        print_success("Uptime is " + str(uptime_a) +
                      " secs, Free mem/Max alloc before=" + str(freemem_a) + "/" + str(maxalloc_a))
        end = timer()

    # run test count times
    for i in range(count):

        print("(" + str(round(end - start, 1)) +
              ")\t5. Running test (count #" + str(i+1) + " of " + str(count)+")...", end="")

        # check if name start with a /
        if name[0] == "/":
            # use URL
            response = requests.get(
                BASE_URL + url, headers=GET_HEADERS, verify=False)
            print("Response: ", response.json())
        else:
            # run a named test
            response = requests.get(
                TEST_URL, headers=GET_HEADERS, verify=False)

        if (response.status_code != 200):
            print_fail("Test Failed!")
            return

        print_success("Test ran successfully")
        end = timer()

        # wait n seconds
        print("(" + str(round(end - start, 1)) + ")\t6. Waiting for " +
              str(wait) + " seconds...", flush=True, end="")
        time.sleep(wait)
        print_success("Done")
        end = timer()

        # get latest stats
        print("(" + str(round(end - start, 1)) +
              ")\t7. Getting latest memory stats...", end="")
        response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
        uptime_b = response.json()['system']['uptimeSec']
        freemem_b = response.json()['system']['freeMem']
        maxalloc_b = response.json()['system']['maxAlloc']
        print_success("Uptime is " + str(uptime_b) +
                      " secs, Free mem/Max alloc after=" + str(freemem_b) + "/" + str(maxalloc_b))
        print()

    if not skip:
        # check if it worked and report back
        if (uptime_b <= uptime_a):
            print_fail("Error! EMS-ESP crashed and restarted :-(")
        else:
            print("In the " + str(uptime_b - uptime_a) +
                  " seconds elapsed, we have Free mem/Max alloc: ", end="")
            cprint("before=" + str(freemem_a) + "/" + str(maxalloc_a) +
                   " after=" + str(freemem_b) + "/" + str(maxalloc_b) +
                   " diff=" + str(freemem_a - freemem_b) + "/" + str(maxalloc_a - maxalloc_b), "cyan", attrs=["bold"])

    # finish
    print()


# main
parser = argparse.ArgumentParser(
    description="Benchmark EMS-ESP, memory profiler")
parser.add_argument("-s", "--skip", metavar="SKIP", type=bool,
                    default=False, help="skip all checks and just run the test")
parser.add_argument("-i", "--ip", metavar="IP", type=str,
                    default="ems-esp.local", help="IP address of EMS-ESP")
parser.add_argument("-w", "--wait", metavar="WAIT", type=int,
                    default="10", help="time to wait between test")
parser.add_argument("-n", "--name", metavar="NAME", type=str,
                    default="memory", help="Name of test to run")
parser.add_argument("-c", "--count", metavar="COUNT", type=int,
                    default="1", help="number of times to run the test")
parser.add_argument("-u", "--url", metavar="URL", type=str,
                    help="custom URL")
parser.add_argument("-t", "--token", metavar="TOKEN", type=str,
                    default="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWV9.2bHpWya2C7Q12WjNUBD6_7N3RCD7CMl-EGhyQVzFdDg", help="Bearer Token")
args = parser.parse_args()
run_test(**vars(args))
