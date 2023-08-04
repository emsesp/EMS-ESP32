import { xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import ReactHook from 'alova/react';
import { unpack } from '../api/unpack';

export const ACCESS_TOKEN = 'access_token';

const host = window.location.host;
export const WEB_SOCKET_ROOT = 'ws://' + host + '/ws/';
export const EVENT_SOURCE_ROOT = 'http://' + host + '/es/';

export const alovaInstance = createAlova({
  statesHook: ReactHook,
  timeout: 3000, // 3 seconds but throwing a timeout error
  localCache: null,
  // localCache: {
  //   GET: {
  //     mode: 'placeholder', // see https://alova.js.org/learning/response-cache/#cache-replaceholder-mode
  //     expire: 2000
  //   }
  // },
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
    // http errors like 401 (unauthorized) are handled either in the methods or AuthenticatedRouting()
    // onError: (error, method) => {
    //   alert(error.message);
    // }
  }
});

export const alovaInstanceGH = createAlova({
  baseURL: 'https://api.github.com/repos/emsesp/EMS-ESP32/releases',
  statesHook: ReactHook,
  requestAdapter: xhrRequestAdapter()
});
