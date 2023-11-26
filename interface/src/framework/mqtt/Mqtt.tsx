import { Tab } from '@mui/material';
import { useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';
import MqttSettingsForm from './MqttSettingsForm';
import MqttStatusForm from './MqttStatusForm';
import type { FC } from 'react';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';

import { useI18nContext } from 'i18n/i18n-react';

const Mqtt: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle('MQTT');

  const authenticatedContext = useContext(AuthenticatedContext);
  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="/mqtt/status" label={LL.STATUS_OF('MQTT')} />
        <Tab value="/mqtt/settings" label={LL.SETTINGS_OF('MQTT')} disabled={!authenticatedContext.me.admin} />
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
        <Route path="*" element={<Navigate replace to="/mqtt/status" />} />
      </Routes>
    </>
  );
};

export default Mqtt;
