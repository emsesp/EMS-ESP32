import { type FC, useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { Layout } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import Settings from 'framework/Settings';
import AccessPoint from 'framework/ap/AccessPoint';
import Mqtt from 'framework/mqtt/Mqtt';
import Network from 'framework/network/Network';
import NetworkTime from 'framework/ntp/NetworkTime';
import Security from 'framework/security/Security';
import ESPSystemStatus from 'framework/system/ESPSystemStatus';
import System from 'framework/system/System';
import UploadDownload from 'framework/system/UploadDownload';
import ApplicationSettings from 'project/ApplicationSettings';
import CustomEntities from 'project/CustomEntities';
import Customization from 'project/Customization';
import Devices from 'project/Devices';
import Scheduler from 'project/Scheduler';
import Sensors from 'project/Sensors';

import Help from './project/Help';

const AuthenticatedRouting: FC = () => {
  const { me } = useContext(AuthenticatedContext);
  return (
    <Layout>
      <Routes>
        <Route path="/devices/*" element={<Devices />} />
        <Route path="/sensors/*" element={<Sensors />} />
        <Route path="/system/*" element={<System />} />
        <Route path="/help/*" element={<Help />} />
        <Route path="/*" element={<Navigate to="/" />} />
        {me.admin && (
          <>
            <Route path="/customizations/*" element={<Customization />} />
            <Route path="/scheduler/*" element={<Scheduler />} />
            <Route path="/customentities/*" element={<CustomEntities />} />
            <Route path="/settings/*" element={<Settings />} />
            <Route path="/settings/network/*" element={<Network />} />
            <Route path="/settings/ems-esp/*" element={<ApplicationSettings />} />
            <Route path="/settings/ap/*" element={<AccessPoint />} />
            <Route path="/settings/ntp/*" element={<NetworkTime />} />
            <Route path="/settings/mqtt/*" element={<Mqtt />} />
            <Route path="/settings/security/*" element={<Security />} />
            <Route
              path="/settings/espsystemstatus/*"
              element={<ESPSystemStatus />}
            />
            <Route path="/settings/upload/*" element={<UploadDownload />} />
          </>
        )}
      </Routes>
    </Layout>
  );
};

export default AuthenticatedRouting;
