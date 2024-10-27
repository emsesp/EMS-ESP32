const LOCALE_FORMAT = new Intl.DateTimeFormat([...window.navigator.languages], {
  day: 'numeric',
  month: 'short',
  year: 'numeric',
  hour: 'numeric',
  minute: 'numeric',
  second: 'numeric',
  hour12: false
});

export const formatDateTime = (dateTime: string) =>
  LOCALE_FORMAT.format(new Date(dateTime.substring(0, 19)));

export const formatLocalDateTime = (date: Date) =>
  new Date(date.getTime() - date.getTimezoneOffset() * 60000)
    .toISOString()
    .slice(0, -1)
    .substring(0, 19);
