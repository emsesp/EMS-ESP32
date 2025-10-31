import { memo } from 'react';

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

export type OptionType =
  | 'deleted'
  | 'readonly'
  | 'web_exclude'
  | 'api_mqtt_exclude'
  | 'favorite';

type IconPair = [
  React.ComponentType<SvgIconProps>,
  React.ComponentType<SvgIconProps>
];

const OPTION_ICONS: Record<OptionType, IconPair> = {
  deleted: [DeleteForeverIcon, DeleteOutlineIcon],
  readonly: [EditOffOutlinedIcon, EditOutlinedIcon],
  web_exclude: [VisibilityOffOutlinedIcon, VisibilityOutlinedIcon],
  api_mqtt_exclude: [CommentsDisabledOutlinedIcon, InsertCommentOutlinedIcon],
  favorite: [StarIcon, StarOutlineIcon]
} as const;

const ICON_SIZE = 16;
const ICON_SX = { fontSize: ICON_SIZE, verticalAlign: 'middle' } as const;

export interface OptionIconProps {
  readonly type: OptionType;
  readonly isSet: boolean;
}

const OptionIcon = ({ type, isSet }: OptionIconProps) => {
  const [SetIcon, UnsetIcon] = OPTION_ICONS[type];
  const Icon = isSet ? SetIcon : UnsetIcon;

  return <Icon {...(isSet && { color: 'primary' })} sx={ICON_SX} />;
};

export default memo(OptionIcon);
