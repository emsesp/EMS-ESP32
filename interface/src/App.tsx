import React, { Component, RefObject } from 'react';
import { Redirect, Route, Switch } from 'react-router';
import { SnackbarProvider } from 'notistack';

import { IconButton } from '@material-ui/core';
import CloseIcon from '@material-ui/icons/Close';

import AppRouting from './AppRouting';
import CustomMuiTheme from './CustomMuiTheme';
import { PROJECT_NAME } from './api';
import FeaturesWrapper from './features/FeaturesWrapper';

import LanguageWrapper from './i18n/LanguageWrapper';

// this redirect forces a call to authenticationContext.refresh() which invalidates the JWT if it is invalid.
const unauthorizedRedirect = () => <Redirect to="/" />;

class App extends Component {
  notistackRef: RefObject<any> = React.createRef();

  componentDidMount() {
    document.title = PROJECT_NAME;
  }

  onClickDismiss = (key: string | number | undefined) => () => {
    this.notistackRef.current.closeSnackbar(key);
  };

  render() {
    return (
      <CustomMuiTheme>
        <SnackbarProvider
          autoHideDuration={3000}
          maxSnack={3}
          anchorOrigin={{ vertical: 'bottom', horizontal: 'left' }}
          ref={this.notistackRef}
          action={(key) => (
            <IconButton onClick={this.onClickDismiss(key)} size="small">
              <CloseIcon />
            </IconButton>
          )}
        >
          <LanguageWrapper>
            <FeaturesWrapper>
              <Switch>
                <Route
                  exact
                  path="/unauthorized"
                  component={unauthorizedRedirect}
                />
                <Route component={AppRouting} />
              </Switch>
            </FeaturesWrapper>
          </LanguageWrapper>
        </SnackbarProvider>
      </CustomMuiTheme>
    );
  }
}

export default App;
