#!/bin/sh

# creates an CSV file called "dump_telegrams.cvs" with all devices and their telegrams
# run from top folder like `sh ./scripts/dump_telegrams.sh`
rm -f dump_telegrams.csv
make clean
make ARGS=-DEMSESP_STANDALONE
echo "test telegram_dump" | ./emsesp | python3 ./scripts/strip_csv.py > dump_telegrams.csv
cat dump_telegrams.csv