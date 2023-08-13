import { alovaInstance } from './endpoints';

import type { APSettings, APStatus } from 'types';

export const readAPStatus = () => alovaInstance.Get<APStatus>('/rest/apStatus');
export const readAPSettings = () => alovaInstance.Get<APSettings>('/rest/apSettings');
export const updateAPSettings = (data: APSettings) => alovaInstance.Post<APSettings>('/rest/apSettings', data);
