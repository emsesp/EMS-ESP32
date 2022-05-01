import { useEffect, FC, useState, useCallback, useRef } from 'react';
import { useSnackbar } from 'notistack';

import { AxiosError } from 'axios';

import { Button } from '@mui/material';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';

import * as NetworkApi from '../../api/network';
import { WiFiNetwork, WiFiNetworkList } from '../../types';
import { ButtonRow, FormLoader, SectionContent } from '../../components';

import WiFiNetworkSelector from './WiFiNetworkSelector';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 500;

const compareNetworks = (network1: WiFiNetwork, network2: WiFiNetwork) => {
  if (network1.rssi < network2.rssi) return 1;
  if (network1.rssi > network2.rssi) return -1;
  return 0;
};

const WiFiNetworkScanner: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const pollCount = useRef(0);
  const [networkList, setNetworkList] = useState<WiFiNetworkList>();
  const [errorMessage, setErrorMessage] = useState<string>();

  const finishedWithError = useCallback(
    (message: string) => {
      enqueueSnackbar(message, { variant: 'error' });
      setNetworkList(undefined);
      setErrorMessage(message);
    },
    [enqueueSnackbar]
  );

  const pollNetworkList = useCallback(async () => {
    try {
      const response = await NetworkApi.listNetworks();
      if (response.status === 202) {
        const completedPollCount = pollCount.current + 1;
        if (completedPollCount < NUM_POLLS) {
          pollCount.current = completedPollCount;
          setTimeout(pollNetworkList, POLLING_FREQUENCY);
        } else {
          finishedWithError('Device did not return network list in timely manner');
        }
      } else {
        const newNetworkList = response.data;
        newNetworkList.networks.sort(compareNetworks);
        setNetworkList(newNetworkList);
      }
    } catch (error: unknown) {
      if (error instanceof AxiosError) {
        finishedWithError('Problem listing WiFi networks ' + error.response?.data.message);
      } else {
        finishedWithError('Problem listing WiFi networks');
      }
    }
  }, [finishedWithError]);

  const startNetworkScan = useCallback(async () => {
    pollCount.current = 0;
    setNetworkList(undefined);
    setErrorMessage(undefined);
    try {
      await NetworkApi.scanNetworks();
      setTimeout(pollNetworkList, POLLING_FREQUENCY);
    } catch (error: unknown) {
      if (error instanceof AxiosError) {
        finishedWithError('Problem scanning for WiFi networks ' + error.response?.data.message);
      } else {
        finishedWithError('Problem scanning for WiFi networks');
      }
    }
  }, [finishedWithError, pollNetworkList]);

  useEffect(() => {
    startNetworkScan();
  }, [startNetworkScan]);

  const renderNetworkScanner = () => {
    if (!networkList) {
      return <FormLoader message="Scanning&hellip;" errorMessage={errorMessage} />;
    }
    return <WiFiNetworkSelector networkList={networkList} />;
  };

  return (
    <SectionContent title="Network Scanner">
      {renderNetworkScanner()}
      <ButtonRow>
        <Button
          startIcon={<PermScanWifiIcon />}
          variant="outlined"
          color="secondary"
          onClick={startNetworkScan}
          disabled={!errorMessage && !networkList}
        >
          Scan again&hellip;
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default WiFiNetworkScanner;
