import { AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';
import type { MqttSettings, MqttStatus } from 'types';

export function readMqttStatus(): AxiosPromise<MqttStatus> {
  return AXIOS.get('/mqttStatus');
}

export function readMqttSettings(): AxiosPromise<MqttSettings> {
  return AXIOS.get('/mqttSettings');
}

export function updateMqttSettings(mqttSettings: MqttSettings): AxiosPromise<MqttSettings> {
  return AXIOS.post('/mqttSettings', mqttSettings);
}
