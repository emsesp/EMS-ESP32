#!/bin/sh

# creates an CSV file called "dump_entities.cvs" with all devices and their entities
# run from top folder like `sh ./scripts/dump_entities.sh`
rm -f dump_entities.csv
make clean
make ARGS=-DEMSESP_STANDALONE_DUMP
echo "test entity_dump" | ./emsesp | python3 ./scripts/dump_entities.py > dump_entities.csv
cat dump_entities.csv