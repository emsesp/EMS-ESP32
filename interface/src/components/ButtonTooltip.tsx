import { Tooltip, type TooltipProps, styled, tooltipClasses } from '@mui/material';

export const ButtonTooltip = styled(({ className, ...props }: TooltipProps) => (
  <Tooltip {...props} placement="top" arrow classes={{ popper: className }} />
))(({ theme }) => ({
  [`& .${tooltipClasses.arrow}`]: {
    color: theme.palette.success.main
  },
  [`& .${tooltipClasses.tooltip}`]: {
    backgroundColor: theme.palette.success.main,
    color: 'rgba(0, 0, 0, 0.87)',
    boxShadow: theme.shadows[1],
    fontSize: 10
  }
}));

export default ButtonTooltip;
