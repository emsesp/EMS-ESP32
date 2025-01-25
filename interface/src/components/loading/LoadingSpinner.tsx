import { Box, CircularProgress } from '@mui/material';
import type { Theme } from '@mui/material';

interface LoadingSpinnerProps {
  height?: number | string;
}

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
      <CircularProgress
        sx={(theme: Theme) => ({
          margin: theme.spacing(4),
          color: theme.palette.text.secondary
        })}
        size={100}
      />
    </Box>
  );
};

export default LoadingSpinner;
