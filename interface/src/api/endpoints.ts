import { xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import ReactHook from 'alova/react';
import axios from 'axios';
import { unpack } from '../api/unpack';

import type { AxiosPromise, CancelToken, AxiosProgressEvent } from 'axios';

export const ACCESS_TOKEN = 'access_token';

const host = window.location.host;
export const WEB_SOCKET_ROOT = 'ws://' + host + '/ws/';
export const EVENT_SOURCE_ROOT = 'http://' + host + '/es/';

export const alovaInstance = createAlova({
  statesHook: ReactHook,
  timeout: 3000,
  localCache: {
    GET: {
      mode: 'placeholder',
      // see https://alova.js.org/learning/response-cache/#cache-replaceholder-mode
      expire: 2000
    }
  },
  requestAdapter: xhrRequestAdapter(),
  beforeRequest(method) {
    if (localStorage.getItem(ACCESS_TOKEN)) {
      method.config.headers.Authorization = 'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
    }
  },

  responded: {
    onSuccess: async (response) => {
      // if (response.status === 202) {
      //   throw new Error('Wait'); // wifi scan in progress
      // } else
      if (response.status === 205) {
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
    }

    // Interceptor for request failure. This interceptor will be entered when the request is wrong.
    // TODO how best to handle http errors like 401 (unauthorized) but I think this is handled correctly in AppRouting?
    // onError: (error, method) => {
    //   alert(error.message);
    // }
  }
});

export const alovaInstanceGH = createAlova({
  baseURL: 'https://api.github.com/repos/emsesp/EMS-ESP32',
  statesHook: ReactHook,
  requestAdapter: xhrRequestAdapter()
});

export const AXIOS = axios.create({
  baseURL: '/rest/',
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

// TODO fileupload move to alova
// see https://alova.js.org/next-step/download-upload-progress
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
