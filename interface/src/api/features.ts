import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { Features } from 'types';

export function readFeatures(): AxiosPromise<Features> {
  return AXIOS.get('/features');
}
