import { FC } from 'react';

import { CircularProgress, Box, Typography, Theme } from '@mui/material';

interface LoadingSpinnerProps {
  height?: number | string;
}

const LoadingSpinner: FC<LoadingSpinnerProps> = ({ height = '100%' }) => (
  <Box display="flex" alignItems="center" justifyContent="center" flexDirection="column" padding={2} height={height}>
    <CircularProgress
      sx={(theme: Theme) => ({
        margin: theme.spacing(4),
        color: theme.palette.text.secondary
      })}
      size={100}
    />
    <Typography variant="h4" color="textSecondary">
      Loading&hellip;
    </Typography>
  </Box>
);

export default LoadingSpinner;
