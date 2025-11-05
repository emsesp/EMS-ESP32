import { type FC, Suspense, lazy, memo, useContext, useEffect, useRef } from 'react';
import { Navigate, Route, Routes } from 'react-router';
import { toast } from 'react-toastify';

import {
  LoadingSpinner,
  RequireAuthenticated,
  RequireUnauthenticated
} from 'components';
import { Authentication, AuthenticationContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

// Lazy load route components for better code splitting
const SignIn = lazy(() => import('SignIn'));
const AuthenticatedRouting = lazy(() => import('AuthenticatedRouting'));

interface SecurityRedirectProps {
  readonly message: string;
  readonly signOut?: boolean;
}

const RootRedirect: FC<SecurityRedirectProps> = memo(
  ({ message, signOut = false }) => {
    const { signOut: contextSignOut } = useContext(AuthenticationContext);
    const hasShownToast = useRef(false);

    useEffect(() => {
      // Prevent duplicate toasts on strict mode or re-renders
      if (!hasShownToast.current) {
        hasShownToast.current = true;
        if (signOut) {
          contextSignOut(false);
        }
        toast.success(message);
      }
      // Only run once on mount - using ref to track execution
    }, []);

    return <Navigate to="/" replace />;
  }
);

const AppRouting: FC = memo(() => {
  const { LL } = useI18nContext();

  return (
    <Authentication>
      <Suspense fallback={<LoadingSpinner />}>
        <Routes>
          <Route
            path="/unauthorized"
            element={<RootRedirect message={LL.PLEASE_SIGNIN()} signOut />}
          />
          <Route
            path="/fileUpdated"
            element={<RootRedirect message={LL.UPLOAD_SUCCESSFUL()} />}
          />
          <Route
            path="/"
            element={
              <RequireUnauthenticated>
                <SignIn />
              </RequireUnauthenticated>
            }
          />
          <Route
            path="/*"
            element={
              <RequireAuthenticated>
                <AuthenticatedRouting />
              </RequireAuthenticated>
            }
          />
        </Routes>
      </Suspense>
    </Authentication>
  );
});

export default AppRouting;
