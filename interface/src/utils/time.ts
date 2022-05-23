import parseMilliseconds from 'parse-ms';

const LOCALE_FORMAT = new Intl.DateTimeFormat([...window.navigator.languages], {
  day: 'numeric',
  month: 'short',
  year: 'numeric',
  hour: 'numeric',
  minute: 'numeric',
  second: 'numeric',
  hour12: false
});

export const formatDateTime = (dateTime: string) => {
  return LOCALE_FORMAT.format(new Date(dateTime.substr(0, 19)));
};

export const formatLocalDateTime = (date: Date) => {
  return new Date(date.getTime() - date.getTimezoneOffset() * 60000).toISOString().slice(0, -1).substr(0, 19);
};

export const pluralize = (count: number, noun: string) =>
  `${Intl.NumberFormat().format(count)} ${noun}${count !== 1 ? 's' : ''}`;

export const formatDurationMin = (duration_min: number) => {
  const { days, hours, minutes } = parseMilliseconds(duration_min * 60000);
  let formatted = '';
  if (days) {
    formatted += pluralize(days, 'day') + ' ';
  }
  if (hours) {
    formatted += pluralize(hours, 'hour') + ' ';
  }
  if (minutes) {
    formatted += pluralize(minutes, 'minute') + ' ';
  }
  return formatted;
};

export const formatDurationSec = (duration_sec: number) => {
  if (duration_sec === 0) {
    return ' ';
  }
  const roundTowardsZero = duration_sec > 0 ? Math.floor : Math.ceil;
  return (
    ', ' +
    roundTowardsZero(duration_sec / 86400) +
    'd ' +
    (roundTowardsZero(duration_sec / 3600) % 24) +
    'h ' +
    (roundTowardsZero(duration_sec / 60) % 60) +
    'm ' +
    (roundTowardsZero(duration_sec) % 60) +
    's'
  );
};
