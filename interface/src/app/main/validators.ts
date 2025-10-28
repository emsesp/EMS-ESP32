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

// Helper to create GPIO validator from invalid ranges
const createGPIOValidator = (
  invalidRanges: Array<number | [number, number]>,
  maxValue: number
) => ({
  validator(
    _rule: InternalRuleItem,
    value: number,
    callback: (error?: string) => void
  ) {
    if (!value) {
      callback();
      return;
    }

    if (value < 0 || value > maxValue) {
      callback('Must be an valid GPIO port');
      return;
    }

    for (const range of invalidRanges) {
      if (typeof range === 'number') {
        if (value === range) {
          callback('Must be an valid GPIO port');
          return;
        }
      } else {
        const [start, end] = range;
        if (value >= start && value <= end) {
          callback('Must be an valid GPIO port');
          return;
        }
      }
    }

    callback();
  }
});

export const GPIO_VALIDATOR = createGPIOValidator(
  [[6, 11], 1, 20, 24, [28, 31]],
  40
);

export const GPIO_VALIDATORC3 = createGPIOValidator([[11, 19]], 21);

export const GPIO_VALIDATORS2 = createGPIOValidator(
  [
    [19, 20],
    [22, 32]
  ],
  40
);

export const GPIO_VALIDATORS3 = createGPIOValidator(
  [
    [19, 20],
    [22, 37],
    [39, 42]
  ],
  48
);

const GPIO_FIELD_NAMES = [
  'led_gpio',
  'dallas_gpio',
  'pbutton_gpio',
  'tx_gpio',
  'rx_gpio'
] as const;

type ValidationRules = Array<{
  required?: boolean;
  message?: string;
  [key: string]: unknown;
}>;

const createGPIOValidations = (
  validator: typeof GPIO_VALIDATOR
): Record<string, ValidationRules> =>
  GPIO_FIELD_NAMES.reduce(
    (acc, field) => {
      const fieldName = field.replace('_gpio', '');
      acc[field] = [
        { required: true, message: `${fieldName.toUpperCase()} GPIO is required` },
        validator
      ];
      return acc;
    },
    {} as Record<string, ValidationRules>
  );

const PLATFORM_VALIDATORS = {
  ESP32: GPIO_VALIDATOR,
  ESP32C3: GPIO_VALIDATORC3,
  ESP32S2: GPIO_VALIDATORS2,
  ESP32S3: GPIO_VALIDATORS3
} as const;

export const createSettingsValidator = (settings: Settings) => {
  const schema: Record<string, ValidationRules> = {};

  // Add GPIO validations for CUSTOM board profiles
  if (
    settings.board_profile === 'CUSTOM' &&
    settings.platform in PLATFORM_VALIDATORS
  ) {
    Object.assign(
      schema,
      createGPIOValidations(
        PLATFORM_VALIDATORS[settings.platform as keyof typeof PLATFORM_VALIDATORS]
      )
    );
  }

  // Syslog validations
  if (settings.syslog_enabled) {
    schema.syslog_host = [
      { required: true, message: 'Host is required' },
      IP_OR_HOSTNAME_VALIDATOR
    ];
    schema.syslog_port = [
      { required: true, message: 'Port is required' },
      { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
    ];
    schema.syslog_mark_interval = [
      { required: true, message: 'Mark interval is required' },
      { type: 'number', min: 0, max: 10, message: 'Must be between 0 and 10' }
    ];
  }

  // Modbus validations
  if (settings.modbus_enabled) {
    schema.modbus_max_clients = [
      { required: true, message: 'Max clients is required' },
      { type: 'number', min: 0, max: 50, message: 'Invalid number' }
    ];
    schema.modbus_port = [
      { required: true, message: 'Port is required' },
      { type: 'number', min: 0, max: 65535, message: 'Invalid Port' }
    ];
    schema.modbus_timeout = [
      { required: true, message: 'Timeout is required' },
      {
        type: 'number',
        min: 100,
        max: 20000,
        message: 'Must be between 100 and 20000'
      }
    ];
  }

  // Shower timer validations
  if (settings.shower_timer) {
    schema.shower_min_duration = [
      {
        type: 'number',
        min: 10,
        max: 360,
        message: 'Time must be between 10 and 360 seconds'
      }
    ];
  }

  // Shower alert validations
  if (settings.shower_alert) {
    schema.shower_alert_trigger = [
      {
        type: 'number',
        min: 1,
        max: 20,
        message: 'Time must be between 1 and 20 minutes'
      }
    ];
    schema.shower_alert_coldshot = [
      {
        type: 'number',
        min: 1,
        max: 10,
        message: 'Time must be between 1 and 10 seconds'
      }
    ];
  }

  // Remote timeout validations
  if (settings.remote_timeout_en) {
    schema.remote_timeout = [
      {
        type: 'number',
        min: 1,
        max: 240,
        message: 'Timeout must be between 1 and 240 hours'
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
      callback('Name already in use');
    } else {
      callback();
    }
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
      callback('Name already in use');
    } else {
      callback();
    }
  }
});

const NAME_PATTERN = {
  type: 'string' as const,
  pattern: /^[a-zA-Z0-9_]{0,19}$/,
  message: "Must be <20 characters: alphanumeric or '_'"
};

const NAME_PATTERN_REQUIRED = {
  type: 'string' as const,
  pattern: /^[a-zA-Z0-9_]{1,19}$/,
  message: "Must be <20 characters: alphanumeric or '_'"
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
        min: 1,
        max: 300,
        message: 'Command must be 1-300 characters'
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
    if (!value || isNaN(parseInt(value, 16))) {
      callback('Is required and must be in hex format');
    } else {
      callback();
    }
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
      { type: 'number', min: 0, max: 255, message: 'Must be between 0 and 255' }
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

export const isGPIOUniqueValidator = (sensors: AnalogSensor[]) => ({
  validator(
    _rule: InternalRuleItem,
    gpio: number,
    callback: (error?: string) => void
  ) {
    if (sensors.find((as) => as.g === gpio)) {
      callback('GPIO already in use');
    } else {
      callback();
    }
  }
});

export const uniqueAnalogNameValidator = (
  sensors: AnalogSensor[],
  o_name?: string
) => createUniqueFieldNameValidator(sensors, (s) => s.n, o_name);

export const analogSensorItemValidation = (
  sensors: AnalogSensor[],
  sensor: AnalogSensor,
  creating: boolean,
  platform: string
) => {
  const gpioValidator =
    platform === 'ESP32S3'
      ? GPIO_VALIDATORS3
      : platform === 'ESP32S2'
        ? GPIO_VALIDATORS2
        : platform === 'ESP32C3'
          ? GPIO_VALIDATORC3
          : GPIO_VALIDATOR;

  return new Schema({
    n: [NAME_PATTERN, uniqueAnalogNameValidator(sensors, sensor.o_n)],
    g: [
      { required: true, message: 'GPIO is required' },
      gpioValidator,
      ...(creating ? [isGPIOUniqueValidator(sensors)] : [])
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
            callback('Value out of range');
          } else {
            callback();
          }
        }
      }
    ]
  });
