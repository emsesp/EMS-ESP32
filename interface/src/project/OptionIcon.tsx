import { FC } from 'react';
import { SvgIconProps } from '@mui/material';

import EditOutlinedIcon from '@mui/icons-material/EditOutlined';
import EditOffOutlinedIcon from '@mui/icons-material/EditOffOutlined';

import StarIcon from '@mui/icons-material/Star';
import StarOutlineIcon from '@mui/icons-material/StarOutline';

import VisibilityOffOutlinedIcon from '@mui/icons-material/VisibilityOffOutlined';
import VisibilityOutlinedIcon from '@mui/icons-material/VisibilityOutlined';

import CommentsDisabledOutlinedIcon from '@mui/icons-material/CommentsDisabledOutlined';
import InsertCommentOutlinedIcon from '@mui/icons-material/InsertCommentOutlined';

import DeleteOutlineIcon from '@mui/icons-material/DeleteOutline';
import DeleteForeverIcon from '@mui/icons-material/DeleteForever';

type OptionType = 'deleted' | 'readonly' | 'web_exclude' | 'api_mqtt_exclude' | 'favorite';

const OPTION_ICONS: { [type in OptionType]: [React.ComponentType<SvgIconProps>, React.ComponentType<SvgIconProps>] } = {
  deleted: [DeleteForeverIcon, DeleteOutlineIcon],
  readonly: [EditOffOutlinedIcon, EditOutlinedIcon],
  web_exclude: [VisibilityOffOutlinedIcon, VisibilityOutlinedIcon],
  api_mqtt_exclude: [CommentsDisabledOutlinedIcon, InsertCommentOutlinedIcon],
  favorite: [StarIcon, StarOutlineIcon]
};

interface OptionIconProps {
  type: OptionType;
  isSet: boolean;
}

const OptionIcon: FC<OptionIconProps> = ({ type, isSet }) => {
  const Icon = OPTION_ICONS[type][isSet ? 0 : 1];
  return isSet ? (
    <Icon color="primary" sx={{ fontSize: 14, verticalAlign: 'middle' }} />
  ) : (
    <Icon sx={{ fontSize: 14, verticalAlign: 'middle' }} />
  );
};

export default OptionIcon;
