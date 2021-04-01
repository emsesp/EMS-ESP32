
(https://github.com/emsesp/EMS-ESP32/issues/41)

When developing and testing the web interface, it's handy not to bother with reflashing an ESP32 each time. The idea is to mimic the ESP using a mock/stub server that responds to HTTP POST and GET requests. 

Currently a first draft at this is in the `ft_mockapi` branch. To run it do
```sh
% cd interface
% npm run dev
```

The hardcoded data is limited at the moment and can be easily extended by changing the file `/mock-api/server.js`
