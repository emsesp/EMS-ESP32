import type { FC } from 'react';

import { Divider, Paper } from '@mui/material';

import type { RequiredChildrenProps } from 'utils';

interface SectionContentProps extends RequiredChildrenProps {
  title?: string;
  id?: string;
}

const SectionContent: FC<SectionContentProps> = (props) => {
  const { children, title, id } = props;
  return (
    <Paper id={id} sx={{ p: 2, m: 2 }}>
      {title && (
        <Divider sx={{ pb: 2, borderColor: 'primary.main', fontSize: 20, color: 'primary.main' }}>{title}</Divider>
      )}
      {children}
    </Paper>
  );
};

export default SectionContent;
