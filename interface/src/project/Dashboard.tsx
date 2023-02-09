import { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

import DashboardStatus from './DashboardStatus';
import DashboardData from './DashboardData';

const Dashboard: FC = () => {
  const { routerTab } = useRouterTab();

  const { LL } = useI18nContext();
  useLayoutTitle(LL.DASHBOARD());

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="data" label={LL.DEVICES_SENSORS()} />
        <Tab value="status" label="Status" />
      </RouterTabs>
      <Routes>
        <Route path="data" element={<DashboardData />} />
        <Route path="status" element={<DashboardStatus />} />
        <Route path="/*" element={<Navigate replace to="data" />} />
      </Routes>
    </>
  );
};

export default Dashboard;
