import type { RecoveryStatus, UploadResult } from './types';

const TOKEN_KEY = 'recovery_token';

export const getToken = (): string | null => sessionStorage.getItem(TOKEN_KEY);

export const setToken = (token: string | null): void => {
  if (token) {
    sessionStorage.setItem(TOKEN_KEY, token);
  } else {
    sessionStorage.removeItem(TOKEN_KEY);
  }
};

export class HttpError extends Error {
  readonly status: number;

  constructor(status: number, message: string) {
    super(message);
    this.status = status;
  }
}

const authHeaders = (): Record<string, string> => {
  const token = getToken();
  return token ? { Authorization: `Bearer ${token}` } : {};
};

const request = async <T>(
  method: string,
  url: string,
  body?: unknown
): Promise<T> => {
  const init: RequestInit = { method, headers: authHeaders(), cache: 'no-store' };
  if (body !== undefined) {
    init.headers = { ...authHeaders(), 'Content-Type': 'application/json' };
    init.body = JSON.stringify(body);
  }
  const response = await fetch(url, init);
  if (!response.ok) {
    throw new HttpError(
      response.status,
      response.statusText || `HTTP ${response.status}`
    );
  }
  const text = await response.text();
  return (text ? JSON.parse(text) : undefined) as T;
};

export const STATUS_URL = '/rest/recovery/status';

export const readStatus = () => request<RecoveryStatus>('GET', STATUS_URL);

export const signIn = (username: string, password: string) =>
  request<{ access_token: string }>('POST', '/rest/signIn', { username, password });

export const setBootPartition = (partition: string) =>
  request<void>('POST', '/rest/recovery/boot', { partition });

export const restart = () => request<void>('POST', '/rest/recovery/restart');

export interface UploadHandle {
  promise: Promise<UploadResult>;
  abort: () => void;
}

// XHR instead of fetch, for upload progress events
export const uploadFile = (
  file: File,
  partition: string,
  onProgress: (percent: number) => void
): UploadHandle => {
  const xhr = new XMLHttpRequest();
  const promise = new Promise<UploadResult>((resolve, reject) => {
    xhr.upload.onprogress = (event) => {
      if (event.lengthComputable && event.total > 0) {
        onProgress(Math.round((event.loaded * 100) / event.total));
      }
    };
    xhr.onload = () => {
      if (xhr.status >= 200 && xhr.status < 300) {
        try {
          resolve(
            (xhr.responseText ? JSON.parse(xhr.responseText) : {}) as UploadResult
          );
        } catch {
          resolve({});
        }
      } else {
        reject(new HttpError(xhr.status, xhr.statusText));
      }
    };
    // the device closes the connection on unsupported files, so there is no status code
    xhr.onerror = () => reject(new HttpError(0, 'Connection closed'));
    xhr.onabort = () => reject(new HttpError(-1, 'Aborted'));
  });

  const formData = new FormData();
  formData.append('file', file);
  xhr.open('POST', `/rest/uploadFile?partition=${encodeURIComponent(partition)}`);
  for (const [key, value] of Object.entries(authHeaders())) {
    xhr.setRequestHeader(key, value);
  }
  xhr.send(formData);

  return { promise, abort: () => xhr.abort() };
};
