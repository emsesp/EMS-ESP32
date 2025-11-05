import { useEffect, useRef } from 'react';

const DEFAULT_DELAY = 3000;

/**
 * Custom hook for setting up an interval with proper cleanup
 * Adapted from https://www.joshwcomeau.com/snippets/react-hooks/use-interval/
 *
 * @param callback - Function to be called at each interval
 * @param delay - Delay in milliseconds (default: 3000ms)
 * @param immediate - If true, executes callback immediately on mount (default: false)
 * @returns Reference to the interval ID
 */
export const useInterval = (
  callback: () => void,
  delay: number = DEFAULT_DELAY,
  immediate = false
) => {
  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const savedCallback = useRef(callback);

  // Remember the latest callback without resetting the interval
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  useEffect(() => {
    const tick = () => savedCallback.current();

    // Execute immediately if requested
    if (immediate) {
      tick();
    }

    intervalRef.current = setInterval(tick, delay);
    return () => {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
    };
  }, [delay, immediate]);

  return intervalRef;
};
