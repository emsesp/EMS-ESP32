import { useRequest } from 'alova';

import { FeaturesContext } from '.';
import type { FC } from 'react';

import type { RequiredChildrenProps } from 'utils';
import * as FeaturesApi from 'api/features';

const FeaturesLoader: FC<RequiredChildrenProps> = (props) => {
  const { data: features } = useRequest(FeaturesApi.readFeatures);

  return (
    <FeaturesContext.Provider
      value={{
        features
      }}
    >
      {props.children}
    </FeaturesContext.Provider>
  );
};

export default FeaturesLoader;
