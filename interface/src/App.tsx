import { memo, useEffect, useState } from 'react';
import { Outlet } from 'react-router';

import CustomTheme from 'CustomTheme';
import { Toaster } from 'components/toast';
import { Authentication } from 'contexts/authentication';
import TypesafeI18n from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';
import { detectLocale, navigatorDetector } from 'typesafe-i18n/detectors';

const ALL_LOCALES = [
  'cs',
  'de',
  'en',
  'fr',
  'hu',
  'it',
  'nl',
  'no',
  'pl',
  'sk',
  'sv',
  'tr'
] as Locales[];

// Optional build-time allow-list (e.g. VITE_APP_LOCALES="en,de,nl"). When unset,
// every locale is available. `en` is always kept as the fallback locale, and the
// progmem generator embeds the matching subset into firmware flash.
const localeAllowList = (import.meta.env.VITE_APP_LOCALES ?? '')
  .split(',')
  .map((locale) => locale.trim())
  .filter(Boolean);

const AVAILABLE_LOCALES: Locales[] = localeAllowList.length
  ? ALL_LOCALES.filter(
      (locale) => locale === 'en' || localeAllowList.includes(locale)
    )
  : ALL_LOCALES;

const App = memo(() => {
  const [wasLoaded, setWasLoaded] = useState(false);
  const [locale, setLocale] = useState<Locales>('en');

  useEffect(() => {
    const initializeLocale = async () => {
      const browserLocale = detectLocale('en', AVAILABLE_LOCALES, navigatorDetector);
      const stored = localStorage.getItem('lang');
      // Ignore a stored locale that isn't available (e.g. trimmed from this build).
      const newLocale =
        stored && AVAILABLE_LOCALES.includes(stored as Locales)
          ? (stored as Locales)
          : browserLocale;
      localStorage.setItem('lang', newLocale);
      setLocale(newLocale);
      await loadLocaleAsync(newLocale);
      setWasLoaded(true);
    };
    void initializeLocale();
  }, []);

  if (!wasLoaded) return null;

  return (
    <TypesafeI18n locale={locale}>
      <CustomTheme>
        <Authentication>
          <Outlet />
        </Authentication>
        <Toaster />
      </CustomTheme>
    </TypesafeI18n>
  );
});

export default App;
