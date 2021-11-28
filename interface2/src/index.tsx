import React from 'react';
import ReactDOM from 'react-dom';

import '@fontsource/roboto/latin-400.css';
import '@fontsource/roboto/latin-500.css';

// These fonts are not used by the framework by default, they are omitted to reduce the artefact size.
// If using light or strong font variants these can be re-enabled as required.
//
// import '@fontsource/roboto/latin-300.css';
// import '@fontsource/roboto/latin-700.css';

import { BrowserRouter } from 'react-router-dom';

import App from './App';

ReactDOM.render(
  <React.StrictMode>
    <BrowserRouter>
      <App />
    </BrowserRouter>
  </React.StrictMode>,
  document.getElementById('root')
);
