import type { FC } from 'react';
import { useNavigate } from 'react-router';

import { Tabs, useMediaQuery, useTheme } from '@mui/material';

import type { RequiredChildrenProps } from 'utils';

interface RouterTabsProps extends RequiredChildrenProps {
  value: string | false;
}

const RouterTabs: FC<RouterTabsProps> = ({ value, children }) => {
  const navigate = useNavigate();

  const theme = useTheme();
  const smallDown = useMediaQuery(theme.breakpoints.down('sm'));

  const handleTabChange = (_event: unknown, path: string) => {
    void navigate(path);
  };

  return (
    <Tabs
      value={value}
      onChange={handleTabChange}
      variant={smallDown ? 'scrollable' : 'fullWidth'}
    >
      {children}
    </Tabs>
  );
};

export default RouterTabs;
