import { AXIOS, alovaInstance } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { APSettings, APStatus } from 'types';

export const readAPStatus = () => alovaInstance.Get<APStatus>('/apStatus');

export function readAPSettings(): AxiosPromise<APSettings> {
  return AXIOS.get('/apSettings');
}

export function updateAPSettings(apSettings: APSettings): AxiosPromise<APSettings> {
  return AXIOS.post('/apSettings', apSettings);
}
