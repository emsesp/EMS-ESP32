#!/bin/sh

# creates an CSV file called "dump_entities.cvs" with all devices and their entities
# run from top folder like `sh ./scripts/dump_entities.sh`
make clean; make ARGS=-DEMSESP_STANDALONE_DUMP; echo "test dump" | ./emsesp | python3 ./scripts/dump_entities.py > dump_entities.csv
