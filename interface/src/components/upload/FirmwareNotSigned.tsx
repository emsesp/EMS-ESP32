import { Link, Typography } from '@mui/material';

export const FIRMWARE_NOT_SIGNED_URL =
  'https://docs.emsesp.org/FAQ#firmware-not-signed-by-ems-esp';

interface FirmwareNotSignedProps {
  message: string;
  linkText: string;
}

const FirmwareNotSigned = ({ message, linkText }: FirmwareNotSignedProps) => (
  <Typography sx={{ mt: 2 }} variant="body2" color="error">
    {message}.{' '}
    <Link
      href={FIRMWARE_NOT_SIGNED_URL}
      target="_blank"
      rel="noreferrer"
      sx={{ color: 'lightblue' }}
    >
      {linkText}
    </Link>
  </Typography>
);

export default FirmwareNotSigned;
