import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import {
  Navigate,
  Outlet,
  Route,
  RouterProvider,
  createBrowserRouter,
  createRoutesFromElements,
  useRouteError
} from 'react-router';

import App from 'App';
import SignIn from 'SignIn';
import Commands from 'app/main/Commands';
import CustomEntities from 'app/main/CustomEntities';
import Customizations from 'app/main/Customizations';
import Dashboard from 'app/main/Dashboard';
import Devices from 'app/main/Devices';
import Help from 'app/main/Help';
import Modules from 'app/main/Modules';
import Scheduler from 'app/main/Scheduler';
import Sensors from 'app/main/Sensors';
import UserProfile from 'app/main/UserProfile';
import APSettings from 'app/settings/APSettings';
import ApplicationSettings from 'app/settings/ApplicationSettings';
import DownloadUpload from 'app/settings/DownloadUpload';
import MqttSettings from 'app/settings/MqttSettings';
import NTPSettings from 'app/settings/NTPSettings';
import Settings from 'app/settings/Settings';
import Version from 'app/settings/Version';
import Network from 'app/settings/network/Network';
import NetworkSettings from 'app/settings/network/NetworkSettings';
import WiFiNetworkScanner from 'app/settings/network/WiFiNetworkScanner';
import ManageUsers from 'app/settings/security/ManageUsers';
import Security from 'app/settings/security/Security';
import SecuritySettings from 'app/settings/security/SecuritySettings';
import APStatus from 'app/status/APStatus';
import Activity from 'app/status/Activity';
import HardwareStatus from 'app/status/HardwareStatus';
import MqttStatus from 'app/status/MqttStatus';
import NTPStatus from 'app/status/NTPStatus';
import NetworkStatus from 'app/status/NetworkStatus';
import Status from 'app/status/Status';
import SystemLog from 'app/status/SystemLog';
import {
  Layout,
  RequireAdmin,
  RequireAuthenticated,
  RequireUnauthenticated,
  RootRedirect
} from 'components';

const errorPageStyles = {
  container: {
    display: 'flex',
    flexDirection: 'column' as const,
    alignItems: 'center',
    justifyContent: 'center',
    minHeight: '100vh',
    padding: '2rem',
    textAlign: 'center' as const,
    backgroundColor: '#1e1e1e',
    color: '#eee'
  },
  logo: {
    width: '100px',
    height: '100px',
    marginBottom: '2rem'
  },
  title: {
    fontSize: '2rem',
    margin: '0 0 1rem 0',
    color: '#90CAF9',
    fontWeight: 500 as const
  },
  status: {
    color: '#2196f3',
    fontSize: '1.5rem',
    fontWeight: 400 as const,
    margin: '0 0 1rem 0'
  },
  message: {
    fontFamily: 'monospace, monospace',
    fontSize: '1.1rem',
    color: '#9e9e9e',
    maxWidth: '600px',
    margin: '0 0 2rem 0'
  },
  message2: {
    fontSize: '1.1rem',
    color: '#2196f3',
    maxWidth: '600px',
    margin: '0 0 2rem 0'
  }
};
interface ErrorWithStatus {
  status?: number | string;
  statusText?: string;
  message?: string;
}

function isErrorWithDetails(error: unknown): error is ErrorWithStatus {
  return typeof error === 'object' && error !== null;
}

function getErrorStatus(error: unknown): string {
  if (isErrorWithDetails(error) && 'status' in error && error.status != null) {
    return String(error.status);
  }
  return 'Error';
}

function getErrorMessage(error: unknown): string {
  if (!isErrorWithDetails(error)) {
    return 'Something went wrong';
  }
  return error.statusText || error.message || 'Something went wrong';
}

// Custom Error Component
function ErrorPage() {
  const error = useRouteError();

  return (
    <div style={errorPageStyles.container}>
      <img src="/app/icon.png" alt="EMS-ESP Logo" style={errorPageStyles.logo} />
      <h1 style={errorPageStyles.title}>The WebUI is having problems</h1>
      <p style={errorPageStyles.message}>
        {getErrorStatus(error)}: {getErrorMessage(error)}
      </p>
      <p style={errorPageStyles.message2}>
        Please report on{' '}
        <a
          href="https://emsesp.org/Support"
          target="_blank"
          rel="noreferrer"
          style={{ color: 'inherit', textDecoration: 'underline' }}
        >
          https://emsesp.org/Support
        </a>
      </p>
    </div>
  );
}

const router = createBrowserRouter(
  createRoutesFromElements(
    <Route path="/" element={<App />} errorElement={<ErrorPage />}>
      <Route
        index
        element={
          <RequireUnauthenticated>
            <SignIn />
          </RequireUnauthenticated>
        }
      />
      <Route path="unauthorized" element={<RootRedirect kind="unauthorized" />} />
      <Route path="fileUpdated" element={<RootRedirect kind="fileUpdated" />} />

      <Route
        element={
          <RequireAuthenticated>
            <Layout>
              <Outlet />
            </Layout>
          </RequireAuthenticated>
        }
      >
        <Route path="dashboard/*" element={<Dashboard />} />
        <Route path="devices/*" element={<Devices />} />
        <Route path="sensors/*" element={<Sensors />} />
        <Route path="help/*" element={<Help />} />
        <Route path="user/*" element={<UserProfile />} />

        <Route path="status/*" element={<Status />} />
        <Route path="status/hardwarestatus/*" element={<HardwareStatus />} />
        <Route path="status/activity" element={<Activity />} />
        <Route path="status/log" element={<SystemLog />} />
        <Route path="status/mqtt" element={<MqttStatus />} />
        <Route path="status/ntp" element={<NTPStatus />} />
        <Route path="status/ap" element={<APStatus />} />
        <Route path="status/network" element={<NetworkStatus />} />

        <Route element={<RequireAdmin />}>
          <Route path="settings" element={<Settings />} />
          <Route path="settings/version" element={<Version />} />
          <Route path="settings/application" element={<ApplicationSettings />} />
          <Route path="settings/mqtt" element={<MqttSettings />} />
          <Route path="settings/ntp" element={<NTPSettings />} />
          <Route path="settings/ap" element={<APSettings />} />
          <Route path="settings/modules" element={<Modules />} />
          <Route path="settings/downloadUpload" element={<DownloadUpload />} />

          <Route path="settings/network" element={<Network />}>
            <Route
              index
              element={<Navigate replace to="/settings/network/settings" />}
            />
            <Route path="settings" element={<NetworkSettings />} />
            <Route path="scan" element={<WiFiNetworkScanner />} />
            <Route
              path="*"
              element={<Navigate replace to="/settings/network/settings" />}
            />
          </Route>

          <Route path="settings/security" element={<Security />}>
            <Route
              index
              element={<Navigate replace to="/settings/security/settings" />}
            />
            <Route path="settings" element={<SecuritySettings />} />
            <Route path="users" element={<ManageUsers />} />
            <Route
              path="*"
              element={<Navigate replace to="/settings/security/settings" />}
            />
          </Route>

          <Route path="customizations" element={<Customizations />} />
          <Route path="commands" element={<Commands />} />
          <Route path="scheduler" element={<Scheduler />} />
          <Route path="customentities" element={<CustomEntities />} />
        </Route>

        <Route path="*" element={<Navigate replace to="/" />} />
      </Route>
    </Route>
  )
);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <RouterProvider router={router} />
  </StrictMode>
);
