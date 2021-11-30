import React from 'react';
import ReactDOM from 'react-dom';

// Note: Not importing from @fontsource in order only bundle woff2 fonts
// This reduces the bundle size by ~40k
import './fonts/roboto/latin-400.css';
import './fonts/roboto/latin-500.css';
// Uncomment if using strong or light typography variants.
// import './fonts/roboto/latin-300.css';
// import './fonts/roboto/latin-700.css';

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
