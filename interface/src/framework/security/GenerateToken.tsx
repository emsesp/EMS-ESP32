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

import { extractErrorMessage } from '../../utils';
import { useSnackbar } from 'notistack';
import { MessageBox } from '../../components';
import * as SecurityApi from '../../api/security';
import { Token } from '../../types';

interface GenerateTokenProps {
  username?: string;
  onClose: () => void;
}

const GenerateToken: FC<GenerateTokenProps> = ({ username, onClose }) => {
  const [token, setToken] = useState<Token>();
  const open = !!username;

  const { enqueueSnackbar } = useSnackbar();

  const getToken = useCallback(async () => {
    try {
      setToken((await SecurityApi.generateToken(username)).data);
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem generating token'), { variant: 'error' });
    }
  }, [username, enqueueSnackbar]);

  useEffect(() => {
    if (open) {
      getToken();
    }
  }, [open, getToken]);

  return (
    <Dialog onClose={onClose} aria-labelledby="generate-token-dialog-title" open={!!username} fullWidth maxWidth="sm">
      <DialogTitle id="generate-token-dialog-title">Access Token for {username}</DialogTitle>
      <DialogContent dividers>
        {token ? (
          <>
            <MessageBox
              message="The token below is used with REST API calls that require authorization. It can be passed either as a Bearer token in the
        'Authorization' header or in the 'access_token' URL query parameter."
              level="info"
              my={2}
            />
            <Box mt={2} mb={2}>
              <TextField label="Token" multiline value={token.token} fullWidth contentEditable={false} />
            </Box>
          </>
        ) : (
          <Box m={4} textAlign="center">
            <LinearProgress />
            <Typography variant="h6">Generating token&hellip;</Typography>
          </Box>
        )}
      </DialogContent>
      <DialogActions>
        <Button startIcon={<CloseIcon />} variant="outlined" onClick={onClose} color="secondary">
          Close
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default GenerateToken;
