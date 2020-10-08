import React, { Component } from 'react';

import { CssBaseline } from '@material-ui/core';
import { MuiThemeProvider, createMuiTheme, StylesProvider } from '@material-ui/core/styles';
import { blueGrey, orange, red, green } from '@material-ui/core/colors';

const theme = createMuiTheme({
  palette: {
    type: "dark",
    primary: {
      main: '#33bfff',
    },
    secondary: {
      main: '#3d5afe',
    },
    info: {
      main: blueGrey[500]
    },
    warning: {
      main: orange[500]
    },
    error: {
      main: red[500]
    },
    success: {
      main: green[500]
    }
  }
});

export default class CustomMuiTheme extends Component {

  render() {
    return (
      <StylesProvider>
        <MuiThemeProvider theme={theme}>
          <CssBaseline />
          {this.props.children}
        </MuiThemeProvider>
      </StylesProvider>
    );
  }

}
