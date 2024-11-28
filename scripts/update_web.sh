#!/bin/sh

# run from root
# make sure ncu is installed globally (https://github.com/raineorshine/npm-check-updates)

cd interface
ncu -u
yarn set version stable
yarn
yarn format
yarn lint

cd ../mock-api
ncu -u
yarn set version stable
yarn
yarn format

cd ..