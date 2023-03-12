import { FC } from 'react';
import { Link, useLocation } from 'react-router-dom';

import { ListItem, ListItemButton, ListItemIcon, ListItemText, SvgIconProps } from '@mui/material';

import { routeMatches } from 'utils';

import { grey } from '@mui/material/colors';

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
        <ListItemIcon sx={{ color: selected ? '#90caf9' : grey[500] }}>
          <Icon />
        </ListItemIcon>
        <ListItemText sx={{ color: selected ? '#90caf9' : grey[100] }}>{label}</ListItemText>
      </ListItemButton>
    </ListItem>
  );
};

export default LayoutMenuItem;
