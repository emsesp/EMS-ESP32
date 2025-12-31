import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import {
  Route,
  RouterProvider,
  createBrowserRouter,
  createRoutesFromElements,
  useRouteError
} from 'react-router';

import App from 'App';

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
    <Route path="/*" element={<App />} errorElement={<ErrorPage />} />
  )
);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <RouterProvider router={router} />
  </StrictMode>
);
