import { FC, useCallback, useState, useEffect } from 'react';
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

import CloseIcon from '@mui/icons-material/Close';

import { extractErrorMessage } from 'utils';
import { toast } from 'react-toastify';
import { MessageBox } from 'components';
import * as SecurityApi from 'api/security';
import { Token } from 'types';

import { useI18nContext } from 'i18n/i18n-react';

interface GenerateTokenProps {
  username?: string;
  onClose: () => void;
}

const GenerateToken: FC<GenerateTokenProps> = ({ username, onClose }) => {
  const [token, setToken] = useState<Token>();
  const open = !!username;

  const { LL } = useI18nContext();

  const getToken = useCallback(async () => {
    try {
      setToken((await SecurityApi.generateToken(username)).data);
    } catch (error) {
      toast.error(extractErrorMessage(error, LL.PROBLEM_UPDATING()));
    }
  }, [username, LL]);

  useEffect(() => {
    if (open) {
      getToken();
    }
  }, [open, getToken]);

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
