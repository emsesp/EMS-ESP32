import { useEffect, useState } from 'react';
import type { FC } from 'react';
import { useLocation } from 'react-router-dom';

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

  return (
    // TODO wrap title/setTitle in a useMemo()
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
