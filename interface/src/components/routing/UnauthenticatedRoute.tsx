import React, { FC, useContext } from 'react';
import {
  Redirect, Route,
  RouteProps
} from "react-router-dom";

import * as AuthenticationApi from '../../api/authentication';
import { AuthenticationContext } from '../../contexts/authentication';
import { FeaturesContext } from '../../contexts/features';

type UnauthenticatedRouteProps = Omit<RouteProps, 'component' | 'render'>;

const UnauthenticatedRoute: FC<UnauthenticatedRouteProps> = ({ children, ...rest }) => {
  const { features } = useContext(FeaturesContext);
  const authenticationContext = useContext(AuthenticationContext);
  return (
    <Route
      {...rest}
      render={() => {
        if (!authenticationContext.me) {
          return (<>{children}</>);
        }
        return (<Redirect to={AuthenticationApi.fetchLoginRedirect(features)} />);
      }}
    />
  );
};

export default UnauthenticatedRoute;
