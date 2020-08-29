import * as H from 'history';

import history from '../history';
import { Features } from '../features/types';
import { getDefaultRoute } from '../AppRouting';

export const ACCESS_TOKEN = 'access_token';
export const SIGN_IN_PATHNAME = 'signInPathname';
export const SIGN_IN_SEARCH = 'signInSearch';

/**
 * Fallback to sessionStorage if localStorage is absent. WebView may not have local storage enabled.
 */
export function getStorage() {
  return localStorage || sessionStorage;
}

export function storeLoginRedirect(location?: H.Location) {
  if (location) {
    getStorage().setItem(SIGN_IN_PATHNAME, location.pathname);
    getStorage().setItem(SIGN_IN_SEARCH, location.search);
  }
}

export function clearLoginRedirect() {
  getStorage().removeItem(SIGN_IN_PATHNAME);
  getStorage().removeItem(SIGN_IN_SEARCH);
}

export function fetchLoginRedirect(features: Features): H.LocationDescriptorObject {
  const signInPathname = getStorage().getItem(SIGN_IN_PATHNAME);
  const signInSearch = getStorage().getItem(SIGN_IN_SEARCH);
  clearLoginRedirect();
  return {
    pathname: signInPathname || getDefaultRoute(features),
    search: (signInPathname && signInSearch) || undefined
  };
}

/**
 * Wraps the normal fetch routene with one with provides the access token if present.
 */
export function authorizedFetch(url: RequestInfo, params?: RequestInit): Promise<Response> {
  const accessToken = getStorage().getItem(ACCESS_TOKEN);
  if (accessToken) {
    params = params || {};
    params.credentials = 'include';
    params.headers = {
      ...params.headers,
      "Authorization": 'Bearer ' + accessToken
    };
  }
  return fetch(url, params);
}

/**
 * fetch() does not yet support upload progress, this wrapper allows us to configure the xhr request 
 * for a single file upload and takes care of adding the Authroization header and redirecting on 
 * authroization errors as we do for normal fetch operations.
 */
export function redirectingAuthorizedUpload(xhr: XMLHttpRequest, url: string, file: File, onProgress: (event: ProgressEvent<EventTarget>) => void): Promise<void> {
  return new Promise((resolve, reject) => {
    xhr.open("POST", url, true);
    const accessToken = getStorage().getItem(ACCESS_TOKEN);
    if (accessToken) {
      xhr.withCredentials = true;
      xhr.setRequestHeader("Authorization", 'Bearer ' + accessToken);
    }
    xhr.upload.onprogress = onProgress;
    xhr.onload = function () {
      if (xhr.status === 401 || xhr.status === 403) {
        history.push("/unauthorized");
      } else {
        resolve();
      }
    };
    xhr.onerror = function (event: ProgressEvent<EventTarget>) {
      reject(new DOMException('Error', 'UploadError'));
    };
    xhr.onabort = function () {
      reject(new DOMException('Aborted', 'AbortError'));
    };
    const formData = new FormData();
    formData.append('file', file);
    xhr.send(formData);
  });
}

/**
 * Wraps the normal fetch routene which redirects on 401 response.
 */
export function redirectingAuthorizedFetch(url: RequestInfo, params?: RequestInit): Promise<Response> {
  return new Promise<Response>((resolve, reject) => {
    authorizedFetch(url, params).then(response => {
      if (response.status === 401 || response.status === 403) {
        history.push("/unauthorized");
      } else {
        resolve(response);
      }
    }).catch(error => {
      reject(error);
    });
  });
}

export function addAccessTokenParameter(url: string) {
  const accessToken = getStorage().getItem(ACCESS_TOKEN);
  if (!accessToken) {
    return url;
  }
  const parsedUrl = new URL(url);
  parsedUrl.searchParams.set(ACCESS_TOKEN, accessToken);
  return parsedUrl.toString();
}
