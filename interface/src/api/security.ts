import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';

import type { SecuritySettings, Token } from 'types';

// TODO move these 3 to Alova
export function readSecuritySettings(): AxiosPromise<SecuritySettings> {
  return AXIOS.get('/securitySettings');
}

export function updateSecuritySettings(securitySettings: SecuritySettings): AxiosPromise<SecuritySettings> {
  return AXIOS.post('/securitySettings', securitySettings);
}

export function generateToken(username?: string): AxiosPromise<Token> {
  return AXIOS.get('/generateToken', { params: { username } });
}
