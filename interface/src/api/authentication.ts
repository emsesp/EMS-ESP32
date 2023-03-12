import { AxiosPromise } from 'axios';
import * as H from 'history';
import jwtDecode from 'jwt-decode';
import { Path } from 'react-router-dom';

import { Me, SignInRequest, SignInResponse } from 'types';

import { ACCESS_TOKEN, AXIOS } from './endpoints';

export const SIGN_IN_PATHNAME = 'loginPathname';
export const SIGN_IN_SEARCH = 'loginSearch';

export function verifyAuthorization(): AxiosPromise<void> {
  return AXIOS.get('/verifyAuthorization');
}

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
export const decodeMeJWT = (accessToken: string): Me => jwtDecode(accessToken) as Me;

export function addAccessTokenParameter(url: string) {
  const accessToken = getStorage().getItem(ACCESS_TOKEN);
  if (!accessToken) {
    return url;
  }
  const parsedUrl = new URL(url);
  parsedUrl.searchParams.set(ACCESS_TOKEN, accessToken);
  return parsedUrl.toString();
}
