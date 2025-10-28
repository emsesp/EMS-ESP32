import type { TranslationFunctions } from 'i18n/i18n-types';

import { DeviceValueUOM, DeviceValueUOM_s } from './types';

// Cache NumberFormat instances for better performance
const numberFormatter = new Intl.NumberFormat();
const numberFormatterWithDecimal = new Intl.NumberFormat(undefined, {
  minimumFractionDigits: 1
});

const formatDurationMin = (LL: TranslationFunctions, duration_min: number) => {
  const totalMs = duration_min * 60000;
  const days = Math.trunc(totalMs / 86400000);
  const hours = Math.trunc(totalMs / 3600000) % 24;
  const minutes = Math.trunc(duration_min) % 60;

  const parts: string[] = [];
  if (days) {
    parts.push(LL.NUM_DAYS({ num: days }));
  }
  if (hours) {
    parts.push(LL.NUM_HOURS({ num: hours }));
  }
  if (minutes) {
    parts.push(LL.NUM_MINUTES({ num: minutes }));
  }

  return parts.join(' ');
};

export function formatValue(
  LL: TranslationFunctions,
  value?: unknown,
  uom?: DeviceValueUOM
) {
  // Handle non-numeric values or missing data
  if (typeof value !== 'number' || uom === undefined || value === undefined) {
    if (value === undefined || typeof value === 'boolean') {
      return '';
    }
    // Type assertion is safe here since we know it's not a number, boolean, or undefined
    return (
      (value as string) +
      (value === '' || uom === undefined || uom === DeviceValueUOM.NONE
        ? ''
        : ' ' + DeviceValueUOM_s[uom])
    );
  }

  // Handle numeric values
  switch (uom) {
    case DeviceValueUOM.HOURS:
      return value ? formatDurationMin(LL, value * 60) : LL.NUM_HOURS({ num: 0 });
    case DeviceValueUOM.MINUTES:
      return value ? formatDurationMin(LL, value) : LL.NUM_MINUTES({ num: 0 });
    case DeviceValueUOM.SECONDS:
      return LL.NUM_SECONDS({ num: value });
    case DeviceValueUOM.NONE:
      return numberFormatter.format(value);
    case DeviceValueUOM.DEGREES:
    case DeviceValueUOM.DEGREES_R:
    case DeviceValueUOM.FAHRENHEIT:
      return numberFormatterWithDecimal.format(value) + ' ' + DeviceValueUOM_s[uom];
    default:
      return numberFormatter.format(value) + ' ' + DeviceValueUOM_s[uom];
  }
}
