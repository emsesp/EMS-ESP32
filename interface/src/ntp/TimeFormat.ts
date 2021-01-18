import parseMilliseconds from 'parse-ms';

const LOCALE_FORMAT = new Intl.DateTimeFormat('default', {
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
}

export const formatLocalDateTimeNow = () => {
  return formatIsoDateTime(new Date()).substr(0, 19);
}

export const formatLocalDateTime = (dateTime: string) => {
  return formatIsoDateTime(new Date(dateTime)).substr(0, 19);
}

export const formatIsoDateTime = (date: Date) => {
  return new Date(date.getTime() - date.getTimezoneOffset() * 60000).toISOString().slice(0, -1);
}

export const formatDuration = (duration: number) => {
  const { days, hours, minutes, seconds } = parseMilliseconds(duration * 1000);
  var formatted = '';
  if (days) {
    formatted += days + ' days ';
  }
  if (formatted || hours) {
    formatted += hours + ' hours ';
  }
  if (formatted || minutes) {
    formatted += minutes + ' minutes ';
  }
  if (formatted || seconds) {
    formatted += seconds + ' seconds';
  }
  return formatted;
}