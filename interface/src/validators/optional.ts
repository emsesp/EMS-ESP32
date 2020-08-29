export default (validator: (value: any) => boolean) => (value: any) => !value || validator(value);
