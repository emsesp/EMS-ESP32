import { Tooltip, type TooltipProps } from '@mui/material';

export const ButtonTooltip = ({ children, ...props }: TooltipProps) => (
  <Tooltip {...props}>{children}</Tooltip>
);

export default ButtonTooltip;
