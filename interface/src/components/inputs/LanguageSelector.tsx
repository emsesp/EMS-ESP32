import { memo, useCallback, useContext, useMemo } from 'react';
import type { ChangeEventHandler } from 'react';
import type { CSSProperties } from 'react';

import { MenuItem, TextField } from '@mui/material';

import CZflag from 'i18n/CZ.svg';
import DEflag from 'i18n/DE.svg';
import FRflag from 'i18n/FR.svg';
import GBflag from 'i18n/GB.svg';
import ITflag from 'i18n/IT.svg';
import NLflag from 'i18n/NL.svg';
import NOflag from 'i18n/NO.svg';
import PLflag from 'i18n/PL.svg';
import SKflag from 'i18n/SK.svg';
import SVflag from 'i18n/SV.svg';
import TRflag from 'i18n/TR.svg';
import { I18nContext } from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';

const flagStyle: CSSProperties = { width: 16, verticalAlign: 'middle' };

interface LanguageOption {
  key: Locales;
  flag: string;
  label: string;
}

const LANGUAGE_OPTIONS: LanguageOption[] = [
  { key: 'cz', flag: CZflag, label: 'CZ' },
  { key: 'de', flag: DEflag, label: 'DE' },
  { key: 'en', flag: GBflag, label: 'EN' },
  { key: 'fr', flag: FRflag, label: 'FR' },
  { key: 'it', flag: ITflag, label: 'IT' },
  { key: 'nl', flag: NLflag, label: 'NL' },
  { key: 'no', flag: NOflag, label: 'NO' },
  { key: 'pl', flag: PLflag, label: 'PL' },
  { key: 'sk', flag: SKflag, label: 'SK' },
  { key: 'sv', flag: SVflag, label: 'SV' },
  { key: 'tr', flag: TRflag, label: 'TR' }
];

const LanguageSelector = () => {
  const { setLocale, locale, LL } = useContext(I18nContext);

  const onLocaleSelected: ChangeEventHandler<HTMLInputElement> = useCallback(
    async ({ target }) => {
      const loc = target.value as Locales;
      localStorage.setItem('lang', loc);
      await loadLocaleAsync(loc);
      setLocale(loc);
    },
    [setLocale]
  );

  // Memoize menu items to prevent recreation on every render
  const menuItems = useMemo(
    () =>
      LANGUAGE_OPTIONS.map(({ key, flag, label }) => (
        <MenuItem key={key} value={key}>
          <img src={flag} style={flagStyle} alt={label} />
          &nbsp;{label}
        </MenuItem>
      )),
    []
  );

  return (
    <TextField
      name="locale"
      variant="outlined"
      aria-label={LL.LANGUAGE()}
      value={locale}
      onChange={onLocaleSelected}
      size="small"
      select
    >
      {menuItems}
    </TextField>
  );
};

export default memo(LanguageSelector);
