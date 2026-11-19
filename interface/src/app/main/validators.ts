import { vLL } from 'validators/messages';
import Schema from 'validators/schema';
import type { InternalRuleItem } from 'validators/schema';
import { IP_OR_HOSTNAME_VALIDATOR, required } from 'validators/shared';

import type {
  AnalogSensor,
  CommandItem,
  DeviceValue,
  EntityItem,
  ScheduleItem,
  Settings,
  TemperatureSensor
} from './types';
import { ScheduleFlag } from './types';

export const VALIDATION_LIMITS = {
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
  WRITE_LIMIT_MIN: 0,
  WRITE_LIMIT_MAX: 255,
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

const between = (min: number, max: number) => ({
  type: 'number',
  min,
  max,
  message: vLL().V_BETWEEN({ min, max })
});

export const createSettingsValidator = (settings: Settings) => {
  const schema: Record<string, ValidationRules> = {};

  // Syslog validations
  if (settings.syslog_enabled) {
    schema.syslog_host = [required(), IP_OR_HOSTNAME_VALIDATOR];
    schema.syslog_port = [
      required(),
      between(VALIDATION_LIMITS.PORT_MIN, VALIDATION_LIMITS.PORT_MAX)
    ];
    schema.syslog_mark_interval = [
      required(),
      between(
        VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MIN,
        VALIDATION_LIMITS.SYSLOG_MARK_INTERVAL_MAX
      )
    ];
  }

  // Modbus validations
  if (settings.modbus_enabled) {
    schema.modbus_max_clients = [
      required(),
      between(
        VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MIN,
        VALIDATION_LIMITS.MODBUS_MAX_CLIENTS_MAX
      )
    ];
    schema.modbus_port = [
      required(),
      between(VALIDATION_LIMITS.PORT_MIN, VALIDATION_LIMITS.PORT_MAX)
    ];
    schema.modbus_timeout = [
      required(),
      between(
        VALIDATION_LIMITS.MODBUS_TIMEOUT_MIN,
        VALIDATION_LIMITS.MODBUS_TIMEOUT_MAX
      )
    ];
  }

  if (settings.email_enabled) {
    schema.email_server = [required(), IP_OR_HOSTNAME_VALIDATOR];
    schema.email_port = [
      required(),
      between(VALIDATION_LIMITS.PORT_MIN, VALIDATION_LIMITS.PORT_MAX)
    ];
  }

  // Shower timer validations
  if (settings.shower_timer) {
    schema.shower_min_duration = [
      between(
        VALIDATION_LIMITS.SHOWER_MIN_DURATION_MIN,
        VALIDATION_LIMITS.SHOWER_MIN_DURATION_MAX
      )
    ];
  }

  // Shower alert validations
  if (settings.shower_alert) {
    schema.shower_alert_trigger = [
      between(
        VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MIN,
        VALIDATION_LIMITS.SHOWER_ALERT_TRIGGER_MAX
      )
    ];
    schema.shower_alert_coldshot = [
      between(
        VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MIN,
        VALIDATION_LIMITS.SHOWER_ALERT_COLDSHOT_MAX
      )
    ];
  }

  schema.write_limit = [
    between(VALIDATION_LIMITS.WRITE_LIMIT_MIN, VALIDATION_LIMITS.WRITE_LIMIT_MAX)
  ];

  // Remote timeout validations
  if (settings.remote_timeout_en) {
    schema.remote_timeout = [
      between(
        VALIDATION_LIMITS.REMOTE_TIMEOUT_MIN,
        VALIDATION_LIMITS.REMOTE_TIMEOUT_MAX
      )
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
      callback(vLL().V_IN_USE());
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
      callback(vLL().V_IN_USE());
      return;
    }
    callback();
  }
});

const NAME_PATTERN_BASE = '[a-zA-Z0-9_]';
const namePattern = (min: 0 | 1) => ({
  type: 'string' as const,
  pattern: new RegExp(
    `^${NAME_PATTERN_BASE}{${min},${VALIDATION_LIMITS.NAME_MAX_LENGTH}}$`
  ),
  message: vLL().V_NAME({ max: VALIDATION_LIMITS.NAME_MAX_LENGTH })
});

