import { AxiosPromise } from 'axios';

import { SecuritySettings } from '../types';

import { AXIOS } from './endpoints';

export function readSecuritySettings(): AxiosPromise<SecuritySettings> {
  return AXIOS.get('/securitySettings');
}

export function updateSecuritySettings(securitySettings: SecuritySettings): AxiosPromise<SecuritySettings> {
  return AXIOS.post('/securitySettings', securitySettings);
}
