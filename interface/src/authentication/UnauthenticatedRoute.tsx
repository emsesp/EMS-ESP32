import * as React from 'react';
import {
  Redirect,
  Route,
  RouteProps,
  RouteComponentProps
} from 'react-router-dom';

import {
  withAuthenticationContext,
  AuthenticationContextProps
} from './AuthenticationContext';
import * as Authentication from './Authentication';
import { WithFeaturesProps, withFeatures } from '../features/FeaturesContext';

interface UnauthenticatedRouteProps
  extends RouteProps,
    AuthenticationContextProps,
    WithFeaturesProps {
  component:
    | React.ComponentType<RouteComponentProps<any>>
    | React.ComponentType<any>;
}

class UnauthenticatedRoute extends Route<UnauthenticatedRouteProps> {
  public render() {
    const { authenticationContext, features, ...rest } = this.props;
    if (authenticationContext.me) {
      return <Redirect to={Authentication.fetchLoginRedirect(features)} />;
    }
    return <Route {...rest} />;
  }
}

export default withFeatures(withAuthenticationContext(UnauthenticatedRoute));
