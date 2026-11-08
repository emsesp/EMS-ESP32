import { memo, useContext } from 'react';

import { Box, Divider, Drawer, Toolbar, Typography, styled } from '@mui/material';

import { AuthenticatedContext } from 'contexts/authentication';
import { PROJECT_NAME } from 'env';

import { DRAWER_WIDTH } from './Layout';
import LayoutMenu from './LayoutMenu';

const LayoutDrawerLogo = styled('img')(({ theme }) => ({
  [theme.breakpoints.down('sm')]: {
    height: 24,
    marginRight: theme.spacing(2)
  },
  [theme.breakpoints.up('sm')]: {
    height: 38,
    marginRight: theme.spacing(2)
  }
}));

interface LayoutDrawerProps {
  mobileOpen: boolean;
  onClose: () => void;
}

const LayoutDrawerComponent = ({ mobileOpen, onClose }: LayoutDrawerProps) => {
  const { systemName } = useContext(AuthenticatedContext);

  const drawer = (
    <>
      <Toolbar disableGutters>
        <Box sx={{ display: 'flex', alignItems: 'center', p: 2 }}>
          <LayoutDrawerLogo src="/app/icon.png" alt={PROJECT_NAME} />
          <Box
            sx={{
              display: 'flex',
              flexDirection: 'column',
              justifyContent: 'center'
            }}
          >
            <Typography>{PROJECT_NAME}</Typography>
            {systemName && (
              <Typography color="secondary" variant="body2">
                {systemName}
              </Typography>
            )}
          </Box>
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

const LayoutDrawer = memo(LayoutDrawerComponent);

export default LayoutDrawer;
