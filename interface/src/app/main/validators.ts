import Schema from 'async-validator';
import type { InternalRuleItem } from 'async-validator';
import { IP_OR_HOSTNAME_VALIDATOR } from 'validators/shared';

import type {
  AnalogSensor,
  DeviceValue,
  EntityItem,
  ScheduleItem,
  Settings,
  TemperatureSensor
} from './types';

// Constants
const ERROR_MESSAGES = {
  GPIO_INVALID: 'Must be an valid GPIO port',
  NAME_DUPLICATE: 'Name already in use',
  GPIO_DUPLICATE: 'GPIO already in use',
  VALUE_OUT_OF_RANGE: 'Value out of range',
  HEX_REQUIRED: 'Is required and must be in hex format'
} as const;

const VALIDATION_LIMITS = {
  PORT_MIN: 0,
  PORT_MAX: 65535,
  MODBUS_MAX_CLIENTS_MIN: 0,
  MODBUS_MAX_CLIENTS_MAX: 50,
  MODBUS_TIMEOUT_MIN: 100,
  MODBUS_TIMEOUT_MAX: 20000,
  SYSLOG_MARK_INTERVAL_MIN: 0,
  SYSLOG_MARK_INTERVAL_MAX: 3600,
  SHOWER_MIN_DURATION_MIN: 10,
  SHOWER_MIN_DURATION_MAX: 360,
  SHOWER_ALERT_TRIGGER_MIN: 1,
  SHOWER_ALERT_TRIGGER_MAX: 20,
  SHOWER_ALERT_COLDSHOT_MIN: 1,
  SHOWER_ALERT_COLDSHOT_MAX: 10,
  REMOTE_TIMEOUT_MIN: 1,
  REMOTE_TIMEOUT_MAX: 240,
  OFFSET_MIN: 0,
  OFFSET_MAX: 255,
  COMMAND_MIN: 1,
  COMMAND_MAX: 300,
  NAME_MAX_LENGTH: 19,
  HEX_BASE: 16
} as const;

type ValidationRules = Array<{
  required?: boolean;
  message?: string;
  [key: string]: unknown;
}>;

export const createSettingsValidator = (settings: Settings) => {
  const schema: Record<string, ValidationRules> = {};

  // Syslog validations
  if (settings.syslog_enabled) {
    schema.syslog_host = [
      { required: true, message: 'Host is required' },
      IP_OR_HOSTNAME_VALIDATOR
    ];
    schema.syslog_port = [
      { required: true, message: 'Port is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.PORT_MIN,
        max: VALIDATION_LIMITS.PORT_MAX,
        message: 'Invalid Port'
      }
    ];
    schema.syslog_mark_interval = [
      { required: true, message: 'Mark interval is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MIN,
        max: VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MAX,
        message: `Must be between ${VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MIN} and ${VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MAX}`
      }
    ];
  }

  // Modbus validations
  if (settings.modbus_enabled) {
    schema.modbus_max_clients = [
      { required: true, message: 'Max clients is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MIN,
        max: VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MAX,
        message: 'Invalid number'
      }
    ];
    schema.modbus_port = [
      { required: true, message: 'Port is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.PORT_MIN,
        max: VALIDATION_LIMITS.PORT_MAX,
        message: 'Invalid Port'
      }
    ];
    schema.modbus_timeout = [
      { required: true, message: 'Timeout is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.MODBUS_TIMEOUT_MIN,
        max: VALIDATION_LIMITS.MODBUS_TIMEOUT_MAX,
        message: `Must be between ${VALIDATION_LIMITS.MODBUS_TIMEOUT_MIN} and ${VALIDATION_LIMITS.MODBUS_TIMEOUT_MAX}`
      }
    ];
  }

  // Shower timer validations
  if (settings.shower_timer) {
    schema.shower_min_duration = [
      {
        type: 'number',
        min: VALIDATION_LIMITS.SHOWER_MIN_DURATION_MIN,
        max: VALIDATION_LIMITS.SHOWER_MIN_DURATION_MAX,
        message: `Time must be between ${VALIDATION_LIMITS.SHOWER_MIN_DURATION_MIN} and ${VALIDATION_LIMITS.SHOWER_MIN_DURATION_MAX} seconds`
      }
    ];
  }

  // Shower alert validations
  if (settings.shower_alert) {
    schema.shower_alert_trigger = [
      {
        type: 'number',
        min: VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MIN,
        max: VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MAX,
        message: `Time must be between ${VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MIN} and ${VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MAX} minutes`
      }
    ];
    schema.shower_alert_coldshot = [
      {
        type: 'number',
        min: VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MIN,
        max: VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MAX,
        message: `Time must be between ${VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MIN} and ${VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MAX} seconds`
      }
    ];
  }

  // Remote timeout validations
  if (settings.remote_timeout_en) {
    schema.remote_timeout = [
      {
        type: 'number',
        min: VALIDATION_LIMITS.REMOTE_TIMEOUT_MIN,
        max: VALIDATION_LIMITS.REMOTE_TIMEOUT_MAX,
        message: `Timeout must be between ${VALIDATION_LIMITS.REMOTE_TIMEOUT_MIN} and ${VALIDATION_LIMITS.REMOTE_TIMEOUT_MAX} hours`
      }
    ];
  }

  return new Schema(schema);
};

