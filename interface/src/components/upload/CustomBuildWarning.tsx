import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Link, Typography } from '@mui/material';

export const CUSTOM_BUILD_URL =
  'https://docs.emsesp.org/FAQ#official-and-custom-builds';

interface CustomBuildWarningProps {
  message: string;
  linkText: string;
  installText: string;
  onInstall: () => void;
}

// shown when the device stopped an upload because it isn't an official EMS-ESP build
const CustomBuildWarning = ({
  message,
  linkText,
  installText,
  onInstall
}: CustomBuildWarningProps) => (
  <Box sx={{ mt: 2 }}>
    <Typography variant="body2" color="warning">
      {message}.{' '}
      <Link
        href={CUSTOM_BUILD_URL}
        target="_blank"
        rel="noreferrer"
        sx={{ color: 'lightblue' }}
      >
        {linkText}
      </Link>
    </Typography>
    <Button
      sx={{ mt: 1 }}
      size="small"
      startIcon={<WarningIcon />}
      variant="outlined"
      color="warning"
      onClick={onInstall}
    >
      {installText}
    </Button>
  </Box>
);

export default CustomBuildWarning;
