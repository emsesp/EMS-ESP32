import { useCallback, useEffect, useState } from 'react';

import { FeaturesContext } from '.';
import type { FC } from 'react';

import type { Features } from 'types';
import type { RequiredChildrenProps } from 'utils';
import * as FeaturesApi from 'api/features';
import { ApplicationError, LoadingSpinner } from 'components';
import { extractErrorMessage } from 'utils';

const FeaturesLoader: FC<RequiredChildrenProps> = (props) => {
  const [errorMessage, setErrorMessage] = useState<string>();
  const [features, setFeatures] = useState<Features>();

  const loadFeatures = useCallback(async () => {
    try {
      const response = await FeaturesApi.readFeatures();
      setFeatures(response.data);
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, 'Failed to fetch application details.'));
    }
  }, []);

  useEffect(() => {
    void loadFeatures();
  }, [loadFeatures]);

  if (features) {
    return (
      <FeaturesContext.Provider
        value={{
          features
        }}
      >
        {props.children}
      </FeaturesContext.Provider>
    );
  }

  if (errorMessage) {
    return <ApplicationError message={errorMessage} />;
  }

  return <LoadingSpinner height="100vh" />;
};

export default FeaturesLoader;
