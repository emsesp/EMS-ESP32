import { useEffect, useRef } from 'react';

import { demoPaused, subscribeDemoPaused } from './demoIdle';

// Vite replaces import.meta.env.MODE at build time, so in device/production builds
// this is `false` and the demo-idle code below is removed by the bundler.
const DEMO = import.meta.env.MODE === 'hosted';

const DEFAULT_DELAY = 5000;

/**
 * Custom hook for setting up an interval with proper cleanup
 * Adapted from https://www.joshwcomeau.com/snippets/react-hooks/use-interval/
 *
 * @param callback - Function to be called at each interval
 * @param delay - Delay in milliseconds (default: 3000ms)
 * @param immediate - If true, executes callback immediately on mount (default: false)
 * @returns Reference to the interval ID
 *
 * In the hosted demo, ticks are skipped while the tab is hidden or idle (see demoIdle.ts),
 * and one tick runs straight away when the user comes back so the data is fresh.
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
    const run = () => savedCallback.current();
    const tick = DEMO
      ? () => {
          if (!demoPaused()) run();
        }
      : run;

    // Execute immediately if requested
    if (immediate) {
      tick();
    }

    intervalRef.current = setInterval(tick, delay);

    // refresh as soon as the demo wakes up again
    const unsubscribe = DEMO
      ? subscribeDemoPaused(() => {
          if (!demoPaused()) run();
        })
      : undefined;

    return () => {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
      unsubscribe?.();
    };
  }, [delay, immediate]);

  return intervalRef;
};
