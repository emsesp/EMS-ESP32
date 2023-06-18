import { alovaInstance } from './endpoints';

import type { WiFiNetworkList, NetworkSettings, NetworkStatus } from 'types';

export const readNetworkStatus = () => alovaInstance.Get<NetworkStatus>('/rest/networkStatus');
export const scanNetworks = () => alovaInstance.Get('/rest/scanNetworks');
export const listNetworks = () =>
  alovaInstance.Get<WiFiNetworkList>('/rest/listNetworks', {
    name: 'listNetworks'
  });
export const readNetworkSettings = () =>
  alovaInstance.Get<NetworkSettings>('/rest/networkSettings', { name: 'networkSettings' });
export const updateNetworkSettings = (wifiSettings: NetworkSettings) =>
  alovaInstance.Post<NetworkSettings>('/rest/networkSettings', wifiSettings);
