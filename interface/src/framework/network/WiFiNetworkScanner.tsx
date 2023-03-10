import { useEffect, FC, useState, useCallback, useRef } from 'react';
import { toast } from 'react-toastify';

import { Button } from '@mui/material';
import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';

import * as NetworkApi from 'api/network';
import { WiFiNetwork, WiFiNetworkList } from 'types';
import { ButtonRow, FormLoader, SectionContent } from 'components';

import WiFiNetworkSelector from './WiFiNetworkSelector';

import { useI18nContext } from 'i18n/i18n-react';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 500;

const compareNetworks = (network1: WiFiNetwork, network2: WiFiNetwork) => {
  if (network1.rssi < network2.rssi) return 1;
  if (network1.rssi > network2.rssi) return -1;
  return 0;
};

const WiFiNetworkScanner: FC = () => {
  const { LL } = useI18nContext();

  const pollCount = useRef(0);
  const [networkList, setNetworkList] = useState<WiFiNetworkList>();
  const [errorMessage, setErrorMessage] = useState<string>();

  const finishedWithError = useCallback((message: string) => {
    toast.error(message);
    setNetworkList(undefined);
    setErrorMessage(message);
  }, []);

  const pollNetworkList = useCallback(async () => {
    try {
      const response = await NetworkApi.listNetworks();
      if (response.status === 202) {
        const completedPollCount = pollCount.current + 1;
        if (completedPollCount < NUM_POLLS) {
          pollCount.current = completedPollCount;
          setTimeout(pollNetworkList, POLLING_FREQUENCY);
        } else {
          finishedWithError(LL.PROBLEM_LOADING());
        }
      } else {
        const newNetworkList = response.data;
        newNetworkList.networks.sort(compareNetworks);
        setNetworkList(newNetworkList);
      }
    } catch (error) {
      if (error.response) {
        finishedWithError(LL.PROBLEM_LOADING() + ' ' + error.response?.data.message);
      } else {
        finishedWithError(LL.PROBLEM_LOADING());
      }
    }
  }, [finishedWithError, LL]);

  const startNetworkScan = useCallback(async () => {
    pollCount.current = 0;
    setNetworkList(undefined);
    setErrorMessage(undefined);
    try {
      await NetworkApi.scanNetworks();
      setTimeout(pollNetworkList, POLLING_FREQUENCY);
    } catch (error) {
      if (error.response) {
        finishedWithError(LL.PROBLEM_LOADING() + ' ' + error.response?.data.message);
      } else {
        finishedWithError(LL.PROBLEM_LOADING());
      }
    }
  }, [finishedWithError, pollNetworkList, LL]);

  useEffect(() => {
    startNetworkScan();
  }, [startNetworkScan]);

  const renderNetworkScanner = () => {
    if (!networkList) {
      return <FormLoader message={LL.SCANNING() + '...'} errorMessage={errorMessage} />;
    }
    return <WiFiNetworkSelector networkList={networkList} />;
  };

  return (
    <SectionContent title={LL.NETWORK_SCANNER()}>
      {renderNetworkScanner()}
      <ButtonRow>
        <Button
          startIcon={<PermScanWifiIcon />}
          variant="outlined"
          color="secondary"
          onClick={startNetworkScan}
          disabled={!errorMessage && !networkList}
        >
          {LL.SCAN_AGAIN()}&hellip;
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default WiFiNetworkScanner;
