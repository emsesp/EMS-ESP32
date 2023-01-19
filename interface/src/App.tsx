import { FC, createRef, useEffect, useState, RefObject } from 'react';
import { SnackbarProvider } from 'notistack';

import { IconButton } from '@mui/material';
import CloseIcon from '@mui/icons-material/Close';

import { FeaturesLoader } from './contexts/features';

import CustomTheme from './CustomTheme';
import AppRouting from './AppRouting';

import { localStorageDetector } from 'typesafe-i18n/detectors';
import TypesafeI18n from './i18n/i18n-react';
import { detectLocale } from './i18n/i18n-util';
import { loadLocaleAsync } from './i18n/i18n-util.async';

const detectedLocale = detectLocale(localStorageDetector);

const App: FC = () => {
  const notistackRef: RefObject<any> = createRef();

  const onClickDismiss = (key: string | number | undefined) => () => {
    notistackRef.current.closeSnackbar(key);
  };

  const [wasLoaded, setWasLoaded] = useState(false);

  useEffect(() => {
    loadLocaleAsync(detectedLocale).then(() => setWasLoaded(true));
  }, []);

  if (!wasLoaded) return null;

  return (
    <TypesafeI18n locale={detectedLocale}>
      <CustomTheme>
        <SnackbarProvider
          maxSnack={3}
          anchorOrigin={{ vertical: 'bottom', horizontal: 'left' }}
          ref={notistackRef}
          action={(key) => (
            <IconButton onClick={onClickDismiss(key)} size="small">
              <CloseIcon />
            </IconButton>
          )}
        >
          <FeaturesLoader>
            <AppRouting />
          </FeaturesLoader>
        </SnackbarProvider>
      </CustomTheme>
    </TypesafeI18n>
  );
};

export default App;
