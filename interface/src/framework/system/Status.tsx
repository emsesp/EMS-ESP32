import { Tab } from '@mui/material';
import { Navigate, Routes, Route } from 'react-router-dom';
import SystemLog from './SystemLog';
import SystemStatus from './SystemStatus';
import type { FC } from 'react';

import { useRouterTab, RouterTabs, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import Activity from 'project/Activity';

const Status: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.STATUS_OF(''));

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label={LL.SYSTEM(1)} />
        <Tab value="activity" label={LL.ACTIVITY()} />
        <Tab value="log" label={LL.LOG_OF('')} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<SystemStatus />} />
        <Route path="activity" element={<Activity />} />
        <Route path="log" element={<SystemLog />} />
        <Route path="*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default Status;
