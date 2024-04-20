import type { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import NTPSettings from './NTPSettings';
import NTPStatus from './NTPStatus';

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
        <Route path="status" element={<NTPStatus />} />
        <Route path="settings" element={<NTPSettings />} />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default NetworkTime;
