import React, { FC } from 'react';
import { useNavigate } from 'react-router-dom';

import { Tabs, useMediaQuery, useTheme } from '@mui/material';

import { RequiredChildrenProps } from 'utils';

interface RouterTabsProps extends RequiredChildrenProps {
  value: string | false;
}

const RouterTabs: FC<RouterTabsProps> = ({ value, children }) => {
  const navigate = useNavigate();

  const theme = useTheme();
  const smallDown = useMediaQuery(theme.breakpoints.down('sm'));

  // TODO fix this
  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    navigate(path);
  };

  return (
    <Tabs value={value} onChange={handleTabChange} variant={smallDown ? 'scrollable' : 'fullWidth'}>
      {children}
    </Tabs>
  );
};

export default RouterTabs;
