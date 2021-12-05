# Changes

These are the customized changes I made to esp8266-react Web interface for this EMS-ESP project (ESP32)

## files changed in interface

- `.env` - changed project names
- `config-overrides.js` - added conditional for hosted (https://ems-esp.derbyshire.nl/)
- `package.json` - added new scripts, changed proxy, added dev, added "@msgpack/msgpack" and some other things
- `public/index.html` - my own project name
- `public/app/manifest.json` - my own project name
- `public/app/icon.png` - my own icon
- `public/favicon.ico` - my own icon
- `src/CustomTheme.tsx` (mode: dark) added background, made font size smaller to 13, removed other colors
- `src/api/env.ts` - changed default project name
- `src/api/endpoints.ts` - added a AXIOS_API for /api and also AXIOS_BIN using msgpack decoding, also ES
- `src/components/layout/Layout.tsx` - changed DRAWER_WIDTH to 230
- `src/components/layout/LayoutAppBar.tsx` - changed DRAWER_WIDTH to 230, set backgroundColor and color to the sx
- `src/components/layout/LayoutMenuItem.tsx` - changed color of icons to grey
- `src/components/ButtonRow.tsx` - added top padding (mt) to give space
- `src/components/SectionContext.tsx` - added divider with title in center. Added a new props 'id' needed in SystemLog.tsx
- `src/components/MessageBox.tsx` - hardcoded to white. Warning showed as black and didn't look nice with dark theme
- `src/validators/network.ts` - added the extra fields I needed for Ethernet
- `src/validators/shared.ts` - added validator for IPv6 and IPv4
- `src/Project/ProjectRouting.tsx` - had to add fixed and wildcards for nav to work - need to double check if this is ok now
- `src/AppRouting.tsx` - renamed 'log in' to 'sign in' to be consistent
- `src/utils/useRest.ts` - changed 'Update successful' to 'Settings saved'. Added bool restartNeeded which is set when return from save is status code 201. Matches changes in the backend code, in HttpEndpoint.h and StatefulService.h
- `src/*` changed all Button variant="contained" to variant="outlined"
- `src/*` removed pt={2} from all ButtonRow's and added it to the ButtonRow component
- `src/*` removed ? from labels, e.g. "Enable MQTT?" to "Enable MQTT". Also made sure each command started with a verb

## new files added in interface

- `.env.hosted`
- `.env.production`
- `.env.development` was removed
- `.prettierrc`

## customized the features

- MQTT - additional fields for my EMS-ESP project
- Wifi/Network - renamed "wifi" to "network" everywhere, including the api endpoints to allow for Ethernet. Renamed Static IP to "Use Fixed IP address"
- System - added system log and version check, plus other params like uptime.

### to test locally using static data

```sh
cd mock-api; npm install
cd interface; npm run standalone
```
