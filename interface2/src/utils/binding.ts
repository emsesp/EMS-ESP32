type UpdateEntity<S> = (
  state: ((prevState: Readonly<S>) => S)
) => void;

export const extractEventValue = (event: React.ChangeEvent<HTMLInputElement>) => {
  switch (event.target.type) {
    case "number":
      return event.target.valueAsNumber;
    case "checkbox":
      return event.target.checked;
    default:
      return event.target.value;
  }
};

export const updateValue = <S>(updateEntity: UpdateEntity<S>) => (
  (event: React.ChangeEvent<HTMLInputElement>) => {
    updateEntity((prevState) => ({
      ...prevState,
      [event.target.name]: extractEventValue(event)
    }));
  }
);
