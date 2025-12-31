import { useEffect, useMemo, useState } from 'react';

export const usePersistState = <T>(
  initial_value: T,
  id: string
): [T, (new_state: T) => void] => {
  // Set initial value - only computed once on mount
  const _initial_value = useMemo(() => {
    try {
      const local_storage_value_str = localStorage.getItem(`state:${id}`);
      // If there is a value stored in localStorage, use that
      if (local_storage_value_str) {
        return JSON.parse(local_storage_value_str) as T;
      }
    } catch (error) {
      // If parsing fails, fall back to initial_value
      console.warn(
        `Failed to parse localStorage value for key "state:${id}"`,
        error
      );
    }
    // Otherwise use initial_value that was passed to the function
    return initial_value;
  }, [id]); // initial_value intentionally omitted - only read on first mount

  const [state, setState] = useState(_initial_value);

  useEffect(() => {
    try {
      const state_str = JSON.stringify(state);
      localStorage.setItem(`state:${id}`, state_str);
    } catch (error) {
      console.warn(
        `Failed to save state to localStorage for key "state:${id}"`,
        error
      );
    }
  }, [state, id]);

  return [state, setState];
};
