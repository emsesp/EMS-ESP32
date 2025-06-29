#!/bin/sh

# run from root folder, like `sh ./scripts/update_all.sh`
# as well as GNUMake (make) and python3

cd interface
rm -rf node_modules
corepack use pnpm@latest-10
pnpm update
pnpm install
pnpm format
pnpm lint

cd ../mock-api
rm -rf node_modules
corepack use pnpm@latest-10
pnpm update
pnpm install
pnpm format

cd ..
cd interface
pnpm build
pnpm webUI

cd ..
npx cspell "**"

sh ./scripts/generate_csv_and_headers.sh
