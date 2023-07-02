import CloseIcon from '@mui/icons-material/Close';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  LinearProgress,
  Typography,
  TextField,
  Button
} from '@mui/material';
import { useRequest } from 'alova';
import { useEffect } from 'react';

import type { FC } from 'react';
import * as SecurityApi from 'api/security';
import { MessageBox } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

interface GenerateTokenProps {
  username?: string;
  onClose: () => void;
}

const GenerateToken: FC<GenerateTokenProps> = ({ username, onClose }) => {
  const { LL } = useI18nContext();
  const open = !!username;

  const { data: token, send: generateToken } = useRequest(SecurityApi.generateToken(username), {
    immediate: false
  });

  useEffect(() => {
    if (open) {
      void generateToken();
    }
  }, [open, generateToken]);

  return (
    <Dialog onClose={onClose} aria-labelledby="generate-token-dialog-title" open={!!username} fullWidth maxWidth="sm">
      <DialogTitle id="generate-token-dialog-title">{LL.ACCESS_TOKEN_FOR() + ' ' + username}</DialogTitle>
      <DialogContent dividers>
        {token ? (
          <>
            <MessageBox message={LL.ACCESS_TOKEN_TEXT()} level="info" my={2} />
            <Box mt={2} mb={2}>
              <TextField label="Token" multiline value={token.token} fullWidth contentEditable={false} />
            </Box>
          </>
        ) : (
          <Box m={4} textAlign="center">
            <LinearProgress />
            <Typography variant="h6">{LL.GENERATING_TOKEN()}&hellip;</Typography>
          </Box>
        )}
      </DialogContent>
      <DialogActions>
        <Button startIcon={<CloseIcon />} variant="outlined" onClick={onClose} color="secondary">
          {LL.CLOSE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default GenerateToken;
