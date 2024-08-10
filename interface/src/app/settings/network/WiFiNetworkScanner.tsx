import { useRef, useState } from 'react';

import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import { Button } from '@mui/material';

import * as NetworkApi from 'api/network';

import { updateState, useRequest } from 'alova/client';
import { ButtonRow, FormLoader, SectionContent } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import WiFiNetworkSelector from './WiFiNetworkSelector';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 1000;

const WiFiNetworkScanner = () => {
  const pollCount = useRef(0);
  const { LL } = useI18nContext();
  const [errorMessage, setErrorMessage] = useState<string>();

  // is called on page load to start network scan
  const { send: scanNetworks } = useRequest(NetworkApi.scanNetworks).onComplete(
    () => {
      pollCount.current = 0;
      setErrorMessage(undefined);
      void updateState(NetworkApi.listNetworks(), () => undefined);
      void getNetworkList();
    }
  );

  const { data: networkList, send: getNetworkList } = useRequest(
    NetworkApi.listNetworks,
    {
      immediate: false
    }
  ).onSuccess((event) => {
    // is called when network scan is completed
    if (!event.data) {
      const completedPollCount = pollCount.current + 1;
      if (completedPollCount < NUM_POLLS) {
        pollCount.current = completedPollCount;
        setTimeout(getNetworkList, POLLING_FREQUENCY);
      } else {
        setErrorMessage(LL.PROBLEM_LOADING());
        pollCount.current = 0;
      }
    }
  });

  const renderNetworkScanner = () => {
    if (!networkList) {
      return (
        <FormLoader message={LL.SCANNING() + '...'} errorMessage={errorMessage} />
      );
    }
    return <WiFiNetworkSelector networkList={networkList} />;
  };

  return (
    <SectionContent>
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
