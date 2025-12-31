// Cache for formatters to avoid recreation (with size limits to prevent memory leaks)
const MAX_CACHE_SIZE = 50;
const formatterCache = new Map<string, Intl.DateTimeFormat>();
const rtfCache = new Map<string, Intl.RelativeTimeFormat>();

// Pre-computed constants
const MS_TO_MINUTES = 60000; // 60 * 1000
const TIME_DIVISIONS = [
  { amount: 60, name: 'seconds' as const },
  { amount: 60, name: 'minutes' as const },
  { amount: 24, name: 'hours' as const },
  { amount: 7, name: 'days' as const },
  { amount: 4.34524, name: 'weeks' as const },
  { amount: 12, name: 'months' as const },
  { amount: Number.POSITIVE_INFINITY, name: 'years' as const }
] as const;

// Cached navigator languages to avoid repeated array spreads
let cachedLanguages: readonly string[] | null = null;

/**
 * Get navigator languages with caching
 */
function getNavigatorLanguages(): readonly string[] {
  if (!cachedLanguages) {
    cachedLanguages = window.navigator.languages;
  }
  return cachedLanguages;
}

/**
 * Create a fast cache key from DateTimeFormat options
 */
function createFormatterKey(options: Intl.DateTimeFormatOptions): string {
  // Build key from most common properties for better performance than JSON.stringify
  return `${options.day}-${options.month}-${options.year}-${options.hour}-${options.minute}-${options.second}-${options.hour12}`;
}

/**
 * Get or create a cached DateTimeFormat instance with LRU-like cache management
 */
function getDateTimeFormatter(
  options: Intl.DateTimeFormatOptions
): Intl.DateTimeFormat {
  const key = createFormatterKey(options);

  if (formatterCache.has(key)) {
    // Move to end for LRU behavior
    const formatter = formatterCache.get(key)!;
    formatterCache.delete(key);
    formatterCache.set(key, formatter);
    return formatter;
  }

  // Limit cache size
  if (formatterCache.size >= MAX_CACHE_SIZE) {
    const firstKey = formatterCache.keys().next().value;
    if (firstKey) {
      formatterCache.delete(firstKey);
    }
  }

  const formatter = new Intl.DateTimeFormat(getNavigatorLanguages(), options);
  formatterCache.set(key, formatter);
  return formatter;
}

/**
 * Get or create a cached RelativeTimeFormat instance with cache size management
 */
function getRelativeTimeFormatter(locale: string): Intl.RelativeTimeFormat {
  if (rtfCache.has(locale)) {
    // Move to end for LRU behavior
    const formatter = rtfCache.get(locale)!;
    rtfCache.delete(locale);
    rtfCache.set(locale, formatter);
    return formatter;
  }

  // Limit cache size
  if (rtfCache.size >= MAX_CACHE_SIZE) {
    const firstKey = rtfCache.keys().next().value;
    if (firstKey) {
      rtfCache.delete(firstKey);
    }
  }

  const formatter = new Intl.RelativeTimeFormat(locale, { numeric: 'auto' });
  rtfCache.set(locale, formatter);
  return formatter;
}

/**
 * Format a date as relative time (e.g., "2 hours ago", "in 3 days")
 */
function formatTimeAgo(locale: string, date: Date): string {
  const now = Date.now();
  const targetTime = date.getTime();
  let duration = (targetTime - now) / 1000;

  const rtf = getRelativeTimeFormatter(locale);

  // Find the appropriate time division
  for (const division of TIME_DIVISIONS) {
    if (Math.abs(duration) < division.amount) {
      return rtf.format(Math.round(duration), division.name);
    }
    duration /= division.amount;
  }

  // This should never be reached due to POSITIVE_INFINITY in divisions
  return rtf.format(Math.round(duration), 'years');
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

  // Calculate local time offset using pre-computed constant
  const offsetMs = date.getTimezoneOffset() * MS_TO_MINUTES;
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
