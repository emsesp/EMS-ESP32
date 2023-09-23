#!/bin/sh
# This is an example file to run sonar from a Linux command line
# Run from the root of the project like `./scripts/run_sonar.sh`
# Follow the setup in Sonar for your prpject and make sure chmod 755 +x on all the files
# and make sure you set the token in the shell like export SONAR_TOKEN="eed5ee77e74042a275fed6f653c372452d5a21d0"

make clean
~/sonar/build-wrapper-linux-x86-64 --out-dir bw-output make all

~/sonar/sonar-scanner/bin/sonar-scanner \
  -Dsonar.organization=proddy \
  -Dsonar.projectKey=proddy_EMS-ESP32 \
  -Dsonar.projectName=EMS-ESP32 \
  -Dsonar.sources="./src, ./lib/framework" \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io
