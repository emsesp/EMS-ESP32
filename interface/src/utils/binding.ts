export const numberValue = (value?: number) => {
  if (value !== undefined) {
    return isNaN(value) ? '' : value.toString();
  }
  return '';
};

export const extractEventValue = (event: React.ChangeEvent<HTMLInputElement>) => {
  switch (event.target.type) {
    case 'number':
      return event.target.valueAsNumber;
    case 'checkbox':
      return event.target.checked;
    default:
      return event.target.value;
  }
};

type UpdateEntity<S> = (state: (prevState: Readonly<S>) => S) => void;

export const updateValue =
  <S>(updateEntity: UpdateEntity<S>) =>
  (event: React.ChangeEvent<HTMLInputElement>) => {
    updateEntity((prevState) => ({
      ...prevState,
      [event.target.name]: extractEventValue(event)
    }));
  };

export const updateValueDirty =
  (
    origData,
    dirtyFlags: string[],
    setDirtyFlags: React.Dispatch<React.SetStateAction<string[]>>,
    updateDataValue: (unknown) => void
  ) =>
  (event: React.ChangeEvent<HTMLInputElement>) => {
    const updated_value = extractEventValue(event);
    const name = event.target.name;

    updateDataValue((prevState: unknown) => ({
      ...(prevState as Record<string, unknown>),
      [name]: updated_value
    }));

    const arr: string[] = dirtyFlags;

    if ((origData as Record<string, unknown>)[name] !== updated_value) {
      if (!arr.includes(name)) {
        arr.push(name);
      }
    } else {
      const startIndex = arr.indexOf(name);
      if (startIndex !== -1) {
        arr.splice(startIndex, 1);
      }
    }

    setDirtyFlags(arr);
  };
