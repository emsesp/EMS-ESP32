import { type FC, memo, useMemo } from 'react';

import CheckCircleOutlineOutlinedIcon from '@mui/icons-material/CheckCircleOutlineOutlined';
import ErrorIcon from '@mui/icons-material/Error';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import ReportProblemOutlinedIcon from '@mui/icons-material/ReportProblemOutlined';
import { Box, Typography, useTheme } from '@mui/material';
import type { BoxProps, SvgIconProps } from '@mui/material';

type MessageBoxLevel = 'warning' | 'success' | 'info' | 'error';

export interface MessageBoxProps extends BoxProps {
  level: MessageBoxLevel;
  message?: string;
}

const LEVEL_ICONS: Record<MessageBoxLevel, React.ComponentType<SvgIconProps>> = {
  success: CheckCircleOutlineOutlinedIcon,
  info: InfoOutlinedIcon,
  warning: ReportProblemOutlinedIcon,
  error: ErrorIcon
};

const LEVEL_PALETTE_PATHS: Record<MessageBoxLevel, string> = {
  success: 'success.dark',
  info: 'info.main',
  warning: 'warning.dark',
  error: 'error.dark'
};

const MessageBox: FC<MessageBoxProps> = ({
  level,
  message,
  sx,
  children,
  ...rest
}) => {
  const theme = useTheme();

  const { Icon, backgroundColor } = useMemo(() => {
    const Icon = LEVEL_ICONS[level];
    const palettePath = LEVEL_PALETTE_PATHS[level];
    const [key, shade] = palettePath.split('.') as [
      keyof typeof theme.palette,
      string
    ];
    const paletteKey = theme.palette[key] as unknown as Record<string, string>;
    const backgroundColor = paletteKey[shade];

    return { Icon, backgroundColor };
  }, [level, theme]);

  return (
    <Box
      p={2}
      display="flex"
      alignItems="center"
      borderRadius={1}
      sx={{ backgroundColor, color: 'white', ...sx }}
      {...rest}
    >
      <Icon />
      {(message || children) && (
        <Typography sx={{ ml: 2 }} variant="body1">
          {message}
          {children}
        </Typography>
      )}
    </Box>
  );
};

export default memo(MessageBox);
