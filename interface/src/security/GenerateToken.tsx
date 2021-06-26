import React, { Fragment } from 'react';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  LinearProgress,
  Typography,
  TextField
} from '@material-ui/core';

import { FormButton } from '../components';
import { redirectingAuthorizedFetch } from '../authentication';
import { GENERATE_TOKEN_ENDPOINT } from '../api';
import { withSnackbar, WithSnackbarProps } from 'notistack';

interface GenerateTokenProps extends WithSnackbarProps {
  username: string;
  onClose: () => void;
}

interface GenerateTokenState {
  token?: string;
}

class GenerateToken extends React.Component<
  GenerateTokenProps,
  GenerateTokenState
> {
  state: GenerateTokenState = {};

  componentDidMount() {
    const { username } = this.props;
    redirectingAuthorizedFetch(
      GENERATE_TOKEN_ENDPOINT + '?' + new URLSearchParams({ username }),
      { method: 'GET' }
    )
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        } else {
          throw Error('Error generating token: ' + response.status);
        }
      })
      .then((generatedToken) => {
        // console.log(generatedToken);
        this.setState({ token: generatedToken.token });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem generating token',
          { variant: 'error' }
        );
      });
  }

  render() {
    const { onClose, username } = this.props;
    const { token } = this.state;
    return (
      <Dialog
        onClose={onClose}
        aria-labelledby="generate-token-dialog-title"
        open
        fullWidth
        maxWidth="sm"
      >
        <DialogTitle id="generate-token-dialog-title">
          Token for: {username}
        </DialogTitle>
        <DialogContent dividers>
          {token ? (
            <Fragment>
              <Box
                bgcolor="primary.main"
                color="primary.contrastText"
                p={2}
                mt={2}
                mb={2}
              >
                <Typography variant="body1">
                  The token below may be used to access the secured APIs, either
                  as a Bearer authentication in the "Authorization" header or
                  using the "access_token" query parameter.
                </Typography>
              </Box>
              <Box mt={2} mb={2}>
                <TextField
                  label="Token"
                  multiline
                  value={token}
                  fullWidth
                  contentEditable={false}
                />
              </Box>
            </Fragment>
          ) : (
            <Box m={4} textAlign="center">
              <LinearProgress />
              <Typography variant="h6">Generating token&hellip;</Typography>
            </Box>
          )}
        </DialogContent>
        <DialogActions>
          <FormButton
            variant="contained"
            color="primary"
            type="submit"
            onClick={onClose}
          >
            Close
          </FormButton>
        </DialogActions>
      </Dialog>
    );
  }
}

export default withSnackbar(GenerateToken);
