import { useLocation, useNavigate } from 'react-router';

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
  const back_path = pathnames.length > 1 ? '/' + pathnames[0] : undefined;
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

        {back_path && (
          <IconButton
            sx={{ mr: 1 }}
            color="inherit"
            edge="start"
            onClick={() => navigate(back_path)}
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
