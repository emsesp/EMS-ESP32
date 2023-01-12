import argparse
import requests
import time

# Setting go here
EMSESP_IP = "10.10.10.20" # IP or hostname.local of EMS-ESP
DEFAULT_TEST_NAME = "general" # default name of test
WAIT_TIME = 1 # in seconds

# fixed variables
INFO_URL = "http://" + EMSESP_IP + "/api/system/info"
TEST_URL = "http://" + EMSESP_IP + "/api?device=system&cmd=test&data="
GET_HEADERS = { 'Content-Type': 'application/json'}
TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0"
POST_HEADERS = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + TOKEN }

def run_test(test_name):

    # example for POSTs:
    #   BODY = json.dumps({ "value": 22.5 })
    #   response = requests.post(url, headers=HEADERS, data=BODY, verify=False)

    print()
    print("* Getting initial stats", end="")
    response = requests.get(INFO_URL, headers=GET_HEADERS, verify=False)
    uptime_a = response.json()['System Info']['uptime (seconds)']
    freemem_a = response.json()['System Info']['free mem']
    maxalloc_a = response.json()['System Info']['max alloc']
    print(" -> uptime is " + str(uptime_a) + " secs, Free mem/Max alloc is " + str(freemem_a) + "/" + str(maxalloc_a) )

    # run test
    print("* Running test '" + test_name + "'", end="")
    response = requests.get(TEST_URL + test_name, headers=GET_HEADERS, verify=False)
    test_output = response.json()['message']
    if (test_output != 'OK'):
        print(" -> Test Failed!")
        return

    print(" -> Test ran successfully, output: " + response.json()['message'])

    # wait 10 seconds
    print("* Waiting for " + str(WAIT_TIME) + " seconds...")
    time.sleep(WAIT_TIME)

    # get latest stats
    print("* Getting refreshed stats", end="")
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
parser = argparse.ArgumentParser()
parser.add_argument('test name', type=str, default=DEFAULT_TEST_NAME, help='Name of test to run')
args = parser.parse_args()
run_test(args.test)