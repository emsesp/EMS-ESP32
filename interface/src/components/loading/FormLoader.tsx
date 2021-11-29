import { FC } from 'react';

import { Box, Button, LinearProgress, Typography } from '@mui/material';

interface FormLoaderProps {
  message?: string;
  errorMessage?: string;
  loadData?: () => void;
}

const FormLoader: FC<FormLoaderProps> = ({ errorMessage, loadData, message = '' }) => {
  if (errorMessage) {
    return (
      <Box m={1} display="flex" flexDirection="column">
        <Typography variant="h6" textAlign="center">
          {errorMessage}
        </Typography>
        {loadData && (
          <Button variant="outlined" color="secondary" onClick={loadData}>
            Retry
          </Button>
        )}
      </Box>
    );
  }
  return (
    <Box m={1} display="flex" flexDirection="column">
      <LinearProgress />
      <Typography variant="h6" textAlign="center">
        {message}
      </Typography>
    </Box>
  );
};

export default FormLoader;
