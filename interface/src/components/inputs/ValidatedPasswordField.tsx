import VisibilityIcon from '@mui/icons-material/Visibility';
import VisibilityOffIcon from '@mui/icons-material/VisibilityOff';
import { IconButton, InputAdornment } from '@mui/material';
import { useState } from 'react';

import ValidatedTextField from './ValidatedTextField';
import type { ValidatedTextFieldProps } from './ValidatedTextField';
import type { FC } from 'react';

type ValidatedPasswordFieldProps = Omit<ValidatedTextFieldProps, 'type'>;

const ValidatedPasswordField: FC<ValidatedPasswordFieldProps> = ({ InputProps, ...props }) => {
  const [showPassword, setShowPassword] = useState<boolean>(false);

  return (
    <ValidatedTextField
      {...props}
      type={showPassword ? 'text' : 'password'}
      InputProps={{
        ...InputProps,
        endAdornment: (
          <InputAdornment position="end">
            <IconButton onClick={() => setShowPassword(!showPassword)} edge="end">
              {showPassword ? <VisibilityIcon /> : <VisibilityOffIcon />}
            </IconButton>
          </InputAdornment>
        )
      }}
    />
  );
};

export default ValidatedPasswordField;
