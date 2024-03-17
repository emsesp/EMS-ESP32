import { Tab } from '@mui/material';
import { Navigate, Routes, Route } from 'react-router-dom';
import SystemLog from './SystemLog';
import SystemStatusForm from './SystemStatusForm';
import type { FC } from 'react';

import { useRouterTab, RouterTabs, useLayoutTitle } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import EMSStatus from 'project/EMSStatus';

const Status: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.STATUS_OF(''));

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label={LL.SYSTEM(1)} />
        <Tab value="emsesp-status" label="EMS-ESP" />
        <Tab value="log" label={LL.LOG_OF(LL.SYSTEM(2))} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<SystemStatusForm />} />
        <Route path="emsesp-status" element={<EMSStatus />} />
        <Route path="log" element={<SystemLog />} />
        <Route path="*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default Status;
