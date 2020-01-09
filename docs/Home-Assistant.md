The files to read MQTT and parse as sensors and thermostat components into HA can be found [here](integrations/homeassistant/ha) along with a LoveLace dashboard which looks like:

![Home Assistant panel](_media/home%20assistant/ha.png ':size=500')

and the alerts on an iOS/Android device using PushBullet, PushOver or any notification service would look like:

![Home Assistant iPhone notify](_media/home%20assistant/ha_notify.jpg ':size=300')

## Supporting multiple thermostat heating circuits

Create a component for each, using the HC number appended to the end of the topics. For example

```yaml
- platform: mqtt
  name: Thermostat HC2
  modes:
    - "auto"
    - "heat"
    - "off"
  mode_command_topic: "home/ems-esp/thermostat_cmd_mode2"
  temperature_command_topic: "home/ems-esp/thermostat_cmd_temp2"
   
  mode_state_topic: "home/ems-esp/thermostat_data"
  current_temperature_topic: "home/ems-esp/thermostat_data"
  temperature_state_topic: "home/ems-esp/thermostat_data"

  mode_state_template: "{% if value_json.hc2.mode == 'manual' %} heat {% else %} {{value_json.hc2.mode}} {% endif %}"

  current_temperature_template: "{{ value_json.hc2.currtemp }}"
  temperature_state_template: "{{ value_json.hc2.seltemp }}"
   
  temp_step: 0.5

```

## Sending out a heartbeat

Thanks to @S-Przybylski for these additions.

`sensors.yaml`:

```yaml
###################################################
# EMS-ESP Statistics - heartbeat
###################################################
- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_wifi'
  unit_of_measurement: '%'
  value_template: '{{ value_json.rssid }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"

- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_load'
  unit_of_measurement: '%'
  value_template: '{{ value_json.load }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"

- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_freemem'
  unit_of_measurement: '%'
  value_template: '{{ value_json.freemem }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"

- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_uptime'
  unit_of_measurement: 's'
  value_template: '{{ value_json.uptime }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"

- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_version'
  value_template: '{{ value_json.version }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"

- platform: mqtt
  state_topic: 'home/ems-esp/heartbeat'
  name: 'ems_esp_mqtt_disconnects'
  value_template: '{{ value_json.MQTTdisconnects }}'
  qos: 1
  availability_topic: 'home/ems-esp/status'
  payload_available: "online"
  payload_not_available: "offline"
```

and in `customize.yaml`:

```yaml
sensor.ems_esp_wifi:
  icon: mdi:wifi
  friendly_name: 'Wifi strength'

sensor.ems_esp_load:
  icon: mdi:chart-line
  friendly_name: 'System load'

sensor.ems_esp_freemem:
  icon: mdi:memory
  friendly_name: 'Free Memory'

sensor.ems_esp_uptime:
  icon: mdi:update
  friendly_name: 'Uptime'

sensor.ems_esp_version:
  icon: mdi:mini-sd
  friendly_name: 'Firmare Version'

sensor.ems_esp_mqtt_disconnects:
  icon: mdi:chart-line
  friendly_name: 'MQTT disconnects'
```

## Activating one-time hot water charging DHW once (OneTimeWater) 

switch.yaml
```yaml
      one_time_water:
        friendly_name: OneTimeWater
        value_template: "{{ is_state('sensor.one_time_water', 'on') }}"
        turn_on:
          service: script.turn_on
          entity_id: script.one_time_water_on
        turn_off:
          service: script.turn_on
          entity_id: script.one_time_water_off
```

scripts.yaml
```yaml
  one_time_water_on:
    sequence:
      - service: mqtt.publish
        data_template:
          topic: 'home/ems-esp/boiler_cmd_wwonetime'
          payload: '1'
          
  one_time_water_off:
    sequence:
      - service: mqtt.publish
        data_template:
          topic: 'home/ems-esp/boiler_cmd_wwonetime'
          payload: '0'
```

sensors.yaml
```yaml
  - platform: mqtt
    state_topic: 'home/ems-esp/boiler_data'
    name: 'one_time_water'
    value_template: '{{ value_json.wWOnetime }}'
```
