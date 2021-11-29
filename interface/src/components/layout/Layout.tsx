import { FC, useState } from 'react';

import { Box, Toolbar } from '@mui/material';

import LayoutDrawer from './LayoutDrawer';
import LayoutAppBar from './LayoutAppBar';
import { LayoutContext } from './context';
import { PROJECT_NAME } from '../../api/env';

export const DRAWER_WIDTH = 230;

const Layout: FC = ({ children }) => {
  const [mobileOpen, setMobileOpen] = useState(false);
  const [title, setTitle] = useState(PROJECT_NAME);

  const handleDrawerToggle = () => {
    setMobileOpen(!mobileOpen);
  };

  return (
    <LayoutContext.Provider value={{ title, setTitle }}>
      <Box sx={{ display: 'flex' }}>
        <LayoutAppBar title={title} onToggleDrawer={handleDrawerToggle} />
        <LayoutDrawer mobileOpen={mobileOpen} onClose={handleDrawerToggle} />
        <Box component="main" sx={{ flexGrow: 1, width: { md: `calc(100% - ${DRAWER_WIDTH}px)` } }}>
          <Toolbar />
          {children}
        </Box>
      </Box>
    </LayoutContext.Provider>
  );
};

export default Layout;
