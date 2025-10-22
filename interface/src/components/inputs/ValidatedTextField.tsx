import type { FC } from 'react';

import { FormHelperText, TextField } from '@mui/material';
import type { TextFieldProps } from '@mui/material';

import type { ValidateFieldsError } from 'async-validator';

interface ValidatedFieldProps {
  fieldErrors?: ValidateFieldsError;
  name: string;
}

export type ValidatedTextFieldProps = ValidatedFieldProps & TextFieldProps;

const ValidatedTextField: FC<ValidatedTextFieldProps> = ({
  fieldErrors,
  ...rest
}) => {
  const errors = fieldErrors?.[rest.name];

  return (
    <>
      <TextField error={!!errors} {...rest} />
      {errors?.map((e) => (
        <FormHelperText key={e.message}>{e.message}</FormHelperText>
      ))}
    </>
  );
};

export default ValidatedTextField;
