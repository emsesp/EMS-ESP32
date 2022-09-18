import { FC } from 'react';

import { CircularProgress, Box, Typography, Theme } from '@mui/material';

import { useI18nContext } from '../../i18n/i18n-react';

interface LoadingSpinnerProps {
  height?: number | string;
}

const LoadingSpinner: FC<LoadingSpinnerProps> = ({ height = '100%' }) => {
  const { LL } = useI18nContext();

  return (
    <Box display="flex" alignItems="center" justifyContent="center" flexDirection="column" padding={2} height={height}>
      <CircularProgress
        sx={(theme: Theme) => ({
          margin: theme.spacing(4),
          color: theme.palette.text.secondary
        })}
        size={100}
      />
      <Typography variant="h4" color="textSecondary">
        {LL.LOADING()}&hellip;
      </Typography>
    </Box>
  );
};

export default LoadingSpinner;
