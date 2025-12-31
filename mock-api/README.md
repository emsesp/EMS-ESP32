When developing and testing the web interface, it's handy not to bother with re-flashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to the REST (HTTP POST & GET) and WebSocket calls.

# prerequisites

- Install the latest LTS of NodeJS
- install corepack `npm install -g corepack@latest` and `orepack enable`
- Install pnpm (`corepack use pnpm@latest-10` on linux or `winget install -e --id pnpm.pnpm` on windows). See <https://pnpm.io/installation>
- Install bun (<https://bun.sh/docs/installation>)
- type `pnpm install` from this `mock-api` folder to build

# To run

- `pnpm standalone` from the main `interface` folder and then navigate to <http://localhost:3000>
