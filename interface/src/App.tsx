import { useEffect, useState } from 'react';
import { ToastContainer, Zoom } from 'react-toastify';

import AppRouting from 'AppRouting';
import CustomTheme from 'CustomTheme';
import TypesafeI18n from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';
import { detectLocale, navigatorDetector } from 'typesafe-i18n/detectors';

const availableLocales = [
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
];

const App = () => {
  const [wasLoaded, setWasLoaded] = useState(false);
  const [locale, setLocale] = useState<Locales>('en');

  useEffect(() => {
    // determine locale, take from session if set other default to browser language
    const browserLocale = detectLocale('en', availableLocales, navigatorDetector);
    const newLocale = (localStorage.getItem('lang') || browserLocale) as Locales;
    localStorage.setItem('lang', newLocale);
    setLocale(newLocale);
    void loadLocaleAsync(newLocale).then(() => setWasLoaded(true));
  }, []);

  if (!wasLoaded) return null;

  return (
    <TypesafeI18n locale={locale}>
      <CustomTheme>
        <AppRouting />
        <ToastContainer
          position="bottom-left"
          autoClose={3000}
          hideProgressBar={false}
          newestOnTop={false}
          closeOnClick
          rtl={false}
          pauseOnFocusLoss
          draggable={false}
          pauseOnHover={false}
          transition={Zoom}
          closeButton={false}
          theme="dark"
          toastStyle={{
            border: '1px solid #177ac9'
          }}
        />
      </CustomTheme>
    </TypesafeI18n>
  );
};

export default App;
