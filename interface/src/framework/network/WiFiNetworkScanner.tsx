import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import { Button } from '@mui/material';
import { useRequest } from 'alova';
import { useState, useCallback, useRef } from 'react';
import { toast } from 'react-toastify';

import WiFiNetworkSelector from './WiFiNetworkSelector';
import type { FC } from 'react';
import * as NetworkApi from 'api/network';
import { ButtonRow, FormLoader, SectionContent } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 1000;

const WiFiNetworkScanner: FC = () => {
  const { LL } = useI18nContext();

  const pollCount = useRef(0);
  const [errorMessage, setErrorMessage] = useState<string>();

  const { data: networkList, send: getNetworkList } = useRequest(NetworkApi.listNetworks, {
    immediate: false
  });

  const {
    send: scanNetworks,
    onSuccess: onSuccessScanNetworks,
    onError: onErrorScanNetworks
  } = useRequest(NetworkApi.scanNetworks);

  const finishedWithError = useCallback((message: string) => {
    toast.error(message);
    setErrorMessage(message);
    pollCount.current = 0;
  }, []);

  onErrorScanNetworks((event) => {
    console.log('onErrorScanNetworks'); // TODO fix
    if (event.error?.message === 'Wait') {
      // 202
      console.log('not ready...: ', event.error?.message); // TODO fix
      const completedPollCount = pollCount.current + 1;
      if (completedPollCount < NUM_POLLS) {
        pollCount.current = completedPollCount;
        setTimeout(scanNetworks, POLLING_FREQUENCY);
      } else {
        finishedWithError(LL.PROBLEM_LOADING());
      }
    } else {
      finishedWithError(LL.PROBLEM_LOADING());
    }
  });

  onSuccessScanNetworks(() => {
    console.log('onCompleteScanNetworks'); // TODO fix
    pollCount.current = 0;
    void getNetworkList(); // fetch the list
  });

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
          onClick={scanNetworks}
          disabled={!errorMessage && !networkList}
        >
          {LL.SCAN_AGAIN()}&hellip;
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default WiFiNetworkScanner;
