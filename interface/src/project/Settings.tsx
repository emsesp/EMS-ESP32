import { Tab } from '@mui/material';
import { Navigate, Route, Routes } from 'react-router-dom';

import SettingsApplication from './SettingsApplication';
import SettingsCustomEntities from './SettingsCustomEntities';
import SettingsCustomization from './SettingsCustomization';
import SettingsScheduler from './SettingsScheduler';
import type { FC } from 'react';
import { RouterTabs, useRouterTab, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  const { routerTab } = useRouterTab();

  useLayoutTitle(LL.SETTINGS_OF(''));

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="/settings/application" label={LL.APPLICATION_SETTINGS()} />
        <Tab value="/settings/customization" label={LL.CUSTOMIZATIONS()} />
        <Tab value="/settings/scheduler" label={LL.SCHEDULER()} />
        <Tab value="/settings/customentities" label={LL.CUSTOM_ENTITIES(0)} />
      </RouterTabs>
      <Routes>
        <Route path="application" element={<SettingsApplication />} />
        <Route path="customization" element={<SettingsCustomization />} />
        <Route path="scheduler" element={<SettingsScheduler />} />
        <Route path="customentities" element={<SettingsCustomEntities />} />
        <Route path="*" element={<Navigate replace to="/settings/application" />} />
      </Routes>
    </>
  );
};

export default Settings;
