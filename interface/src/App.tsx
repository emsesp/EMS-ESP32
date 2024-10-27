import { useEffect, useState } from 'react';
import { Slide, ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.min.css';

import AppRouting from 'AppRouting';
import CustomTheme from 'CustomTheme';
import TypesafeI18n from 'i18n/i18n-react';
import { detectLocale } from 'i18n/i18n-util';
import { loadLocaleAsync } from 'i18n/i18n-util.async';
import { localStorageDetector } from 'typesafe-i18n/detectors';

const detectedLocale = detectLocale(localStorageDetector);

const App = () => {
  const [wasLoaded, setWasLoaded] = useState(false);

  useEffect(() => {
    void loadLocaleAsync(detectedLocale).then(() => setWasLoaded(true));
  }, []);

  if (!wasLoaded) return null;

  return (
    <TypesafeI18n locale={detectedLocale}>
      <CustomTheme>
        <AppRouting />
        <ToastContainer
          position="bottom-left"
          autoClose={3000}
          hideProgressBar={false}
          newestOnTop={false}
          closeOnClick={true}
          rtl={false}
          pauseOnFocusLoss={false}
          draggable={false}
          pauseOnHover={false}
          transition={Slide}
          closeButton={false}
          theme="light"
        />
      </CustomTheme>
    </TypesafeI18n>
  );
};

export default App;
