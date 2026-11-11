#!/usr/bin/env bash
#
# Command line test for the API
#

emsesp_url="http://ems-espT.local"

# every /api call needs an API key, create a "Read and Write" one in Security > API Keys
emsesp_key="emsesp_..."

# /rest needs a WebUI sign-in instead, the token is fetched by signin below
emsesp_user="admin"
emsesp_password="..."

signin() {
    emsesp_token=$(curl -s -X POST \
        -H "Content-Type: application/json" \
        -d "{\"username\":\"${emsesp_user}\",\"password\":\"${emsesp_password}\"}" \
        ${emsesp_url}/rest/signIn | sed -n 's/.*"access_token":"\([^"]*\)".*/\1/p')
    if [ -z "${emsesp_token}" ]; then
        echo "sign-in failed"
        exit 1
    fi
}

# This example will get the system info, via a GET request
test0() {
    curl -X GET -H "Authorization: Bearer ${emsesp_key}" ${emsesp_url}/api/system/info
}

# This example will execute a read command on product ID 8 and type ID 1
test1() {
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_key}" \
        -H "Content-Type: application/json" \
        -d '{"data":"8 1"}' \
        ${emsesp_url}/api/system/read
}

# This example will use the message command to get the locale (en)
test2() {
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_key}" \
        -H "Content-Type: application/json" \
        -d '{"value":"system/settings/locale"}' \
        ${emsesp_url}/api/system/message
}

# This example will export all values to a json file, including custom entities, sensors and schedules
test3() {
    signin
    curl -X POST \
        -H "Authorization: Bearer ${emsesp_token}" \
        -H "Content-Type: application/json" \
        -d '{"action":"export", "param":"allvalues"}' \
        ${emsesp_url}/rest/action
}

# Get all versions
test4() {
    signin
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_token}" \
    -H "Content-Type: application/json" \
        -d '{"action":"getVersions"}' \
        ${emsesp_url}/rest/action
}

# led tests
test5() {
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_key}" \
    -H "Content-Type: application/json" \
    -d '{"data":"red:blink1"}' \
    ${emsesp_url}/api/system/led
}

test6() {
    curl -X POST \
    -H "Authorization: Bearer ${emsesp_key}" \
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
