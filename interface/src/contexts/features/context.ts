import { createContext } from 'react';

import { Features } from 'types';

export interface FeaturesContextValue {
  features: Features;
}

const FeaturesContextDefaultValue = {} as FeaturesContextValue;
export const FeaturesContext = createContext(FeaturesContextDefaultValue);
