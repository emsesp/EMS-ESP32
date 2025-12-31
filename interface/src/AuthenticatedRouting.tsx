import { Suspense, lazy, memo, useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router';

import { Layout, LoadingSpinner } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';

// Lazy load all route components for better code splitting
const Dashboard = lazy(() => import('app/main/Dashboard'));
const Devices = lazy(() => import('app/main/Devices'));
const Sensors = lazy(() => import('app/main/Sensors'));
const Help = lazy(() => import('app/main/Help'));
const Customizations = lazy(() => import('app/main/Customizations'));
const Scheduler = lazy(() => import('app/main/Scheduler'));
const CustomEntities = lazy(() => import('app/main/CustomEntities'));
const Modules = lazy(() => import('app/main/Modules'));
const UserProfile = lazy(() => import('app/main/UserProfile'));

const Status = lazy(() => import('app/status/Status'));
const HardwareStatus = lazy(() => import('app/status/HardwareStatus'));
const Activity = lazy(() => import('app/status/Activity'));
const SystemLog = lazy(() => import('app/status/SystemLog'));
const MqttStatus = lazy(() => import('app/status/MqttStatus'));
const NTPStatus = lazy(() => import('app/status/NTPStatus'));
const APStatus = lazy(() => import('app/status/APStatus'));
const NetworkStatus = lazy(() => import('app/status/NetworkStatus'));
const Version = lazy(() => import('app/status/Version'));

const Settings = lazy(() => import('app/settings/Settings'));
const ApplicationSettings = lazy(() => import('app/settings/ApplicationSettings'));
const MqttSettings = lazy(() => import('app/settings/MqttSettings'));
const NTPSettings = lazy(() => import('app/settings/NTPSettings'));
const APSettings = lazy(() => import('app/settings/APSettings'));
const DownloadUpload = lazy(() => import('app/settings/DownloadUpload'));
const Network = lazy(() => import('app/settings/network/Network'));
const Security = lazy(() => import('app/settings/security/Security'));

const AuthenticatedRouting = memo(() => {
  const { me } = useContext(AuthenticatedContext);
  return (
    <Layout>
      <Suspense fallback={<LoadingSpinner />}>
        <Routes>
          <Route path="/dashboard/*" element={<Dashboard />} />
          <Route path="/devices/*" element={<Devices />} />
          <Route path="/sensors/*" element={<Sensors />} />
          <Route path="/help/*" element={<Help />} />
          <Route path="/user/*" element={<UserProfile />} />

          <Route path="/status/*" element={<Status />} />
          <Route path="/status/hardwarestatus/*" element={<HardwareStatus />} />
          <Route path="/status/activity" element={<Activity />} />
          <Route path="/status/log" element={<SystemLog />} />
          <Route path="/status/mqtt" element={<MqttStatus />} />
          <Route path="/status/ntp" element={<NTPStatus />} />
          <Route path="/status/ap" element={<APStatus />} />
          <Route path="/status/network" element={<NetworkStatus />} />
          <Route path="/status/version" element={<Version />} />

          {me.admin && (
            <>
              <Route path="/settings" element={<Settings />} />
              <Route
                path="/settings/application"
                element={<ApplicationSettings />}
              />
              <Route path="/settings/mqtt" element={<MqttSettings />} />
              <Route path="/settings/ntp" element={<NTPSettings />} />
              <Route path="/settings/ap" element={<APSettings />} />
              <Route path="/settings/modules" element={<Modules />} />
              <Route path="/settings/downloadUpload" element={<DownloadUpload />} />

              <Route path="/settings/network/*" element={<Network />} />
              <Route path="/settings/security/*" element={<Security />} />

              <Route path="/customizations" element={<Customizations />} />
              <Route path="/scheduler" element={<Scheduler />} />
              <Route path="/customentities" element={<CustomEntities />} />
            </>
          )}

          <Route path="/*" element={<Navigate to="/" />} />
        </Routes>
      </Suspense>
    </Layout>
  );
});

export default AuthenticatedRouting;
