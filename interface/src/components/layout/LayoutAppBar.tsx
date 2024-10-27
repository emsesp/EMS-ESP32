import { useLocation, useNavigate } from 'react-router-dom';

import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import MenuIcon from '@mui/icons-material/Menu';
import { AppBar, IconButton, Toolbar, Typography } from '@mui/material';

export const DRAWER_WIDTH = 210;

interface LayoutAppBarProps {
  title: string;
  onToggleDrawer: () => void;
}

const LayoutAppBar = ({ title, onToggleDrawer }: LayoutAppBarProps) => {
  const pathnames = useLocation()
    .pathname.split('/')
    .filter((x) => x);
  const show_back = pathnames.length > 1;

  const navigate = useNavigate();

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
          edge="start"
          onClick={onToggleDrawer}
          sx={{ mr: 2, display: { md: 'none' } }}
        >
          <MenuIcon />
        </IconButton>

        {show_back && (
          <IconButton
            sx={{ mr: 1 }}
            color="inherit"
            edge="start"
            onClick={() => navigate(pathnames[0])}
          >
            <ArrowBackIcon />
          </IconButton>
        )}

        <Typography variant="h6" noWrap component="div">
          {title}
        </Typography>
      </Toolbar>
    </AppBar>
  );
};

export default LayoutAppBar;
