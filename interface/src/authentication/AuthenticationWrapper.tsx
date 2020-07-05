import * as React from 'react';
import { withSnackbar, WithSnackbarProps } from 'notistack';
import jwtDecode from 'jwt-decode';

import history from '../history'
import { VERIFY_AUTHORIZATION_ENDPOINT } from '../api';
import { ACCESS_TOKEN, authorizedFetch, getStorage } from './Authentication';
import { AuthenticationContext, Me } from './AuthenticationContext';
import FullScreenLoading from '../components/FullScreenLoading';
import { withFeatures, WithFeaturesProps } from '../features/FeaturesContext';

export const decodeMeJWT = (accessToken: string): Me => jwtDecode(accessToken);

interface AuthenticationWrapperState {
  context: AuthenticationContext;
  initialized: boolean;
}

type AuthenticationWrapperProps = WithSnackbarProps & WithFeaturesProps;

class AuthenticationWrapper extends React.Component<AuthenticationWrapperProps, AuthenticationWrapperState> {

  constructor(props: AuthenticationWrapperProps) {
    super(props);
    this.state = {
      context: {
        refresh: this.refresh,
        signIn: this.signIn,
        signOut: this.signOut,
      },
      initialized: false
    };
  }

  componentDidMount() {
    this.refresh();
  }

  render() {
    return (
      <React.Fragment>
        {this.state.initialized ? this.renderContent() : this.renderContentLoading()}
      </React.Fragment>
    );
  }

  renderContent() {
    return (
      <AuthenticationContext.Provider value={this.state.context}>
        {this.props.children}
      </AuthenticationContext.Provider>
    );
  }

  renderContentLoading() {
    return (
      <FullScreenLoading />
    );
  }

  refresh = () => {
    if (!this.props.features.security) {
      this.setState({ initialized: true, context: { ...this.state.context, me: { admin: true, username: "admin" } } });
      return;
    }
    const accessToken = getStorage().getItem(ACCESS_TOKEN)
    if (accessToken) {
      authorizedFetch(VERIFY_AUTHORIZATION_ENDPOINT)
        .then(response => {
          const me = response.status === 200 ? decodeMeJWT(accessToken) : undefined;
          this.setState({ initialized: true, context: { ...this.state.context, me } });
        }).catch(error => {
          this.setState({ initialized: true, context: { ...this.state.context, me: undefined } });
          this.props.enqueueSnackbar("Error verifying authorization: " + error.message, {
            variant: 'error',
          });
        });
    } else {
      this.setState({ initialized: true, context: { ...this.state.context, me: undefined } });
    }
  }

  signIn = (accessToken: string) => {
    try {
      getStorage().setItem(ACCESS_TOKEN, accessToken);
      const me: Me = decodeMeJWT(accessToken);
      this.setState({ context: { ...this.state.context, me } });
      this.props.enqueueSnackbar(`Logged in as ${me.username}`, { variant: 'success' });
    } catch (err) {
      this.setState({ initialized: true, context: { ...this.state.context, me: undefined } });
      throw new Error("Failed to parse JWT " + err.message);
    }
  }

  signOut = () => {
    getStorage().removeItem(ACCESS_TOKEN);
    this.setState({
      context: {
        ...this.state.context,
        me: undefined
      }
    });
    this.props.enqueueSnackbar("You have signed out.", { variant: 'success', });
    history.push('/');
  }

}

export default withFeatures(withSnackbar(AuthenticationWrapper))
