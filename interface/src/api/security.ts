import type { ApiKeyType, SecuritySettingsType } from 'types';

import { alovaInstance } from './endpoints';

export const readSecuritySettings = () =>
  alovaInstance.Get<SecuritySettingsType>('/rest/securitySettings');

export const updateSecuritySettings = (securitySettings: SecuritySettingsType) =>
  alovaInstance.Post('/rest/securitySettings', securitySettings);

export const readApiKeys = () =>
  alovaInstance.Get<{ keys: ApiKeyType[] }>('/rest/apiKeys');

export const apiKeyAction = (action: {
  action: string;
  label?: string;
  write?: boolean;
  id?: string;
}) => alovaInstance.Post<{ key?: string }>('/rest/apiKeys', action);
