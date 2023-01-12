import argparse
import requests
import time
from termcolor import cprint

def print_success(x): return cprint(x, 'green')
def print_fail(x): return cprint(x, 'red')

def run_test(ip, wait, name, token):

    WAIT_REBOOT = 20
    BASE_URL = "http://" + str(ip)
    INFO_URL = BASE_URL + "/api/system/info"
    RESTART_URL = BASE_URL + "/api/system/restart"
    TEST_URL = BASE_URL + "/api?device=system&cmd=test&data=" + name
    GET_HEADERS = { 'Content-Type': 'application/json'}
    GET_HEADERS_SECURE = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + str(token) }
    # BODY = json.dumps({ "value": 22.5 })

    # Print welcome message
    print()
    print("Benchmarking EMS-ESP, memory profiling")
    print(" Base URL: " + BASE_URL)
    print(" Test Name: " + name)
    print(" 6 steps will run now:")
    print()

    # Restart EMS-ESP
    print("1. Doing a cold restart...", end="")
    response = requests.get(RESTART_URL, headers=GET_HEADERS_SECURE, verify=False)
    if (response.status_code != 200):
        print_fail("Failed.")
        return
    print_success("Success")

    # Wait for EMS-ESP to come back up and reconnect to WiFi
    print("2. Waiting " + str(WAIT_REBOOT) + " seconds for EMS-ESP to come back up...")
    time.sleep(WAIT_REBOOT)

    print("3. Getting initial memory stats", end="")
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_a = response.json()['System Info']['uptime (seconds)']
    freemem_a = response.json()['System Info']['free mem']
    maxalloc_a = response.json()['System Info']['max alloc']
    print(" -> uptime is " + str(uptime_a) + " secs, Free mem/Max alloc is " + str(freemem_a) + "/" + str(maxalloc_a) )

    # run test
    print("4. Running test", end="")
    response = requests.get(TEST_URL, headers=GET_HEADERS, verify=False)
    test_output = response.json()['message']
    if (test_output != 'OK'):
        print_fail(" -> Test Failed!")
        return
    print_success(" -> Test ran successfully")

    # wait 10 seconds
    print("5. Waiting for " + str(wait) + " seconds...")
    time.sleep(wait)

    # get latest stats
    print("6. Getting latest memory stats", end="")
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_b = response.json()['System Info']['uptime (seconds)']
    freemem_b = response.json()['System Info']['free mem']
    maxalloc_b = response.json()['System Info']['max alloc']
    print(" -> uptime is " + str(uptime_b) + " secs, Free mem/Max alloc is " + str(freemem_b) + "/" + str(maxalloc_b) )
    print()

    # check if it worked and report back
    if (uptime_b <= uptime_a):
        print(" Error! EMS-ESP crashed and restarted :-(")
    else:
        print_success("Success!")
        print("In the " + str(uptime_b - uptime_a) + " seconds, we have:")
        cprint("Free mem/Max alloc before=" + str(freemem_a) + "/" + str(maxalloc_a) +
              " after=" + str(freemem_b) + "/" + str(maxalloc_b) +
              " diff=" + str(freemem_a - freemem_b) + "/" + str(maxalloc_a - maxalloc_b), "cyan")

    # finish
    print()

# main
parser = argparse.ArgumentParser(description="Benchmark EMS-ESP, memory profiler")
parser.add_argument("-i", "--ip", metavar="IP", type=str, default="ems-esp.local", help="IP address of EMS-ESP")
parser.add_argument("-w", "--wait", metavar="WAIT", type=int, default="10", help="time to wait between test")
parser.add_argument("-n", "--name", metavar="NAME", type=str, default="general", help="Name of test to run")
parser.add_argument("-t", "--token", metavar="TOKEN", type=str, help="Bearer Token")
args = parser.parse_args()
run_test(**vars(args))
