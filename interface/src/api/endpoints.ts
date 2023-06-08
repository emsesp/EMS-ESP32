import { xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
// import GlobalFetch from 'alova/GlobalFetch';
import ReactHook from 'alova/react';
import axios from 'axios';
import { unpack } from '../api/unpack';

import type { AxiosPromise, CancelToken, AxiosProgressEvent } from 'axios';

export const WS_BASE_URL = '/ws/';
export const ES_BASE_URL = '/es/';

export const REST_BASE_URL = '/rest/';
export const API_BASE_URL = '/api/';

export const ACCESS_TOKEN = 'access_token';

const location = window.location;
const webProtocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
export const WEB_SOCKET_ROOT = webProtocol + '//' + location.host + WS_BASE_URL;
export const EVENT_SOURCE_ROOT = location.protocol + '//' + location.host + ES_BASE_URL;

export const alovaInstance = createAlova({
  baseURL: '/rest/',
  statesHook: ReactHook,
  requestAdapter: xhrRequestAdapter(),
  // requestAdapter: GlobalFetch(),
  beforeRequest(method) {
    // TODO check if bearer works
    if (localStorage.getItem(ACCESS_TOKEN)) {
      method.config.headers.token = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
    }
  },

  responded: {
    onSuccess: async (response) => {
      if (response.status === 400) {
        throw new Error('Invalid command');
      }
      if (response.status >= 400) {
        throw new Error(response.statusText);
      }
      const data = await response.data;
      if (response.data instanceof ArrayBuffer) {
        return unpack(data);
      }
      return data;
    },

    onError: (error) => {
      alert(error.message);
    }
  }
});

export const AXIOS = axios.create({
  baseURL: REST_BASE_URL,
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

export const AXIOS_BIN = axios.create({
  baseURL: REST_BASE_URL,
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
  transformResponse: [(data) => unpack(data)]
});

// TODO replace with alova
// TODO see https://alova.js.org/next-step/download-upload-progress
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
