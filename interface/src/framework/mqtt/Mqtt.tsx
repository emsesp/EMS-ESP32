import { Tab } from '@mui/material';
import { Navigate, Route, Routes } from 'react-router-dom';
import MqttSettingsForm from './MqttSettingsForm';
import MqttStatusForm from './MqttStatusForm';
import type { FC } from 'react';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const Mqtt: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle('MQTT');

  const { routerTab } = useRouterTab();

  return (
    <>
      <RouterTabs value={routerTab}>
        <Tab value="settings" label={LL.SETTINGS_OF('MQTT')} />
        <Tab value="status" label={LL.STATUS_OF('MQTT')} />
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
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default Mqtt;
