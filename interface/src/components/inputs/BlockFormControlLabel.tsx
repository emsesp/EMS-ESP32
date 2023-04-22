import { FormControlLabel } from '@mui/material';
import type { FormControlLabelProps } from '@mui/material';
import type { FC } from 'react';

const BlockFormControlLabel: FC<FormControlLabelProps> = (props) => (
  <div>
    <FormControlLabel {...props} />
  </div>
);

export default BlockFormControlLabel;
