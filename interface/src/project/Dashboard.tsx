import { Tab } from '@mui/material';
import { Navigate, Route, Routes } from 'react-router-dom';

import DashboardDevices from './DashboardDevices';
import DashboardSensors from './DashboardSensors';
import DashboardStatus from './DashboardStatus';

import type { FC } from 'react';

import { RouterTabs, useRouterTab, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const Dashboard: FC = () => {
  const { routerTab } = useRouterTab();

  const { LL } = useI18nContext();
  useLayoutTitle(LL.DASHBOARD());

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="devices" label={LL.DEVICES()} />
        <Tab value="sensors" label={LL.SENSORS()} />
        <Tab value="status" label="Status" />
      </RouterTabs>
      <Routes>
        <Route path="devices" element={<DashboardDevices />} />
        <Route path="sensors" element={<DashboardSensors />} />
        <Route path="status" element={<DashboardStatus />} />
        <Route path="/*" element={<Navigate replace to="devices" />} />
      </Routes>
    </>
  );
};

export default Dashboard;
