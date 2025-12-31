import { memo } from 'react';
import type { FC } from 'react';

import { Paper } from '@mui/material';
import type { SxProps, Theme } from '@mui/material/styles';

import type { RequiredChildrenProps } from 'utils';

interface SectionContentProps extends RequiredChildrenProps {
  id?: string;
}

// Extract styles to avoid recreation on every render
const paperStyles: SxProps<Theme> = {
  p: 1.5,
  m: 1.5,
  borderRadius: 3,
  border: '1px solid rgb(65, 65, 65)'
};

const SectionContent: FC<SectionContentProps> = ({ children, id }) => (
  <Paper id={id} sx={paperStyles}>
    {children}
  </Paper>
);

// Memoize to prevent unnecessary re-renders
export default memo(SectionContent);
