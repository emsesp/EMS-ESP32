import React, { FC, useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from '../../components';
import { AuthenticatedContext } from '../../contexts/authentication';

import NTPStatusForm from './NTPStatusForm';
import NTPSettingsForm from './NTPSettingsForm';

const NetworkTime: FC = () => {
  useLayoutTitle('Network Time');

  const authenticatedContext = useContext(AuthenticatedContext);
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label="NTP Status" />
        <Tab value="settings" label="NTP Settings" disabled={!authenticatedContext.me.admin} />
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
        <Route path="/*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default NetworkTime;
