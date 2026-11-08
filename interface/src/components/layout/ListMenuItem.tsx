import { memo } from 'react';
import type { CSSProperties } from 'react';
import { Link } from 'react-router';

import NavigateNextIcon from '@mui/icons-material/NavigateNext';
import {
  Avatar,
  Box,
  ListItem,
  ListItemAvatar,
  ListItemButton,
  ListItemIcon,
  ListItemText
} from '@mui/material';
import type { SvgIconProps } from '@mui/material';

interface ListMenuItemProps {
  icon: React.ComponentType<SvgIconProps>;
  bgcolor?: string;
  label: string;
  text: string;
  to?: string;
  disabled?: boolean;
  badge?: boolean;
}

const iconStyles: CSSProperties = {
  justifyContent: 'right',
  color: 'lightblue',
  verticalAlign: 'middle'
};

const Badge = () => (
  <Box
    component="span"
    aria-label="update available"
    sx={{
      display: 'inline-block',
      width: 8,
      height: 8,
      ml: 1,
      verticalAlign: 'middle',
      borderRadius: '50%',
      backgroundColor: '#ffeb3b',
      boxShadow: '0 0 6px rgba(255, 235, 59, 0.8)'
    }}
  />
);

const RenderIcon = memo(
  ({ icon: Icon, bgcolor, label, text, badge }: ListMenuItemProps) => (
    <>
      <ListItemAvatar>
        <Avatar sx={{ bgcolor, color: 'white' }}>
          <Icon />
        </Avatar>
      </ListItemAvatar>
      <ListItemText
        primary={
          <>
            {label}
            {badge && <Badge />}
          </>
        }
        secondary={text}
      />
    </>
  )
);

const LayoutMenuItem = ({
  icon,
  bgcolor,
  label,
  text,
  to,
  disabled,
  badge
}: ListMenuItemProps) => (
  <>
    {to && !disabled ? (
      <ListItem
        disablePadding
        secondaryAction={
          <ListItemIcon style={iconStyles}>
            <NavigateNextIcon />
          </ListItemIcon>
        }
      >
        <ListItemButton component={Link} to={to}>
          <RenderIcon
            icon={icon}
            {...(bgcolor && { bgcolor })}
            label={label}
            text={text}
            {...(badge && { badge })}
          />
        </ListItemButton>
      </ListItem>
    ) : (
      <ListItem>
        <RenderIcon
          icon={icon}
          {...(bgcolor && { bgcolor })}
          label={label}
          text={text}
          {...(badge && { badge })}
        />
      </ListItem>
    )}
  </>
);

export default memo(LayoutMenuItem);
