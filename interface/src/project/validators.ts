import Schema, { InternalRuleItem } from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from 'validators/shared';
import { Settings, ScheduleItem } from './types';

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

export const uniqueNameValidator = (schedule: ScheduleItem[], o_name?: string) => ({
  validator(rule: InternalRuleItem, name: string, callback: (error?: string) => void) {
    if (name && o_name && o_name !== name && schedule.find((si) => si.name === name)) {
      callback('Name already in use');
    } else {
      callback();
    }
  }
});

export const schedulerItemValidation = (schedule: ScheduleItem[], scheduleItem: ScheduleItem) =>
  new Schema({
    name: [
      {
        type: 'string',
        pattern: /^[a-zA-Z0-9_\\.]{0,15}$/,
        message: "Must be <15 characters: alpha numeric, '_' or '.'"
      },
      ...[uniqueNameValidator(schedule, scheduleItem.o_name)]
    ],
    cmd: [
      { required: true, message: 'Command is required' },
      { type: 'string', min: 1, max: 64, message: 'Command must be 1-64 characters' }
    ]
  });
