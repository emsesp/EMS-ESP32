import jwtDecode from 'jwt-decode';
import { ACCESS_TOKEN, AXIOS } from './endpoints';
import type { AxiosPromise } from 'axios';
import type * as H from 'history';
import type { Path } from 'react-router-dom';

import type { Me, SignInRequest, SignInResponse } from 'types';

export const SIGN_IN_PATHNAME = 'loginPathname';
export const SIGN_IN_SEARCH = 'loginSearch';

// TODO move verifyAuthorization to Alova
export function verifyAuthorization(): AxiosPromise<void> {
  return AXIOS.get('/verifyAuthorization');
}
// TODO move signIn to Alova
export function signIn(request: SignInRequest): AxiosPromise<SignInResponse> {
  return AXIOS.post('/signIn', request);
}

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

export function fetchLoginRedirect(): Partial<Path> {
  const signInPathname = getStorage().getItem(SIGN_IN_PATHNAME);
  const signInSearch = getStorage().getItem(SIGN_IN_SEARCH);
  clearLoginRedirect();
  return {
    pathname: signInPathname || `/dashboard`,
    search: (signInPathname && signInSearch) || undefined
  };
}

export const clearAccessToken = () => localStorage.removeItem(ACCESS_TOKEN);
export const decodeMeJWT = (accessToken: string): Me => jwtDecode(accessToken);

export function addAccessTokenParameter(url: string) {
  const accessToken = getStorage().getItem(ACCESS_TOKEN);
  if (!accessToken) {
    return url;
  }
  const parsedUrl = new URL(url);
  parsedUrl.searchParams.set(ACCESS_TOKEN, accessToken);
  return parsedUrl.toString();
}
