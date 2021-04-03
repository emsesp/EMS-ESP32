
(https://github.com/emsesp/EMS-ESP32/issues/41)

When developing and testing the web interface, it's handy not to bother with re-flashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to the REST (HTTP POST & GET) and WebSocket calls. 

To set it up it do
```sh
% cd mock-api
% npm install
% cd interface
% npm install
```

and to run it
```sh
% cd interface
% npm run dev
```

The hardcoded data is limited at the moment and can be easily extended by changing the file `/mock-api/server.js`
