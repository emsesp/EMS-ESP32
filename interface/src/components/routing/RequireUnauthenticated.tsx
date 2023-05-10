import { useContext } from 'react';
import { Navigate } from 'react-router-dom';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';
import * as AuthenticationApi from 'api/authentication';
import { AuthenticationContext } from 'contexts/authentication';

const RequireUnauthenticated: FC<RequiredChildrenProps> = ({ children }) => {
  const authenticationContext = useContext(AuthenticationContext);

  return authenticationContext.me ? <Navigate to={AuthenticationApi.fetchLoginRedirect()} /> : <>{children}</>;
};

export default RequireUnauthenticated;
