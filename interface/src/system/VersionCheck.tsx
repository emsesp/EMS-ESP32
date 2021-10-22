import React, { Fragment } from 'react';
import {
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  Link,
  LinearProgress,
  Typography
} from '@material-ui/core';

import { FormButton } from '../components';
import { withSnackbar, WithSnackbarProps } from 'notistack';

export const VERSIONCHECK_ENDPOINT =
  'https://api.github.com/repos/emsesp/EMS-ESP32/releases/latest';

export const VERSIONCHECK_DEV_ENDPOINT =
  'https://api.github.com/repos/emsesp/EMS-ESP32/releases/tags/latest';

interface VersionCheckProps extends WithSnackbarProps {
  currentVersion: string;
  onClose: () => void;
}

interface VersionCheckState {
  latestVersion?: string;
  latestVersionUrl?: string;
  latestDevVersion?: string;
  latestDevVersionUrl?: string;
}

class VersionCheck extends React.Component<
  VersionCheckProps,
  VersionCheckState
> {
  state: VersionCheckState = {};

  componentDidMount() {
    fetch(VERSIONCHECK_ENDPOINT)
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        } else {
          throw Error(
            'Unable to get version information. Check internet connection. (' +
              response.status +
              ')'
          );
        }
      })
      .then((data) => {
        this.setState({ latestVersion: data.name });
        this.setState({
          latestVersionUrl: data.assets[1].browser_download_url
        });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem getting response',
          { variant: 'error' }
        );
        this.setState({ latestVersion: undefined });
        this.props.onClose();
      });

    fetch(VERSIONCHECK_DEV_ENDPOINT)
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        } else {
          throw Error(
            'Unable to get version information. Check internet connection. (' +
              response.status +
              ')'
          );
        }
      })
      .then((data) => {
        this.setState({ latestDevVersion: data.name.split(/\s+/).splice(-1) });
        this.setState({
          latestDevVersionUrl: data.assets[1].browser_download_url
        });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem getting response',
          { variant: 'error' }
        );
        this.setState({ latestDevVersion: undefined });
        this.props.onClose();
      });
  }

  render() {
    const { onClose, currentVersion } = this.props;
    const {
      latestVersion,
      latestVersionUrl,
      latestDevVersion,
      latestDevVersionUrl
    } = this.state;
    return (
      <Dialog
        onClose={onClose}
        aria-labelledby="version-check-dialog-title"
        open
        fullWidth
        maxWidth="sm"
      >
        <DialogTitle id="version-check-dialog-title">
          Firmware Update Check
        </DialogTitle>
        <DialogContent dividers>
          {latestVersion ? (
            <Fragment>
              <Box
                bgcolor="primary.main"
                color="primary.contrastText"
                p={2}
                mt={2}
                mb={2}
              >
                <Typography variant="body1">
                  You are currently on version <b>v{currentVersion}</b>
                </Typography>
              </Box>
              <Box mt={2} mb={2}>
                The latest stable version is <b>{latestVersion}</b>.
                Download&nbsp;
                <Link target="_blank" href={latestVersionUrl} color="primary">
                  {'here'}.
                </Link>
              </Box>
              <Box mt={2} mb={2}>
                The latest development (beta) version is{' '}
                <b>{latestDevVersion}</b>. Download&nbsp;
                <Link
                  target="_blank"
                  href={latestDevVersionUrl}
                  color="primary"
                >
                  {'here'}.
                </Link>
              </Box>
              <Box color="warning.main" p={0} pl={0} pr={0} mt={4} mb={0}>
                <Typography variant="body2">
                  <i>
                    After downloading the firmware, go to UPLOAD FIRMWARE to
                    install the new version
                  </i>
                </Typography>
              </Box>
            </Fragment>
          ) : (
            <Box m={4} textAlign="center">
              <LinearProgress />
              <Typography variant="h6">
                Fetching version details&hellip;
              </Typography>
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

export default withSnackbar(VersionCheck);
