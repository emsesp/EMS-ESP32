import { Tab } from '@mui/material';
import { Navigate, Routes, Route } from 'react-router-dom';
import ManageUsersForm from './ManageUsersForm';
import SecuritySettingsForm from './SecuritySettingsForm';
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
        <Tab value="users" label={LL.MANAGE_USERS()} />
        <Tab value="settings" label={LL.SETTINGS_OF(LL.SECURITY(1))} />
      </RouterTabs>
      <Routes>
        <Route path="users" element={<ManageUsersForm />} />
        <Route path="settings" element={<SecuritySettingsForm />} />
        <Route path="/*" element={<Navigate replace to="users" />} />
      </Routes>
    </>
  );
};

export default Security;
