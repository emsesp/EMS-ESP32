import argparse
import requests
import time

def run_test(ip, wait, name):

    BASE_URL = "http://" + str(ip)
    INFO_URL = BASE_URL + "/api/system/info"
    TEST_URL = BASE_URL + "/api?device=system&cmd=test&data=" + name
    GET_HEADERS = { 'Content-Type': 'application/json'}

    # example for POSTs:
    #   BODY = json.dumps({ "value": 22.5 })
    #   response = requests.post(url, headers=HEADERS, data=BODY, verify=False)
    #   TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0"
    #   POST_HEADERS = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + TOKEN }

    print()
    print("Benchmarking EMS-ESP")
    print(" Base URL: " + BASE_URL)
    print(" Test Name: " + name)
    print()

    print("1. Getting initial stats", end="")
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_a = response.json()['System Info']['uptime (seconds)']
    freemem_a = response.json()['System Info']['free mem']
    maxalloc_a = response.json()['System Info']['max alloc']
    print(" -> uptime is " + str(uptime_a) + " secs, Free mem/Max alloc is " + str(freemem_a) + "/" + str(maxalloc_a) )

    # run test
    print("2. Running test", end="")
    response = requests.get(TEST_URL, headers=GET_HEADERS, verify=False)
    test_output = response.json()['message']
    if (test_output != 'OK'):
        print(" -> Test Failed!")
        return

    print(" -> Test ran successfully, output: " + response.json()['message'])

    # wait 10 seconds
    print("3. Waiting for " + str(wait) + " seconds...")
    time.sleep(wait)

    # get latest stats
    print("4. Getting refreshed stats", end="")
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
        print(" Success! The delta's are: uptime " + str(uptime_b - uptime_a) + " secs, Free mem/Max alloc " + str(freemem_a - freemem_b) + "/" + str(maxalloc_a - maxalloc_b) )

    print()

# main
parser = argparse.ArgumentParser(description="Benchmark EMS-ESP, memory profiler")
parser.add_argument("-i", "--ip", metavar="IP", type=str, default="ems-esp.local", help="IP address of EMS-ESP")
parser.add_argument("-w", "--wait", metavar="WAIT", type=int, default="10", help="time to wait between test")
parser.add_argument("-n", "--name", metavar="NAME", type=str, default="general", help="Name of test to run")
args = parser.parse_args()
run_test(args.ip, args.wait, args.name)

