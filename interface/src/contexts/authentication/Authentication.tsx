import { useCallback, useEffect, useState } from 'react';
import type { FC } from 'react';
import { redirect } from 'react-router-dom';
import { toast } from 'react-toastify';

import * as AuthenticationApi from 'api/authentication';
import { ACCESS_TOKEN } from 'api/endpoints';

import { useRequest } from 'alova';
import { LoadingSpinner } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { Me } from 'types';
import type { RequiredChildrenProps } from 'utils';

import { AuthenticationContext } from './context';

const Authentication: FC<RequiredChildrenProps> = ({ children }) => {
  const { LL } = useI18nContext();

  const [initialized, setInitialized] = useState<boolean>(false);
  const [me, setMe] = useState<Me>();

  const { send: verifyAuthorization } = useRequest(AuthenticationApi.verifyAuthorization(), {
    immediate: false
  });

  const signIn = (accessToken: string) => {
    try {
      AuthenticationApi.getStorage().setItem(ACCESS_TOKEN, accessToken);
      const decodedMe = AuthenticationApi.decodeMeJWT(accessToken);
      setMe(decodedMe);
      toast.success(LL.LOGGED_IN({ name: decodedMe.username }));
    } catch (error) {
      setMe(undefined);
      throw new Error('Failed to parse JWT');
    }
  };

  const signOut = (doRedirect: boolean) => {
    AuthenticationApi.clearAccessToken();
    setMe(undefined);
    if (doRedirect) {
      // navigate('/');
      redirect('/');
    }
  };

  const refresh = useCallback(async () => {
    const accessToken = AuthenticationApi.getStorage().getItem(ACCESS_TOKEN);
    if (accessToken) {
      await verifyAuthorization()
        .then(() => {
          setMe(AuthenticationApi.decodeMeJWT(accessToken));
          setInitialized(true);
        })
        .catch(() => {
          setMe(undefined);
          setInitialized(true);
        });
    } else {
      setMe(undefined);
      setInitialized(true);
    }
  }, []);

  useEffect(() => {
    void refresh();
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
