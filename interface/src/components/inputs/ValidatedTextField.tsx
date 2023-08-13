import { FormHelperText, TextField } from '@mui/material';
import type { TextFieldProps } from '@mui/material';
import type { ValidateFieldsError } from 'async-validator';
import type { FC } from 'react';

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
