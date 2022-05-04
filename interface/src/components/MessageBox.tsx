import { FC } from 'react';

import { Box, BoxProps, SvgIconProps, Theme, Typography, useTheme } from '@mui/material';

import CheckCircleOutlineOutlinedIcon from '@mui/icons-material/CheckCircleOutlineOutlined';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import ReportProblemOutlinedIcon from '@mui/icons-material/ReportProblemOutlined';
import ErrorIcon from '@mui/icons-material/Error';

type MessageBoxLevel = 'warning' | 'success' | 'info' | 'error';

export interface MessageBoxProps extends BoxProps {
  level: MessageBoxLevel;
  message: string;
}

const LEVEL_ICONS: { [type in MessageBoxLevel]: React.ComponentType<SvgIconProps> } = {
  success: CheckCircleOutlineOutlinedIcon,
  info: InfoOutlinedIcon,
  warning: ReportProblemOutlinedIcon,
  error: ErrorIcon
};

const LEVEL_BACKGROUNDS: { [type in MessageBoxLevel]: (theme: Theme) => string } = {
  success: (theme: Theme) => theme.palette.success.dark,
  info: (theme: Theme) => theme.palette.info.main,
  warning: (theme: Theme) => theme.palette.warning.dark,
  error: (theme: Theme) => theme.palette.error.dark
};

const MessageBox: FC<MessageBoxProps> = ({ level, message, sx, children, ...rest }) => {
  const theme = useTheme();
  const Icon = LEVEL_ICONS[level];
  const backgroundColor = LEVEL_BACKGROUNDS[level](theme);
  const color = 'white';
  return (
    <Box p={2} display="flex" alignItems="center" borderRadius={1} sx={{ backgroundColor, color, ...sx }} {...rest}>
      <Icon />
      <Typography sx={{ ml: 2, flexGrow: 1 }} variant="body1">
        {message}
      </Typography>
      {children}
    </Box>
  );
};

export default MessageBox;
