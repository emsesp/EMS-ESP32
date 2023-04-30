import Schema from 'async-validator';
import { DeviceValueUOM } from './types';
import type { AnalogSensor, DeviceValue, Settings } from './types';
import type { InternalRuleItem } from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from 'validators/shared';

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
        value > 40 ||
        value < 0)
    ) {
      callback('Must be an valid GPIO port');
    } else {
      callback();
    }
  }
};

export const GPIO_VALIDATORC3 = {
  validator(rule: InternalRuleItem, value: number, callback: (error?: string) => void) {
    if (value && ((value >= 11 && value <= 19) || value > 21 || value < 0)) {
      callback('Must be an valid GPIO port');
    } else {
      callback();
    }
  }
};

export const GPIO_VALIDATORS2 = {
  validator(rule: InternalRuleItem, value: number, callback: (error?: string) => void) {
    if (value && ((value >= 19 && value <= 20) || (value >= 22 && value <= 32) || value > 40 || value < 0)) {
      callback('Must be an valid GPIO port');
    } else {
      callback();
    }
  }
};

export const createSettingsValidator = (settings: Settings) =>
  new Schema({
    ...(settings.board_profile === 'CUSTOM' &&
      settings.platform === 'ESP32' && {
        led_gpio: [{ required: true, message: 'LED GPIO is required' }, GPIO_VALIDATOR],
        dallas_gpio: [{ required: true, message: 'GPIO is required' }, GPIO_VALIDATOR],
        pbutton_gpio: [{ required: true, message: 'Button GPIO is required' }, GPIO_VALIDATOR],
        tx_gpio: [{ required: true, message: 'Tx GPIO is required' }, GPIO_VALIDATOR],
        rx_gpio: [{ required: true, message: 'Rx GPIO is required' }, GPIO_VALIDATOR]
      }),
    ...(settings.board_profile === 'CUSTOM' &&
      settings.platform === 'ESP32-C3' && {
        led_gpio: [{ required: true, message: 'LED GPIO is required' }, GPIO_VALIDATORC3],
        dallas_gpio: [{ required: true, message: 'GPIO is required' }, GPIO_VALIDATORC3],
        pbutton_gpio: [{ required: true, message: 'Button GPIO is required' }, GPIO_VALIDATORC3],
        tx_gpio: [{ required: true, message: 'Tx GPIO is required' }, GPIO_VALIDATORC3],
        rx_gpio: [{ required: true, message: 'Rx GPIO is required' }, GPIO_VALIDATORC3]
      }),
    ...(settings.board_profile === 'CUSTOM' &&
      settings.platform === 'ESP32-S2' && {
        led_gpio: [{ required: true, message: 'LED GPIO is required' }, GPIO_VALIDATORS2],
        dallas_gpio: [{ required: true, message: 'GPIO is required' }, GPIO_VALIDATORS2],
        pbutton_gpio: [{ required: true, message: 'Button GPIO is required' }, GPIO_VALIDATORS2],
        tx_gpio: [{ required: true, message: 'Tx GPIO is required' }, GPIO_VALIDATORS2],
        rx_gpio: [{ required: true, message: 'Rx GPIO is required' }, GPIO_VALIDATORS2]
      }),
    ...(settings.syslog_enabled && {
      syslog_host: [{ required: true, message: 'Host is required' }, IP_OR_HOSTNAME_VALIDATOR],
      syslog_port: [
        { required: true, message: 'Port is required' },
        { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
      ],
      syslog_mark_interval: [
        { required: true, message: 'Mark interval is required' },
        { type: 'number', min: 0, max: 10, message: 'Must be between 0 and 10' }
      ]
    }),
    ...(settings.shower_alert && {
      shower_alert_trigger: [{ type: 'number', min: 1, max: 20, message: 'Time must be between 1 and 20 minutes' }],
      shower_alert_coldshot: [{ type: 'number', min: 1, max: 10, message: 'Time must be between 1 and 10 seconds' }]
    })
  });

export const schedulerItemValidation = () =>
  new Schema({
    name: [
      {
        type: 'string',
        pattern: /^[a-zA-Z0-9_\\.]{0,15}$/,
        message: "Must be <15 characters: alpha numeric, '_' or '.'"
      }
    ],
    cmd: [
      { required: true, message: 'Command is required' },
      { type: 'string', min: 1, max: 64, message: 'Command must be 1-64 characters' }
    ]
  });

export const entityItemValidation = () =>
  new Schema({
    name: [
      { required: true, message: 'Name is required' },
      {
        type: 'string',
        pattern: /^[a-zA-Z0-9_\\.]{1,15}$/,
        message: "Must be <15 characters: alpha numeric, '_' or '.'"
      }
    ],
    device_id: [
      {
        validator(rule: InternalRuleItem, value: string, callback: (error?: string) => void) {
          if (isNaN(parseInt(value, 16))) {
            callback('Is required and must be in hex format');
          }
          callback();
        }
      }
    ],
    type_id: [
      {
        validator(rule: InternalRuleItem, value: string, callback: (error?: string) => void) {
          if (isNaN(parseInt(value, 16))) {
            callback('Is required and must be in hex format');
          }
          callback();
        }
      }
    ],
    offset: [
      { required: true, message: 'Offset is required' },
      { type: 'number', min: 0, max: 255, message: 'Must be between 0 and 255' }
    ]
  });

export const temperatureSensorItemValidation = () =>
  new Schema({
    n: [{ required: true, message: 'Name is required' }]
  });

export const isGPIOUniqueValidator = (sensors: AnalogSensor[]) => ({
  validator(rule: InternalRuleItem, gpio: number, callback: (error?: string) => void) {
    if (sensors.find((as) => as.g === gpio)) {
      callback('GPIO already in use');
    } else {
      callback();
    }
  }
});

export const analogSensorItemValidation = (sensors: AnalogSensor[], creating: boolean) =>
  new Schema({
    n: [{ required: true, message: 'Name is required' }],
    g: [
      { required: true, message: 'GPIO is required' },
      GPIO_VALIDATOR,
      ...(creating ? [isGPIOUniqueValidator(sensors)] : [])
    ]
  });

export const deviceValueItemValidation = (dv: DeviceValue) =>
  new Schema({
    v: [
      { required: true, message: 'Value is required' },
      {
        validator(rule: InternalRuleItem, value: string, callback: (error?: string) => void) {
          if (dv.u !== DeviceValueUOM.NONE && isNaN(+value)) {
            // not a number
            dv.m && dv.x
              ? callback('Value must be a number between ' + dv.m + ' and ' + dv.x)
              : callback('Value must be a number');
          }
          // is a number
          (dv.m && Number(value) < dv.m) || (dv.x && Number(value) > dv.x)
            ? callback('Value must be between ' + dv.m + ' and ' + dv.x)
            : callback();
        }
      }
    ]
  });
