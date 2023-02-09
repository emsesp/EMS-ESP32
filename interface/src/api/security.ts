import { AxiosPromise } from 'axios';

import { SecuritySettings, Token } from 'types';

import { AXIOS } from './endpoints';

export function readSecuritySettings(): AxiosPromise<SecuritySettings> {
  return AXIOS.get('/securitySettings');
}

export function updateSecuritySettings(securitySettings: SecuritySettings): AxiosPromise<SecuritySettings> {
  return AXIOS.post('/securitySettings', securitySettings);
}

export function generateToken(username?: string): AxiosPromise<Token> {
  return AXIOS.get('/generateToken', { params: { username } });
}
