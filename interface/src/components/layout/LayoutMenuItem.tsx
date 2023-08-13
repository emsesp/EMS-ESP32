import { ListItem, ListItemButton, ListItemIcon, ListItemText } from '@mui/material';
import { Link, useLocation } from 'react-router-dom';
import type { SvgIconProps } from '@mui/material';
import type { FC } from 'react';

import { routeMatches } from 'utils';

interface LayoutMenuItemProps {
  icon: React.ComponentType<SvgIconProps>;
  label: string;
  to: string;
  disabled?: boolean;
}

const LayoutMenuItem: FC<LayoutMenuItemProps> = ({ icon: Icon, label, to, disabled }) => {
  const { pathname } = useLocation();

  const selected = routeMatches(to, pathname);

  return (
    <ListItem disablePadding>
      <ListItemButton component={Link} to={to} disabled={disabled} selected={selected}>
        <ListItemIcon sx={{ color: selected ? '#90caf9' : '#9e9e9e' }}>
          <Icon />
        </ListItemIcon>
        <ListItemText sx={{ color: selected ? '#90caf9' : '#f5f5f5' }}>{label}</ListItemText>
      </ListItemButton>
    </ListItem>
  );
};

export default LayoutMenuItem;
