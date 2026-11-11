import type {
  ApiKeyType,
  SecuritySettingsType,
  SetupStateType,
  SetupType
} from 'types';

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

export const readPasswordConfirmed = () =>
  alovaInstance.Get<{ confirmed: boolean }>('/rest/confirmPassword');

export const confirmPassword = (password: string) =>
  alovaInstance.Post('/rest/confirmPassword', { password });

export const readSetup = () => alovaInstance.Get<SetupStateType>('/rest/setup');

export const saveSetup = (setup: SetupType) =>
  alovaInstance.Post<{ access_token: string }>('/rest/setup', setup);
