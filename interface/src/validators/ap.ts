import { isAPEnabled } from 'app/settings/APSettings';
import Schema from 'async-validator';
import type { APSettingsType } from 'types';

import { IP_ADDRESS_VALIDATOR } from './shared';

// Reusable validation rules
const IP_FIELD_RULE = (fieldName: string) => [
  { required: true, message: `${fieldName} is required` },
  IP_ADDRESS_VALIDATOR
];

const SSID_RULES = [
  { required: true, message: 'Please provide an SSID' },
  { type: 'string' as const, max: 32, message: 'SSID must be 32 characters or less' }
];

const PASSWORD_RULES = [
  { required: true, message: 'Please provide an access point password' },
  {
    type: 'string' as const,
    min: 8,
    max: 64,
    message: 'Password must be 8-64 characters'
  }
];

const CHANNEL_RULES = [
  { required: true, message: 'Please provide a network channel' },
  { type: 'number' as const, message: 'Channel must be between 1 and 14' }
];

const MAX_CLIENTS_RULES = [
  { required: true, message: 'Please specify a value for max clients' },
  {
    type: 'number' as const,
    min: 1,
    max: 9,
    message: 'Max clients must be between 1 and 9'
  }
];

export const createAPSettingsValidator = (apSettings: APSettingsType) =>
  new Schema({
    provision_mode: {
      required: true,
      message: 'Please provide a provision mode'
    },
    ...(isAPEnabled(apSettings) && {
      ssid: SSID_RULES,
      password: PASSWORD_RULES,
      channel: CHANNEL_RULES,
      max_clients: MAX_CLIENTS_RULES,
      local_ip: IP_FIELD_RULE('Local IP address'),
      gateway_ip: IP_FIELD_RULE('Gateway IP address'),
      subnet_mask: IP_FIELD_RULE('Subnet mask')
    })
  });
