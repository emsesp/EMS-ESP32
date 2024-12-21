import { useEffect, useRef } from 'react';

const DEFAULT_DELAY = 3000;

// adapted from https://www.joshwcomeau.com/snippets/react-hooks/use-interval/
export const useInterval = (callback: () => void, delay: number = DEFAULT_DELAY) => {
  const intervalRef = useRef<number | null>(null);
  const savedCallback = useRef<() => void>(callback);

  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  useEffect(() => {
    const tick = () => savedCallback.current();
    intervalRef.current = window.setInterval(tick, delay);
    return () => {
      if (intervalRef.current !== null) {
        window.clearInterval(intervalRef.current);
      }
    };
  }, [delay]);

  return intervalRef;
};
