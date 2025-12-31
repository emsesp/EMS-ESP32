import { memo } from 'react';

import RefreshIcon from '@mui/icons-material/Refresh';
import { Box, Button, CircularProgress } from '@mui/material';

import { MessageBox } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

interface FormLoaderProps {
  errorMessage?: string;
  onRetry?: () => void;
}

const FormLoaderComponent = ({ errorMessage, onRetry }: FormLoaderProps) => {
  const { LL } = useI18nContext();

  if (errorMessage) {
    return (
      <MessageBox level="error" message={errorMessage}>
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

const FormLoader = memo(FormLoaderComponent);

export default FormLoader;
