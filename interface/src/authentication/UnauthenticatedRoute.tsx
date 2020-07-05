import * as React from 'react';
import { Redirect, Route, RouteProps, RouteComponentProps } from "react-router-dom";

import { withAuthenticationContext, AuthenticationContextProps } from './AuthenticationContext';
import * as Authentication from './Authentication';
import { WithFeaturesProps, withFeatures } from '../features/FeaturesContext';

interface UnauthenticatedRouteProps extends RouteProps, AuthenticationContextProps, WithFeaturesProps {
  component: React.ComponentType<RouteComponentProps<any>> | React.ComponentType<any>;
}

type RenderComponent = (props: RouteComponentProps<any>) => React.ReactNode;

class UnauthenticatedRoute extends Route<UnauthenticatedRouteProps> {

  public render() {
    const { authenticationContext, component: Component, features, ...rest } = this.props;
    const renderComponent: RenderComponent = (props) => {
      if (authenticationContext.me) {
        return (<Redirect to={Authentication.fetchLoginRedirect(features)} />);
      }
      return (<Component {...props} />);
    }
    return (
      <Route {...rest} render={renderComponent} />
    );
  }
}

export default withFeatures(withAuthenticationContext(UnauthenticatedRoute));
