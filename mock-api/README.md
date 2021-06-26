(<https://github.com/emsesp/EMS-ESP32/issues/41>)

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
% npm run standalone
```

## Notes

- new file `interface/src/setupProxy.js`
- new files `mock-api/server.js` with the hardcoded data. Requires its own npm packages for express

## Testing

```bash
% curl -i http://localhost:3080/rest/emsespSettings
```

or from a browser use port 3000 since `setupProxy.js` is redirecting, like http://172.22.227.82:3000/rest/emsespSettings

http://172.22.227.82:3090/es/log?access_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjAuMmIwIn0.MsHSgoJKI1lyYz77EiT5ZN3ECMrb4mPv9FNy3udq0TU

Testing the EventSource/SSE use http://172.22.227.82:3090/es/log

## To Do

- add filter rule to prevent from exposing yourself to malicious attacks when running the dev server(<https://medium.com/webpack/webpack-dev-server-middleware-security-issues-1489d950874a>)
