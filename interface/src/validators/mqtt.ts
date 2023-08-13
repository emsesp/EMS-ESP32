import Schema from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from './shared';
import type { MqttSettings } from 'types';

export const createMqttSettingsValidator = (mqttSettings: MqttSettings) =>
  new Schema({
    ...(mqttSettings.enabled && {
      host: [{ required: true, message: 'Host is required' }, IP_OR_HOSTNAME_VALIDATOR],
      base: { required: true, message: 'Base is required' },
      port: [
        { required: true, message: 'Port is required' },
        { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
      ],
      keep_alive: [
        { required: true, message: 'Keep alive is required' },
        { type: 'number', min: 1, max: 86400, message: 'Keep alive must be between 1 and 86400' }
      ],
      publish_time_heartbeat: [
        { required: true, message: 'Heartbeat is required' },
        { type: 'number', min: 10, max: 86400, message: 'Heartbeat must be between 10 and 86400' }
      ]
    })
  });
