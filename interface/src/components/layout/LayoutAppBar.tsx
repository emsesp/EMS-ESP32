import { memo, useCallback, useMemo } from 'react';
import { Link, useLocation, useNavigate } from 'react-router';

import ArrowBackIcon from '@mui/icons-material/ArrowBack';
import MenuIcon from '@mui/icons-material/Menu';
import { AppBar, IconButton, Toolbar, Typography } from '@mui/material';
import type { SxProps, Theme } from '@mui/material/styles';

import { useI18nContext } from 'i18n/i18n-react';

export const DRAWER_WIDTH = 210;

interface LayoutAppBarProps {
  title: string;
  onToggleDrawer: () => void;
}

// Extract static styles
const appBarStyles: SxProps<Theme> = {
  width: { md: `calc(100% - ${DRAWER_WIDTH}px)` },
  ml: { md: `${DRAWER_WIDTH}px` },
  boxShadow: 'none',
  backgroundColor: '#2e586a'
};

const menuButtonStyles: SxProps<Theme> = {
  mr: 2,
  display: { md: 'none' }
};

const backButtonStyles: SxProps<Theme> = {
  mr: 1,
  fontSize: 20,
  verticalAlign: 'middle'
};

const LayoutAppBarComponent = ({ title, onToggleDrawer }: LayoutAppBarProps) => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  const location = useLocation();

  const pathnames = useMemo(
    () => location.pathname.split('/').filter((x) => x),
    [location.pathname]
  );

  const handleBackClick = useCallback(() => {
    void navigate('/' + pathnames[0]);
  }, [navigate, pathnames]);

  return (
    <AppBar position="fixed" sx={appBarStyles}>
      <Toolbar>
        <IconButton
          color="inherit"
          edge="start"
          onClick={onToggleDrawer}
          sx={menuButtonStyles}
        >
          <MenuIcon />
        </IconButton>

        {pathnames.length > 1 && (
          <>
            <IconButton
              sx={backButtonStyles}
              color="primary"
              edge="start"
              onClick={handleBackClick}
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

const LayoutAppBar = memo(LayoutAppBarComponent);

export default LayoutAppBar;
