import axios from 'axios';
import { unpack } from './unpack';

import type { AxiosPromise, CancelToken, AxiosProgressEvent } from 'axios';

export const WS_BASE_URL = '/ws/';
export const API_BASE_URL = '/rest/';
export const ES_BASE_URL = '/es/';
export const EMSESP_API_BASE_URL = '/api/';
export const ACCESS_TOKEN = 'access_token';

const location = window.location;
const webProtocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
export const WEB_SOCKET_ROOT = webProtocol + '//' + location.host + WS_BASE_URL;
export const EVENT_SOURCE_ROOT = location.protocol + '//' + location.host + ES_BASE_URL;

export const AXIOS = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json'
  },
  transformRequest: [
    (data, headers) => {
      if (headers) {
        if (localStorage.getItem(ACCESS_TOKEN)) {
          headers.Authorization = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
        }
        if (headers['Content-Type'] !== 'application/json') {
          return data;
        }
      }
      return JSON.stringify(data);
    }
  ]
});

export const AXIOS_API = axios.create({
  baseURL: EMSESP_API_BASE_URL,
  headers: {
    'Content-Type': 'application/json'
  },
  transformRequest: [
    (data, headers) => {
      if (headers) {
        if (localStorage.getItem(ACCESS_TOKEN)) {
          headers.Authorization = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
        }
        if (headers['Content-Type'] !== 'application/json') {
          return data;
        }
      }
      return JSON.stringify(data);
    }
  ]
});

export const AXIOS_BIN = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json'
  },
  responseType: 'arraybuffer',
  transformRequest: [
    (data, headers) => {
      if (headers) {
        if (localStorage.getItem(ACCESS_TOKEN)) {
          headers.Authorization = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
        }
        if (headers['Content-Type'] !== 'application/json') {
          return data;
        }
      }
      return JSON.stringify(data);
    }
  ],
  // transformResponse: [(data) => decode(data)]
  transformResponse: [(data) => unpack(data)] // new using msgpackr
});

export interface FileUploadConfig {
  cancelToken?: CancelToken;
  onUploadProgress?: (progressEvent: AxiosProgressEvent) => void;
}

export const startUploadFile = (url: string, file: File, config?: FileUploadConfig): AxiosPromise<void> => {
  const formData = new FormData();
  formData.append('file', file);

  return AXIOS.post(url, formData, {
    headers: {
      'Content-Type': 'multipart/form-data'
    },
    ...(config || {})
  });
};
