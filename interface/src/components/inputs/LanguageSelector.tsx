import { type ChangeEventHandler, useContext } from 'react';

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

const LanguageSelector = () => {
  const { setLocale, locale } = useContext(I18nContext);

  const onLocaleSelected: ChangeEventHandler<HTMLInputElement> = async ({
    target
  }) => {
    const loc = target.value as Locales;
    localStorage.setItem('lang', loc);
    await loadLocaleAsync(loc);
    setLocale(loc);
  };

  return (
    <TextField
      name="locale"
      variant="outlined"
      value={locale}
      onChange={onLocaleSelected}
      size="small"
      select
    >
      <MenuItem key="cz" value="cz">
        <img src={CZflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;CZ
      </MenuItem>
      <MenuItem key="de" value="de">
        <img src={DEflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;DE
      </MenuItem>
      <MenuItem key="en" value="en">
        <img src={GBflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;EN
      </MenuItem>
      <MenuItem key="fr" value="fr">
        <img src={FRflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;FR
      </MenuItem>
      <MenuItem key="it" value="it">
        <img src={ITflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;IT
      </MenuItem>
      <MenuItem key="nl" value="nl">
        <img src={NLflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;NL
      </MenuItem>
      <MenuItem key="no" value="no">
        <img src={NOflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;NO
      </MenuItem>
      <MenuItem key="pl" value="pl">
        <img src={PLflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;PL
      </MenuItem>
      <MenuItem key="sk" value="sk">
        <img src={SKflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;SK
      </MenuItem>
      <MenuItem key="sv" value="sv">
        <img src={SVflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;SV
      </MenuItem>
      <MenuItem key="tr" value="tr">
        <img src={TRflag} style={{ width: 16, verticalAlign: 'middle' }} />
        &nbsp;TR
      </MenuItem>
    </TextField>
  );
};

export default LanguageSelector;
