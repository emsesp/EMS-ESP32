const OPTIONAL = (validator: (value: any) => boolean) => (value: any) => !value || validator(value);

export default OPTIONAL;
