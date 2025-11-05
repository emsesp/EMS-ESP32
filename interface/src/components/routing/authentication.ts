import type { Path } from 'react-router';

import { jwtDecode } from 'jwt-decode';
import type { Me, SignInRequest, SignInResponse } from 'types';

import { ACCESS_TOKEN, alovaInstance } from '../../api/endpoints';

export const SIGN_IN_PATHNAME = 'loginPathname';
export const SIGN_IN_SEARCH = 'loginSearch';

export const verifyAuthorization = () =>
  alovaInstance.Get('/rest/verifyAuthorization');
export const signIn = (request: SignInRequest) =>
  alovaInstance.Post<SignInResponse>('/rest/signIn', request);

// Cache storage reference to avoid repeated checks
let cachedStorage: Storage | undefined;

export function getStorage() {
  if (!cachedStorage) {
    cachedStorage = localStorage || sessionStorage;
  }
  return cachedStorage;
}

export function storeLoginRedirect(location?: { pathname: string; search: string }) {
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
    ...(signInPathname && signInSearch && { search: signInSearch })
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
