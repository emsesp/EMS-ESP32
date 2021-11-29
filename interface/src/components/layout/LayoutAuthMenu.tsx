import React, { FC, useContext } from 'react';

import { Box, Button, Divider, IconButton, Popover, Typography, Avatar, styled, TypographyProps } from '@mui/material';

import PersonIcon from '@mui/icons-material/Person';
import AccountCircleIcon from '@mui/icons-material/AccountCircle';

import { AuthenticatedContext } from '../../contexts/authentication';

const ItemTypography = styled(Typography)<TypographyProps>({
  maxWidth: '250px',
  whiteSpace: 'nowrap',
  overflow: 'hidden',
  textOverflow: 'ellipsis'
});

const LayoutAuthMenu: FC = () => {
  const { me, signOut } = useContext(AuthenticatedContext);

  const [anchorEl, setAnchorEl] = React.useState<HTMLButtonElement | null>(null);

  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  const open = Boolean(anchorEl);
  const id = anchorEl ? 'app-menu-popover' : undefined;

  return (
    <>
      <IconButton id="open-auth-menu" sx={{ padding: 0 }} aria-describedby={id} color="inherit" onClick={handleClick}>
        <AccountCircleIcon />
      </IconButton>
      <Popover
        id="app-menu-popover"
        sx={{ mt: 1 }}
        open={open}
        anchorEl={anchorEl}
        onClose={handleClose}
        anchorOrigin={{
          vertical: 'bottom',
          horizontal: 'center'
        }}
        transformOrigin={{
          vertical: 'top',
          horizontal: 'center'
        }}
      >
        <Box display="flex" flexDirection="row" alignItems="center" p={2}>
          <Avatar sx={{ width: 80, height: 80 }}>
            <PersonIcon fontSize="large" />
          </Avatar>
          <Box pl={2}>
            <ItemTypography variant="h6">{me.username}</ItemTypography>
            <ItemTypography variant="body1">{me.admin ? 'Admin User' : 'Guest User'}</ItemTypography>
          </Box>
        </Box>
        <Divider />
        <Box p={1.5}>
          <Button variant="outlined" fullWidth color="primary" onClick={() => signOut(true)}>
            Sign Out
          </Button>
        </Box>
      </Popover>
    </>
  );
};

export default LayoutAuthMenu;
