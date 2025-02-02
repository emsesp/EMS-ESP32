#!/bin/sh

#
# Command line test for the API
#

emsesp_url="http://192.168.1.225"

# get the token from the Security page. This is the token for the admin user, unless changed it'll always be the same
emsesp_token="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWV9.2bHpWya2C7Q12WjNUBD6_7N3RCD7CMl-EGhyQVzFdDg"

# This example will get the system info, via a GET request
curl -X GET ${emsesp_url}/api/system/info
echo "\n"

# This example will execute a read command on product ID 8 and type ID 1
curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
    -d '{"data":"8 1"}' \
    ${emsesp_url}/api/system/read

echo "\n"

# This example will export all values to a json file, including custom entities, sensors and schedules
curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
    -d '{"action":"export", "param":"allvalues"}' \
    ${emsesp_url}/rest/action

echo "\n"

# This example is how to call a service in Home Assistant via the API
# Which can be added to an EMS-EPS schedule

ha_url="http://192.168.1.42:8123"
ha_token="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiIwMzMyZjU1MjhlZmM0NGIyOTgyMjIxNThiODU1NDkyNSIsImlhdCI6MTcyMTMwNDg2NSwiZXhwIjoyMDM2NjY0ODY1fQ.Q-Y7E_i7clH3ff4Ma-OMmhZfbN7aMi_CahKwmoar"

curl -X POST \
    ${ha_url}/api/services/script/test_notify \
    -H "Authorization: Bearer ${ha_token}" \
    -H "Content-Type: application/json"
