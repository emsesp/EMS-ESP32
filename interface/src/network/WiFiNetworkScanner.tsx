import React, { Component } from 'react';
import { withSnackbar, WithSnackbarProps } from 'notistack';

import { createStyles, WithStyles, Theme, withStyles, Typography, LinearProgress } from '@material-ui/core';
import PermScanWifiIcon from '@material-ui/icons/PermScanWifi';

import { FormActions, FormButton, SectionContent } from '../components';
import { redirectingAuthorizedFetch } from '../authentication';
import { SCAN_NETWORKS_ENDPOINT, LIST_NETWORKS_ENDPOINT } from '../api';

import WiFiNetworkSelector from './WiFiNetworkSelector';
import { WiFiNetworkList, WiFiNetwork } from './types';

const NUM_POLLS = 10
const POLLING_FREQUENCY = 500
const RETRY_EXCEPTION_TYPE = "retry"

interface WiFiNetworkScannerState {
  scanningForNetworks: boolean;
  errorMessage?: string;
  networkList?: WiFiNetworkList;
}

const styles = (theme: Theme) => createStyles({
  scanningSettings: {
    margin: theme.spacing(0.5),
  },
  scanningSettingsDetails: {
    margin: theme.spacing(4),
    textAlign: "center"
  },
  scanningProgress: {
    margin: theme.spacing(4),
    textAlign: "center"
  }
});

type WiFiNetworkScannerProps = WithSnackbarProps & WithStyles<typeof styles>;

class WiFiNetworkScanner extends Component<WiFiNetworkScannerProps, WiFiNetworkScannerState> {

  pollCount: number = 0;

  state: WiFiNetworkScannerState = {
    scanningForNetworks: false,
  };

  componentDidMount() {
    this.scanNetworks();
  }

  requestNetworkScan = () => {
    const { scanningForNetworks } = this.state;
    if (!scanningForNetworks) {
      this.scanNetworks();
    }
  }

  scanNetworks() {
    this.pollCount = 0;
    this.setState({ scanningForNetworks: true, networkList: undefined, errorMessage: undefined });
    redirectingAuthorizedFetch(SCAN_NETWORKS_ENDPOINT).then(response => {
      if (response.status === 202) {
        this.schedulePollTimeout();
        return;
      }
      throw Error("Scanning for networks returned unexpected response code: " + response.status);
    }).catch(error => {
      this.props.enqueueSnackbar("Problem scanning: " + error.message, {
        variant: 'error',
      });
      this.setState({ scanningForNetworks: false, networkList: undefined, errorMessage: error.message });
    });
  }

  schedulePollTimeout() {
    setTimeout(this.pollNetworkList, POLLING_FREQUENCY);
  }

  retryError() {
    return {
      name: RETRY_EXCEPTION_TYPE,
      message: "Network list not ready, will retry in " + POLLING_FREQUENCY + "ms."
    };
  }

  compareNetworks(network1: WiFiNetwork, network2: WiFiNetwork) {
    if (network1.rssi < network2.rssi)
      return 1;
    if (network1.rssi > network2.rssi)
      return -1;
    return 0;
  }

  pollNetworkList = () => {
    redirectingAuthorizedFetch(LIST_NETWORKS_ENDPOINT)
      .then(response => {
        if (response.status === 200) {
          return response.json();
        }
        if (response.status === 202) {
          if (++this.pollCount < NUM_POLLS) {
            this.schedulePollTimeout();
            throw this.retryError();
          } else {
            throw Error("Device did not return network list in timely manner.");
          }
        }
        throw Error("Device returned unexpected response code: " + response.status);
      })
      .then(json => {
        json.networks.sort(this.compareNetworks)
        this.setState({ scanningForNetworks: false, networkList: json, errorMessage: undefined })
      })
      .catch(error => {
        if (error.name !== RETRY_EXCEPTION_TYPE) {
          this.props.enqueueSnackbar("Problem scanning: " + error.message, {
            variant: 'error',
          });
          this.setState({ scanningForNetworks: false, networkList: undefined, errorMessage: error.message });
        }
      });
  }

  renderNetworkScanner() {
    const { classes } = this.props;
    const { scanningForNetworks, networkList, errorMessage } = this.state;
    if (scanningForNetworks || !networkList) {
      return (
        <div className={classes.scanningSettings}>
          <LinearProgress className={classes.scanningSettingsDetails} />
          <Typography variant="h6" className={classes.scanningProgress}>
            Scanning&hellip;
          </Typography>
        </div>
      );
    }
    if (errorMessage) {
      return (
        <div className={classes.scanningSettings}>
          <Typography variant="h6" className={classes.scanningSettingsDetails}>
            {errorMessage}
          </Typography>
        </div>
      );
    }
    return (
      <WiFiNetworkSelector networkList={networkList} />
    );
  }

  render() {
    const { scanningForNetworks } = this.state;
    return (
      <SectionContent title="Network Scanner">
        {this.renderNetworkScanner()}
        <FormActions>
          <FormButton startIcon={<PermScanWifiIcon />} variant="contained" color="secondary" onClick={this.requestNetworkScan} disabled={scanningForNetworks}>
            Scan again&hellip;
          </FormButton>
        </FormActions>
      </SectionContent>
    );
  }

}

export default withSnackbar(withStyles(styles)(WiFiNetworkScanner));
