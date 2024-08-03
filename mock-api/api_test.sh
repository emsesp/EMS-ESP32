#!/bin/sh

# API

curl -X GET http://ems-esp.local/api/system/info
echo "\n"

# HA

ha_url="http://192.168.1.42:8123"
ha_token="eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiIwMzMyZjU1MjhlZmM0NGIyOTgyMjIxNThiODU1NDkyNSIsImlhdCI6MTcyMTMwNDg2NSwiZXhwIjoyMDM2NjY0ODY1fQ.Q-Y7E_i7clH3ff4Ma-OMmhZfbN7aMi_CahKwmoar"

curl -X POST \
    ${ha_url}/api/services/script/test_notify \
    -H "Authorization: Bearer ${ha_token}" \
    -H "Content-Type: application/json" \
