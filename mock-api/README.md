When developing and testing the web interface, it's handy not to bother with re-flashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to the REST (HTTP POST & GET) and WebSocket calls.

# prerequisites

- Install the latest LTS of NodeJS
- Install yarn (`npm install -g yarn`)
- Install bun (<https://bun.sh/docs/installation>)
- type `yarn` from this `mock-api` folder to build

# To run

- `yarn standalone` from the main `interface` folder and then navigate to <http://localhost:3000>
