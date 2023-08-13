import { Tabs, useMediaQuery, useTheme } from '@mui/material';
import { useNavigate } from 'react-router-dom';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';

interface RouterTabsProps extends RequiredChildrenProps {
  value: string | false;
}

const RouterTabs: FC<RouterTabsProps> = ({ value, children }) => {
  const navigate = useNavigate();

  const theme = useTheme();
  const smallDown = useMediaQuery(theme.breakpoints.down('sm'));

  const handleTabChange = (event: React.ChangeEvent<HTMLInputElement>, path: string) => {
    navigate(path);
  };

  return (
    <Tabs value={value} onChange={handleTabChange} variant={smallDown ? 'scrollable' : 'fullWidth'}>
      {children}
    </Tabs>
  );
};

export default RouterTabs;