export const uniqueNameValidator = (
  schedule: ScheduleItem[],
  originalName?: string
) => createUniqueNameValidator(schedule, originalName);

// scheduleItem is the edited item, originalName the name of the row being edited (none when adding)
export const schedulerItemValidation = (
  schedule: ScheduleItem[],
  scheduleItem: ScheduleItem,
  originalName?: string
) =>
  new Schema({
    name: [required(), namePattern(1), uniqueNameValidator(schedule, originalName)],
    cmd_name: [required()],
    ...(scheduleItem.flags < ScheduleFlag.SCHEDULE_TIMER && {
      flags: [
        {
          validator(
            _rule: InternalRuleItem,
            flags: number,
            callback: (error?: string) => void
          ) {
            callback(flags === 0 ? vLL().V_DAY() : undefined);
          }
        }
      ]
    }),
    ...(scheduleItem.flags === ScheduleFlag.SCHEDULE_ONCHANGE && {
      time: [required()]
    }),
    ...(scheduleItem.flags === ScheduleFlag.SCHEDULE_CONDITION && {
      time: [required()]
    })
  });

export const uniqueCommandNameValidator = (
  commands: CommandItem[],
  originalName?: string
) => createUniqueNameValidator(commands, originalName);

// originalName: the name of the edited row when its dialog opened, undefined for a new row
export const commandItemValidation = (
  commands: CommandItem[],
  originalName?: string
) =>
  new Schema({
    name: [
      required(),
      namePattern(1),
      uniqueCommandNameValidator(commands, originalName)
    ],
    cmd: [
      required(),
      {
        type: 'string',
        min: VALIDATION_LIMITS.COMMAND_MIN,
        max: VALIDATION_LIMITS.COMMAND_MAX,
        message: vLL().V_CHARS({
          min: VALIDATION_LIMITS.COMMAND_MIN,
          max: VALIDATION_LIMITS.COMMAND_MAX
        })
      }
    ]
  });

export const uniqueCustomNameValidator = (
  entity: EntityItem[],
  originalName?: string
) => createUniqueNameValidator(entity, originalName);

const hexValidator = () => ({
  validator(
    _rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    if (!value || Number.isNaN(Number.parseInt(value, VALIDATION_LIMITS.HEX_BASE))) {
      callback(vLL().V_HEX());
      return;
    }
    callback();
  }
});

// originalName: the name of the edited row when its dialog opened, undefined for a new row
export const entityItemValidation = (entity: EntityItem[], originalName?: string) =>
  new Schema({
    name: [
      required(),
      namePattern(1),
      uniqueCustomNameValidator(entity, originalName)
    ],
    device_id: [hexValidator()],
    type_id: [hexValidator()],
    offset: [
      required(),
      between(VALIDATION_LIMITS.OFFSET_MIN, VALIDATION_LIMITS.OFFSET_MAX)
    ],
    factor: [required()]
  });

export const uniqueTemperatureNameValidator = (
  sensors: TemperatureSensor[],
  o_name?: string
) => createUniqueFieldNameValidator(sensors, (s) => s.n, o_name);

export const temperatureSensorItemValidation = (
  sensors: TemperatureSensor[],
  originalName?: string
) =>
  new Schema({
    n: [namePattern(0), uniqueTemperatureNameValidator(sensors, originalName)]
  });

export const uniqueAnalogNameValidator = (
  sensors: AnalogSensor[],
  o_name?: string
) => createUniqueFieldNameValidator(sensors, (s) => s.n, o_name);

export const analogSensorItemValidation = (
  sensors: AnalogSensor[],
  originalName?: string
) =>
  new Schema({
    n: [required(), namePattern(0), uniqueAnalogNameValidator(sensors, originalName)]
  });

export const deviceValueItemValidation = (dv: DeviceValue) =>
  new Schema({
    v: [
      required(),
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
            callback(vLL().V_BETWEEN({ min: dv.m, max: dv.x }));
            return;
          }
          callback();
        }
      }
    ]
  });
