#!/bin/sh

# make sure you add the soanr token here
# export SONAR_TOKEN=""

make clean
build-wrapper-linux-x86-64 --out-dir build_wrapper_output_directory make all
sonar-scanner
