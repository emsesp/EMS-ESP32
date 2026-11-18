import type { IconName } from 'components/ui';
import type { TranslationFunctions } from 'i18n/i18n-types';

import { ScheduleFlag } from './types';

export const ALL_DAYS = 127;
export const WORK_DAYS =
  ScheduleFlag.SCHEDULE_MON |
  ScheduleFlag.SCHEDULE_TUE |
  ScheduleFlag.SCHEDULE_WED |
  ScheduleFlag.SCHEDULE_THU |
  ScheduleFlag.SCHEDULE_FRI;

// display order, Monday first
export const DAY_FLAGS = [
  ScheduleFlag.SCHEDULE_MON,
  ScheduleFlag.SCHEDULE_TUE,
  ScheduleFlag.SCHEDULE_WED,
  ScheduleFlag.SCHEDULE_THU,
  ScheduleFlag.SCHEDULE_FRI,
  ScheduleFlag.SCHEDULE_SAT,
  ScheduleFlag.SCHEDULE_SUN
];

// flags up to 127 are the days of a weekly schedule, above that the type itself
export const scheduleType = (flags: number) =>
  flags < ScheduleFlag.SCHEDULE_TIMER
    ? ScheduleFlag.SCHEDULE_DAY
    : (flags as ScheduleFlag);

export const SCHEDULE_TYPES: {
  flag: ScheduleFlag;
  icon: IconName;
  label: (LL: TranslationFunctions) => string;
}[] = [
  {
    flag: ScheduleFlag.SCHEDULE_DAY,
    icon: 'calendar_month',
    label: (LL) => LL.WEEKLY()
  },
  { flag: ScheduleFlag.SCHEDULE_TIMER, icon: 'timer', label: (LL) => LL.TIMER(0) },
  {
    flag: ScheduleFlag.SCHEDULE_ONCHANGE,
    icon: 'change_circle',
    label: (LL) => LL.ONCHANGE()
  },
  {
    flag: ScheduleFlag.SCHEDULE_CONDITION,
    icon: 'rule',
    label: (LL) => LL.CONDITION()
  }
];

// weekday names in DAY_FLAGS order; 1 Jan 2017 was a Sunday
export const weekdayNames = (locale: string, style: 'short' | 'narrow') => {
  const format = new Intl.DateTimeFormat(locale, {
    weekday: style,
    timeZone: 'UTC'
  });
  return DAY_FLAGS.map((flag) =>
    format.format(new Date(Date.UTC(2017, 0, 1 + Math.log2(flag))))
  );
};
