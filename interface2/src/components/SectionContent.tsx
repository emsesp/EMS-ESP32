import React from 'react';

import { Paper, Divider } from '@mui/material';

interface SectionContentProps {
  title: string;
  titleGutter?: boolean;
}

const SectionContent: React.FC<SectionContentProps> = (props) => {
  const { children, title } = props;
  return (
    <Paper sx={{ p: 2, m: 3 }}>
      <Divider sx={{ pb: 2, borderColor: 'primary.main', color: 'primary.main', fontSize: 20, fontWeight: 'medium' }}>
        {title}
      </Divider>
      {children}
    </Paper>
  );
};

export default SectionContent;
