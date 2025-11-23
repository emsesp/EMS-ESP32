import { memo } from 'react';

import { Box, CircularProgress } from '@mui/material';
import type { SxProps, Theme } from '@mui/material';

interface LoadingSpinnerProps {
  height?: number | string;
}

const circularProgressStyles: SxProps<Theme> = (theme: Theme) => ({
  margin: theme.spacing(4),
  color: theme.palette.text.secondary
});

const LoadingSpinner = ({ height = '100%' }: LoadingSpinnerProps) => {
  return (
    <Box
      display="flex"
      alignItems="center"
      justifyContent="center"
      flexDirection="column"
      padding={2}
      height={height}
    >
      <CircularProgress sx={circularProgressStyles} size={100} />
    </Box>
  );
};

export default memo(LoadingSpinner);
