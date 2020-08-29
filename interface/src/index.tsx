import React from 'react';
import { render } from 'react-dom';

import history from './history';
import { Router } from 'react-router';

import App from './App';

render((
  <Router history={history}>
    <App/>
  </Router>
), document.getElementById("root"))
