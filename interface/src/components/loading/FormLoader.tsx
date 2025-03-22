import RefreshIcon from '@mui/icons-material/Refresh';
import { Box, Button, CircularProgress } from '@mui/material';

import { MessageBox } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

interface FormLoaderProps {
  errorMessage?: string;
  onRetry?: () => void;
}

const FormLoader = ({ errorMessage, onRetry }: FormLoaderProps) => {
  const { LL } = useI18nContext();

  if (errorMessage) {
    return (
      <MessageBox my={2} level="error" message={errorMessage}>
        {onRetry && (
          <Button
            sx={{ ml: 2 }}
            startIcon={<RefreshIcon />}
            variant="contained"
            color="error"
            onClick={onRetry}
          >
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
    </Box>
  );
};

export default FormLoader;
