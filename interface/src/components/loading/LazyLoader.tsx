import { memo } from 'react';

import { Box, CircularProgress } from '@mui/material';

const LazyLoader = memo(() => (
  <Box
    display="flex"
    justifyContent="center"
    alignItems="center"
    minHeight="200px"
    sx={{
      backgroundColor: 'background.default',
      borderRadius: 1,
      border: '1px solid',
      borderColor: 'divider'
    }}
  >
    <CircularProgress size={40} />
  </Box>
));

export default LazyLoader;
