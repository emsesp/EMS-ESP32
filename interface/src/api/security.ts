import { alovaInstance } from './endpoints';

import type { SecuritySettings, Token } from 'types';

export const readSecuritySettings = () => alovaInstance.Get<SecuritySettings>('/rest/securitySettings');

export const updateSecuritySettings = (securitySettings: SecuritySettings) =>
  alovaInstance.Post('/rest/securitySettings', securitySettings);

export const generateToken = (username?: string) =>
  alovaInstance.Get<Token>('/rest/generateToken', {
    params: { username }
  });