// Generic unique name validator factory
const createUniqueNameValidator = <T extends { name: string }>(
  items: T[],
  originalName?: string
) => ({
  validator(
    _rule: InternalRuleItem,
    name: string,
    callback: (error?: string) => void
  ) {
    if (
      name !== '' &&
      (originalName === undefined ||
        originalName.toLowerCase() !== name.toLowerCase()) &&
      items.find((item) => item.name.toLowerCase() === name.toLowerCase())
    ) {
      callback(ERROR_MESSAGES.NAME_DUPLICATE);
      return;
    }
    callback();
  }
});

// Generic field name validator (for cases where the name field has different property names)
const createUniqueFieldNameValidator = <T>(
  items: T[],
  getName: (item: T) => string,
  originalName?: string
) => ({
  validator(
    _rule: InternalRuleItem,
    name: string,
    callback: (error?: string) => void
  ) {
    if (
      name !== '' &&
      (originalName === undefined ||
        originalName.toLowerCase() !== name.toLowerCase()) &&
      items.find((item) => getName(item).toLowerCase() === name.toLowerCase())
    ) {
      callback(ERROR_MESSAGES.NAME_DUPLICATE);
      return;
    }
    callback();
  }
});

const NAME_PATTERN_BASE = '[a-zA-Z0-9_]';
const NAME_PATTERN_MESSAGE = `Must be <${VALIDATION_LIMITS.NAME_MAX_LENGTH + 1} characters: alphanumeric or '_'`;

const NAME_PATTERN = {
  type: 'string' as const,
  pattern: new RegExp(
    `^${NAME_PATTERN_BASE}{0,${VALIDATION_LIMITS.NAME_MAX_LENGTH}}$`
  ),
  message: NAME_PATTERN_MESSAGE
};

const NAME_PATTERN_REQUIRED = {
  type: 'string' as const,
  pattern: new RegExp(
    `^${NAME_PATTERN_BASE}{1,${VALIDATION_LIMITS.NAME_MAX_LENGTH}}$`
  ),
  message: NAME_PATTERN_MESSAGE
};

export const uniqueNameValidator = (schedule: ScheduleItem[], o_name?: string) =>
  createUniqueNameValidator(schedule, o_name);

export const schedulerItemValidation = (
  schedule: ScheduleItem[],
  scheduleItem: ScheduleItem
) =>
  new Schema({
    name: [NAME_PATTERN, uniqueNameValidator(schedule, scheduleItem.o_name)],
    cmd: [
      { required: true, message: 'Command is required' },
      {
        type: 'string',
        min: VALIDATION_LIMITS.COMMAND_MIN,
        max: VALIDATION_LIMITS.COMMAND_MAX,
        message: `Command must be ${VALIDATION_LIMITS.COMMAND_MIN}-${VALIDATION_LIMITS.COMMAND_MAX} characters`
      }
    ]
  });

export const uniqueCustomNameValidator = (entity: EntityItem[], o_name?: string) =>
  createUniqueNameValidator(entity, o_name);

const hexValidator = {
  validator(
    _rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    if (!value || Number.isNaN(Number.parseInt(value, VALIDATION_LIMITS.HEX_BASE))) {
      callback(ERROR_MESSAGES.HEX_REQUIRED);
      return;
    }
    callback();
  }
};

export const entityItemValidation = (entity: EntityItem[], entityItem: EntityItem) =>
  new Schema({
    name: [
      { required: true, message: 'Name is required' },
      NAME_PATTERN_REQUIRED,
      uniqueCustomNameValidator(entity, entityItem.o_name)
    ],
    device_id: [hexValidator],
    type_id: [hexValidator],
    offset: [
      { required: true, message: 'Offset is required' },
      {
        type: 'number',
        min: VALIDATION_LIMITS.OFFSET_MIN,
        max: VALIDATION_LIMITS.OFFSET_MAX,
        message: `Must be between ${VALIDATION_LIMITS.OFFSET_MIN} and ${VALIDATION_LIMITS.OFFSET_MAX}`
      }
    ],
    factor: [{ required: true, message: 'is required' }]
  });

export const uniqueTemperatureNameValidator = (
  sensors: TemperatureSensor[],
  o_name?: string
) => createUniqueFieldNameValidator(sensors, (s) => s.n, o_name);

export const temperatureSensorItemValidation = (
  sensors: TemperatureSensor[],
  sensor: TemperatureSensor
) =>
  new Schema({
    n: [NAME_PATTERN, uniqueTemperatureNameValidator(sensors, sensor.o_n)]
  });

export const uniqueAnalogNameValidator = (
  sensors: AnalogSensor[],
  o_name?: string
) => createUniqueFieldNameValidator(sensors, (s) => s.n, o_name);

export const analogSensorItemValidation = (
  sensors: AnalogSensor[],
  sensor: AnalogSensor
) => {
  return new Schema({
    // name is required and must be unique
    n: [
      { required: true, message: 'Name is required' },
      NAME_PATTERN,
      uniqueAnalogNameValidator(sensors, sensor.o_n)
    ],
    g: [
      {
        required: true,
        type: 'number',
        min: 1,
        message: 'GPIO is required'
      }
    ]
  });
};

export const deviceValueItemValidation = (dv: DeviceValue) =>
  new Schema({
    v: [
      { required: true, message: 'Value is required' },
      {
        validator(
          _rule: InternalRuleItem,
          value: unknown,
          callback: (error?: string) => void
        ) {
          if (
            typeof value === 'number' &&
            dv.m !== undefined &&
            dv.x !== undefined &&
            (value < dv.m || value > dv.x)
          ) {
            callback(ERROR_MESSAGES.VALUE_OUT_OF_RANGE);
            return;
          }
          callback();
        }
      }
    ]
  });
