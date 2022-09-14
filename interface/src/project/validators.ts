import Schema, { InternalRuleItem } from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from '../validators/shared';
import { Settings } from './types';

export const GPIO_VALIDATOR = {
  validator(rule: InternalRuleItem, value: number, callback: (error?: string) => void) {
    if (
      value &&
      (value === 1 ||
        (value >= 10 && value <= 12) ||
        (value >= 14 && value <= 15) ||
        value === 20 ||
        value === 24 ||
        (value >= 28 && value <= 31) ||
        value > 40)
    ) {
      callback('Must be an valid GPIO port');
    } else {
      callback();
    }
  }
};

export const createSettingsValidator = (settings: Settings) =>
  new Schema({
    ...(settings.board_profile === 'CUSTOM' && {
      led_gpio: [{ required: true, message: 'LED GPIO is required' }, GPIO_VALIDATOR],
      dallas_gpio: [{ required: true, message: 'GPIO is required' }, GPIO_VALIDATOR],
      pbutton_gpio: [{ required: true, message: 'Button GPIO is required' }, GPIO_VALIDATOR],
      tx_gpio: [{ required: true, message: 'Tx GPIO is required' }, GPIO_VALIDATOR],
      rx_gpio: [{ required: true, message: 'Rx GPIO is required' }, GPIO_VALIDATOR]
    }),
    ...(settings.syslog_enabled && {
      syslog_host: [{ required: true, message: 'Host is required' }, IP_OR_HOSTNAME_VALIDATOR],
      syslog_port: [
        { required: true, message: 'Port is required' },
        { type: 'number', min: 0, max: 65535, message: 'Port must be between 0 and 65535' }
      ],
      syslog_mark_interval: [
        { required: true, message: 'Mark interval is required' },
        { type: 'number', min: 0, max: 10, message: 'Port must be between 0 and 10' }
      ]
    }),
    ...(settings.shower_alert && {
      shower_alert_trigger: [{ type: 'number', min: 1, max: 20, message: 'Time must be between 1 and 20 minutes' }],
      shower_alert_coldshot: [{ type: 'number', min: 1, max: 10, message: 'Time must be between 1 and 10 seconds' }]
    })
  });
