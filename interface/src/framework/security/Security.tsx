import { FC } from 'react';
import { Navigate, Routes, Route } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../../components';

import SecuritySettingsForm from './SecuritySettingsForm';
import ManageUsersForm from './ManageUsersForm';

import { useI18nContext } from '../../i18n/i18n-react';

const Security: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SECURITY());

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="users" label="Manage Users" />
        <Tab value="settings" label="Security Settings" />
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
