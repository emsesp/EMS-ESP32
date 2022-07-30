import { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../components';

import { useI18nContext } from '../i18n/i18n-react';

import SettingsApplication from './SettingsApplication';
import SettingsCustomization from './SettingsCustomization';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  const { routerTab } = useRouterTab();

  useLayoutTitle(LL.SETTINGS());

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="application" label="Application Settings" />
        <Tab value="customization" label="Customization" />
      </RouterTabs>
      <Routes>
        <Route path="application" element={<SettingsApplication />} />
        <Route path="customization" element={<SettingsCustomization />} />
        <Route path="/*" element={<Navigate replace to="application" />} />
      </Routes>
    </>
  );
};

export default Settings;
