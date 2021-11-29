import { AxiosPromise } from 'axios';

import { Features } from '../types';

import { AXIOS } from './endpoints';

export function readFeatures(): AxiosPromise<Features> {
  return AXIOS.get('/features');
}
