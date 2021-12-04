import { FC } from 'react';
import { Box, BoxProps } from '@mui/material';

const ButtonRow: FC<BoxProps> = ({ children, ...rest }) => {
  return (
    <Box
      sx={{
        '& button, & a, & .MuiCard-root': {
          mt: 2,
          mx: 0.6,
          '&:last-child': {
            mr: 0
          },
          '&:first-of-type': {
            ml: 0
          }
        }
      }}
      {...rest}
    >
      {children}
    </Box>
  );
};

export default ButtonRow;
