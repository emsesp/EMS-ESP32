import Schema from 'async-validator';
import { HOSTNAME_VALIDATOR, IP_ADDRESS_VALIDATOR } from './shared';
import type { NetworkSettings } from 'types';

export const createNetworkSettingsValidator = (networkSettings: NetworkSettings) =>
  new Schema({
    ssid: [{ type: 'string', max: 32, message: 'SSID must be 32 characters or less' }],
    bssid: [{ type: 'string', max: 17, message: 'BSSID must be 17 characters or empty' }],
    password: { type: 'string', max: 64, message: 'Password must be 64 characters or less' },
    hostname: [{ required: true, message: 'Hostname is required' }, HOSTNAME_VALIDATOR],
    ...(networkSettings.static_ip_config && {
      local_ip: [{ required: true, message: 'Local IP is required' }, IP_ADDRESS_VALIDATOR],
      gateway_ip: [{ required: true, message: 'Gateway IP is required' }, IP_ADDRESS_VALIDATOR],
      subnet_mask: [{ required: true, message: 'Subnet mask is required' }, IP_ADDRESS_VALIDATOR],
      dns_ip_1: IP_ADDRESS_VALIDATOR,
      dns_ip_2: IP_ADDRESS_VALIDATOR
    }),
    tx_power: [
      { required: true, message: 'Tx Power is required' },
      { type: 'number', min: 0, max: 20, message: 'Tx Power must be between 0 and 20dBm' }
    ],
    channel: [
      { required: true, message: 'Channel is required' },
      { type: 'number', min: 0, max: 13, message: 'Channel must be between 0 and 13' }
    ]
  });
