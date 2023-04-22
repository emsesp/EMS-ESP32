import { Box, Divider, Drawer, Toolbar, Typography, styled } from '@mui/material';
import { DRAWER_WIDTH } from './Layout';
import LayoutMenu from './LayoutMenu';
import type { FC } from 'react';

import { PROJECT_NAME } from 'api/env';

const LayoutDrawerLogo = styled('img')(({ theme }) => ({
  [theme.breakpoints.down('sm')]: {
    height: 24,
    marginRight: theme.spacing(2)
  },
  [theme.breakpoints.up('sm')]: {
    height: 36,
    marginRight: theme.spacing(2)
  }
}));

interface LayoutDrawerProps {
  mobileOpen: boolean;
  onClose: () => void;
}

const LayoutDrawer: FC<LayoutDrawerProps> = ({ mobileOpen, onClose }) => {
  const drawer = (
    <>
      <Toolbar disableGutters>
        <Box display="flex" alignItems="center" px={2}>
          <LayoutDrawerLogo src="/app/icon.png" alt={PROJECT_NAME} />
          <Typography variant="h6" color="textPrimary">
            {PROJECT_NAME}
          </Typography>
        </Box>
        <Divider absolute />
      </Toolbar>
      <Divider />
      <LayoutMenu />
    </>
  );

  return (
    <Box component="nav" sx={{ width: { md: DRAWER_WIDTH }, flexShrink: { md: 0 } }}>
      <Drawer
        variant="temporary"
        open={mobileOpen}
        onClose={onClose}
        ModalProps={{
          keepMounted: true
        }}
        sx={{
          display: { xs: 'block', md: 'none' },
          '& .MuiDrawer-paper': { boxSizing: 'border-box', width: DRAWER_WIDTH }
        }}
      >
        {drawer}
      </Drawer>
      <Drawer
        variant="permanent"
        sx={{
          display: { xs: 'none', md: 'block' },
          '& .MuiDrawer-paper': { boxSizing: 'border-box', width: DRAWER_WIDTH }
        }}
        open
      >
        {drawer}
      </Drawer>
    </Box>
  );
};

export default LayoutDrawer;
