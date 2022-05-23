import { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useRouterTab, useLayoutTitle } from '../components';

import DashboardStatus from './DashboardStatus';
import DashboardData from './DashboardData';

const Dashboard: FC = () => {
  useLayoutTitle('Dashboard');
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="data" label="Devices &amp; Sensors" />
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
