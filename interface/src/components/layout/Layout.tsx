import { useEffect, useMemo, useState } from 'react';
import type { FC } from 'react';
import { useLocation } from 'react-router';

import { Box, Toolbar } from '@mui/material';

import { PROJECT_NAME } from 'env';
import type { RequiredChildrenProps } from 'utils';

import LayoutAppBar from './LayoutAppBar';
import LayoutDrawer from './LayoutDrawer';
import { LayoutContext } from './context';

export const DRAWER_WIDTH = 210;

const Layout: FC<RequiredChildrenProps> = ({ children }) => {
  const [mobileOpen, setMobileOpen] = useState(false);
  const [title, setTitle] = useState(PROJECT_NAME);
  const { pathname } = useLocation();

  const handleDrawerToggle = () => {
    setMobileOpen(!mobileOpen);
  };

  useEffect(() => setMobileOpen(false), [pathname]);

  // cache the object to prevent unnecessary re-renders
  const obj = useMemo(() => ({ title, setTitle }), [title]);

  return (
    <LayoutContext.Provider value={obj}>
      <LayoutAppBar title={title} onToggleDrawer={handleDrawerToggle} />
      <LayoutDrawer mobileOpen={mobileOpen} onClose={handleDrawerToggle} />
      <Box component="main" sx={{ marginLeft: { md: `${DRAWER_WIDTH}px` } }}>
        <Toolbar />
        {children}
      </Box>
    </LayoutContext.Provider>
  );
};

export default Layout;
