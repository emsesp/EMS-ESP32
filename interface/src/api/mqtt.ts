import { AxiosPromise } from 'axios';
import { MqttSettings, MqttStatus } from '../types';

import { AXIOS } from './endpoints';

export function readMqttStatus(): AxiosPromise<MqttStatus> {
  return AXIOS.get('/mqttStatus');
}

export function readMqttSettings(): AxiosPromise<MqttSettings> {
  return AXIOS.get('/mqttSettings');
}

export function updateMqttSettings(ntpSettings: MqttSettings): AxiosPromise<MqttSettings> {
  return AXIOS.post('/mqttSettings', ntpSettings);
}
