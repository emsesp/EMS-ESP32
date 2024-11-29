import { Link, useLocation, useNavigate } from 'react-router';

import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import MenuIcon from '@mui/icons-material/Menu';
import { AppBar, IconButton, Toolbar, Typography } from '@mui/material';

import { useI18nContext } from 'i18n/i18n-react';

export const DRAWER_WIDTH = 210;

interface LayoutAppBarProps {
  title: string;
  onToggleDrawer: () => void;
}

const LayoutAppBar = ({ title, onToggleDrawer }: LayoutAppBarProps) => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();

  const pathnames = useLocation()
    .pathname.split('/')
    .filter((x) => x);

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

        {pathnames.length > 1 && (
          <>
            <IconButton
              sx={{ mr: 1, fontSize: 20, verticalAlign: 'middle' }}
              color="primary"
              edge="start"
              onClick={() => navigate('/' + pathnames[0])}
            >
              <ArrowBackIcon />
            </IconButton>

            <Link
              to={'/' + pathnames[0]}
              style={{ textDecoration: 'none', color: 'white' }}
            >
              <Typography variant="h6">
                {pathnames[0] === 'status' ? LL.STATUS_OF('') : LL.SETTINGS(0)}
                <span style={{ color: '#90caf9' }}>&nbsp;&nbsp;|&nbsp;&nbsp;</span>
              </Typography>
            </Link>
          </>
        )}

        <Typography variant="h6">{title}</Typography>
      </Toolbar>
    </AppBar>
  );
};

export default LayoutAppBar;
