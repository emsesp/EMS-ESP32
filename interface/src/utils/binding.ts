/**
 * Converts a number value to a string for input fields.
 * Returns empty string for undefined or NaN values.
 */
export const numberValue = (value?: number): string =>
  value === undefined || isNaN(value) ? '' : String(value);

/**
 * Extracts the appropriate value from an input event based on input type.
 */
export const extractEventValue = (
  event: React.ChangeEvent<HTMLInputElement>
): string | number | boolean => {
  const { type, checked, value } = event.target;

  if (type === 'number') {
    if (value === '') return NaN;

    const normalizedValue = value.replace(',', '.');

    // For incomplete number entries, keep the raw string to allow smooth typing
    // This includes: "0.", "1.0", "0.00", "-", "-.", "-.5", etc.
    const endsWithDecimalAndZeros = /\.\d*0$/.test(normalizedValue);
    const isIncomplete =
      normalizedValue.endsWith('.') ||
      normalizedValue === '-' ||
      normalizedValue === '-.' ||
      endsWithDecimalAndZeros;

    if (isIncomplete) {
      return normalizedValue;
    }

    const parsedValue = parseFloat(normalizedValue);
    return parsedValue;
  }
  if (type === 'checkbox') return checked;
  return value;
};

type UpdateEntity<S> = (state: (prevState: Readonly<S>) => S) => void;

/**
 * Creates an event handler that updates an entity's state based on input changes.
 */
export const updateValue =
  <S extends Record<string, unknown>>(updateEntity: UpdateEntity<S>) =>
  (event: React.ChangeEvent<HTMLInputElement>): void => {
    const { name } = event.target;
    const value = extractEventValue(event);

    updateEntity((prevState) => ({
      ...prevState,
      [name]: value
    }));
  };

/**
 * Creates an event handler that tracks dirty flags for modified fields.
 * Optimized to minimize state updates and unnecessary array operations.
 */
export const updateValueDirty =
  <T extends Record<string, unknown>>(
    origData: T,
    dirtyFlags: string[],
    setDirtyFlags: React.Dispatch<React.SetStateAction<string[]>>,
    updateDataValue: (updater: (prevState: T) => T) => void
  ) =>
  (event: React.ChangeEvent<HTMLInputElement>): void => {
    const { name } = event.target;
    const updatedValue = extractEventValue(event);

    updateDataValue((prevState) => ({
      ...prevState,
      [name]: updatedValue
    }));

    const isDirty = origData[name] !== updatedValue;
    const wasDirty = dirtyFlags.includes(name);

    // Only update dirty flags if the state changed
    if (isDirty !== wasDirty) {
      setDirtyFlags(
        isDirty ? [...dirtyFlags, name] : dirtyFlags.filter((f) => f !== name)
      );
    }
  };
