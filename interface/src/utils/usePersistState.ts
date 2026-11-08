import { useEffect, useState } from 'react';

export const usePersistState = <T>(
  initial_value: T,
  id: string
): [T, (new_state: T) => void] => {
  const [state, setState] = useState<T>(() => {
    try {
      const stored = localStorage.getItem(`state:${id}`);
      if (stored) {
        return JSON.parse(stored) as T;
      }
    } catch (error) {
      console.warn(
        `Failed to parse localStorage value for key "state:${id}"`,
        error
      );
    }
    return initial_value;
  });

  useEffect(() => {
    try {
      localStorage.setItem(`state:${id}`, JSON.stringify(state));
    } catch (error) {
      console.warn(
        `Failed to save state to localStorage for key "state:${id}"`,
        error
      );
    }
  }, [state, id]);

  return [state, setState];
};
