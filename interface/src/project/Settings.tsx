import { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../components';

import SettingsApplication from './SettingsApplication';
import SettingsCustomize from './SettingsCustomize';

const Settings: FC = () => {
  useLayoutTitle('Settings');
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="application" label="Application Settings" />
        <Tab value="customize" label="Device Customization" />
      </RouterTabs>
      <Routes>
        <Route path="application" element={<SettingsApplication />} />
        <Route path="customize" element={<SettingsCustomize />} />
        <Route path="/*" element={<Navigate replace to="application" />} />
      </Routes>
    </>
  );
};

export default Settings;
