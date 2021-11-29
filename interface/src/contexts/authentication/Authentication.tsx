import { useSnackbar } from 'notistack';
import { FC, useCallback, useContext, useEffect, useState } from 'react';
import { useHistory } from 'react-router-dom';

import * as AuthenticationApi from '../../api/authentication';
import { ACCESS_TOKEN } from '../../api/endpoints';

import { LoadingSpinner } from '../../components';
import { Me } from '../../types';
import { FeaturesContext } from '../features';

import { AuthenticationContext } from './context';

const Authentication: FC = ({ children }) => {
  const { features } = useContext(FeaturesContext);
  const history = useHistory();
  const { enqueueSnackbar } = useSnackbar();

  const [initialized, setInitialized] = useState<boolean>(false);
  const [me, setMe] = useState<Me>();

  const signIn = (accessToken: string) => {
    try {
      AuthenticationApi.getStorage().setItem(ACCESS_TOKEN, accessToken);
      const decodedMe = AuthenticationApi.decodeMeJWT(accessToken);
      setMe(decodedMe);
      enqueueSnackbar(`Logged in as ${decodedMe.username}`, { variant: 'success' });
    } catch (error: any) {
      setMe(undefined);
      throw new Error('Failed to parse JWT ' + error.message);
    }
  };

  const signOut = (redirect: boolean) => {
    AuthenticationApi.clearAccessToken();
    setMe(undefined);
    if (redirect) {
      history.push('/');
    }
  };

  const refresh = useCallback(async () => {
    if (!features.security) {
      setMe({ admin: true, username: 'admin' });
      setInitialized(true);
      return;
    }
    const accessToken = AuthenticationApi.getStorage().getItem(ACCESS_TOKEN);
    if (accessToken) {
      try {
        await AuthenticationApi.verifyAuthorization();
        setMe(AuthenticationApi.decodeMeJWT(accessToken));
        setInitialized(true);
      } catch (error: any) {
        setMe(undefined);
        setInitialized(true);
      }
    } else {
      setMe(undefined);
      setInitialized(true);
    }
  }, [features]);

  useEffect(() => {
    refresh();
  }, [refresh]);

  if (initialized) {
    return (
      <AuthenticationContext.Provider
        value={{
          signIn,
          signOut,
          me,
          refresh
        }}
      >
        {children}
      </AuthenticationContext.Provider>
    );
  }

  return <LoadingSpinner height="100vh" />;
};

export default Authentication;
