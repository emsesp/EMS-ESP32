import React, { FC, useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from '../../components';
import { AuthenticatedContext } from '../../contexts/authentication';

import MqttStatusForm from './MqttStatusForm';
import MqttSettingsForm from './MqttSettingsForm';

const Mqtt: FC = () => {
  useLayoutTitle('MQTT');

  const authenticatedContext = useContext(AuthenticatedContext);
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="status" label="MQTT Status" />
        <Tab value="settings" label="MQTT Settings" disabled={!authenticatedContext.me.admin} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<MqttStatusForm />} />
        <Route
          path="settings"
          element={
            <RequireAdmin>
              <MqttSettingsForm />
            </RequireAdmin>
          }
        />
        <Route path="/*" element={<Navigate replace to="status" />} />
      </Routes>
    </>
  );
};

export default Mqtt;
