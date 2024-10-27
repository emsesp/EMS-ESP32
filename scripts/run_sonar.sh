#!/bin/sh
# This is an example file to run sonar from a Linux command line
#
# Make sure Sonar CLI is installed (https://docs.sonarsource.com/sonarqube/latest/analyzing-source-code/scanners/sonarscanner/)
# Download Solar Scanner from https://github.com/SonarSource/sonar-scanner-cli/releases
# or https://binaries.sonarsource.com/?prefix=Distribution/sonar-scanner-cli/
#
# Run from the root of the project like `./scripts/run_sonar.sh`
# Follow the setup in Sonar for your project and make sure chmod 755 +x on all the files
# and make sure you set the token in the shell like export SONAR_TOKEN="xxxxx"

make clean
~/sonar/build-wrapper-linux-x86-64 --out-dir bw-output make all

~/sonar/sonar-scanner/bin/sonar-scanner \
  -Dsonar.organization=proddy \
  -Dsonar.projectKey=proddy_EMS-ESP32 \
  -Dsonar.projectName=EMS-ESP32 \
  -Dsonar.sources="./src, ./lib/framework" \
  -Dsonar.cfamily.compile-commands=compile_commands.json \
  -Dsonar.host.url=https://sonarcloud.io
