import { FC, useContext } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { AuthenticatedContext } from '../../contexts/authentication';
import { FeaturesContext } from '../../contexts/features';

import SystemStatusForm from './SystemStatusForm';
import SystemLog from './SystemLog';
import OTASettingsForm from './OTASettingsForm';
import UploadFirmwareForm from './UploadFirmwareForm';
import { AdminRoute, useLayoutTitle } from '../../components';

const SystemRouting: FC = () => {
  useLayoutTitle('System');

  const history = useHistory();
  const { url } = useRouteMatch();
  const { me } = useContext(AuthenticatedContext);
  const { features } = useContext(FeaturesContext);

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value="/system/status" label="System Status" />
        <Tab value="/system/log" label="System Log" />
        {features.ota && <Tab value="/system/ota" label="OTA Settings" disabled={!me.admin} />}
        {features.upload_firmware && <Tab value="/system/upload" label="Upload Firmware" disabled={!me.admin} />}
      </Tabs>
      <Switch>
        <Route exact path="/system/status">
          <SystemStatusForm />
        </Route>
        <Route exact path="/system/log">
          <SystemLog />
        </Route>
        {features.ota && (
          <AdminRoute exact path="/system/ota">
            <OTASettingsForm />
          </AdminRoute>
        )}
        {features.upload_firmware && (
          <AdminRoute exact path="/system/upload">
            <UploadFirmwareForm />
          </AdminRoute>
        )}
        <Redirect to="/system/status" />
      </Switch>
    </>
  );
};

const System: FC = () => (
  <Switch>
    <Route exact path="/system/*">
      <SystemRouting />
    </Route>
    <Redirect to="/system/status" />
  </Switch>
);

export default System;
