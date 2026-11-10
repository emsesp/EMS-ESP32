import { type AlovaXHRResponse, xhrRequestAdapter } from '@alova/adapter-xhr';
import { createAlova } from 'alova';
import type { AlovaGenerics, Method } from 'alova';
import ReactHook from 'alova/react';

import {
  PASSWORD_REQUIRED,
  askPassword,
  passwordCancelledMessage
} from './passwordPrompt';
import { unpack } from './unpack';

export const ACCESS_TOKEN = 'access_token' as const;

// the error message for a 400 response
export const REQUEST_FAILED = 'Request Failed';

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

const handleResponse = async (
  response: AlovaXHRResponse,
  method: Method<AlovaGenerics>
) => {
  // ask for the password and send the request again
  if (response.status === PASSWORD_REQUIRED) {
    if (await askPassword()) {
      return method.send(true) as Promise<unknown>;
    }
    throw new Error(passwordCancelledMessage());
  }

  // Handle various HTTP status codes
  if (response.status === 205) {
    throw new Error('Reboot required');
  }
  if (response.status === 400) {
    throw new Error(REQUEST_FAILED);
  }
  if (response.status === 429) {
    throw new Error('Too Many Requests');
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
