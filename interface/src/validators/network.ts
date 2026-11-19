import type { NetworkSettingsType } from 'types';
import Schema from 'validators/schema';

import { vLL } from './messages';
import { HOSTNAME_VALIDATOR, IP_ADDRESS_VALIDATOR, required } from './shared';

const REQUIRED_IP_VALIDATOR = () => [required(), IP_ADDRESS_VALIDATOR];

const OPTIONAL_IP_VALIDATOR = [IP_ADDRESS_VALIDATOR];

// Helper to create static IP validation rules
const createStaticIpRules = () => ({
  local_ip: REQUIRED_IP_VALIDATOR(),
  gateway_ip: REQUIRED_IP_VALIDATOR(),
  subnet_mask: REQUIRED_IP_VALIDATOR()
});

export const createNetworkSettingsValidator = (
  networkSettings: NetworkSettingsType
) =>
  new Schema({
    ssid: [{ type: 'string', max: 32, message: vLL().V_MAX_CHARS({ max: 32 }) }],
    bssid: [
      {
        type: 'string',
        max: 17,
        message: vLL().V_MAX_CHARS({ max: 17 })
      }
    ],
    password: [
      {
        type: 'string',
        max: 64,
        message: vLL().V_MAX_CHARS({ max: 64 })
      }
    ],
    hostname: [required(), HOSTNAME_VALIDATOR],
    // DNS servers are optional and also apply as an override when using DHCP
    dns_ip_1: OPTIONAL_IP_VALIDATOR,
    dns_ip_2: OPTIONAL_IP_VALIDATOR,
    // "*" is still accepted for existing settings, the page warns about it
    ...(networkSettings.enableCORS && {
      CORSOrigin: [
        required(),
        {
          type: 'string',
          pattern: /^(\*|https?:\/\/[^\s/]+)$/,
          message: vLL().V_EXAMPLE({ example: 'http://homeassistant.local:8123' })
        }
      ]
    }),
    ...(networkSettings.static_ip_config && createStaticIpRules())
  });
