#!/usr/bin/env bash
#
# Command line test for the API
#

emsesp_url="http://ems-espT.local"

# get the token from the Security page. This is the token for the admin user, unless changed it'll always be the same
emsesp_token="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWV9.2bHpWya2C7Q12WjNUBD6_7N3RCD7CMl-EGhyQVzFdDg"

# This example will get the system info, via a GET request
test0() {
    curl -X GET ${emsesp_url}/api/system/info
}

# This example will execute a read command on product ID 8 and type ID 1
test1() {
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_token}" \
        -H "Content-Type: application/json" \
        -d '{"data":"8 1"}' \
        ${emsesp_url}/api/system/read
}

# This example will use the message command to get the locale (en)
test2() {
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_token}" \
        -H "Content-Type: application/json" \
        -d '{"value":"system/settings/locale"}' \
        ${emsesp_url}/api/system/message
}

# This example will export all values to a json file, including custom entities, sensors and schedules
test3() {
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_token}" \
        -H "Content-Type: application/json" \
        -d '{"action":"export", "param":"allvalues"}' \
        ${emsesp_url}/rest/action
}

# Get all versions
test4() {
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
        -d '{"action":"getVersions"}' \
        ${emsesp_url}/rest/action
}

# led tests
test5() {
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
    -d '{"data":"red:blink1"}' \
    ${emsesp_url}/api/system/led
}

test6() {
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
    -d '{"data":"rgb"}' \
    ${emsesp_url}/api/system/led
}

# Create a list of test function names to run
tests_to_run=(
    # test0
    # test1
    # test2
    # test3
    # test4
    test5
    test6
)

for test_func in "${tests_to_run[@]}"; do
    echo "Running $test_func..."
    $test_func
    echo ""
done


# This example is how to call a service in Home Assistant via the API
# Which can be added to an EMS-EPS schedule
#
# ha_url="http://192.168.1.86:8123"
# ha_token="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiIwMzMyZjU1MjhlZmM0NGIyOTgyMjIxNThiODU1NDkyNSIsImlhdCI6MTcyMTMwNDg2NSwiZXhwIjoyMDM2NjY0ODY1fQ.Q-Y7E_i7clH3ff4Ma-OMmhZfbN7aMi_CahKwmoar"
#
# curl -X POST \
#     ${ha_url}/api/services/script/test_notify \
#     -H "Authorization: Bearer ${ha_token}" \
#     -H "Content-Type: application/json"
