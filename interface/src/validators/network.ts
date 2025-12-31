import Schema from 'async-validator';
import type { NetworkSettingsType } from 'types';

import { HOSTNAME_VALIDATOR, IP_ADDRESS_VALIDATOR } from './shared';

// Reusable validator rules
const REQUIRED_IP_VALIDATOR = (fieldName: string) => [
  { required: true, message: `${fieldName} is required` },
  IP_ADDRESS_VALIDATOR
];

const OPTIONAL_IP_VALIDATOR = [IP_ADDRESS_VALIDATOR];

// Helper to create static IP validation rules
const createStaticIpRules = () => ({
  local_ip: REQUIRED_IP_VALIDATOR('Local IP'),
  gateway_ip: REQUIRED_IP_VALIDATOR('Gateway IP'),
  subnet_mask: REQUIRED_IP_VALIDATOR('Subnet mask'),
  dns_ip_1: OPTIONAL_IP_VALIDATOR,
  dns_ip_2: OPTIONAL_IP_VALIDATOR
});

export const createNetworkSettingsValidator = (
  networkSettings: NetworkSettingsType
) =>
  new Schema({
    ssid: [
      { type: 'string', max: 32, message: 'SSID must be 32 characters or less' }
    ],
    bssid: [
      {
        type: 'string',
        max: 17,
        message: 'BSSID must be 17 characters or empty'
      }
    ],
    password: [
      {
        type: 'string',
        max: 64,
        message: 'Password must be 64 characters or less'
      }
    ],
    hostname: [
      { required: true, message: 'Hostname is required' },
      HOSTNAME_VALIDATOR
    ],
    ...(networkSettings.static_ip_config && createStaticIpRules())
  });
