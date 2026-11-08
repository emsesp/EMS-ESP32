import { useCallback, useEffect, useState } from 'react';
import type { FC } from 'react';
import { useNavigate } from 'react-router';

import { callAction } from 'api/app';
import { ACCESS_TOKEN } from 'api/endpoints';

import * as AuthenticationApi from 'components/routing/authentication';
import { useRequest } from 'alova/client';
import { LoadingSpinner } from 'components';
import { verifyAuthorization } from 'components/routing/authentication';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import type { Me, VersionsResponse } from 'types';
import type { RequiredChildrenProps } from 'utils';

import { AuthenticationContext } from './context';

const Authentication: FC<RequiredChildrenProps> = ({ children }) => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();

  const [initialized, setInitialized] = useState<boolean>(false);
  const [me, setMe] = useState<Me>();
  const [versions, setVersions] = useState<VersionsResponse>();
  const [systemName, setSystemName] = useState<string>();

  const { send: sendVerifyAuthorization } = useRequest(verifyAuthorization(), {
    immediate: false
  });

  const { send: sendGetVersions } = useRequest(
    () => callAction({ action: 'getVersions' }),
    { immediate: false }
  )
    .onSuccess((event) => {
      const response = event.data as VersionsResponse;
      setVersions(response);
      setSystemName(response.system_name);
    })
    .onError(() => {
      setVersions(undefined);
    });

  const refreshVersions = useCallback(async () => {
    await sendGetVersions().catch(() => undefined);
  }, []);

  const signIn = (accessToken: string) => {
    try {
      AuthenticationApi.getStorage().setItem(ACCESS_TOKEN, accessToken);
      const decodedMe = AuthenticationApi.decodeMeJWT(accessToken);
      setMe(decodedMe);
      toast.success(LL.LOGGED_IN({ name: decodedMe.username }));
      void refreshVersions();
    } catch {
      setMe(undefined);
      throw new Error('Failed to parse JWT');
    }
  };

  const signOut = (doRedirect: boolean) => {
    AuthenticationApi.clearAccessToken();
    setMe(undefined);
    setVersions(undefined);
    setSystemName(undefined);
    if (doRedirect) {
      void navigate('/', { replace: true });
    }
  };

  const refresh = useCallback(async () => {
    const accessToken = AuthenticationApi.getStorage().getItem(ACCESS_TOKEN);
    if (accessToken) {
      await sendVerifyAuthorization()
        .then(async () => {
          setMe(AuthenticationApi.decodeMeJWT(accessToken));
          await refreshVersions();
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
          refresh,
          refreshVersions,
          setSystemName,
          ...(me && { me }),
          ...(versions && { versions }),
          ...(systemName !== undefined && { systemName })
        }}
      >
        {children}
      </AuthenticationContext.Provider>
    );
  }

  return <LoadingSpinner height="100vh" />;
};

export default Authentication;
