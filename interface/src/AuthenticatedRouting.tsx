import { Navigate, Routes, Route } from 'react-router-dom';
import Help from './project/Help';
import type { FC } from 'react';
import { Layout, RequireAdmin } from 'components';
import Settings from 'framework/Settings';
import AccessPoint from 'framework/ap/AccessPoint';
import Mqtt from 'framework/mqtt/Mqtt';
import NetworkConnection from 'framework/network/NetworkConnection';
import NetworkTime from 'framework/ntp/NetworkTime';
import OTASettingsForm from 'framework/ota/OTASettingsForm';
import Security from 'framework/security/Security';
import Status from 'framework/system/Status';
import UploadDownload from 'framework/system/UploadDownload';
import ApplicationSettings from 'project/ApplicationSettings';
import CustomEntities from 'project/CustomEntities';
import Customization from 'project/Customization';
import Devices from 'project/Devices';
import Scheduler from 'project/Scheduler';
import Sensors from 'project/Sensors';

const AuthenticatedRouting: FC = () => (
  <Layout>
    <Routes>
      <Route path="/devices/*" element={<Devices />} />
      <Route path="/sensors/*" element={<Sensors />} />

      <Route path="/customizations/*" element={<Customization />} />
      <Route path="/scheduler/*" element={<Scheduler />} />
      <Route path="/customentities/*" element={<CustomEntities />} />

      {/* TODO only show the rest here if admin */}

      <Route path="/status/*" element={<Status />} />

      <Route
        path="settings/*"
        element={
          <RequireAdmin>
            <Settings />
          </RequireAdmin>
        }
      />
      <Route path="/settings/network/*" element={<NetworkConnection />} />
      <Route path="/settings/ems-esp/*" element={<ApplicationSettings />} />
      <Route path="/settings/ap/*" element={<AccessPoint />} />
      <Route path="/settings/ntp/*" element={<NetworkTime />} />
      <Route path="/settings/mqtt/*" element={<Mqtt />} />
      <Route path="/settings/ota/*" element={<OTASettingsForm />} />
      <Route
        path="/settings/security/*"
        element={
          <RequireAdmin>
            <Security />
          </RequireAdmin>
        }
      />
      <Route path="/settings/upload/*" element={<UploadDownload />} />

      <Route path="/help/*" element={<Help />} />

      <Route path="/*" element={<Navigate to="/" />} />
    </Routes>
  </Layout>
);

export default AuthenticatedRouting;
