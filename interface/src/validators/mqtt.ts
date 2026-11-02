import type { MqttSettingsType } from 'types';
import Schema from 'validators/schema';

import { IP_OR_HOSTNAME_VALIDATOR } from './shared';

// Constants for validation ranges
const PORT_MIN = 0;
const PORT_MAX = 65535;
const KEEP_ALIVE_MIN = 1;
const KEEP_ALIVE_MAX = 86400;
const HEARTBEAT_MIN = 10;
const HEARTBEAT_MAX = 86400;

// Reusable validator rules
const REQUIRED_HOST_VALIDATOR = [
  { required: true, message: 'Host is required' },
  IP_OR_HOSTNAME_VALIDATOR
];

const REQUIRED_BASE_VALIDATOR = [{ required: true, message: 'Base is required' }];

const PORT_VALIDATOR = [
  { required: true, message: 'Port is required' },
  {
    type: 'number' as const,
    min: PORT_MIN,
    max: PORT_MAX,
    message: `Port must be between ${PORT_MIN} and ${PORT_MAX}`
  }
];

// how MQTT over TLS checks the broker, see src/core/mqtt_tls.h. rootCA holds 'insecure', a fingerprint or a CA certificate
export type TlsCheck = 'none' | 'fingerprint' | 'ca';

const FINGERPRINT = /^[0-9a-f]{2}(:?[0-9a-f]{2}){19}$/i;

export const isFingerprint = (value: string) =>
  FINGERPRINT.test(value.replace(/\s/g, ''));

export const tlsCheckOf = (rootCA = ''): TlsCheck =>
  rootCA === '' || rootCA === 'insecure'
    ? 'none'
    : isFingerprint(rootCA)
      ? 'fingerprint'
      : 'ca';

const createNumberValidator = (fieldName: string, min: number, max: number) => [
  { required: true, message: `${fieldName} is required` },
  {
    type: 'number' as const,
    min,
    max,
    message: `${fieldName} must be between ${min} and ${max}`
  }
];

export const createMqttSettingsValidator = (
  mqttSettings: MqttSettingsType,
  tlsCheck: TlsCheck = 'none'
) =>
  new Schema({
    ...(mqttSettings.enabled &&
      mqttSettings.enableTLS &&
      tlsCheck === 'fingerprint' && {
        rootCA: [
          { required: true, message: 'Fingerprint is required' },
          {
            validator(
              _rule: unknown,
              value: string,
              callback: (error?: string) => void
            ) {
              callback(
                isFingerprint(value)
                  ? undefined
                  : 'Must be a SHA-1 fingerprint, 40 hex digits'
              );
            }
          }
        ]
      }),
    ...(mqttSettings.enabled &&
      mqttSettings.enableTLS &&
      tlsCheck === 'ca' && {
        rootCA: [{ required: true, message: 'Certificate is required' }]
      }),
    ...(mqttSettings.enabled && {
      host: REQUIRED_HOST_VALIDATOR,
      base: REQUIRED_BASE_VALIDATOR,
      port: PORT_VALIDATOR,
      keep_alive: createNumberValidator(
        'Keep alive',
        KEEP_ALIVE_MIN,
        KEEP_ALIVE_MAX
      ),
      publish_time_heartbeat: createNumberValidator(
        'Heartbeat',
        HEARTBEAT_MIN,
        HEARTBEAT_MAX
      )
    })
  });
