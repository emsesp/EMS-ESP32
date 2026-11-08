import { createContext } from 'react';

import type { Me, VersionsResponse } from 'types';

export interface AuthenticationContextValue {
  refresh: () => Promise<void>;
  signIn: (accessToken: string) => void;
  signOut: (redirect: boolean) => void;
  me?: Me;
  versions?: VersionsResponse;
  refreshVersions: () => Promise<void>;
  systemName?: string;
  setSystemName: (systemName: string) => void;
}

const AuthenticationContextDefaultValue = {} as AuthenticationContextValue;
export const AuthenticationContext = createContext(
  AuthenticationContextDefaultValue
);

export interface AuthenticatedContextValue extends AuthenticationContextValue {
  me: Me;
}

const AuthenticatedContextDefaultValue = {} as AuthenticatedContextValue;
export const AuthenticatedContext = createContext(AuthenticatedContextDefaultValue);
