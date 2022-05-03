import { FC, useContext } from 'react';

import { AppBar, Box, IconButton, Toolbar, Typography } from '@mui/material';
import MenuIcon from '@mui/icons-material/Menu';

import LayoutAuthMenu from './LayoutAuthMenu';

import { FeaturesContext } from '../../contexts/features';

export const DRAWER_WIDTH = 240;

interface LayoutAppBarProps {
  title: string;
  onToggleDrawer: () => void;
}

const LayoutAppBar: FC<LayoutAppBarProps> = ({ title, onToggleDrawer }) => {
  const { features } = useContext(FeaturesContext);

  return (
    <AppBar
      position="fixed"
      sx={{
        width: { md: `calc(100% - ${DRAWER_WIDTH}px)` },
        ml: { md: `${DRAWER_WIDTH}px` },
        boxShadow: 'none',
        backgroundColor: '#2e586a'
      }}
    >
      <Toolbar>
        <IconButton
          color="inherit"
          aria-label="open drawer"
          edge="start"
          onClick={onToggleDrawer}
          sx={{ mr: 2, display: { md: 'none' } }}
        >
          <MenuIcon />
        </IconButton>
        <Typography variant="h6" noWrap component="div">
          {title}
        </Typography>
        <Box flexGrow={1} />
        {features.security && <LayoutAuthMenu />}
      </Toolbar>
    </AppBar>
  );
};

export default LayoutAppBar;
