import { useEffect, useRef } from 'react';

const DEFAULT_DELAY = 5000;

// Hosted demo (vite build --mode hosted): no automatic data refresh. The mock API
// returns static data, and every poll is a billed Cloudflare Worker request.
// Vite folds this to `false` in production/firmware builds, so nothing changes there.
const DEMO = import.meta.env.MODE === 'hosted';

/**
 * Custom hook for setting up an interval with proper cleanup
 * Adapted from https://www.joshwcomeau.com/snippets/react-hooks/use-interval/
 *
 * @param callback - Function to be called at each interval
 * @param delay - Delay in milliseconds (default: 5000ms)
 * @param immediate - If true, executes callback immediately on mount (default: false)
 * @param always - Keep polling in the hosted demo too (for flows that wait on a result,
 *                 e.g. restart monitor). Default false: demo does not auto-refresh.
 * @returns Reference to the interval ID
 */
export const useInterval = (
  callback: () => void,
  delay: number = DEFAULT_DELAY,
  immediate = false,
  always = false
) => {
  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const savedCallback = useRef(callback);

  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  useEffect(() => {
    const tick = () => savedCallback.current();

    if (immediate) {
      tick();
    }

    if (DEMO && !always) {
      return;
    }

    intervalRef.current = setInterval(tick, delay);
    return () => {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
    };
  }, [delay, immediate, always]);

  return intervalRef;
};
