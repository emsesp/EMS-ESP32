import { memo, useCallback, useEffect, useState } from 'react';
import { ToastContainer, Zoom } from 'react-toastify';

import AppRouting from 'AppRouting';
import CustomTheme from 'CustomTheme';
import TypesafeI18n from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';
import { detectLocale, navigatorDetector } from 'typesafe-i18n/detectors';

const AVAILABLE_LOCALES = [
  'de',
  'en',
  'it',
  'fr',
  'nl',
  'no',
  'pl',
  'sk',
  'sv',
  'tr',
  'cz'
] as Locales[];

// Static toast configuration - no need to recreate on every render
const TOAST_CONTAINER_PROPS = {
  position: 'bottom-left' as const,
  autoClose: 3000,
  hideProgressBar: false,
  newestOnTop: false,
  closeOnClick: true,
  rtl: false,
  pauseOnFocusLoss: true,
  draggable: false,
  pauseOnHover: false,
  transition: Zoom,
  closeButton: false,
  theme: 'dark' as const,
  toastStyle: {
    border: '1px solid #177ac9',
    width: 'fit-content'
  }
};

const App = memo(() => {
  const [wasLoaded, setWasLoaded] = useState(false);
  const [locale, setLocale] = useState<Locales>('en');

  // Memoize locale initialization to prevent unnecessary re-runs
  const initializeLocale = useCallback(async () => {
    const browserLocale = detectLocale('en', AVAILABLE_LOCALES, navigatorDetector);
    const newLocale = (localStorage.getItem('lang') || browserLocale) as Locales;
    localStorage.setItem('lang', newLocale);
    setLocale(newLocale);
    await loadLocaleAsync(newLocale);
    setWasLoaded(true);
  }, []);

  useEffect(() => {
    void initializeLocale();
  }, [initializeLocale]);

  if (!wasLoaded) return null;

  return (
    <TypesafeI18n locale={locale}>
      <CustomTheme>
        <AppRouting />
        <ToastContainer {...TOAST_CONTAINER_PROPS} />
      </CustomTheme>
    </TypesafeI18n>
  );
});

export default App;
