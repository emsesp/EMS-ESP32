import { useRef, useState } from 'react';
import type { FC } from 'react';

import PermScanWifiIcon from '@mui/icons-material/PermScanWifi';
import { Button } from '@mui/material';

import * as NetworkApi from 'api/network';

import { updateState, useRequest } from 'alova';
import { ButtonRow, FormLoader, SectionContent } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import WiFiNetworkSelector from './WiFiNetworkSelector';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 1000;

const WiFiNetworkScanner: FC = () => {
  const pollCount = useRef(0);
  const { LL } = useI18nContext();
  const [errorMessage, setErrorMessage] = useState<string>();

  const { send: scanNetworks, onComplete: onCompleteScanNetworks } = useRequest(
    NetworkApi.scanNetworks
  ); // is called on page load to start network scan
  const {
    data: networkList,
    send: getNetworkList,
    onSuccess: onSuccessNetworkList
  } = useRequest(NetworkApi.listNetworks, {
    immediate: false
  });

  onSuccessNetworkList((event) => {
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

  onCompleteScanNetworks(() => {
    pollCount.current = 0;
    setErrorMessage(undefined);
    updateState('listNetworks', () => undefined);
    void getNetworkList();
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
