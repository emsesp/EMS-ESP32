# Notes on customizing the code


## **Basic Design Principles**

- The core services like telnet, logging and shell are based off the libraries from @nomis. I also adopted his general design pattens such as making everything as asynchronous as possible so that no one operation should starve another operation of it's time to execute (https://isocpp.org/wiki/faq/ctors#static-init-order).
- All EMS devices (e.g. boiler, thermostat, solar modules, mixing units etc) are derived from a factory base class and each class handles its own registering of telegram and mqtt handlers. This makes the EMS device code easier to manage and we can extend with new telegrams types and features.
- For debugging there is an offline mode where the code can be compiled and executed standalone without uploading to an ESP controller. Use `make` (based off GNUMakefile).

## **Customizing the Web UI**

The Web is based off Rick's awesome [esp8266-react](https://github.com/rjwats/esp8266-react/) framework. These are the files that are modified:

**`interface:`**
  * `.env` project name and project path to ems-esp
  * `.env.development` CORS URL
 
**`interface/public:`**
  * `app/manifest.json` ems-esp name
  * `index.html` ems-esp name
  * `app/icon.png` 256x256 PNG
  * `favicon.ico` replaced

**`interface/src:`**
  * `CustomMuiTheme.tsx` colors for dark mode
  * `interface/src/wifi/WifiSettingsController.tsx` rename esp8266-react

**`interface/src/project:`**
  * `ProjectRouting.tsx` removed demo, added paths to ems-esp/status, ems-esp/settings and *
  * `DemoProject.tsx` remove /demo/ and changed title, renamed to EMSESP.tsx
  * `ProjectMenu.tsx` title change, added /ems-esp/settings
  * `DemoInformation.tsx` removed file
  * `types.ts` add variables
  * added all custom files starting with EMSESP*

**`interface/src/mqtt:`**
  * `types.ts` added mqtt_fails
  * `MqttStatusForm.tsx` added MQTT Publish Errors
  * `MqttStatus.ts` added function mqttPublishHighlight
  * `MqttSettingsForm.tsx` added publish time, qos, format

**`interface/src/authentication:`**
  * `AuthenticationWrapper.tsx` commented out features.security because we added version
  * `AuthenticationContext.tsx` added version
  * `MqttSettingsForm.tsx` added publish time, qos, format

**`interface/src/components:`**
  * `MenuAppBar.tsx` added version to toolbar

**`interface/src/system:`**
  * `types.ts` added uptime and free_mem
  * `SystemStatusForm.tsx` added system uptime, % free mem

**`lib/framework`:**
  * `SystemStatus.h` added #include <LittleFS.h>, #include <uuid/log.h>, #include "../../src/system.h"
  * `SystemStatus.cpp` added LittleFS.info(fs_info); root["uptime"], root["free_mem"]
  * Commented out all `Serial.print`'s in all files
  * `MqttStatus.h` added #include "../../src/mqtt.h"
  * `MqttStatus.cpp` added root["mqtt_fails"]
  * `SecuritySettingsService.cpp` added version to the JWT payload
  * `SecuritySettingsService.h` #include "../../src/version.h"
  * `WiFiSettingsService.cpp` added WiFi.setOutputPower(20.0f), moved setHostname
  * `OTASettingsService.h` added #include "../../src/system.h" 
  * `OTASettingsService.cpp` added call to emsesp::System::upload_status(true)
  * `features.ini`: -D FT_NTP=0
  * `platformio.ini` using our own version
  * `factory_settings.ini` modified with `ems-esp-neo` as password and `ems-esp` everywhere else




