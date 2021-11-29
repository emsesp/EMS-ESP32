import { FC } from 'react';
import { ValidateFieldsError } from 'async-validator';

import { FormHelperText, TextField, TextFieldProps } from '@mui/material';

interface ValidatedFieldProps {
  fieldErrors?: ValidateFieldsError;
  name: string;
}

export type ValidatedTextFieldProps = ValidatedFieldProps & TextFieldProps;

const ValidatedTextField: FC<ValidatedTextFieldProps> = ({ fieldErrors, ...rest }) => {
  const errors = fieldErrors && fieldErrors[rest.name];
  const renderErrors = () => errors && errors.map((e, i) => <FormHelperText key={i}>{e.message}</FormHelperText>);
  return (
    <>
      <TextField error={!!errors} {...rest} />
      {renderErrors()}
    </>
  );
};

export default ValidatedTextField;
