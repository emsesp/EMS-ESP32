import { useRequest } from 'alova';
import { useCallback, useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-toastify';
import { AuthenticationContext } from './context';
import type { FC } from 'react';

import type { Me } from 'types';
import type { RequiredChildrenProps } from 'utils';
import * as AuthenticationApi from 'api/authentication';
import { ACCESS_TOKEN } from 'api/endpoints';
import { LoadingSpinner } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Authentication: FC<RequiredChildrenProps> = ({ children }) => {
  const { LL } = useI18nContext();

  const navigate = useNavigate();

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

  const signOut = (redirect: boolean) => {
    AuthenticationApi.clearAccessToken();
    setMe(undefined);
    if (redirect) {
      navigate('/');
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
    // eslint-disable-next-line react-hooks/exhaustive-deps
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
