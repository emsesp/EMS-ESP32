import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { APSettings, APStatus } from 'types';

export function readAPStatus(): AxiosPromise<APStatus> {
  return AXIOS.get('/apStatus');
}

export function readAPSettings(): AxiosPromise<APSettings> {
  return AXIOS.get('/apSettings');
}

export function updateAPSettings(apSettings: APSettings): AxiosPromise<APSettings> {
  return AXIOS.post('/apSettings', apSettings);
}
