#!/usr/bin/env bash
# Install:
#  npm install -g autocannon
#  yarn global add autocannon
#
# or run https://github.com/nearform/autocannon-ui

TEST_IP="10.10.10.135"
TEST_TIME=60
LOG_FILE=http-loadtest.log
TIMEOUT=10000
PROTOCOL=http
#PROTOCOL=https

if test -f "$LOG_FILE"; then
  rm $LOG_FILE
fi

# for CONCURRENCY in 1 2 3 4 5 6 7 8 9 10 15 20
for CONCURRENCY in 1
#for CONCURRENCY in 20
do
  printf "\n\nCLIENTS: *** $CONCURRENCY ***\n\n" >> $LOG_FILE
  echo "Testing $CONCURRENCY clients on $PROTOCOL://$TEST_IP/"
  autocannon -c $CONCURRENCY -w 1 -d $TEST_TIME --renderStatusCodes "$PROTOCOL://$TEST_IP/" >> $LOG_FILE 2>&1
  printf "\n\n----------------\n\n" >> $LOG_FILE
  sleep 1

  echo "Testing $CONCURRENCY clients on $PROTOCOL://$TEST_IP/api/system/commands"
  autocannon -c $CONCURRENCY -w 1 -d $TEST_TIME --renderStatusCodes "$PROTOCOL://$TEST_IP/api/system/commands" >> $LOG_FILE 2>&1
  printf "\n\n----------------\n\n" >> $LOG_FILE
  sleep 1

  echo "Testing $CONCURRENCY clients on $PROTOCOL://$TEST_IP/app/icon.png"
  autocannon -c $CONCURRENCY -w 1 -d $TEST_TIME --renderStatusCodes "$PROTOCOL://$TEST_IP/app/icon.png" >> $LOG_FILE 2>&1
  printf "\n\n----------------\n\n" >> $LOG_FILE
  sleep 1
done