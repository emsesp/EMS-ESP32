import type { MqttSettingsType, MqttStatusType } from 'types';

import { alovaInstance } from './endpoints';

export const readMqttStatus = () =>
  alovaInstance.Get<MqttStatusType>('/rest/mqttStatus');
export const readMqttSettings = () =>
  alovaInstance.Get<MqttSettingsType>('/rest/mqttSettings');
export const updateMqttSettings = (data: MqttSettingsType) =>
  alovaInstance.Post<MqttSettingsType>('/rest/mqttSettings', data);
