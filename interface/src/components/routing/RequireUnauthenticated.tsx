import { useContext } from 'react';
import type { FC } from 'react';
import { Navigate } from 'react-router';

import { fetchLoginRedirect } from 'components/routing/authentication';
import { AuthenticationContext } from 'contexts/authentication';
import type { RequiredChildrenProps } from 'utils';

const RequireUnauthenticated: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticationContext = useContext(AuthenticationContext);

  return authenticationContext.me ? (
    <Navigate to={fetchLoginRedirect()} />
  ) : (
    <>{children}</>
  );
};

export default RequireUnauthenticated;
