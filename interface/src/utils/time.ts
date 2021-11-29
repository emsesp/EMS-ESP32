import parseMilliseconds from 'parse-ms';

const LOCALE_FORMAT = new Intl.DateTimeFormat(
  [...window.navigator.languages],
  {
    day: 'numeric',
    month: 'short',
    year: 'numeric',
    hour: 'numeric',
    minute: 'numeric',
    second: 'numeric',
    hour12: false
  }
);

export const formatDateTime = (dateTime: string) => {
  return LOCALE_FORMAT.format(new Date(dateTime.substr(0, 19)));
};

export const formatLocalDateTime = (date: Date) => {
  return new Date(date.getTime() - date.getTimezoneOffset() * 60000)
    .toISOString()
    .slice(0, -1)
    .substr(0, 19);
};

export const formatDuration = (duration: number) => {
  const { days, hours, minutes, seconds } = parseMilliseconds(duration * 1000);
  var formatted = '';
  if (days) {
    formatted += pluralize(days, 'day');
  }
  if (formatted || hours) {
    formatted += pluralize(hours, 'hour');
  }
  if (formatted || minutes) {
    formatted += pluralize(minutes, 'minute');
  }
  if (formatted || seconds) {
    formatted += pluralize(seconds, 'second');
  }
  return formatted;
};

const pluralize = (count: number, noun: string, suffix: string = 's') => ` ${count} ${noun}${count !== 1 ? suffix : ''} `;
