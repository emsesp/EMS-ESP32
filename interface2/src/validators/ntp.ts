import Schema from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from './shared';

export const NTP_SETTINGS_VALIDATOR = new Schema({
  server: [
    { required: true, message: "Server is required", },
    IP_OR_HOSTNAME_VALIDATOR
  ],
  tz_label: {
    required: true, message: "Time zone is required"
  }
});
