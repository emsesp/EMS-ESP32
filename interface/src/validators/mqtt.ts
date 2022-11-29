import Schema from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from './shared';

export const MQTT_SETTINGS_VALIDATOR = new Schema({
  host: [{ required: true, message: 'Host is required' }, IP_OR_HOSTNAME_VALIDATOR],
  base: { required: true, message: 'Base is required' },
  port: [
    { required: true, message: 'Port is required' },
    { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
  ],
  keep_alive: [
    { required: true, message: 'Keep alive is required' },
    { type: 'number', min: 1, max: 86400, message: 'Keep alive must be between 1 and 86400' }
  ]
});
