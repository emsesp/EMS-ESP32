export default (validator1: (value: any) => boolean, validator2: (value: any) => boolean) => {
    return (value: any) => validator1(value) || validator2(value);
}
