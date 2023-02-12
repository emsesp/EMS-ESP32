(Based on <https://github.com/emsesp/EMS-ESP32/issues/41>)

When developing and testing the web interface, it's handy not to bother with re-flashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to the REST (HTTP POST & GET) and WebSocket calls.

To use first make sure you have nodejs installed (>v18) then install yarn (`npm install -g yarn`). Now type `yarn` from this `mock-api` folder. To run EMS-ESP's WebUI navigate up to the `interface` folder and type `yarn run standalone`. This will start the mock API server on port 3080 using static dummy data from `mock-api/server.js` and also the web server which is at <http://localhost:3000>.
