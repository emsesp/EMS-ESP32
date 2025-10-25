import { Link, useLocation } from 'react-router';

import { ListItemButton, ListItemIcon, ListItemText } from '@mui/material';
import type { SvgIconProps } from '@mui/material';

import { routeMatches } from 'utils';

interface LayoutMenuItemProps {
  icon: React.ComponentType<SvgIconProps>;
  label: string;
  to: string;
  disabled?: boolean;
}

const LayoutMenuItem = ({
  icon: Icon,
  label,
  to,
  disabled
}: LayoutMenuItemProps) => {
  const { pathname } = useLocation();

  const selected = routeMatches(to, pathname);

  return (
    <ListItemButton
      component={Link}
      to={to}
      disabled={disabled || false}
      selected={selected}
      sx={{
        transition: 'all 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
        transform: selected ? 'scale(1.02)' : 'scale(1)',
        backgroundColor: selected ? 'rgba(144, 202, 249, 0.1)' : 'transparent',
        borderRadius: '8px',
        margin: '2px 8px',
        '&:hover': {
          backgroundColor: 'rgba(68, 82, 211, 0.39)',
          transform: selected ? 'scale(1.02)' : 'scale(1.01)'
        },
        '&::before': {
          content: '""',
          position: 'absolute',
          left: 0,
          top: 0,
          bottom: 0,
          width: selected ? '4px' : '0px',
          backgroundColor: '#90caf9',
          borderRadius: '0 2px 2px 0',
          transition: 'width 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)'
        }
      }}
    >
      <ListItemIcon
        sx={{
          color: selected ? '#90caf9' : '#9e9e9e',
          transition: 'color 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
          transform: selected ? 'scale(1.1)' : 'scale(1)',
          transitionProperty: 'color, transform'
        }}
      >
        <Icon />
      </ListItemIcon>
      <ListItemText
        sx={{
          color: selected ? '#90caf9' : '#f5f5f5',
          transition: 'color 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
          fontWeight: selected ? '600' : '400',
          transitionProperty: 'color, font-weight'
        }}
      >
        {label}
      </ListItemText>
    </ListItemButton>
  );
};

export default LayoutMenuItem;
