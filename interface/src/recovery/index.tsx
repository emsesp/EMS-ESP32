import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';

import 'components/ui/base.css';
import 'components/ui/tokens.css';

import RecoveryApp from './RecoveryApp';
import UiText from './UiText';

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <UiText>
      <RecoveryApp />
    </UiText>
  </StrictMode>
);
