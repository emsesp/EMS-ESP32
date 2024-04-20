import type { FC } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import MqttSettings from './MqttSettings';
import MqttStatus from './MqttStatus';

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
        <Route path="status" element={<MqttStatus />} />
        <Route path="settings" element={<MqttSettings />} />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </>
  );
};

export default Mqtt;
