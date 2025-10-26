import type { FC } from 'react';

import { Paper } from '@mui/material';

import type { RequiredChildrenProps } from 'utils';

interface SectionContentProps extends RequiredChildrenProps {
  id?: string;
}

const SectionContent: FC<SectionContentProps> = (props) => {
  const { children, id } = props;
  return (
    <Paper
      id={id}
      sx={{ p: 1.5, m: 1.5, borderRadius: 3, border: '1px solid rgb(65, 65, 65)' }}
    >
      {children}
    </Paper>
  );
};

export default SectionContent;
