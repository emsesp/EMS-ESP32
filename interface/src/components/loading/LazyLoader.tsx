import { memo } from 'react';

import { Box, CircularProgress } from '@mui/material';
import type { SxProps, Theme } from '@mui/material';

// Extract styles to prevent recreation on every render
const containerStyles: SxProps<Theme> = {
  display: 'flex',
  justifyContent: 'center',
  alignItems: 'center',
  minHeight: '200px',
  backgroundColor: 'background.default',
  borderRadius: 1,
  border: '1px solid',
  borderColor: 'divider'
};

const LazyLoader = memo(() => (
  <Box sx={containerStyles}>
    <CircularProgress size={40} />
  </Box>
));

export default LazyLoader;
