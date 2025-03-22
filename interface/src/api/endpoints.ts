import { type AlovaXHRResponse, xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import ReactHook from 'alova/react';

import { unpack } from './unpack';

export const ACCESS_TOKEN = 'access_token';

export const alovaInstance = createAlova({
  statesHook: ReactHook,
  // timeout: 3000, // 3 seconds before throwing a timeout error, default is 0 = none
  cacheFor: null, // disable cache
  // cacheFor: {
  //   GET: {
  //     mode: 'memory',
  //     expire: 60 * 10 * 1000 // 60 seconds in cache
  //   }
  // },
  requestAdapter: xhrRequestAdapter(),
  beforeRequest(method) {
    if (localStorage.getItem(ACCESS_TOKEN)) {
      method.config.headers.Authorization =
        'Bearer ' + localStorage.getItem(ACCESS_TOKEN);
    }
    // for simulating very slow networks
    // return new Promise((resolve) => {
    //   const random = 3000 + Math.random() * 2000;
    //   setTimeout(resolve, Math.floor(random));
    // });
  },

  responded: {
    onSuccess: async (response: AlovaXHRResponse) => {
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
      const data: ArrayBuffer = (await response.data) as ArrayBuffer;
      if (response.data instanceof ArrayBuffer) {
        return unpack(data) as ArrayBuffer;
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
  baseURL:
    process.env.NODE_ENV === 'development'
      ? '/gh'
      : 'https://api.github.com/repos/emsesp/EMS-ESP32/releases',
  statesHook: ReactHook,
  requestAdapter: xhrRequestAdapter()
});
