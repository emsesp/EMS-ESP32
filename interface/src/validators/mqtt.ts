import type { MqttSettingsType } from 'types';
import Schema from 'validators/schema';

import { vLL } from './messages';
import { IP_OR_HOSTNAME_VALIDATOR, required } from './shared';

// Constants for validation ranges
const PORT_MIN = 0;
const PORT_MAX = 65535;
const KEEP_ALIVE_MIN = 1;
const KEEP_ALIVE_MAX = 86400;
const HEARTBEAT_MIN = 10;
const HEARTBEAT_MAX = 86400;

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

const createNumberValidator = (min: number, max: number) => [
  required(),
  {
    type: 'number' as const,
    min,
    max,
    message: vLL().V_BETWEEN({ min, max })
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
          required(),
          {
            validator(
              _rule: unknown,
              value: string,
              callback: (error?: string) => void
            ) {
              callback(isFingerprint(value) ? undefined : vLL().V_FINGERPRINT());
            }
          }
        ]
      }),
    ...(mqttSettings.enabled &&
      mqttSettings.enableTLS &&
      tlsCheck === 'ca' && {
        rootCA: [required()]
      }),
    ...(mqttSettings.enabled && {
      host: [required(), IP_OR_HOSTNAME_VALIDATOR],
      base: [required()],
      port: createNumberValidator(PORT_MIN, PORT_MAX),
      keep_alive: createNumberValidator(KEEP_ALIVE_MIN, KEEP_ALIVE_MAX),
      publish_time_heartbeat: createNumberValidator(HEARTBEAT_MIN, HEARTBEAT_MAX)
    })
  });
