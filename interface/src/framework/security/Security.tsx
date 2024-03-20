import { Tab } from '@mui/material';
import { Navigate, Routes, Route } from 'react-router-dom';
import ManageUsers from './ManageUsers';
import SecuritySettings from './SecuritySettings';
import type { FC } from 'react';

import { RouterTabs, useRouterTab, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const Security: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SECURITY(0));

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
