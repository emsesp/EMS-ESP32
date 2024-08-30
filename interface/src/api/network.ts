import type { NetworkSettingsType, NetworkStatusType, WiFiNetworkList } from 'types';

import { alovaInstance } from './endpoints';

export const readNetworkStatus = () =>
  alovaInstance.Get<NetworkStatusType>('/rest/networkStatus');
export const scanNetworks = () => alovaInstance.Get('/rest/scanNetworks');
export const listNetworks = () =>
  alovaInstance.Get<WiFiNetworkList>('/rest/listNetworks', {
    timeout: 20000 // 20 seconds
  });
export const readNetworkSettings = () =>
  alovaInstance.Get<NetworkSettingsType>('/rest/networkSettings');
export const updateNetworkSettings = (wifiSettings: NetworkSettingsType) =>
  alovaInstance.Post<NetworkSettingsType>('/rest/networkSettings', wifiSettings);
