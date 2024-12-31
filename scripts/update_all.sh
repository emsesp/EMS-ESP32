#!/bin/sh

# run from root folder
# make sure ncu is installed globally (https://github.com/raineorshine/npm-check-updates)
# as well as GNUMake (make) and python3

cd interface
rm -rf yarn.lock node_modules
ncu -u
yarn set version stable
yarn
yarn format
yarn lint

cd ../mock-api
rm -rf yarn.lock node_modules
ncu -u
yarn set version stable
yarn
yarn format

cd ..
cd interface
yarn build; yarn webUI

cd ..
npx cspell "**" 

sh ./scripts/generate_csv_and_headers.sh
