import { useRef, useState } from 'react';

import * as NetworkApi from 'api/network';

import { updateState, useRequest } from 'alova/client';
import { Button, Loader, Section } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import WiFiNetworkSelector from './WiFiNetworkSelector';

const NUM_POLLS = 10;
const POLLING_FREQUENCY = 1000;

const WiFiNetworkScanner = () => {
  const pollCount = useRef(0);
  const { LL } = useI18nContext();
  const [errorMessage, setErrorMessage] = useState<string>();

  // starts a scan on page load, then polls the list until the scan is done
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
    { immediate: false }
  ).onSuccess((event) => {
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

  const scanning = !errorMessage && !networkList;

  return (
    <Section
      icon="wifi_find"
      title={LL.NETWORKS_NEARBY()}
      actions={
        <Button
          size="sm"
          icon="refresh"
          disabled={scanning}
          onClick={() => void scanNetworks()}
        >
          {LL.SCAN_AGAIN()}
        </Button>
      }
    >
      {networkList ? (
        <WiFiNetworkSelector networkList={networkList} />
      ) : (
        <Loader error={errorMessage} />
      )}
    </Section>
  );
};

export default WiFiNetworkScanner;
