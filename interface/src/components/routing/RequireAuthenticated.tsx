import { useContext, useEffect } from 'react';
import type { FC } from 'react';
import { Navigate, useLocation } from 'react-router';

import { storeLoginRedirect } from 'components/routing/authentication';
import type { AuthenticatedContextValue } from 'contexts/authentication/context';
import {
  AuthenticatedContext,
  AuthenticationContext
} from 'contexts/authentication/context';
import type { RequiredChildrenProps } from 'utils';

const RequireAuthenticated: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticationContext = useContext(AuthenticationContext);
  const location = useLocation();

  useEffect(() => {
    if (!authenticationContext.me) {
      storeLoginRedirect(location);
    }
  });

  return authenticationContext.me ? (
    <AuthenticatedContext.Provider
      value={authenticationContext as AuthenticatedContextValue}
    >
      {children}
    </AuthenticatedContext.Provider>
  ) : (
    <Navigate to="/unauthorized" />
  );
};

export default RequireAuthenticated;
