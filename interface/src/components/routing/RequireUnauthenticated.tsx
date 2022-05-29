import { FC, useContext } from 'react';
import { Navigate } from 'react-router-dom';

import * as AuthenticationApi from '../../api/authentication';
import { AuthenticationContext } from '../../contexts/authentication';
import { RequiredChildrenProps } from '../../utils';
import { FeaturesContext } from '../../contexts/features';

const RequireUnauthenticated: FC<RequiredChildrenProps> = ({ children }) => {
  const { features } = useContext(FeaturesContext);
  const authenticationContext = useContext(AuthenticationContext);

  return authenticationContext.me ? <Navigate to={AuthenticationApi.fetchLoginRedirect(features)} /> : <>{children}</>;
};

export default RequireUnauthenticated;
