import { xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import ReactHook from 'alova/react';
import axios from 'axios';
import { unpack } from '../api/unpack';

// TODO axios can be removed
import type { AxiosPromise, CancelToken, AxiosProgressEvent } from 'axios';
export const REST_BASE_URL = '/rest/';
export const API_BASE_URL = '/api/';

export const ACCESS_TOKEN = 'access_token';

const host = window.location.host;
export const WEB_SOCKET_ROOT = 'ws://' + host + '/ws/';
export const EVENT_SOURCE_ROOT = 'http://' + host + '/es/';

export const alovaInstance = createAlova({
  statesHook: ReactHook,
  timeout: 3000,
  requestAdapter: xhrRequestAdapter(),
  beforeRequest(method) {
    if (localStorage.getItem(ACCESS_TOKEN)) {
      method.config.headers.Authorization = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
    }
  },

  responded: {
    onSuccess: async (response) => {
      if (response.status == 205) {
        throw new Error('Reboot required');
      } else if (response.status === 400) {
        throw new Error('Request Failed');
      } else if (response.status >= 400) {
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

// TODO replace upload with alova, see https://alova.js.org/next-step/download-upload-progress
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
