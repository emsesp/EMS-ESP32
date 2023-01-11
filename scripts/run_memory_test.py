import requests
import json
import time

WAIT_TIME = 10 # in seconds

# url = "http://ems-esp.local/api/thermostat/temp"
# token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ8.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjEuMWIwIn0.qeGT53Aom4rDYeIT1Pr4BSMdeWyf4_zN9ue2c51ZnM0"
# body = json.dumps({ "value": 22.5 })
# headers = { 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + token }
# response = requests.post(url, headers=headers, data=body, verify=False)

# curl http://10.10.10.20/api/system/info
response = requests.get("http://10.10.10.20/api/system/info", headers={ 'Content-Type': 'application/json'}, verify=False)

uptime_now = response.json()['System Info']['uptime (seconds)']

print("Before test: uptime=" + str(uptime_now) + " secs, Free mem/Max alloc= " + str(response.json()['System Info']['free mem']) + "/" + str(response.json()['System Info']['max alloc']) )

# run test
# curl http://10.10.10.20/api\?device\=system\&cmd\=test\&data\=memory
response = requests.get("http://10.10.10.20/api?device=system&cmd=test&data=memory", headers={ 'Content-Type': 'application/json'}, verify=False)
print("Running test, output: " + response.json()['message'])

# wait 10 seconds
print("Waiting " + str(WAIT_TIME) + " seconds...")
time.sleep(WAIT_TIME) 

response = requests.get("http://10.10.10.20/api/system/info", headers={ 'Content-Type': 'application/json'}, verify=False)

new_uptime = response.json()['System Info']['uptime (seconds)']

print(" After test: uptime=" + str(new_uptime) + " secs, Free mem/Max alloc= " + str(response.json()['System Info']['free mem']) + "/" + str(response.json()['System Info']['max alloc']) )

if (new_uptime < uptime_now):
    print("It crashed!")
else:
    print("Success!")

