import * as React from 'react';
import { Redirect, Route, RouteProps, RouteComponentProps } from "react-router-dom";
import { withSnackbar, WithSnackbarProps } from 'notistack';

import * as Authentication from './Authentication';
import { withAuthenticationContext, AuthenticationContextProps, AuthenticatedContext } from './AuthenticationContext';

type ChildComponent = React.ComponentType<RouteComponentProps<any>> | React.ComponentType<any>;

interface AuthenticatedRouteProps extends RouteProps, WithSnackbarProps, AuthenticationContextProps {
  component: ChildComponent;
}

type RenderComponent = (props: RouteComponentProps<any>) => React.ReactNode;

export class AuthenticatedRoute extends React.Component<AuthenticatedRouteProps> {

  render() {
    const { enqueueSnackbar, authenticationContext, component: Component, ...rest } = this.props;
    const { location } = this.props;
    const renderComponent: RenderComponent = (props) => {
      if (authenticationContext.me) {
        return (
          <AuthenticatedContext.Provider value={authenticationContext as AuthenticatedContext}>
            <Component {...props} />
          </AuthenticatedContext.Provider>
        );
      }
      Authentication.storeLoginRedirect(location);
      enqueueSnackbar("Please sign in to continue.", { variant: 'info' });
      return (
        <Redirect to='/' />
      );
    }
    return (
      <Route {...rest} render={renderComponent} />
    );
  }

}

export default withSnackbar(withAuthenticationContext(AuthenticatedRoute));
