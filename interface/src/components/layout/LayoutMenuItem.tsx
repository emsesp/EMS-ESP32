import { memo } from 'react';
import { Link, useLocation } from 'react-router';

import { Box, ListItemButton, ListItemIcon, ListItemText } from '@mui/material';
import type { SvgIconProps, SxProps, Theme } from '@mui/material';

import { routeMatches } from 'utils';

interface LayoutMenuItemProps {
  icon: React.ComponentType<SvgIconProps>;
  label: string;
  to: string;
  disabled?: boolean;
  badge?: boolean;
}

const LayoutMenuItemComponent = ({
  icon: Icon,
  label,
  to,
  disabled,
  badge
}: LayoutMenuItemProps) => {
  const { pathname } = useLocation();

  const selected = routeMatches(to, pathname);

  const buttonStyles: SxProps<Theme> = {
    transition: 'all 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
    backgroundColor: selected ? 'rgba(144, 202, 249, 0.1)' : 'transparent',
    borderRadius: '8px',
    margin: '2px 8px',
    '&:hover': {
      backgroundColor: 'rgba(68, 82, 211, 0.39)'
    },
    '&::before': {
      content: '""',
      position: 'absolute',
      left: 0,
      top: 0,
      bottom: 0,
      width: selected ? '3px' : '0px',
      backgroundColor: '#90caf9',
      transition: 'width 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)'
    }
  };

  const iconStyles: SxProps<Theme> = {
    color: selected ? '#90caf9' : '#9e9e9e',
    transition: 'color 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
    transform: selected ? 'scale(1.1)' : 'scale(1)',
    transitionProperty: 'color, transform'
  };

  const textStyles: SxProps<Theme> = {
    color: selected ? '#90caf9' : '#f5f5f5',
    transition: 'color 0.05s cubic-bezier(0.55, 0.085, 0.68, 0.53)',
    transitionProperty: 'color, font-weight'
  };

  return (
    <ListItemButton
      component={Link}
      to={to}
      disabled={disabled || false}
      selected={selected}
      sx={buttonStyles}
    >
      <ListItemIcon sx={iconStyles}>
        <Icon />
      </ListItemIcon>
      <ListItemText sx={textStyles}>{label}</ListItemText>
      {badge && (
        <Box
          aria-label="update available"
          sx={{
            width: 8,
            height: 8,
            ml: 1,
            borderRadius: '50%',
            backgroundColor: '#ffeb3b',
            boxShadow: '0 0 6px rgba(255, 235, 59, 0.8)',
            flexShrink: 0
          }}
        />
      )}
    </ListItemButton>
  );
};

const LayoutMenuItem = memo(LayoutMenuItemComponent);

export default LayoutMenuItem;
