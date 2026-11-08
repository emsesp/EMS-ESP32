import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';

import RecoveryApp from './RecoveryApp';

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <RecoveryApp />
  </StrictMode>
);
