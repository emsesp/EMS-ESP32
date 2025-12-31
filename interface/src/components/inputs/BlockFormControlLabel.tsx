import { memo } from 'react';
import type { FC } from 'react';

import { FormControlLabel } from '@mui/material';
import type { FormControlLabelProps } from '@mui/material';

const BlockFormControlLabel: FC<FormControlLabelProps> = (props) => (
  <div>
    <FormControlLabel {...props} />
  </div>
);

export default memo(BlockFormControlLabel);
