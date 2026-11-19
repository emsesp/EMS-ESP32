import { isAPEnabled } from 'app/settings/APSettings';
import type { APSettingsType } from 'types';
import Schema from 'validators/schema';

import { vLL } from './messages';
import { IP_ADDRESS_VALIDATOR, required } from './shared';

const IP_FIELD_RULE = () => [required(), IP_ADDRESS_VALIDATOR];

const SSID_RULES = () => [
  required(),
  { type: 'string' as const, max: 32, message: vLL().V_MAX_CHARS({ max: 32 }) }
];

const PASSWORD_RULES = () => [
  required(),
  {
    type: 'string' as const,
    min: 8,
    max: 64,
    message: vLL().V_CHARS({ min: 8, max: 64 })
  }
];

const CHANNEL_RULES = () => [
  required(),
  {
    type: 'number' as const,
    min: 1,
    max: 14,
    message: vLL().V_BETWEEN({ min: 1, max: 14 })
  }
];

const MAX_CLIENTS_RULES = () => [
  required(),
  {
    type: 'number' as const,
    min: 1,
    max: 9,
    message: vLL().V_BETWEEN({ min: 1, max: 9 })
  }
];

export const createAPSettingsValidator = (apSettings: APSettingsType) =>
  new Schema({
    provision_mode: required(),
    ...(isAPEnabled(apSettings) && {
      ssid: SSID_RULES(),
      password: PASSWORD_RULES(),
      channel: CHANNEL_RULES(),
      max_clients: MAX_CLIENTS_RULES(),
      local_ip: IP_FIELD_RULE(),
      gateway_ip: IP_FIELD_RULE(),
      subnet_mask: IP_FIELD_RULE()
    })
  });
