# Home Assistant Files

* <a href="integrations/homeassistant/automation.yaml">automation.yaml</a>
* <a href="integrations/homeassistant/binary_sensor.yaml">binary_sensor.yaml</a>
* <a href="integrations/homeassistant/climate.yaml">climate.yaml</a>
* <a href="integrations/homeassistant/customize.yaml">customize.yaml</a>
* <a href="integrations/homeassistant/notify.yaml">notify.yaml</a>
* <a href="integrations/homeassistant/script.yaml">script.yaml</a>
* <a href="integrations/homeassistant/sensor.yaml">sensor.yaml</a>
* <a href="integrations/homeassistant/switch.yaml">switch.yaml</a>
* <a href="integrations/homeassistant/ui-lovelace.yaml">ui-lovelace.yaml</a>

Make sure you include these YAML files in your `configuration.yaml` file, e.g.
```yaml
script: !include script.yaml
sensor: !include sensor.yaml
switch: !include switch.yaml
notify: !include notify.yaml
climate: !include climate.yaml
automation: !include automation.yaml
binary_sensor: !include binary_sensor.yaml
```