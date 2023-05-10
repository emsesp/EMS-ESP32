import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { WiFiNetworkList, NetworkSettings, NetworkStatus } from 'types';

export function readNetworkStatus(): AxiosPromise<NetworkStatus> {
  return AXIOS.get('/networkStatus');
}

export function scanNetworks(): AxiosPromise<void> {
  return AXIOS.get('/scanNetworks');
}

export function listNetworks(): AxiosPromise<WiFiNetworkList> {
  return AXIOS.get('/listNetworks');
}

export function readNetworkSettings(): AxiosPromise<NetworkSettings> {
  return AXIOS.get('/networkSettings');
}

export function updateNetworkSettings(wifiSettings: NetworkSettings): AxiosPromise<NetworkSettings> {
  return AXIOS.post('/networkSettings', wifiSettings);
}
