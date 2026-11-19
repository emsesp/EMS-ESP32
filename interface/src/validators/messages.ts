import type { TranslationFunctions } from 'i18n/i18n-types';

let current: TranslationFunctions | undefined;

// set by the app root whenever the language changes; validators are built after that
export const setValidatorLL = (LL: TranslationFunctions) => {
  current = LL;
};

export const vLL = () => {
  if (!current)
    throw new Error('validator messages used before the language loaded');
  return current;
};
