import { Box, Toolbar } from '@mui/material';
import { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import LayoutAppBar from './LayoutAppBar';
import LayoutDrawer from './LayoutDrawer';
import { LayoutContext } from './context';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';
import { PROJECT_NAME } from 'api/env';

export const DRAWER_WIDTH = 240;

const Layout: FC<RequiredChildrenProps> = ({ children }) => {
  const [mobileOpen, setMobileOpen] = useState(false);
  const [title, setTitle] = useState(PROJECT_NAME);
  const { pathname } = useLocation();

  const handleDrawerToggle = () => {
    setMobileOpen(!mobileOpen);
  };

  useEffect(() => setMobileOpen(false), [pathname]);

  return (
    <LayoutContext.Provider value={{ title, setTitle }}>
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
