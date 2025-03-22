import { Navigate, Route, Routes, matchRoutes, useLocation } from 'react-router';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import ManageUsers from './ManageUsers';
import SecuritySettings from './SecuritySettings';

const Security = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SECURITY(0));

  const matchedRoutes = matchRoutes(
    [
      { path: '/settings/security/settings', element: <ManageUsers />, dog: 'woof' },
      { path: '/settings/security/users', element: <SecuritySettings /> }
    ],
    useLocation()
  );
  const routerTab = matchedRoutes?.[0].route.path || false;

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab
          value="/settings/security/settings"
          label={LL.SETTINGS_OF(LL.SECURITY(1))}
        />
        <Tab value="/settings/security/users" label={LL.MANAGE_USERS()} />
      </RouterTabs>
      <Routes>
        <Route path="users" element={<ManageUsers />} />
        <Route path="settings" element={<SecuritySettings />} />
        <Route
          path="*"
          element={<Navigate replace to="/settings/security/settings" />}
        />
      </Routes>
    </>
  );
};

export default Security;
