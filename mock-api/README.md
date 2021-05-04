
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

## Notes

- It's for local development only
- `src/.env.development` is no longer required
- CORS is removed, also the build flag
- new file `interface/src/setupProxy.js`
- new files `mock-api/server.js` with the hardcoded data. Requires its own npm packages for express

## ToDo
- add filter rule to prevent from exposing yourself to malicious attacks when running the dev server(https://medium.com/webpack/webpack-dev-server-middleware-security-issues-1489d950874a)
