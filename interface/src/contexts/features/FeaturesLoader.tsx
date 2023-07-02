import { useRequest } from 'alova';

import { FeaturesContext } from '.';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';
import * as FeaturesApi from 'api/features';
import { ApplicationError, LoadingSpinner } from 'components';

const FeaturesLoader: FC<RequiredChildrenProps> = (props) => {
  const { data: features, error } = useRequest(FeaturesApi.readFeatures);

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

  if (error) {
    return <ApplicationError message={error?.message} />;
  }

  return <LoadingSpinner height="100vh" />;
};

export default FeaturesLoader;
