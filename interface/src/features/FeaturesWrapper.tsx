import { FC } from 'react';

import FullScreenLoading from '../components/FullScreenLoading';
import ApplicationError from '../components/ApplicationError';
import { FEATURES_ENDPOINT } from '../api';
import { useRest } from '../hooks';

import { Features } from './types';
import { FeaturesContext } from './FeaturesContext';

const FeaturesWrapper: FC = ({ children }) => {
  const { data: features, errorMessage: error } = useRest<Features>({
    endpoint: FEATURES_ENDPOINT
  });

  if (features) {
    return (
      <FeaturesContext.Provider value={{ features }}>
        {children}
      </FeaturesContext.Provider>
    );
  }

  if (error) {
    return <ApplicationError error={error} />;
  }

  return <FullScreenLoading />;
};

export default FeaturesWrapper;
