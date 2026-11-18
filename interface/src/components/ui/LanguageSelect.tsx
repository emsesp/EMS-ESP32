import type { FC } from 'react';

import { useI18nContext } from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';

import './LinkList.css';
import Select from './Select';
import { cx } from './cx';

// language names in their own language, so they are found whatever the current language
export const LANGUAGES: { value: Locales; label: string }[] = [
  { value: 'cs', label: 'Čeština' },
  { value: 'de', label: 'Deutsch' },
  { value: 'en', label: 'English' },
  { value: 'fr', label: 'Français' },
  { value: 'hu', label: 'Magyar' },
  { value: 'it', label: 'Italiano' },
  { value: 'nl', label: 'Nederlands' },
  { value: 'no', label: 'Norsk' },
  { value: 'pl', label: 'Polski' },
  { value: 'sk', label: 'Slovenčina' },
  { value: 'sv', label: 'Svenska' },
  { value: 'tr', label: 'Türkçe' }
];

// the web UI language, kept in this browser only
const useChangeLocale = () => {
  const { setLocale } = useI18nContext();
  return async (loc: Locales) => {
    localStorage.setItem('lang', loc);
    await loadLocaleAsync(loc);
    setLocale(loc);
  };
};

const LanguageSelect: FC<{ className?: string }> = ({ className }) => {
  const { locale, LL } = useI18nContext();
  const change = useChangeLocale();

  return (
    <Select
      label={LL.LANGUAGE()}
      hideLabel
      name="locale"
      className={className}
      options={LANGUAGES}
      value={locale}
      onChange={(e) => void change(e.currentTarget.value as Locales)}
    />
  );
};

// every language as a button, for the user profile
export const LanguageGrid: FC = () => {
  const { locale, LL } = useI18nContext();
  const change = useChangeLocale();
  return (
    <div className="lang-grid" role="group" aria-label={LL.LANGUAGE()}>
      {LANGUAGES.map((l) => (
        <button
          key={l.value}
          type="button"
          lang={l.value}
          aria-pressed={l.value === locale}
          className={cx('lang', l.value === locale && 'on')}
          onClick={() => void change(l.value)}
        >
          <span className="code">{l.value.toUpperCase()}</span>
          {l.label}
        </button>
      ))}
    </div>
  );
};

export default LanguageSelect;
