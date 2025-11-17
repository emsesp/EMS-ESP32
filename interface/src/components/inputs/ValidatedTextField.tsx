import { memo } from 'react';
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
      <TextField
        error={!!errors}
        {...rest}
        aria-label="Error"
        slotProps={{
          inputLabel: {
            style: rest.disabled ? { color: 'grey' } : undefined
          }
        }}
      />
      {errors?.map((e) => (
        <FormHelperText key={e.message} sx={{ color: 'rgb(250, 95, 84)' }}>
          {e.message}
        </FormHelperText>
      ))}
    </>
  );
};

export default memo(ValidatedTextField);
