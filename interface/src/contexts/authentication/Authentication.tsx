import { FC, useCallback, useContext, useEffect, useState } from 'react';
import { useSnackbar } from 'notistack';
import { useNavigate } from 'react-router-dom';

import { useI18nContext } from '../../i18n/i18n-react';

import * as AuthenticationApi from '../../api/authentication';
import { ACCESS_TOKEN } from '../../api/endpoints';
import { RequiredChildrenProps } from '../../utils';
import { LoadingSpinner } from '../../components';
import { Me } from '../../types';
import { FeaturesContext } from '../features';
import { AuthenticationContext } from './context';

const Authentication: FC<RequiredChildrenProps> = ({ children }) => {
  const { features } = useContext(FeaturesContext);
  const { LL } = useI18nContext();

  const navigate = useNavigate();
  const { enqueueSnackbar } = useSnackbar();

  const [initialized, setInitialized] = useState<boolean>(false);
  const [me, setMe] = useState<Me>();

  const signIn = (accessToken: string) => {
    try {
      AuthenticationApi.getStorage().setItem(ACCESS_TOKEN, accessToken);
      const decodedMe = AuthenticationApi.decodeMeJWT(accessToken);
      setMe(decodedMe);
      enqueueSnackbar(LL.LOGGED_IN({ name: decodedMe.username }), { variant: 'success' });
    } catch (error: unknown) {
      setMe(undefined);
      throw new Error('Failed to parse JWT');
    }
  };

  const signOut = (redirect: boolean) => {
    AuthenticationApi.clearAccessToken();
    setMe(undefined);
    if (redirect) {
      navigate('/');
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
      } catch (error: unknown) {
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
