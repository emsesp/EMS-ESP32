// Cache for formatters to avoid recreation
const formatterCache = new Map<string, Intl.DateTimeFormat>();
const rtfCache = new Map<string, Intl.RelativeTimeFormat>();

// Pre-computed time divisions for relative time formatting
const TIME_DIVISIONS = [
  { amount: 60, name: 'seconds' as const },
  { amount: 60, name: 'minutes' as const },
  { amount: 24, name: 'hours' as const },
  { amount: 7, name: 'days' as const },
  { amount: 4.34524, name: 'weeks' as const },
  { amount: 12, name: 'months' as const },
  { amount: Number.POSITIVE_INFINITY, name: 'years' as const }
] as const;

/**
 * Get or create a cached DateTimeFormat instance
 */
function getDateTimeFormatter(
  options: Intl.DateTimeFormatOptions
): Intl.DateTimeFormat {
  const key = JSON.stringify(options);
  if (!formatterCache.has(key)) {
    formatterCache.set(
      key,
      new Intl.DateTimeFormat([...window.navigator.languages], options)
    );
  }
  return formatterCache.get(key)!;
}

/**
 * Get or create a cached RelativeTimeFormat instance
 */
function getRelativeTimeFormatter(locale: string): Intl.RelativeTimeFormat {
  if (!rtfCache.has(locale)) {
    rtfCache.set(locale, new Intl.RelativeTimeFormat(locale, { numeric: 'auto' }));
  }
  return rtfCache.get(locale)!;
}

/**
 * Format a date as relative time (e.g., "2 hours ago", "in 3 days")
 */
function formatTimeAgo(locale: string, date: Date): string {
  const now = Date.now();
  const targetTime = date.getTime();
  let duration = (targetTime - now) / 1000;

  const rtf = getRelativeTimeFormatter(locale);

  // Use for...of for better performance and readability
  for (const division of TIME_DIVISIONS) {
    if (Math.abs(duration) < division.amount) {
      return rtf.format(Math.round(duration), division.name);
    }
    duration /= division.amount;
  }

  return rtf.format(0, 'seconds');
}

/**
 * Format a date-time string to locale-specific format
 */
export const formatDateTime = (dateTime: string): string => {
  if (!dateTime || typeof dateTime !== 'string') {
    return 'Invalid date';
  }

  try {
    // Extract only the first 19 characters (YYYY-MM-DDTHH:mm:ss)
    const cleanDateTime = dateTime.substring(0, 19);
    const date = new Date(cleanDateTime);

    if (isNaN(date.getTime())) {
      return 'Invalid date';
    }

    const formatter = getDateTimeFormatter({
      day: 'numeric',
      month: 'short',
      year: 'numeric',
      hour: 'numeric',
      minute: 'numeric',
      second: 'numeric',
      hour12: false
    });

    return formatter.format(date);
  } catch (error) {
    console.warn('Error formatting date:', error);
    return 'Invalid date';
  }
};

/**
 * Convert a Date object to local date-time string (ISO format without timezone)
 */
export const formatLocalDateTime = (date: Date): string => {
  if (!(date instanceof Date) || isNaN(date.getTime())) {
    return 'Invalid date';
  }

  // Calculate local time offset in milliseconds
  const offsetMs = date.getTimezoneOffset() * 60000;
  const localTime = date.getTime() - offsetMs;

  // Convert to ISO string and remove timezone info
  return new Date(localTime).toISOString().slice(0, 19);
};

/**
 * Format a date with both short date format and relative time
 */
export const prettyDateTime = (locale: string, date: Date): string => {
  if (!(date instanceof Date) || isNaN(date.getTime())) {
    return 'Invalid date';
  }

  if (!locale || typeof locale !== 'string') {
    locale = 'en';
  }

  const shortFormatter = getDateTimeFormatter({
    day: 'numeric',
    month: 'short',
    year: 'numeric'
  });

  const shortDate = shortFormatter.format(date);
  const relativeTime = formatTimeAgo(locale, date);

  return `${shortDate} (${relativeTime})`;
};
