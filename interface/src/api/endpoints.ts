import { type AlovaXHRResponse, xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import ReactHook from 'alova/react';

import { unpack } from './unpack';

export const ACCESS_TOKEN = 'access_token' as const;

// Cached token to avoid repeated localStorage access
let cachedToken: string | null = null;

const getAccessToken = (): string | null => {
  if (cachedToken === null) {
    cachedToken = localStorage.getItem(ACCESS_TOKEN);
  }
  return cachedToken;
};

// Clear token cache when needed (e.g., on logout)
export const clearTokenCache = (): void => {
  cachedToken = null;
};

const handleResponse = async (response: AlovaXHRResponse) => {
  // Handle various HTTP status codes
  if (response.status === 205) {
    throw new Error('Reboot required');
  }
  if (response.status === 400) {
    throw new Error('Request Failed');
  }
  if (response.status >= 400) {
    throw new Error(response.statusText);
  }

  const data = (await response.data) as ArrayBuffer;

  // Unpack MessagePack data if ArrayBuffer
  if (data instanceof ArrayBuffer) {
    return unpack(data) as ArrayBuffer;
  }

  return data;
};

export const alovaInstance = createAlova({
  statesHook: ReactHook,
  cacheFor: null, // disable cache
  requestAdapter: xhrRequestAdapter(),
  beforeRequest(method) {
    const token = getAccessToken();
    if (token) {
      method.config.headers.Authorization = `Bearer ${token}`;
    }
  },
  responded: {
    onSuccess: handleResponse
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
