import { Link } from 'react-router';

import NavigateNextIcon from '@mui/icons-material/NavigateNext';
import {
  Avatar,
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
}

function RenderIcon({ icon: Icon, bgcolor, label, text }: ListMenuItemProps) {
  return (
    <>
      <ListItemAvatar>
        <Avatar sx={{ bgcolor, color: 'white' }}>
          <Icon />
        </Avatar>
      </ListItemAvatar>
      <ListItemText primary={label} secondary={text} />
    </>
  );
}

const LayoutMenuItem = ({
  icon,
  bgcolor,
  label,
  text,
  to,
  disabled
}: ListMenuItemProps) => (
  <>
    {to && !disabled ? (
      <ListItem
        disablePadding
        secondaryAction={
          <ListItemIcon
            style={{
              justifyContent: 'right',
              color: 'lightblue',
              verticalAlign: 'middle'
            }}
          >
            <NavigateNextIcon />
          </ListItemIcon>
        }
      >
        <ListItemButton component={Link} to={to}>
          <RenderIcon icon={icon} bgcolor={bgcolor} label={label} text={text} />
        </ListItemButton>
      </ListItem>
    ) : (
      <ListItem>
        <RenderIcon icon={icon} bgcolor={bgcolor} label={label} text={text} />
      </ListItem>
    )}
  </>
);

export default LayoutMenuItem;
