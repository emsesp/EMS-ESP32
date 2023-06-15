import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { Features } from 'types';

// TODO move to Alova
export function readFeatures(): AxiosPromise<Features> {
  return AXIOS.get('/features');
}
