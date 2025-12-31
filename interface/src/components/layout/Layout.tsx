import { memo, useCallback, useEffect, useMemo, useState } from 'react';
import type { FC } from 'react';
import { useLocation } from 'react-router';

import { Box, Toolbar } from '@mui/material';

import { PROJECT_NAME } from 'env';
import type { RequiredChildrenProps } from 'utils';

import LayoutAppBar from './LayoutAppBar';
import LayoutDrawer from './LayoutDrawer';
import { LayoutContext } from './context';

export const DRAWER_WIDTH = 210;

const LayoutComponent: FC<RequiredChildrenProps> = ({ children }) => {
  const [mobileOpen, setMobileOpen] = useState(false);
  const [title, setTitle] = useState(PROJECT_NAME);
  const { pathname } = useLocation();

  // Memoize drawer toggle handler to prevent unnecessary re-renders
  const handleDrawerToggle = useCallback(() => {
    setMobileOpen((prev) => !prev);
  }, []);

  // Close drawer when route changes
  useEffect(() => {
    setMobileOpen(false);
  }, [pathname]);

  // Memoize context value to prevent unnecessary re-renders
  const contextValue = useMemo(() => ({ title, setTitle }), [title]);

  return (
    <LayoutContext.Provider value={contextValue}>
      <LayoutAppBar title={title} onToggleDrawer={handleDrawerToggle} />
      <LayoutDrawer mobileOpen={mobileOpen} onClose={handleDrawerToggle} />
      <Box component="main" sx={{ marginLeft: { md: `${DRAWER_WIDTH}px` } }}>
        <Toolbar />
        {children}
      </Box>
    </LayoutContext.Provider>
  );
};

const Layout = memo(LayoutComponent);

export default Layout;
