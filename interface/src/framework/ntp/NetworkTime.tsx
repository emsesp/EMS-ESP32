import { Tab } from '@mui/material';
import { Navigate, Route, Routes } from 'react-router-dom';
import NTPSettingsForm from './NTPSettingsForm';
import NTPStatusForm from './NTPStatusForm';
import type { FC } from 'react';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const NetworkTime: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle('NTP');

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="settings" label={LL.SETTINGS_OF('NTP')} />
        <Tab value="status" label={LL.STATUS_OF('NTP')} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<NTPStatusForm />} />
        <Route
          path="settings"
          element={
            <RequireAdmin>
              <NTPSettingsForm />
            </RequireAdmin>
          }
        />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default NetworkTime;
