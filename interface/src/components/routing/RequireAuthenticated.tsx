import { FC, useContext, useEffect } from 'react';
import { Navigate, useLocation } from 'react-router-dom';

import {
  AuthenticatedContext,
  AuthenticatedContextValue,
  AuthenticationContext
} from 'contexts/authentication/context';
import { storeLoginRedirect } from 'api/authentication';

import { RequiredChildrenProps } from 'utils';

const RequireAuthenticated: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticationContext = useContext(AuthenticationContext);
  const location = useLocation();

  useEffect(() => {
    if (!authenticationContext.me) {
      storeLoginRedirect(location);
    }
  });

  return authenticationContext.me ? (
    <AuthenticatedContext.Provider value={authenticationContext as AuthenticatedContextValue}>
      {children}
    </AuthenticatedContext.Provider>
  ) : (
    <Navigate to="/unauthorized" />
  );
};

export default RequireAuthenticated;
