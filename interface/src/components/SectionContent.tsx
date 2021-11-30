import { FC } from 'react';

import { Paper, Divider } from '@mui/material';

interface SectionContentProps {
  title: string;
  titleGutter?: boolean;
  id?: string;
}

const SectionContent: FC<SectionContentProps> = (props) => {
  const { children, title, id } = props;
  return (
    <Paper id={id} sx={{ p: 2, m: 3 }}>
      <Divider sx={{ pb: 2, borderColor: 'primary.main', fontSize: 20, color: 'primary.main' }}>{title}</Divider>
      {children}
    </Paper>
  );
};

export default SectionContent;
