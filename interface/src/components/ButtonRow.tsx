import { memo } from 'react';

import { Box } from '@mui/material';
import type { BoxProps } from '@mui/material';

const ButtonRow = memo<BoxProps>(({ children, ...rest }) => (
  <Box
    sx={{
      '& button, & a, & .MuiCard-root': {
        mt: 2,
        mx: 0.6,
        '&:last-child': { mr: 0 },
        '&:first-of-type': { ml: 0 }
      }
    }}
    {...rest}
  >
    {children}
  </Box>
));

export default ButtonRow;
