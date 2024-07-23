import { isAPEnabled } from 'app/settings/APSettings';
import Schema from 'async-validator';
import type { APSettingsType } from 'types';

import { IP_ADDRESS_VALIDATOR } from './shared';

export const createAPSettingsValidator = (apSettings: APSettingsType) =>
  new Schema({
    provision_mode: {
      required: true,
      message: 'Please provide a provision mode'
    },
    ...(isAPEnabled(apSettings) && {
      ssid: [
        { required: true, message: 'Please provide an SSID' },
        {
          type: 'string',
          max: 32,
          message: 'SSID must be 32 characters or less'
        }
      ],
      password: [
        { required: true, message: 'Please provide an access point password' },
        {
          type: 'string',
          min: 8,
          max: 64,
          message: 'Password must be 8-64 characters'
        }
      ],
      channel: [
        { required: true, message: 'Please provide a network channel' },
        { type: 'number', message: 'Channel must be between 1 and 14' }
      ],
      max_clients: [
        { required: true, message: 'Please specify a value for max clients' },
        {
          type: 'number',
          min: 1,
          max: 9,
          message: 'Max clients must be between 1 and 9'
        }
      ],
      local_ip: [
        { required: true, message: 'Local IP address is required' },
        IP_ADDRESS_VALIDATOR
      ],
      gateway_ip: [
        { required: true, message: 'Gateway IP address is required' },
        IP_ADDRESS_VALIDATOR
      ],
      subnet_mask: [
        { required: true, message: 'Subnet mask is required' },
        IP_ADDRESS_VALIDATOR
      ]
    })
  });
