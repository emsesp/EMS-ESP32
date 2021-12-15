import { FC, useContext } from 'react';
import { Navigate } from 'react-router-dom';

import { AuthenticatedContext } from '../../contexts/authentication';

const RequireAdmin: FC = ({ children }) => {
  const authenticatedContext = useContext(AuthenticatedContext);
  return authenticatedContext.me.admin ? <>{children}</> : <Navigate replace to="/" />;
};

export default RequireAdmin;
