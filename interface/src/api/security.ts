import type { SecuritySettingsType, Token } from 'types';

import { alovaInstance } from './endpoints';

export const readSecuritySettings = () =>
  alovaInstance.Get<SecuritySettingsType>('/rest/securitySettings');

export const updateSecuritySettings = (securitySettings: SecuritySettingsType) =>
  alovaInstance.Post('/rest/securitySettings', securitySettings);

export const generateToken = (username?: string) =>
  alovaInstance.Get<Token>('/rest/generateToken', {
    params: { username }
  });
