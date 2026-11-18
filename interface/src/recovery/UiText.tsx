import type { FC, ReactNode } from 'react';

import { I18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';

// The recovery app is English only and loads no translations, but the shared ui components read
// a few labels (Close, Cancel, Show password, ...) from the i18n context, where LL is undefined
// without a provider. Unknown keys show the key name.
const TEXT: Partial<Record<keyof TranslationFunctions, string>> = {
  CANCEL: 'Cancel',
  CLOSE: 'Close',
  FILE_TYPE_INVALID: 'This file type is not supported, choose a {types} file',
  MODIFIED: 'modified',
  NEEDS_RESTART: 'needs a restart',
  PLEASE_WAIT: 'Please wait',
  RETRY: 'Retry',
  SHOW_PASSWORD: 'Show password',
  UPLOAD: 'Upload'
};

const LL = new Proxy(
  {},
  {
    get:
      (_, key: keyof TranslationFunctions) =>
      (args?: Record<string, string | number>) =>
        (TEXT[key] ?? key).replace(/\{(\w+)\}/g, (_m, name: string) =>
          String(args?.[name] ?? '')
        )
  }
) as TranslationFunctions;

const value = { locale: 'en', LL, setLocale: () => {} } as const;

const UiText: FC<{ children: ReactNode }> = ({ children }) => (
  <I18nContext.Provider value={value}>{children}</I18nContext.Provider>
);

export default UiText;
