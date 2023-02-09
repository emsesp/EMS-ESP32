import { FC } from 'react';

import { Box, Button, CircularProgress, Typography } from '@mui/material';
import RefreshIcon from '@mui/icons-material/Refresh';

import { MessageBox } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

interface FormLoaderProps {
  message?: string;
  errorMessage?: string;
  onRetry?: () => void;
}

const FormLoader: FC<FormLoaderProps> = ({ errorMessage, onRetry, message = 'Loading…' }) => {
  const { LL } = useI18nContext();

  if (errorMessage) {
    return (
      <MessageBox my={2} level="error" message={errorMessage}>
        {onRetry && (
          <Button startIcon={<RefreshIcon />} variant="contained" color="error" onClick={onRetry}>
            {LL.RETRY()}
          </Button>
        )}
      </MessageBox>
    );
  }
  return (
    <Box m={2} py={2} display="flex" alignItems="center" flexDirection="column">
      <Box py={2}>
        <CircularProgress size={100} />
      </Box>
      <Typography variant="h6" fontWeight={400} textAlign="center">
        {message}
      </Typography>
    </Box>
  );
};

export default FormLoader;
