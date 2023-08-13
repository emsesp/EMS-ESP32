import { alovaInstance } from './endpoints';
import type { MqttSettings, MqttStatus } from 'types';

export const readMqttStatus = () => alovaInstance.Get<MqttStatus>('/rest/mqttStatus');
export const readMqttSettings = () => alovaInstance.Get<MqttSettings>('/rest/mqttSettings');
export const updateMqttSettings = (data: MqttSettings) => alovaInstance.Post<MqttSettings>('/rest/mqttSettings', data);
