#!/bin/bash

make clean
make ARGS=-DEMSESP_STANDALONE
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py | python3 ./scripts/generate-modbus-register-doc.py
