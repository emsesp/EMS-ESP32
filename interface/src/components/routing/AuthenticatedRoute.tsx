import React, { FC, useContext, useEffect } from 'react';
import { RouteProps, Redirect, Route } from "react-router-dom";

import { AuthenticatedContext, AuthenticatedContextValue, AuthenticationContext } from '../../contexts/authentication/context';
import { storeLoginRedirect } from '../../api/authentication';

type AuthenticatedRouteProps = Omit<RouteProps, 'component' | 'render'>;

const AuthenticatedRoute: FC<AuthenticatedRouteProps> = ({ children, ...rest }) => {
  const authenticationContext = useContext(AuthenticationContext);

  useEffect(() => {
    if (!authenticationContext.me) {
      storeLoginRedirect(rest.location);
    }
  });

  return (
    <Route
      {...rest}
      render={() => {
        if (authenticationContext.me) {
          return (
            <AuthenticatedContext.Provider value={authenticationContext as AuthenticatedContextValue}>
              {children}
            </AuthenticatedContext.Provider>
          );
        }
        return (<Redirect to='/unauthorized' />);
      }}
    />
  );
};

export default AuthenticatedRoute;

