import { useState } from 'react';
import type { FC } from 'react';

import VisibilityIcon from '@mui/icons-material/Visibility';
import VisibilityOffIcon from '@mui/icons-material/VisibilityOff';
import { IconButton, InputAdornment } from '@mui/material';

import ValidatedTextField from './ValidatedTextField';
import type { ValidatedTextFieldProps } from './ValidatedTextField';

type ValidatedPasswordFieldProps = Omit<ValidatedTextFieldProps, 'type'>;

const ValidatedPasswordField: FC<ValidatedPasswordFieldProps> = ({ ...props }) => {
  const [showPassword, setShowPassword] = useState<boolean>(false);

  return (
    <ValidatedTextField
      {...props}
      type={showPassword ? 'text' : 'password'}
      slotProps={{
        input: {
          endAdornment: (
            <InputAdornment position="end">
              <IconButton onClick={() => setShowPassword(!showPassword)} edge="end">
                {showPassword ? <VisibilityIcon /> : <VisibilityOffIcon />}
              </IconButton>
            </InputAdornment>
          )
        }
      }}
    />
  );
};

export default ValidatedPasswordField;
