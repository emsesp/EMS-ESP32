import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import DeleteForeverIcon from '@mui/icons-material/DeleteForever';
import DeleteOutlineIcon from '@mui/icons-material/DeleteOutline';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';
import EditOutlinedIcon from '@mui/icons-material/EditOutlined';
import InsertCommentOutlinedIcon from '@mui/icons-material/InsertCommentOutlined';
import StarIcon from '@mui/icons-material/Star';
import StarOutlineIcon from '@mui/icons-material/StarOutline';
import VisibilityOffOutlinedIcon from '@mui/icons-material/VisibilityOffOutlined';
import VisibilityOutlinedIcon from '@mui/icons-material/VisibilityOutlined';
import type { SvgIconProps } from '@mui/material';

type OptionType =
  | 'deleted'
  | 'readonly'
  | 'web_exclude'
  | 'api_mqtt_exclude'
  | 'favorite';

const OPTION_ICONS: {
  [type in OptionType]: [
    React.ComponentType<SvgIconProps>,
    React.ComponentType<SvgIconProps>
  ];
} = {
  deleted: [DeleteForeverIcon, DeleteOutlineIcon],
  readonly: [EditOffOutlinedIcon, EditOutlinedIcon],
  web_exclude: [VisibilityOffOutlinedIcon, VisibilityOutlinedIcon],
  api_mqtt_exclude: [CommentsDisabledOutlinedIcon, InsertCommentOutlinedIcon],
  favorite: [StarIcon, StarOutlineIcon]
};

const OptionIcon = ({ type, isSet }: { type: OptionType; isSet: boolean }) => {
  const Icon = OPTION_ICONS[type][isSet ? 0 : 1];
  return isSet ? (
    <Icon color="primary" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
  ) : (
    <Icon sx={{ fontSize: 16, verticalAlign: 'middle' }} />
  );
};

export default OptionIcon;
