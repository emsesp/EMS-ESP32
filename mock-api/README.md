(Based on <https://github.com/emsesp/EMS-ESP32/issues/41>)

When developing and testing the web interface, it's handy not to bother with re-flashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to the REST (HTTP POST & GET) and WebSocket calls.

To set it up it do run `yarn install` in the `mock-api` folder, then from the `interface` folder run `yarn run standalone`. This will start the mock API server on port 3080 using data from `mock-api/server.js` and the web server runs on port 3000.
