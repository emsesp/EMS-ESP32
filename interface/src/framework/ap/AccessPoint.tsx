import { Tab } from '@mui/material';
import { useContext } from 'react';
import { Navigate, Routes, Route } from 'react-router-dom';

import APSettingsForm from './APSettingsForm';
import APStatusForm from './APStatusForm';
import type { FC } from 'react';
import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';

import { useI18nContext } from 'i18n/i18n-react';

const AccessPoint: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.ACCESS_POINT(0));

  const authenticatedContext = useContext(AuthenticatedContext);

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label={LL.STATUS_OF(LL.ACCESS_POINT(1))} />
        <Tab value="settings" label={LL.SETTINGS_OF(LL.ACCESS_POINT(1))} disabled={!authenticatedContext.me.admin} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<APStatusForm />} />
        <Route index element={<Navigate to="status" />} />
        <Route
          path="settings"
          element={
            <RequireAdmin>
              <APSettingsForm />
            </RequireAdmin>
          }
        />
        {/* <Route path="/*" element={<Navigate to="status" />} /> */}
        <Route path="/*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default AccessPoint;
