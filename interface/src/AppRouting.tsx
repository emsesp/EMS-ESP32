import { FC, useContext, useEffect } from 'react';
import { Navigate, Routes, Route, useLocation } from 'react-router-dom';
import { useSnackbar, VariantType } from 'notistack';

import { Authentication, AuthenticationContext } from './contexts/authentication';
import { FeaturesContext } from './contexts/features';
import { RequireAuthenticated, RequireUnauthenticated } from './components';

import SignIn from './SignIn';
import AuthenticatedRouting from './AuthenticatedRouting';

interface SecurityRedirectProps {
  message: string;
  variant?: VariantType;
}

const SecurityRedirect: FC<SecurityRedirectProps> = ({ message, variant }) => {
  const authenticationContext = useContext(AuthenticationContext);
  const { enqueueSnackbar } = useSnackbar();
  useEffect(() => {
    authenticationContext.signOut(false);
    enqueueSnackbar(message, { variant });
  }, [message, variant, authenticationContext, enqueueSnackbar]);
  return <Navigate to="/" />;
};

export const RemoveTrailingSlashes = () => {
  const location = useLocation();
  return (
    location.pathname.match('/.*/$') && (
      <Navigate
        to={{
          pathname: location.pathname.replace(/\/+$/, ''),
          search: location.search
        }}
      />
    )
  );
};

const AppRouting: FC = () => {
  const { features } = useContext(FeaturesContext);

  return (
    <Authentication>
      <RemoveTrailingSlashes />
      <Routes>
        <Route path="/unauthorized" element={<SecurityRedirect message="Please log in to continue" />} />
        {features.security && (
          <Route
            path="/"
            element={
              <RequireUnauthenticated>
                <SignIn />
              </RequireUnauthenticated>
            }
          />
        )}
        <Route
          path="/*"
          element={
            <RequireAuthenticated>
              <AuthenticatedRouting />
            </RequireAuthenticated>
          }
        />
      </Routes>
    </Authentication>
  );
};

export default AppRouting;
