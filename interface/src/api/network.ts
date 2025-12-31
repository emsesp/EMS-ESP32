import type { NetworkSettingsType, NetworkStatusType, WiFiNetworkList } from 'types';

import { alovaInstance } from './endpoints';

const LIST_NETWORKS_TIMEOUT = 20000; // 20 seconds

export const readNetworkStatus = () =>
  alovaInstance.Get<NetworkStatusType>('/rest/networkStatus');
export const scanNetworks = () => alovaInstance.Get('/rest/scanNetworks');
export const listNetworks = () =>
  alovaInstance.Get<WiFiNetworkList>('/rest/listNetworks', {
    timeout: LIST_NETWORKS_TIMEOUT
  });
export const readNetworkSettings = () =>
  alovaInstance.Get<NetworkSettingsType>('/rest/networkSettings');
export const updateNetworkSettings = (wifiSettings: NetworkSettingsType) =>
  alovaInstance.Post<NetworkSettingsType>('/rest/networkSettings', wifiSettings);
