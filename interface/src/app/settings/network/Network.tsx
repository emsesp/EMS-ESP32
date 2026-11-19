import { useEffect } from 'react';
import { type Blocker, Outlet, useNavigate } from 'react-router';

import * as NetworkApi from 'api/network';
import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import FormPage, { useFormPage } from 'app/settings/FormPage';
import { TabNav } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { NetworkSettingsType, WiFiNetwork } from 'types';
import { createNetworkSettingsValidator } from 'validators/network';

import { WiFiConnectionContext } from './WiFiConnectionContext';
import { isNetworkOpen } from './WiFiNetworkSelector';

const BASE = '/settings/network/';

const UNBLOCKED = { state: 'unblocked' } as Blocker;

// both tabs share one form, so unsaved edits survive a visit to the scan tab
const Network = () => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  const { data: status } = useRequest(readSystemStatus);

  const form = useFormPage<NetworkSettingsType>({
    read: NetworkApi.readNetworkSettings,
    update: NetworkApi.updateNetworkSettings,
    validator: createNetworkSettingsValidator
  });

  const { blocker } = form;
  const tabSwitch =
    blocker.state === 'blocked' && blocker.location.pathname.startsWith(BASE);

  useEffect(() => {
    if (tabSwitch) blocker.proceed();
  }, [tabSwitch, blocker]);

  const selectNetwork = (network: WiFiNetwork) => {
    form.set({
      ssid: network.ssid,
      bssid: network.bssid,
      ...(isNetworkOpen(network) && { password: '' })
    });
    void navigate(BASE + 'settings');
  };

  return (
    <WiFiConnectionContext.Provider value={{ form, status, selectNetwork }}>
      <TabNav
        label={LL.NETWORK(0)}
        items={[
          {
            to: BASE + 'settings',
            label: LL.SETTINGS(0),
            icon: 'settings_ethernet'
          },
          { to: BASE + 'scan', label: LL.WIFI_SCAN(), icon: 'wifi_find' }
        ]}
      />
      <FormPage form={tabSwitch ? { ...form, blocker: UNBLOCKED } : form}>
        {() => <Outlet />}
      </FormPage>
    </WiFiConnectionContext.Provider>
  );
};

export default Network;
