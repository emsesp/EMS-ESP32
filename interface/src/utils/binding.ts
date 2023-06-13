export const numberValue = (value: number) => (isNaN(value) ? '' : value.toString());

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
  (origData: any, dirtyFlags: any, setDirtyFlags: any, updateDataValue: any) =>
  (event: React.ChangeEvent<HTMLInputElement>) => {
    const updated_value = extractEventValue(event);
    const name = event.target.name;

    // TODO not sure how this is even working!!
    updateDataValue((prevState) => ({
      ...prevState,
      [name]: updated_value
    }));

    const arr: string[] = dirtyFlags;

    // TODO remove comments
    // console.log('updating ' + name + ' to ' + updated_value);
    // console.log('dirtyFlags:', dirtyFlags);
    // console.log('binding.ts origData:', origData);

    if (origData[name] !== updated_value) {
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
