## MQTT configuration options

#### QOS
The QOS (Quality Of Service) is configurable within telnet (`mqtt_qos`) and WebUI. Valid options are At most once (0) At least once (1) and Exactly once (2). The default is 0.

#### Keep Alive
Likewise the Keep Alive (`mqtt_keepalive`) can be set. Keep alive ensures that the connection between the broker and client is still open and that the broker and the client are aware of being connected. This value is the number of seconds before giving up on a response and default set to 60 (1 minute). The lower the number the more WiFi traffic but greater the stability.

#### Retain
`mqtt_retain` sets the MQTT retain flag for all published messages on the broker. It's set to off as default.

[HiveMQ](https://www.hivemq.com/tags/mqtt-essentials/) has a good explanation of MQTT.

## MQTT topics

*all* topics are prefixed with `[base]`/[`host name`]/ where both `base` and `host name` are configurable in the Web UI.  The default is `home/ems-esp/`.

If you want to configure the topic names, these can be changed in the file `src/my_config.h`.

## Publishing Topics

All the key information from each of the EMS devices connected (Boilers, Thermostats, Solar Modules etc) are sent periodically on a single MQTT topic. The frequency of these publishes can be adjusted by the `publish_time` parameter which is set to 2 minutes (120 seconds) as default.

If the data in the payload has not changed since the last publish, it is ignored. Although from version 1.9.4 this can also be configured with the `publish_always` command and WebUI parameter. For example `set publish_always on` will always publish MQTT topics regardless if any of the data parameters have changed.

The table below shows examples of the topics being published:

| Topic               | Description         | Payload Example |
| ------------------- | ------------------- | --------------- |
| `sensors` | temperature readings from any external Dallas sensors attached to the ESP | `{"temp_1":"20.18","temp_2":"40.81"}` |
| `start` | this is the MQTT will and testament messages | `online` or `offline` |
| `heartbeat` | if enabled send out key system stats in JSON | `{"version":"1.9.3b3","IP":"10.10.10.198","rssid":68,"load":100,"uptime":4,"freemem":68,"MQTTdisconnects":0}` |
| `tapwater_active` | individual message if using a boolean sensor to show if hot tap water is running | `0` or `1` |
| `heating_active` | individual message if using a boolean sensor to show if heating is on | `0` or `1` |
| `boiler_data` | all the key data from the Boiler device | `{"wWComfort":"Hot","wWSelTemp":54,....` (see the example in the Monitoring MQTT section below) |
| `thermostat_data` | data from each of the Thermometer's Heating Circuits. Default HC is 1. The data sent depends on the model. | `{ "hc1":{"seltemp":15,"currtemp":20.6,"mode":"auto"}, "hc2":{"seltemp":8,"currtemp":18.2,"mode":"off"} }` |
| `mixing_data` | data from The Mixing Module for each of the Thermometer's Heating Circuits. | `{"hc1":{"flowTemp":55,"pumpMod":"1","valveStatus":"1"}}` |
| `shower_data` | the shower timer and alert toggles plus the duration of the last shower taken | `{"timer":"0","alert":"0","duration":"4 minutes 32 seconds"}` |
| `sm_data` | all data from the Solar Module (if connected) | JSON with `collectortemp` `bottomtemp` `pumpmodulation` `pump` `energylasthour` `energytoday` `energytotal` `pumpWorkMin` |
| `hp_data` | all data from the Heat Pump (if connected) | `{"pumpmodulation":"1", "pumpspeed": 20}` |

## Receiving Topics

EMS-ESP subscribes to the following topics used to send commands to the device:

| Topic               | Description         | Payload Format | Example |
| ------------------- | ------------------- | -------------- | ------- |
| `start` | this is the MQTT will and testament messages back from the broker | `online` or `offline` | `offline` |
| `restart` | restarts the ESP | | |
| `thermostat_cmd_temp<n>` | sets the thermostat current setpoint temperature to the Heating Circuit `n` (1-4). This is specific for the Home Assistant climate component | temperature value as a float | `20.8` |
| `thermostat_cmd_mode<n>` | sets the thermostat current mode to the Heating Circuit `n` (1-4). This is specific for the Home Assistant climate component | auto, day, night, off, manual | `auto` |
| `thermostat_cmd` | send a generic command to control the thermostat | JSON `{"cmd":<command>,"data":<value>` with `command` being `temp<hc>`, `mode<hc>`, `nightemp<hc>`, `daytemp<hc>` or `holidaytemp<hc>` with hc being empty or the heat controller (1-4) and `value` being a numeric value | `{ "cmd":"temp2", "data": 20 }` |
| `shower_data` | for setting the shower timer or alert toggle | JSON as `{"timer|alert":"0|1"}` | `{"timer":"1"}` |
| `generic_cmd` | for sending a command to the EMS-ESP, e.g. sending a shot of cold water | `coldshot` | `coldshot` |
| `boiler_cmd` | for sending generic command to control the Boiler | JSON `{"cmd":<command>,"data":<value>` with `command` being `comfort` and `data` = `[hot,comfort,intelligent]` or `flowtemp` with `data` being the desired temperature or `boiler_cmd_wwonetime` | `{"cmd":"flowtemp",data:55}` |
| `boiler_cmd_wwactivated` | for receiving boiler controls from a HA HVAC component | `"1"` or `"0"` | `"1"` |
| `boiler_cmd_wwtemp` | for receiving boiler temperature setting from a HA HVAC component | the temperature value | `60` |

## Monitoring MQTT

In Telnet the command `mqttlog` will show you the last messages published as well as which topics are being subscribed to. In the WebUI you also see real-time the last set of published messages in the System Status screen. Below is an example of the telnet command.

```
[telnet session]
mqttlog

MQTT publish log:
  Topic:start Payload:start
  Topic:heartbeat Payload:version=1.9.2b6, IP=10.10.10.198, rssid=72%, load=1%, uptime=364secs, freemem=52%
  Topic:boiler_data Payload:{"wWComfort":"Hot","wWSelTemp":54,"wWDesiredTemp":70,"selFlowTemp":5,"selBurnPow":0,"curBurnPow":0,"pumpMod":0,"wWCircPump":0,"wWCurTmp":36.7,"wWCurFlow":0,"curFlowTemp":43.4,"retTemp":41,"sysPress":1.7,"boilTemp":45.5,"wWActivated":"on","burnGas":"off","flameCurr":0,"heatPmp":"off","fanWork":"off","ignWork":"off","wWCirc":"off","heating_temp":75,"pump_mod_max":90,"pump_mod_min":60,"wWHeat":"on","wWStarts":206017,"wWWorkM":73890,"UBAuptime":3471966,"burnStarts":229866,"burnWorkMin":368989,"heatWorkMin":295099,"ServiceCode":"0H","ServiceCodeNumber":203}
  Topic:tapwater_active Payload:0
  Topic:heating_active Payload:0
  Topic:thermostat_data Payload:{"hc1":{"seltemp":15,"currtemp":20.6,"mode":"auto"}}

MQTT subscriptions:
  Topic:home/ems-esp/restart
  Topic:home/ems-esp/start
  Topic:home/ems-esp/thermostat_cmd_temp1
  Topic:home/ems-esp/thermostat_cmd_mode1
  Topic:home/ems-esp/thermostat_cmd_temp2
  Topic:home/ems-esp/thermostat_cmd_mode2
  Topic:home/ems-esp/thermostat_cmd_temp3
  Topic:home/ems-esp/thermostat_cmd_mode3
  Topic:home/ems-esp/thermostat_cmd_temp4
  Topic:home/ems-esp/thermostat_cmd_mode4
  Topic:home/ems-esp/thermostat_cmd
  Topic:home/ems-esp/boiler_cmd
  Topic:home/ems-esp/boiler_cmd_wwactivated
  Topic:home/ems-esp/boiler_cmd_wwtemp
  Topic:home/ems-esp/generic_cmd
  Topic:home/ems-esp/shower_data
```

If you want more precise monitoring of the MQTT traffic I suggest using [MQTT Explorer](http://mqtt-explorer.com/).
