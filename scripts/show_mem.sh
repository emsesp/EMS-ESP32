#!/bin/sh

echo -n "Arduino: "
curl -s http://10.10.10.93/api/system/system/arduino | jq -r '.value'

echo -n "SDK: "
curl -s http://10.10.10.93/api/system/system/sdk | jq -r '.value'

echo -n "Free heap: "
curl -s http://10.10.10.93/api/system/system/freeMem | jq -r '.value'

echo -n "Max Alloc: "
curl -s http://10.10.10.93/api/system/system/maxAlloc | jq -r '.value'
