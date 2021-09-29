import { Component } from 'react';

import { localStorageDetector } from 'typesafe-i18n/detectors';

import TypesafeI18n from './i18n-react';
import { Locales } from './i18n-types';
import { I18nContext } from './i18n-react';
import { detectLocale, locales } from './i18n-util';

interface LanguageWrapperState {
  locale: Locales;
}

class LanguageWrapper extends Component<{}, LanguageWrapperState> {
  state: LanguageWrapperState = { locale: 'en' };

  detectedLocale = detectLocale(localStorageDetector);

  // could load locale from local storage here
  componentDidMount = () => {
    const detectedLocale = detectLocale(localStorageDetector);
    // this.setState({ "en" });
  };

  selectLocale = (locale: Locales) => {
    this.setState({ locale });
    // i18n.activate(language);
    // TODO add
    localStorage.setItem('lang', locale);
    // setLocale(locale);
  };

  render() {
    const { locale } = this.state;

    return (
      <TypesafeI18n initialLocale={locale}>{this.props.children}</TypesafeI18n>
    );
  }
}

export default LanguageWrapper;
