import type { TranslationFunctions } from 'i18n/i18n-types';

import { DeviceValueUOM, DeviceValueUOM_s } from './types';

const formatDurationMin = (LL: TranslationFunctions, duration_min: number) => {
  const days = Math.trunc((duration_min * 60000) / 86400000);
  const hours = Math.trunc((duration_min * 60000) / 3600000) % 24;
  const minutes = Math.trunc((duration_min * 60000) / 60000) % 60;

  let formatted = '';
  if (days) {
    formatted += LL.NUM_DAYS({ num: days });
  }

  if (hours) {
    if (formatted) formatted += ' ';
    formatted += LL.NUM_HOURS({ num: hours });
  }

  if (minutes) {
    if (formatted) formatted += ' ';
    formatted += LL.NUM_MINUTES({ num: minutes });
  }

  return formatted;
};

export function formatValue(
  LL: TranslationFunctions,
  value?: unknown,
  uom?: DeviceValueUOM
) {
  if (typeof value !== 'number' || uom === undefined || value === undefined) {
    if (value === undefined || typeof value === 'boolean') {
      return '';
    }
    return (
      (value as string) +
      (value === '' || uom === undefined || uom === 0
        ? ''
        : ' ' + DeviceValueUOM_s[uom])
    );
  }

  switch (uom) {
    case DeviceValueUOM.HOURS:
      return value ? formatDurationMin(LL, value * 60) : LL.NUM_HOURS({ num: 0 });
    case DeviceValueUOM.MINUTES:
      return value ? formatDurationMin(LL, value) : LL.NUM_MINUTES({ num: 0 });
    case DeviceValueUOM.SECONDS:
      return LL.NUM_SECONDS({ num: value });
    case DeviceValueUOM.NONE:
      return new Intl.NumberFormat().format(value);
    case DeviceValueUOM.DEGREES:
    case DeviceValueUOM.DEGREES_R:
    case DeviceValueUOM.FAHRENHEIT:
      return (
        new Intl.NumberFormat(undefined, {
          minimumFractionDigits: 1
        }).format(value) +
        ' ' +
        DeviceValueUOM_s[uom]
      );
    default:
      return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
  }
}
