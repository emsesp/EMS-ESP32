import { Navigate, Route, Routes } from 'react-router';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import ManageUsers from './ManageUsers';
import SecuritySettings from './SecuritySettings';

const Security = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SETTINGS_OF(LL.SECURITY(0)));

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="settings" label={LL.SETTINGS_OF(LL.SECURITY(1))} />
        <Tab value="users" label={LL.MANAGE_USERS()} />
      </RouterTabs>
      <Routes>
        <Route path="users" element={<ManageUsers />} />
        <Route path="settings" element={<SecuritySettings />} />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default Security;
